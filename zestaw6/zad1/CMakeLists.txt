cmake_minimum_required(VERSION 3.15)
project(systemV C)

set(CMAKE_C_STANDARD 11)

add_executable(server_systemv server.c)
add_executable(client_systemv client.c)
