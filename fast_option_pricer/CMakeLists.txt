add_library(FastOptionPricingLib
        naive_black_scholes.cpp
        naive_math_helper.cpp
)

target_include_directories(FastOptionPricingLib PUBLIC .)