cmake_minimum_required(VERSION 3.15)
project(posix C)

set(CMAKE_C_STANDARD 11)

add_executable(launcher_posix main.c common.h)
add_executable(workerA_posix workerA.c common.h)
add_executable(workerB_posix workerB.c common.h)
add_executable(workerC_posix workerC.c common.h)