//
// Created by Karolis Spukas on 8/2/2024.
//

#pragma

#include <cmath>
#include <random>
#include "hwy/highway.h"

namespace fast_option_pricer {

struct NaiveMathHelper
{
    [[nodiscard]] static inline auto normal_cdf(auto x)
    {
        return 0.5 * std::erfc(-x / std::sqrt(2));
    }

    [[nodiscard]] static inline auto normal_pdf(auto x)
    {
        static constexpr decltype(x) inv_sqrt_2pi = 0.3989422804014327;
        return inv_sqrt_2pi * std::exp(static_cast<decltype(x)>(-0.5) * x * x);
    }
};

}  // namespace fast_option_pricer
