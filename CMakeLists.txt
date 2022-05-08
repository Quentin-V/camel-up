cmake_minimum_required(VERSION 3.22)
project(CamelUp C)

set(CMAKE_C_STANDARD 99)

add_executable(CamelUp main.c CamelUpTypesAndUtils.h)
