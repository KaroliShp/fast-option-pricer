#include <iostream>
#include "src/naive_black_scholes.h"

int main() {
    auto res = fast_pricing::NaiveMathHelper::normal_cdf(0);
    std::cout << "Hello, World!" << std::endl;
    std::cout << res << std::endl;
    return 0;
}
