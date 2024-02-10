cmake_minimum_required(VERSION 3.27)
project(FastOptionPricing LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest REQUIRED)
enable_testing()

find_package(benchmark REQUIRED)

add_subdirectory(fast_option_pricer)
add_subdirectory(tests)
add_subdirectory(examples)