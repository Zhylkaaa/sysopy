cmake_minimum_required(VERSION 3.15)
project(systemV C)

set(CMAKE_C_STANDARD 11)

add_executable(launcher main.c common.h)
add_executable(workerA workerA.c common.h)
add_executable(workerB workerB.c common.h)
add_executable(workerC workerC.c common.h)