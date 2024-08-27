cmake_minimum_required(VERSION 3.28)
project(PingPongOs C)

set(CMAKE_C_STANDARD 11)

add_executable(PingPongOs main.c
        queue.h
        queue.c
        tests/context.c
        ppos_data.h
)
