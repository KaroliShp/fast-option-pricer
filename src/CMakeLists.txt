add_library(FastOptionPricingLib
        naive_black_scholes.cpp
        naive_math_helper.cpp
)

add_executable(FastOptionPricing ${FastOptionPricing_SOURCE_DIR}/main.cpp)

target_include_directories(FastOptionPricingLib PUBLIC .)
target_include_directories(FastOptionPricing PUBLIC .)

target_link_libraries(FastOptionPricing PUBLIC FastOptionPricingLib)