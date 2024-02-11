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
    template <typename T>
    [[nodiscard]] static inline T normal_cdf(T x)
    {
        constexpr T sqrt_2 = 1.41421356237;
        return static_cast<T>(0.5) * std::erfc(-x / sqrt_2);
    }

    template <typename T>
    [[nodiscard]] static inline T normal_pdf(T x)
    {
        static constexpr T inv_sqrt_2pi = 0.3989422804014327;
        return inv_sqrt_2pi * std::exp(static_cast<T>(-0.5) * x * x);
    }
};

}  // namespace fast_option_pricer
