cmake_minimum_required(VERSION 3.28)
project(PingPongOs C)

set(CMAKE_C_STANDARD 11)

add_executable(PingPongOs src/main.c
        include/queue.h
        src/queue.c
        include/ppos_data.h
        include/ppos.h
        src/ppos_core.c

        # tests:
        tests/ppos_tasks_tests/ppos_tasks1_test.c
#        tests/ppos_tasks_tests/ppos_tasks2_test.c
#        tests/ppos_tasks_tests/ppos_tasks3_test.c
)
