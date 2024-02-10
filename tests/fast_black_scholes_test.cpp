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
    OptionPricing<T> op(input, input, input, input, input, input);

    FastBlackScholes<T, hn::ScalableTag<T>>::price<true>(op);
    EXPECT_TRUE(true);
}

}  // namespace fast_option_pricer