//
// Created by Karolis Spukas on 8/2/2024.
//

#include "naive_math_helper.h"
#include <gtest/gtest.h>

TEST(NaiveMathHelperTest, Testing)
{
    auto res = fast_pricing::NaiveMathHelper::normal_cdf(0);
    EXPECT_DOUBLE_EQ(0.5, res);
    EXPECT_TRUE(true);
}