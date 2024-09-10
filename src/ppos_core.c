#include <stdio.h>
#include <stdlib.h>

#include "../include/ppos.h"
#include "../include/ppos_data.h"
#include "../include/queue.h"
#include "../include/logging.h"
#include <ucontext.h>

task_t main_task, dispatcher_task;
task_t *exec_task, *prev_task;
task_t *task_queue;
task_t *free_task;

int tid;

void dispatcher();
void print_elem (void *ptr);

void ppos_init() {
    // Desabilita o buffer do stdout, usado pelo printf, para previnir race conditions.
    setvbuf(stdout, 0, _IONBF, 0);

    // Atribuição de valores à task principal
    tid = 0;
    main_task.id = tid;

    exec_task = &main_task;

    // Criação da task dispatcher
    task_init(&dispatcher_task, dispatcher, NULL);
    queue_remove((queue_t**) &task_queue, (queue_t*) &dispatcher_task);
}

void task_yield() {
    exec_task->state = READY;
    queue_append((queue_t**) &task_queue, (queue_t*) &exec_task);
    task_switch(&dispatcher_task);
}

task_t* scheduler() {
    return task_queue;
}

void dispatcher(void *arg) {
    // queue_print("queue:", (queue_t*) task_queue, print_elem);
    while (queue_size((queue_t*) task_queue) > 0) {
        task_t *next_task = scheduler();

        if (next_task != NULL) {

            if(next_task->state == READY) {
                queue_remove((queue_t**) &task_queue, (queue_t*) next_task);
                queue_append((queue_t**) &task_queue, (queue_t*) next_task);
                task_switch(next_task);

                if(free_task != NULL) {
                    free(free_task->context.uc_stack.ss_sp);
                    VALGRIND_STACK_DEREGISTER (free_task->vg_id);
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
    task->state = READY;

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

void task_exit(int exit_code) {
    exec_task->state = TERMINATED;

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

void print_elem (void *ptr)
{
    task_t *elem = ptr ;

    if (!elem)
        return ;

    elem->prev ? printf ("%d", elem->prev->id) : printf ("*") ;
    printf ("<%d>", elem->id) ;
    elem->next ? printf ("%d", elem->next->id, elem->state) : printf ("*") ;
}
