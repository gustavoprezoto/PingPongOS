#include <stdio.h>
#include <stdlib.h>

#include "../include/ppos.h"
#include "../include/ppos_data.h"
#include "../include/queue.h"
#include "../include/logging.h"
#include <ucontext.h>
#include <signal.h>
#include <sys/time.h>

// Important tasks
task_t main_task, dispatcher_task;
task_t *current_exec_task, *prev_task;
task_t *active_task_queue, *suspended_task_queue;
task_t *free_task;

int task_id_counter;
volatile unsigned int ppos_systime;

// Time preemption global variables/pointer and macros
struct itimerval preemption_timer;
volatile struct sigaction *preemption_timer_handler;
struct sigaction *metrics_timer_handler;

void start_time_preemption();
void time_preemption_handler(int signum);

void dispatcher();

void age_task_dynamic_priority(task_t *task);

void print_elem (void *ptr);

void update_global_metrics_timer();
void finish_task_metrics(task_t *task);

void print_task_metrics(task_t *task);

void ppos_init() {
    // Disabling stdout buffer to prevent race conditions on printf
    setvbuf(stdout, 0, _IONBF, 0);

    // Starting global variables
    task_id_counter = 0;
    main_task.id = task_id_counter;
    ppos_systime = 0;

    start_time_preemption();

    current_exec_task = &main_task;

    // Dispatcher task creation
    task_init(&dispatcher_task, dispatcher, NULL);
    queue_remove((queue_t**) &active_task_queue, (queue_t*) &dispatcher_task);

}

void task_yield() {
    current_exec_task->state = STATE_READY;
    queue_append((queue_t**) &active_task_queue, (queue_t*) &current_exec_task);
    task_switch(&dispatcher_task);
}

task_t* scheduler() {
    task_t *queue_iterator = active_task_queue;
    task_t *next_scheduled_task = active_task_queue; // iteration strategy purposes

    // Iterate the task queue to find the next scheduled task
    do {
        if(task_getprio(queue_iterator) < task_getprio(next_scheduled_task))
            next_scheduled_task = queue_iterator;

        queue_iterator = queue_iterator->next;
    } while (queue_iterator != active_task_queue);

    next_scheduled_task->dynamic_priority = DYNAMIC_PRIORITY_BASE_VALUE;
    next_scheduled_task->quantum = PPOS_QUANTUM_BASE_VALUE;

    // Iterate the task queue again to apply age factor to enqueued tasks
    queue_iterator = active_task_queue;
    do {
        if(queue_iterator != next_scheduled_task)
            age_task_dynamic_priority(queue_iterator);

        queue_iterator = queue_iterator->next;
    } while (queue_iterator != active_task_queue);

    return next_scheduled_task;
}

void age_task_dynamic_priority(task_t *task) {
    if (task->dynamic_priority != PRIORITY_MIN)
        task->dynamic_priority -= PRIORITY_AGING_FACTOR;
}

void dispatcher(void *arg) {

    // queue_print("queue:", (queue_t*) task_queue, print_elem);
    while (queue_size((queue_t*) active_task_queue) > 0) {
        task_t *next_task = scheduler();

        if (next_task != NULL) {

            if(next_task->state == STATE_READY) {
                queue_remove((queue_t**) &active_task_queue, (queue_t*) next_task);
                queue_append((queue_t**) &active_task_queue, (queue_t*) next_task);
                task_switch(next_task);

                // it should be here to not deference and cause a null pointer in exit_task
                if(free_task != NULL) {
                    free(free_task->context.uc_stack.ss_sp);
                    VALGRIND_STACK_DEREGISTER (free_task->vg_id);
                    free_task = NULL;
                }
            }
            else {
                queue_print("queue:", (queue_t*) active_task_queue, print_elem);
                task_exit(-1);
            }
        }
    }

    task_exit(0);
}

int task_init(task_t *task, void (*start_routine)(void *),  void *arg) {
    char *stack;

    // Creating task context
    getcontext(&task->context);

    stack = malloc(STACKSIZE);
    if (stack == NULL)
    {
        perror("malloc failed");
        exit(-1);
    }
    task->context.uc_stack.ss_sp = stack;
    task->context.uc_stack.ss_size = STACKSIZE;
    task->context.uc_stack.ss_flags = 0;
    task->context.uc_link = 0;

    makecontext(&task->context, (void (*)(void))start_routine, 1, arg);

    // Starting base task_t members
    task_id_counter++;

    task->id = task_id_counter;
    task->state = STATE_READY;
    task_setprio(task, 0);
    task->dynamic_priority = DYNAMIC_PRIORITY_BASE_VALUE;
    task->metrics.activation = 0;
    task->metrics.cpu_time = 0;
    task->metrics.execution_time = 0;
    task->metrics.execution_start = systime();
    task->metrics.execution_end = NULL;

    // Valgrind stuff
    void* end = (char*)task->context.uc_stack.ss_sp + task->context.uc_stack.ss_size;
    task->vg_id = VALGRIND_STACK_REGISTER(task->context.uc_stack.ss_sp, end);

    queue_append((queue_t**) &active_task_queue, (queue_t*) task);

    return task->id;
}

int task_switch(task_t *task) {
    prev_task = current_exec_task;
    current_exec_task = task;

    if (swapcontext(&prev_task->context, &task->context) < 0) {
        perror("Erro na troca de contexto: ");
        current_exec_task = prev_task;
        return -1;
    }

    // task->state = RUNNING; ?
    task->metrics.activation++;

    return 0;
}

int task_id() {
    return current_exec_task->id;
}

void task_setprio (task_t *task, int prio) {
    if(task == NULL)
        task = current_exec_task;

    if(prio > PRIORITY_MAX)
        prio = PRIORITY_MAX;


    if(prio < PRIORITY_MIN)
        prio = PRIORITY_MIN;

    task->static_priority = prio;
}

int task_getprio (task_t *task) {
    if (task == NULL)
        task = current_exec_task;

    return task->static_priority + task->dynamic_priority;
}

void task_exit(int exit_code) {
    current_exec_task->state = STATE_TERMINATED;

    free_task = current_exec_task;

    finish_task_metrics(current_exec_task);
    print_task_metrics(current_exec_task);

    if(current_exec_task == &dispatcher_task) {
        task_switch(&main_task);
    }
    else {
        if(current_exec_task != &main_task) {
            queue_remove((queue_t**) &active_task_queue, (queue_t*) current_exec_task);
            // finish_task_metrics(current_exec_task);
        }
        task_switch(&dispatcher_task);
    }
}

struct sigaction* create_SIGALRM_handler(void (*handler)(int)) {
    struct sigaction *action_p = malloc(sizeof(struct sigaction));

    action_p->sa_handler = handler;
    sigemptyset(&action_p->sa_mask);
    action_p->sa_flags = 0;

    if (sigaction (SIGALRM, action_p, 0) < 0)
    {
        perror ("error creating SIGALARM handler") ;
        exit (1) ;
    }

    return action_p;
}

void start_time_preemption() {
    // Set timer
    preemption_timer.it_value.tv_usec = PPOS_TIMER_INTERVAL_USEC; // ?
    preemption_timer.it_value.tv_sec  = 0;
    preemption_timer.it_interval.tv_usec = PPOS_TIMER_INTERVAL_USEC;
    preemption_timer.it_interval.tv_sec  = 0;
    if (setitimer (ITIMER_REAL, &preemption_timer, 0) < 0)
    {
        perror("setitimer failed");
        task_exit(-3);
    }

    // Create timer handlers (preemption and task clock)
    preemption_timer_handler = create_SIGALRM_handler(time_preemption_handler);
}

void time_preemption_handler(int signum) {
    update_global_metrics_timer();

    if(current_exec_task != &dispatcher_task) {
        if(current_exec_task == NULL) {
            perror("exec_task is NULL lol");
            task_exit(-4);
        }

        current_exec_task->quantum -= PPOS_QUANTUM_LOSS_FACTOR_PER_TICK;
        if (current_exec_task->quantum <= 0) {
            task_yield();
        }
    }
}

void update_global_metrics_timer() {
    ppos_systime++;

    current_exec_task->metrics.cpu_time++;
}

void finish_task_metrics(task_t *task) {
    task->metrics.execution_end = systime();
    task->metrics.execution_time = task->metrics.execution_end - task->metrics.execution_start;
}

unsigned int systime() {
    return ppos_systime;
}

void print_elem (void *ptr)
{
    task_t *elem = ptr;

    if (!elem)
        return ;

    elem->prev ? printf ("%d", elem->prev->id) : printf ("*") ;
    printf ("<%d>", elem->id) ;
    elem->next ? printf ("%d", elem->next->id) : printf ("*") ;
}

void print_task_metrics(task_t *task) {
    printf("Task %d: execution time %d ms, processor time %d ms, %d activations\n",
        task->id, task->metrics.execution_time, task->metrics.cpu_time, task->metrics.activation);
}