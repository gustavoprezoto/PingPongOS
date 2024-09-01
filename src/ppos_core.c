#include <stdio.h>
#include <stdlib.h>

#include "../include/ppos.h"
#include "../include/ppos_data.h"
#include <ucontext.h>

task_t main_task;
task_t *exec_task, *prev_task;
int tid;

void ppos_init() {
    // Desabilita o buffer do stdout, usado pelo printf, para previnir race conditions.
    setvbuf(stdout, 0, _IONBF, 0);

    tid = 0;
    main_task.id = tid;

    exec_task = &main_task;
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
    task->status = 0;

    makecontext(&task->context, (void (*)(void))start_routine, 1, arg);

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

    return 0;
}

int task_id() {
    return exec_task->id;
}

void task_exit(int exit_code) {
    if(exec_task == &main_task) {
#ifdef DEBUG
        printf("task_exit: tid 0 (main task). Retornando sem sair da task.\n");
#endif
        return;
    }

#ifdef DEBUG
    printf("task_exit: saindo da exec_task: %d para a task: %d.\n", exec_task->id, prev_task->id);
#endif
    task_switch(&main_task);
    free(exec_task);
}
