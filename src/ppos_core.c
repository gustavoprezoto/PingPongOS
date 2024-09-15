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
task_t *exec_task, *prev_task;
task_t *task_queue;
task_t *free_task;

int tid; // taskId


// Time preemption global variables and macros
struct itimerval preemption_timer;
struct sigaction timer_action;
int executing_ppos_routine;
#define START_EXECUTING_INTERNAL_PPOS_ROUTINE() (executing_ppos_routine = PPOS_ROUTINE_EXECUTING)
#define STOP_EXECUTING_INTERNAL_PPOS_ROUTINE()  (executing_ppos_routine = PPOS_ROUTINE_NOT_EXECUTING)

void start_time_preemption();
void time_preemption_handler(int signum);

void dispatcher();

void age_task_dynamic_priority(task_t *task);
void task_set_base_dynamic_priority(task_t *task);

void print_elem (void *ptr);

void ppos_init() {
    // Desabilita o buffer do stdout, usado pelo printf, para previnir race conditions.
    setvbuf(stdout, 0, _IONBF, 0);

    // Atribuição de valores à task principal
    tid = 0;
    main_task.id = tid;

    start_time_preemption();

    // Set current exec task as main task
    exec_task = &main_task;

    // Criação da task dispatcher
    task_init(&dispatcher_task, dispatcher, NULL);
    queue_remove((queue_t**) &task_queue, (queue_t*) &dispatcher_task);

}

void task_yield() {
    exec_task->state = STATE_READY;
    queue_append((queue_t**) &task_queue, (queue_t*) &exec_task);
    task_switch(&dispatcher_task);
}

task_t* scheduler() {
    task_t *queue_iterator = task_queue;
    task_t *next_scheduled_task = task_queue; // iteration strategy purposes

    // Iterate the task queue to find the next scheduled task
    do {
        if(task_getprio(queue_iterator) < task_getprio(next_scheduled_task))
            next_scheduled_task = queue_iterator;

        queue_iterator = queue_iterator->next;
    } while (queue_iterator != task_queue);

    next_scheduled_task->dynamic_priority = DYNAMIC_PRIORITY_BASE_VALUE;
    next_scheduled_task->quantum = PPOS_QUANTUM_BASE_VALUE;

    // Iterate the task queue again to apply age factor to enqueued tasks
    queue_iterator = task_queue;
    do {
        if(queue_iterator != next_scheduled_task)
            age_task_dynamic_priority(queue_iterator);

        queue_iterator = queue_iterator->next;
    } while (queue_iterator != task_queue);

    return next_scheduled_task;
}

void age_task_dynamic_priority(task_t *task) {
    if (task->dynamic_priority != PRIORITY_MIN)
        task->dynamic_priority -= PRIORITY_AGING_FACTOR;
}

void task_set_base_dynamic_priority(task_t *task) {
    task->dynamic_priority = DYNAMIC_PRIORITY_BASE_VALUE;
}

void dispatcher(void *arg) {

    // queue_print("queue:", (queue_t*) task_queue, print_elem);
    while (queue_size((queue_t*) task_queue) > 0) {
        task_t *next_task = scheduler();

        if (next_task != NULL) {

            if(next_task->state == STATE_READY) {
                queue_remove((queue_t**) &task_queue, (queue_t*) next_task);
                queue_append((queue_t**) &task_queue, (queue_t*) next_task);
                task_switch(next_task);

                if(free_task != NULL) {
                    free(free_task->context.uc_stack.ss_sp);
                    VALGRIND_STACK_DEREGISTER (free_task->vg_id);
                    free_task = NULL;
                }
            }
            else {
                queue_print("queue:", (queue_t*) task_queue, print_elem);
                log_debug("Next task is broken... Exiting it.");
                task_exit(-1);
            }
        }
    }

    task_exit(0);
}

int task_init(task_t *task, void (*start_routine)(void *),  void *arg) {
    char *stack;

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

    tid++;
    task->id = tid;
    task->state = STATE_READY;
    task_setprio(task, 0);
    task->dynamic_priority = DYNAMIC_PRIORITY_BASE_VALUE;

    void* end = (char*)task->context.uc_stack.ss_sp + task->context.uc_stack.ss_size;
    task->vg_id = VALGRIND_STACK_REGISTER (task->context.uc_stack.ss_sp, end);

    makecontext(&task->context, (void (*)(void))start_routine, 1, arg);

    queue_append((queue_t**) &task_queue, (queue_t*) task);
    return tid;
}

int task_switch(task_t *task) {
    prev_task = exec_task;
    exec_task = task;

#ifdef DEBUG
    printf("task_switch: trocando a task %d pela task %d.\n", prev_task->id, task->id);
#endif

    if (swapcontext(&prev_task->context, &task->context) < 0) {
        perror("Erro na troca de contexto: ");
        exec_task = prev_task;
        return -1;
    }

    // task->state = RUNNING; ?

    return 0;
}

int task_id() {
    return exec_task->id;
}

void task_setprio (task_t *task, int prio) {
    if(task == NULL)
        task = exec_task;

    if(prio > PRIORITY_MAX)
        prio = PRIORITY_MAX;


    if(prio < PRIORITY_MIN)
        prio = PRIORITY_MIN;

    task->static_priority = prio;
}

int task_getprio (task_t *task) {
    if (task == NULL)
        task = exec_task;

    return task->static_priority + task->dynamic_priority;
}

void task_exit(int exit_code) {
    exec_task->state = STATE_TERMINATED;

    free_task = exec_task;

    if(exec_task == &dispatcher_task) {
        task_switch(&main_task);
    }
    else {
        if(exec_task != &main_task) {
            queue_remove((queue_t**) &task_queue, (queue_t*) exec_task);
        }
        task_switch(&dispatcher_task);
    }
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

    // Set timer handler
    timer_action.sa_handler = time_preemption_handler;
    sigemptyset(&timer_action.sa_mask);
    timer_action.sa_flags = 0;
    if (sigaction (SIGALRM, &timer_action, 0) < 0)
    {
        perror ("Erro em sigaction: ") ;
        exit (1) ;
    }
}

void time_preemption_handler(int signum) {
    if(exec_task != &dispatcher_task) {
        if(exec_task == NULL) {
            perror("exec_task is NULL lol");
            task_exit(-4);
        }

        exec_task->quantum -= PPOS_QUANTUM_LOSS_FACTOR_PER_TICK;
        if (exec_task->quantum <= 0) {
            task_yield();
        }
    }
}

void print_elem (void *ptr)
{
    task_t *elem = ptr ;

    if (!elem)
        return ;

    elem->prev ? printf ("%d", elem->prev->id) : printf ("*") ;
    printf ("<%d>", elem->id) ;
    elem->next ? printf ("%d", elem->next->id) : printf ("*") ;
}
