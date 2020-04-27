cmake_minimum_required(VERSION 3.15)
project(posix C)

set(CMAKE_C_STANDARD 11)

add_executable(server_posix server.c)
add_executable(client_posix client.c)