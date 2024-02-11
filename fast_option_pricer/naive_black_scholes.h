//
// Created by Karolis Spukas on 8/2/2024.
//

#pragma

#include <cmath>
#include "common.h"
#include "naive_math_helper.h"

namespace fast_option_pricer {

template <IsFloatOrDouble T = double>
class NaiveBlackScholes
{
   public:
    template <bool Call = true>
    static void price(OptionPricing<T>& op)
    {
        for (size_t i = 0; i < op.num_options; ++i) {
            // Calculate shared constants
            const T sigma_root_t =
                op.volatilities[i] * sqrt(op.times_to_expiry[i]);
            const T e_qt = exp(-op.times_to_expiry[i] * op.dividend_yields[i]);
            const T e_rt = exp(-op.times_to_expiry[i] * op.risk_free_rates[i]);

            const T d1 = calc_d1(
                op.underlyings[i], op.strikes[i], op.risk_free_rates[i],
                op.times_to_expiry[i], sigma_root_t);
            const T n_d1 = NaiveMathHelper::normal_cdf(d1);
            const T n_minus_d1 = NaiveMathHelper::normal_cdf(-1 * d1);

            const T d2 = calc_d2(d1, sigma_root_t);
            const T n_d2 = NaiveMathHelper::normal_cdf(d2);

            // Actual price, greeks etc
            if constexpr (Call) {
                op.prices[i] = calc_call_price(
                    op.underlyings[i], e_qt, n_d1, op.strikes[i], e_rt, n_d2);
                op.deltas[i] = calc_call_delta(e_qt, n_d1);
            } else {
                const auto n_minus_d2 = NaiveMathHelper::normal_cdf(-1 * d2);
                op.prices[i] = calc_put_price(
                    op.underlyings[i], e_qt, n_minus_d1, op.strikes[i], e_rt,
                    n_minus_d2);
                op.deltas[i] = calc_put_delta(e_qt, n_minus_d1);
            }
        }
    }

    [[nodiscard]] static inline auto calc_d1(
        auto underlying, auto strike, auto risk_free_rate, auto time_to_expiry,
        auto sigma_root_t)
    {
        return ((std::log(underlying / strike) +
                 risk_free_rate * time_to_expiry) /
                sigma_root_t) +
               0.5 * sigma_root_t;
    }

    [[nodiscard]] static inline auto calc_d2(auto d1, auto sigma_root_t)
    {
        return d1 - sigma_root_t;
    }

    [[nodiscard]] static inline auto calc_call_price(
        auto underlying, auto e_qt, auto n_d1, auto strike, auto e_rt,
        auto n_d2)
    {
        return (n_d1 * underlying * e_qt) - (n_d2 * strike * e_rt);
    }

    [[nodiscard]] static inline auto calc_put_price(
        auto underlying, auto e_qt, auto n_minus_d1, auto strike, auto e_rt,
        auto n_minus_d2)
    {
        return (n_minus_d2 * strike * e_rt) - (n_minus_d1 * underlying * e_qt);
    }

    [[nodiscard]] static inline auto calc_call_delta(auto e_qt, auto n_d1)
    {
        return e_qt * n_d1;
    }

    [[nodiscard]] static inline auto calc_put_delta(auto e_qt, auto n_minus_d1)
    {
        return -1 * e_qt * n_minus_d1;
    }
};

}  // namespace fast_option_pricer
