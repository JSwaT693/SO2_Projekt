cmake_minimum_required(VERSION 3.27)
project(Projekt)

set(CMAKE_CXX_STANDARD 17)

add_executable(Projekt main.cpp
        Philosopher.cpp
        Philosopher.h
        States.h
)
