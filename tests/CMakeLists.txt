add_executable(FastOptionPricingTest
        naive_math_helper_test.cpp)

target_link_libraries(FastOptionPricingTest PRIVATE GTest::gtest_main FastOptionPricingLib)

include(GoogleTest)
gtest_discover_tests(FastOptionPricingTest)