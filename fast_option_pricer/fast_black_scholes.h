//
// Created by Karolis Spukas on 10/2/2024.
//

#pragma once

#include <hwy/highway.h>
#include <iostream>
#include <type_traits>
#include "common.h"
#include "fast_math_helper.h"
#include "math-inl.h"

namespace fast_option_pricer {

namespace hn = hwy::HWY_NAMESPACE;

template <typename T>
concept IsFloatOrDouble =
    std::is_same<T, float>::value || std::is_same<T, double>::value;

template <IsFloatOrDouble T = double, typename D = hn::ScalableTag<T>>
class FastBlackScholes
{
   public:
    using VecT = hn::Vec<D>;

    template <bool Call = true>
    static void price(OptionPricing<T>& op)
    {
        constexpr D d;
        constexpr auto lanes = hn::Lanes(d);
        // std::cout << "Lanes: " << hn::Lanes(d) << std::endl;

        for (size_t i = 0; i < op.num_options; i += lanes) {
            // Load initial option info
            // TODO take care of the last vector in case it's not 0 mod lanes
            const VecT underlying = hn::Load(d, op.underlyings.data() + i);
            const VecT strike = hn::Load(d, op.strikes.data() + i);
            const VecT risk_free_rate =
                hn::Load(d, op.risk_free_rates.data() + i);
            const VecT volatility = hn::Load(d, op.volatilities.data() + i);
            const VecT time_to_expiry =
                hn::Load(d, op.times_to_expiry.data() + i);
            const VecT dividend_yield =
                hn::Load(d, op.dividend_yields.data() + i);

            // Calculate shared constants
            const VecT sigma_root_t =
                hn::Mul(volatility, hn::Sqrt(time_to_expiry));
            const VecT e_qt = hn::Exp(
                d,
                hn::Mul(
                    hn::Set(d, -1), hn::Mul(time_to_expiry, dividend_yield)));
            const VecT e_rt = hn::Exp(
                d,
                hn::Mul(
                    hn::Set(d, -1), hn::Mul(time_to_expiry, risk_free_rate)));

            const VecT d1 = calc_d1<d>(
                underlying, strike, risk_free_rate, time_to_expiry,
                sigma_root_t);
            const VecT n_d1 = FastMathHelper::normal_cdf<T, lanes>(d1, d);
            const VecT n_minus_d1 = FastMathHelper::normal_cdf<T, lanes>(
                hn::Mul(hn::Set(d, -1), d1), d);

            const VecT d2 = calc_d2(d1, sigma_root_t);
            const VecT n_d2 = FastMathHelper::normal_cdf<T, lanes>(d2, d);

            // Actual price, greeks etc
            VecT price;
            VecT delta;
            if constexpr (Call) {
                price =
                    calc_call_price(underlying, e_qt, n_d1, strike, e_rt, n_d2);
                delta = calc_call_delta(e_qt, n_d1);
            } else {
                const VecT n_minus_d2 = FastMathHelper::normal_cdf<T, lanes>(
                    hn::Mul(hn::Set(d, -1), d2), d);
                price = calc_put_price(
                    underlying, e_qt, n_minus_d1, strike, e_rt, n_minus_d2);
                delta = calc_put_delta<d>(e_qt, n_minus_d1);
            }
            hn::Store(price, d, op.prices.data() + i);
            hn::Store(delta, d, op.deltas.data() + i);
        }
    }

    template <D d>
    [[nodiscard]] static inline auto calc_d1(
        const auto& underlying, const auto& strike, const auto& risk_free_rate,
        const auto& time_to_expiry, const auto& sigma_root_t)
    {
        return hn::Add(
            hn::Div(
                hn::Add(
                    hn::Log(d, hn::Div(underlying, strike)),
                    hn::Mul(risk_free_rate, time_to_expiry)),
                sigma_root_t),
            hn::Mul(hn::Set(d, 0.5), sigma_root_t));
    }

    [[nodiscard]] static inline auto calc_d2(
        const auto& d1, const auto& sigma_root_t)
    {
        return hn::Sub(d1, sigma_root_t);
    }

    [[nodiscard]] static inline auto calc_call_price(
        const auto& underlying, const auto& e_qt, const auto& n_d1,
        const auto& strike, const auto& e_rt, const auto& n_d2)
    {
        return hn::Sub(
            hn::Mul(hn::Mul(underlying, e_qt), n_d1),
            hn::Mul(hn::Mul(strike, e_rt), n_d2));
    }

    [[nodiscard]] static inline auto calc_put_price(
        const auto& underlying, const auto& e_qt, const auto& n_minus_d1,
        const auto& strike, const auto& e_rt, const auto& n_minus_d2)
    {
        return hn::Sub(
            hn::Mul(hn::Mul(strike, e_rt), n_minus_d2),
            hn::Mul(hn::Mul(underlying, e_qt), n_minus_d1));
    }

    [[nodiscard]] static inline auto calc_call_delta(
        const auto& e_qt, const auto& n_d1)
    {
        return hn::Mul(e_qt, n_d1);
    }

    template <D d>
    [[nodiscard]] static inline auto calc_put_delta(
        const auto& e_qt, const auto& n_minus_d1)
    {
        return hn::Mul(hn::Set(d, -1), hn::Mul(e_qt, n_minus_d1));
    }
};

}  // namespace fast_option_pricer