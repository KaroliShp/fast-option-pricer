//
// Created by Karolis Spukas on 8/2/2024.
//

#include "fast_math_helper.h"
#include <gtest/gtest.h>

namespace fast_option_pricer {

class FastMathHelperTest : public ::testing::Test
{
};

TEST_F(FastMathHelperTest, NormalCdf)
{
    auto res = fast_option_pricer::FastMathHelper::normal_cdf(0);
    EXPECT_DOUBLE_EQ(0.5, res);
    EXPECT_TRUE(true);
}

}