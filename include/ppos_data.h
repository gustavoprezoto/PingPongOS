// PingPongOS - PingPong Operating System
// Prof. Carlos A. Maziero, DINF UFPR
// Versão 1.5 -- Março de 2023

// Estruturas de dados internas do sistema operacional

#ifndef __PPOS_DATA__
#define __PPOS_DATA__

#include <ucontext.h>		// biblioteca POSIX de trocas de contexto

#define STACKSIZE 64*1024

// Task state constants
#define STATE_READY 0
#define STATE_RUNNING 1
#define STATE_TERMINATED 2

// Task priority constants
#define PRIORITY_MIN -20
#define PRIORITY_MAX 20
#define PRIORITY_AGING_FACTOR 1
#define DYNAMIC_PRIORITY_BASE_VALUE 0

// Preemption constants
#define PPOS_TIMER_INTERVAL_USEC 1000
#define PPOS_QUANTUM_BASE_VALUE 20
#define PPOS_QUANTUM_LOSS_FACTOR_PER_TICK 1
#define PPOS_QUANTUM_MIN_VALUE 0
#define PPOS_QUANTUM_MAX_VALUE 20

// Estrutura que define um Task Control Block (TCB)
typedef struct task_t
{
    struct task_t *prev, *next ;
    int id;
    ucontext_t context;
    int state;
    int static_priority;
    int dynamic_priority;
    int quantum;

    // Valgrind ID (ignore)
    int vg_id;
} task_t ;

// estrutura que define um semáforo
typedef struct
{
    // preencher quando necessário
} semaphore_t ;

// estrutura que define um mutex
typedef struct
{
    // preencher quando necessário
} mutex_t ;

// estrutura que define uma barreira
typedef struct
{
    // preencher quando necessário
} barrier_t ;

// estrutura que define uma fila de mensagens
typedef struct
{
    // preencher quando necessário
} mqueue_t ;

#endif
