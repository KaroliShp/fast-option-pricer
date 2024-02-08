cmake_minimum_required(VERSION 3.27)
project(FastOptionPricing LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(FastOptionPricing main.cpp
        src/NaiveBlackScholes.cpp
        src/NaiveBlackScholes.h
        src/NaiveMathHelper.cpp
        src/NaiveMathHelper.h
)
