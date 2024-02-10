//
// Created by Karolis Spukas on 8/2/2024.
//

#include "naive_black_scholes.h"
#include <gtest/gtest.h>
#include <hwy/highway.h>
#include <iostream>
#include <vector>

namespace fast_option_pricer {

namespace hn = hwy::HWY_NAMESPACE;

class NaiveBlackScholesTest : public ::testing::Test
{
};

TEST_F(NaiveBlackScholesTest, Price)
{
    std::vector<double> input{0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0};
    OptionPricing<double> op(input, input, input, input, input, input);

    NaiveBlackScholes::price<true>(op);
    EXPECT_TRUE(true);
}

}  // namespace fast_option_pricer