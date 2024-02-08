//
// Created by Karolis Spukas on 8/2/2024.
//

#pragma

#include <cmath>
#include "naive_math_helper.h"

namespace fast_pricing {

struct NaiveBlackScholes
{
    [[nodiscard]] static inline double call_price(
        double underlying, double strike, double risk_free_rate,
        double volatility, double time_to_expiry);

    [[nodiscard]] static inline double put_price(
        double underlying, double strike, double risk_free_rate,
        double volatility, double time_to_expiry);

    [[nodiscard]] static inline double call_delta(
        double underlying, double strike, double risk_free_rate,
        double volatility, double time_to_expiry);

    [[nodiscard]] static inline double put_delta(
        double underlying, double strike, double risk_free_rate,
        double volatility, double time_to_expiry);
};

}  // namespace fast_pricing
