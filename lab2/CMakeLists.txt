cmake_minimum_required(VERSION 4.0)
project(lab2)

set(CMAKE_CXX_STANDARD 20)

add_executable(lab2
        ../lab2_extra/matrix_processes_info.cpp
        ../lab2_extra/matrix_threads_info.cpp)
