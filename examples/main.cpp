#include <iostream>
#include "naive_black_scholes.h"

int main() {
    auto res = fast_option_pricer::NaiveMathHelper::normal_cdf(0);
    std::cout << "Hello, World!" << std::endl;
    std::cout << res << std::endl;
    return 0;
}
