//
// Created by Karolis Spukas on 8/2/2024.
//

#pragma

#include <cmath>
#include "hwy/highway.h"

namespace fast_option_pricer {

struct NaiveMathHelper
{
    [[nodiscard]] static inline double normal_cdf(double x)
    {
        return 0.5 * std::erfc(-x / std::sqrt(2));
    }

    [[nodiscard]] static inline double d1(
        double sigma_root_t, double underlying, double strike,
        double risk_free_rate, double time_to_expiry)
    {
        return ((std::log(underlying / strike) +
                 risk_free_rate * time_to_expiry) /
                sigma_root_t) +
               0.5 * sigma_root_t;
    }

    [[nodiscard]] static inline double d2(double d1, double sigma_root_t)
    {
        return d1 - sigma_root_t;
    }
};

}  // namespace fast_option_pricer
