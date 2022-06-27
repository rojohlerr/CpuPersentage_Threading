cmake_minimum_required(VERSION 3.0)
project(CPU_USAGE
        VERSION 1.0
        DESCRIPTION "Multithrading application for CPU usage in Linux"
        LANGUAGES C)

set(CMAKE_C_FLAGS "-pthread")
add_executable(${CMAKE_PROJECT_NAME} Main.c)