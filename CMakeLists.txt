cmake_minimum_required(VERSION 3.28)
project(PingPongOs C)

set(CMAKE_C_STANDARD 11)

add_executable(PingPongOs src/main.c
        # Header Files
        include/queue.h
        include/ppos_data.h
        include/ppos.h
        include/logging.h

        # Impl. Files
        src/queue.c
        src/ppos_core.c
        src/logging.c

        # Playground
        tests/playground.c

        # Tests:
#        tests/ppos_tasks_tests/ppos_tasks2_test.c
#        tests/ppos_tasks_tests/ppos_tasks3_test.c
#        tests/ppos_tasks_tests/ppos_tasks1_test.c
#        tests/debug_test.c
#        tests/dispatcher_tests/dispatcher_test.c
#        tests/scheduler_tests/scheduler_test.c
#        tests/preemption_tests/preemption_test.c
#        tests/time_accounting_tests/accounting_without_prio_test.c
#        tests/suspended_tasks_tests/task_wait_test.c
#        tests/time_accounting_tests/accounting_with_prio_test.c
)
