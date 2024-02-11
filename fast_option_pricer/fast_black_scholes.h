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
            if constexpr (Call) {
                const VecT price =
                    calc_call_price(underlying, e_qt, n_d1, strike, e_rt, n_d2);
                hn::Store(price, d, op.prices.data() + i);

                const VecT delta = calc_call_delta(e_qt, n_d1);
                hn::Store(delta, d, op.deltas.data() + i);

                const VecT rho =
                    calc_call_rho<d>(strike, time_to_expiry, e_rt, n_d2);
                hn::Store(rho, d, op.rhos.data() + i);
            } else {
                const VecT n_minus_d2 = FastMathHelper::normal_cdf<T, lanes>(
                    hn::Mul(hn::Set(d, -1), d2), d);

                const VecT price = calc_put_price(
                    underlying, e_qt, n_minus_d1, strike, e_rt, n_minus_d2);
                hn::Store(price, d, op.prices.data() + i);

                const VecT delta = calc_put_delta<d>(e_qt, n_minus_d1);
                hn::Store(delta, d, op.deltas.data() + i);

                const VecT rho =
                    calc_put_rho<d>(strike, time_to_expiry, e_rt, n_minus_d2);
                hn::Store(rho, d, op.rhos.data() + i);
            }

            const VecT pdf_d1 = FastMathHelper::normal_pdf<T>(d1, d);
            const VecT gamma = calc_gamma(e_qt, strike, sigma_root_t, pdf_d1);
            hn::Store(gamma, d, op.gammas.data() + i);
            const VecT vega =
                calc_vega<d>(underlying, e_qt, time_to_expiry, pdf_d1);
            hn::Store(vega, d, op.vegas.data() + i);
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

    [[nodiscard]] static inline auto calc_gamma(
        auto e_qt, auto underlying, auto sigma_root_t, auto pdf_d1)
    {
        return hn::Mul(
            hn::Div(e_qt, hn::Mul(underlying, sigma_root_t)), pdf_d1);
    }

    template <D d>
    [[nodiscard]] static inline auto calc_vega(
        auto underlying, auto e_qt, auto time_to_expiry, auto pdf_d1)
    {
        return hn::Mul(
            hn::Set(d, C),
            hn::Mul(
                underlying,
                hn::Mul(e_qt, hn::Mul(hn::Sqrt(time_to_expiry), pdf_d1))));
    }

    template <D d>
    [[nodiscard]] static inline auto calc_call_rho(
        auto strike, auto time_to_expiry, auto e_rt, auto n_d2)
    {
        return hn::Mul(
            hn::Set(d, C),
            hn::Mul(strike, hn::Mul(time_to_expiry, hn::Mul(e_rt, n_d2))));
    }

    template <D d>
    [[nodiscard]] static inline auto calc_put_rho(
        auto strike, auto time_to_expiry, auto e_rt, auto n_minus_d2)
    {
        return hn::Mul(
            hn::Set(d, C_minus),
            hn::Mul(
                strike, hn::Mul(time_to_expiry, hn::Mul(e_rt, n_minus_d2))));
    }

   protected:
    static constexpr T C = 1.0 / 100.0;
    static constexpr T C_minus = -1.0 / 100.0;
};

}  // namespace fast_option_pricer