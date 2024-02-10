//
// Created by Karolis Spukas on 10/2/2024.
//

#pragma once

#include <hwy/highway.h>
namespace hn = hwy::HWY_NAMESPACE;

namespace fast_option_pricer {

struct FastMathHelper
{
    [[nodiscard]] static inline double normal_cdf(double x);

    [[nodiscard]] static inline double d1(
        double sigma_root_t, double underlying, double strike,
        double risk_free_rate, double time_to_expiry);

    [[nodiscard]] static inline double d2(double d1, double sigma_root_t);
};

}  // namespace fast_option_pricer