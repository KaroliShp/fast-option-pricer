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
};

}  // namespace fast_option_pricer
