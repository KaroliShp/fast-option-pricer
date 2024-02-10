//
// Created by Karolis Spukas on 8/2/2024.
//

#include <gtest/gtest.h>
#include <hwy/highway.h>
#include <iostream>
#include <vector>
#include "fast_black_scholes.h"
#include "naive_black_scholes.h"

namespace fast_option_pricer {

namespace hn = hwy::HWY_NAMESPACE;

class BlackScholesTest : public ::testing::Test
{
   public:
    using T = double;
};

TEST_F(BlackScholesTest, ComparePrice)
{
    // Assign
    std::vector<T> spot{110.0, 110.0, 110.0, 110.0};
    std::vector<T> strike{120.0, 120.0, 120.0, 120.0};
    std::vector<T> years_to_expiry(spot.size(), 25.0 / 252.0);
    std::vector<T> risk_free_rate(spot.size(), 0.02);
    std::vector<T> volatility{0.15, 0.16, 0.17, 0.18};
    std::vector<T> dividend_yield{0.05, 0.05, 0.05, 0.05};

    // Act
    OptionPricing<T> fast_op(
        spot, strike, risk_free_rate, volatility, years_to_expiry,
        dividend_yield);
    FastBlackScholes<T, hn::ScalableTag<T>>::price<true>(fast_op);

    OptionPricing<T> naive_op(
        spot, strike, risk_free_rate, volatility, years_to_expiry,
        dividend_yield);
    NaiveBlackScholes::price<true>(naive_op);

    // Assert
    EXPECT_EQ(fast_op.prices.size(), naive_op.prices.size());
    EXPECT_EQ(fast_op.deltas.size(), naive_op.deltas.size());
    for (auto i = 0; i < fast_op.prices.size(); ++i) {
        std::cout << "Elem: " << i << std::endl;
        EXPECT_NEAR(fast_op.prices[i], naive_op.prices[i], 1e-5);
        EXPECT_NEAR(fast_op.deltas[i], naive_op.deltas[i], 1e-5);
    }
}

}  // namespace fast_option_pricer