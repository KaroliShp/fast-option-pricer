//
// Created by Karolis Spukas on 8/2/2024.
//

#include "fast_black_scholes.h"
#include <gtest/gtest.h>
#include <hwy/highway.h>
#include <iostream>
#include <vector>

namespace fast_option_pricer {

namespace hn = hwy::HWY_NAMESPACE;

class FastBlackScholesTest : public ::testing::Test
{
   public:
    using T = double;
};

TEST_F(FastBlackScholesTest, Price)
{
    std::vector<T> input{0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0};
    OptionPricing<T> op{
        .underlyings = input,
        .strikes = input,
        .risk_free_rates = input,
        .volatilities = input,
        .times_to_expiry = input,
        .dividend_yields = input,
        .price = std::vector<T>(input.size(), 0),
        .delta = std::vector<T>(input.size(), 0)};

    FastBlackScholes<T, hn::ScalableTag<T>>::price<true>(op);
    EXPECT_TRUE(true);
}

}  // namespace fast_option_pricer