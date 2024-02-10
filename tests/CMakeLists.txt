add_executable(FastOptionPricingTest
        naive_math_helper_test.cpp
        fast_math_helper_test.cpp
        black_scholes_test.cpp)

target_link_libraries(FastOptionPricingTest PRIVATE GTest::gtest_main FastOptionPricingLib benchmark::benchmark)

include(GoogleTest)
gtest_discover_tests(FastOptionPricingTest)