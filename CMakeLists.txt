cmake_minimum_required(VERSION 3.27)
project(FastOptionPricing LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest REQUIRED)

add_executable(FastOptionPricing main.cpp
        src/naive_black_scholes.cpp
        src/naive_black_scholes.h
        src/naive_math_helper.cpp
        src/naive_math_helper.h
)
