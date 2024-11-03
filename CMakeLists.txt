cmake_minimum_required(VERSION 3.28)
project(PingPongOs C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_BUILD_TYPE Debug)
set(PROJECT_SOURCE_DIR /mnt/e/desenv/PingPongOS)
set(PATH_PREFIX "/mnt/e/desenv/PingPongOS/") # at least for my machine :P

add_executable(PingPongOs src/main.c
        ${PATH_PREFIX} src/main.c

        # Header Files
        ${PATH_PREFIX}include/queue.h
        ${PATH_PREFIX}include/ppos_data.h
        ${PATH_PREFIX}include/ppos.h
#        ${PATH_PREFIX}include/logging.h

        # Impl. Files
        ${PATH_PREFIX}src/queue.c
        ${PATH_PREFIX}src/ppos_core.c
#        ${PATH_PREFIX}src/logging.c

        # Playground

        # Test Files
#        ${PATH_PREFIX}tests/ppos_tasks_tests/ppos_tasks2_test.c
#        ${PATH_PREFIX}tests/ppos_tasks_tests/ppos_tasks3_test.c
#        ${PATH_PREFIX}tests/ppos_tasks_tests/ppos_tasks1_test.c
#        ${PATH_PREFIX}tests/debug_test.c
#        ${PATH_PREFIX}tests/dispatcher_tests/dispatcher_test.c
#        ${PATH_PREFIX}tests/scheduler_tests/scheduler_test.c
#        ${PATH_PREFIX}tests/preemption_tests/preemption_test.c
#        ${PATH_PREFIX}tests/time_accounting_tests/accounting_without_prio_test.c
#        ${PATH_PREFIX}tests/suspended_tasks_tests/task_wait_test.c
#        ${PATH_PREFIX}tests/time_accounting_tests/accounting_with_prio_test.c
        ${PATH_PREFIX}tests/sleeping_tasks_tests/sleep_task_test.c
)
