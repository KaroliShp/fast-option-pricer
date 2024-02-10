//
// Created by Karolis Spukas on 8/2/2024.
//

#include "fast_math_helper.h"
#include <gtest/gtest.h>
#include <hwy/highway.h>
#include <iostream>
#include <vector>

namespace hn = hwy::HWY_NAMESPACE;

namespace fast_option_pricer {

class FastMathHelperTest : public ::testing::Test
{
   public:
    explicit FastMathHelperTest()
    {
    }

    FastMathHelper fast_math_helper;
};

TEST_F(FastMathHelperTest, NormalCdf)
{
    const hn::ScalableTag<float> d;
    std::cout << "Lanes: " << hn::Lanes(d) << std::endl;
    std::vector<float> inputs{0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0};
    std::vector<float> output{1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
    for (size_t i = 0; i < inputs.size(); i += hn::Lanes(d)) {
        std::cout << i << std::endl;
        hn::Vec128<float> mul = hn::Load(d, inputs.data() + i);
        mul = hn::MulAdd(mul, mul, mul);
        hn::Store(mul, d, output.data() + i);
    }
    for (int i = 0; i < output.size(); ++i) {
        std::cout << i << ": " << output[i] << std::endl;
    }
    std::cout << "Lanes: " << hn::Lanes(d) << std::endl;
    auto res = fast_math_helper.normal_cdf(0);
    EXPECT_DOUBLE_EQ(0.5, res);
    EXPECT_TRUE(true);
}

}  // namespace fast_option_pricer