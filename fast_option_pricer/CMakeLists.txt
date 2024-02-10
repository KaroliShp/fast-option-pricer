add_library(FastOptionPricingLib
        naive_black_scholes.cpp
        naive_math_helper.cpp
        fast_math_helper.cpp
        fast_black_scholes.h
        math-inl.h
)

target_include_directories(FastOptionPricingLib PUBLIC .)

#set(CMAKE_TOOLCHAIN_FILE /Users/karolis/.vcpkg-clion/vcpkg/scripts/buildsystems/vcpkg.cmake)
find_package(hwy CONFIG REQUIRED)

target_link_libraries(FastOptionPricingLib PUBLIC hwy::hwy)