//
// Created by Karolis Spukas on 8/2/2024.
//

#include "naive_math_helper.h"
#include <gtest/gtest.h>

namespace fast_option_pricer {

class NaiveMathHelperTest : public ::testing::Test
{
};

TEST_F(NaiveMathHelperTest, NormalCdf)
{
    auto res = NaiveMathHelper::normal_cdf<double>(0.0);
    EXPECT_DOUBLE_EQ(0.5, res);
    EXPECT_TRUE(true);
}

}  // namespace fast_option_pricer