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
                d, hn::Mul(
                       hn::Set(d, static_cast<T>(-1.0)),
                       hn::Mul(time_to_expiry, dividend_yield)));
            const VecT e_rt = hn::Exp(
                d, hn::Mul(
                       hn::Set(d, static_cast<T>(-1.0)),
                       hn::Mul(time_to_expiry, risk_free_rate)));

            const VecT d1 = calc_d1<d>(
                underlying, strike, risk_free_rate, time_to_expiry,
                sigma_root_t);
            const VecT n_d1 =
                FastMathHelper::normal_cdf<VecT, T, lanes, D, d>(d1);

            const VecT d2 = calc_d2(d1, sigma_root_t);
            const VecT n_d2 =
                FastMathHelper::normal_cdf<VecT, T, lanes, D, d>(d2);

            // Actual price, greeks etc
            if constexpr (Call) {
                hn::Store(
                    calc_call_price(underlying, e_qt, n_d1, strike, e_rt, n_d2),
                    d, op.prices.data() + i);
                hn::Store(calc_call_delta(e_qt, n_d1), d, op.deltas.data() + i);
                hn::Store(
                    calc_call_rho<d>(strike, time_to_expiry, e_rt, n_d2), d,
                    op.rhos.data() + i);
            } else {
                const VecT n_minus_d1 = hn::Mul(
                    hn::Set(d, static_cast<T>(-1.0)),
                    hn::Sub(n_d1, hn::Set(d, static_cast<T>(1.0))));
                const VecT n_minus_d2 = hn::Mul(
                    hn::Set(d, static_cast<T>(-1.0)),
                    hn::Sub(n_d2, hn::Set(d, static_cast<T>(1.0))));

                hn::Store(
                    calc_put_price(
                        underlying, e_qt, n_minus_d1, strike, e_rt, n_minus_d2),
                    d, op.prices.data() + i);
                hn::Store(
                    calc_put_delta<d>(e_qt, n_minus_d1), d,
                    op.deltas.data() + i);
                hn::Store(
                    calc_put_rho<d>(strike, time_to_expiry, e_rt, n_minus_d2),
                    d, op.rhos.data() + i);
            }

            const VecT pdf_d1 = FastMathHelper::normal_pdf<VecT, T, D, d>(d1);
            hn::Store(
                calc_gamma(e_qt, strike, sigma_root_t, pdf_d1), d,
                op.gammas.data() + i);
            hn::Store(
                calc_vega<d>(underlying, e_qt, time_to_expiry, pdf_d1), d,
                op.vegas.data() + i);
        }
    }

    template <D d>
    [[nodiscard]] static inline VecT calc_d1(
        const VecT& underlying, const VecT& strike, const VecT& risk_free_rate,
        const VecT& time_to_expiry, const VecT& sigma_root_t)
    {
        return hn::Add(
            hn::Div(
                hn::Add(
                    hn::Log(d, hn::Div(underlying, strike)),
                    hn::Mul(risk_free_rate, time_to_expiry)),
                sigma_root_t),
            hn::Mul(hn::Set(d, static_cast<T>(0.5)), sigma_root_t));
    }

    [[nodiscard]] static inline VecT calc_d2(
        const VecT& d1, const VecT& sigma_root_t)
    {
        return hn::Sub(d1, sigma_root_t);
    }

    [[nodiscard]] static inline VecT calc_call_price(
        const VecT& underlying, const VecT& e_qt, const VecT& n_d1,
        const VecT& strike, const VecT& e_rt, const VecT& n_d2)
    {
        return hn::Sub(
            hn::Mul(hn::Mul(underlying, e_qt), n_d1),
            hn::Mul(hn::Mul(strike, e_rt), n_d2));
    }

    [[nodiscard]] static inline VecT calc_put_price(
        const VecT& underlying, const VecT& e_qt, const VecT& n_minus_d1,
        const VecT& strike, const VecT& e_rt, const VecT& n_minus_d2)
    {
        return hn::Sub(
            hn::Mul(hn::Mul(strike, e_rt), n_minus_d2),
            hn::Mul(hn::Mul(underlying, e_qt), n_minus_d1));
    }

    [[nodiscard]] static inline VecT calc_call_delta(
        const VecT& e_qt, const VecT& n_d1)
    {
        return hn::Mul(e_qt, n_d1);
    }

    template <D d>
    [[nodiscard]] static inline VecT calc_put_delta(
        const VecT& e_qt, const VecT& n_minus_d1)
    {
        return hn::Mul(
            hn::Set(d, static_cast<T>(-1.0)), hn::Mul(e_qt, n_minus_d1));
    }

    [[nodiscard]] static inline VecT calc_gamma(
        VecT e_qt, VecT underlying, VecT sigma_root_t, VecT pdf_d1)
    {
        return hn::Mul(
            hn::Div(e_qt, hn::Mul(underlying, sigma_root_t)), pdf_d1);
    }

    template <D d>
    [[nodiscard]] static inline VecT calc_vega(
        VecT underlying, VecT e_qt, VecT time_to_expiry, VecT pdf_d1)
    {
        return hn::Mul(
            hn::Set(d, C),
            hn::Mul(
                underlying,
                hn::Mul(e_qt, hn::Mul(hn::Sqrt(time_to_expiry), pdf_d1))));
    }

    template <D d>
    [[nodiscard]] static inline VecT calc_call_rho(
        VecT strike, VecT time_to_expiry, VecT e_rt, VecT n_d2)
    {
        return hn::Mul(
            hn::Set(d, C),
            hn::Mul(strike, hn::Mul(time_to_expiry, hn::Mul(e_rt, n_d2))));
    }

    template <D d>
    [[nodiscard]] static inline VecT calc_put_rho(
        VecT strike, VecT time_to_expiry, VecT e_rt, VecT n_minus_d2)
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