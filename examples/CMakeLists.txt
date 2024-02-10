add_executable(FastOptionPricing main.cpp)

target_include_directories(FastOptionPricing PUBLIC .)

target_link_libraries(FastOptionPricing PUBLIC FastOptionPricingLib)

