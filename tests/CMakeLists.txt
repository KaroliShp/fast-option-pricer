add_executable(FastOptionPricingTest
        naive_black_scholes_test.cpp)

target_link_libraries(FastOptionPricingTest PRIVATE GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(FastOptionPricingTest)