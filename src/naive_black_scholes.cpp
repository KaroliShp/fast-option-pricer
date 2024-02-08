//
// Created by Karolis Spukas on 8/2/2024.
//

#include "naive_black_scholes.h"

namespace fast_pricing {

inline double naive_black_scholes::call_price(
    double underlying, double strike, double risk_free_rate, double volatility,
    double time_to_expiry)
{
    double sigma_root_t = volatility * std::sqrt(time_to_expiry);
    double d1 = naive_math_helper::d1(
        sigma_root_t, underlying, strike, risk_free_rate, time_to_expiry);
    double d2 = naive_math_helper::d2(d1, sigma_root_t);

    return (naive_math_helper::normal_cdf(d1) * underlying) -
           (naive_math_helper::normal_cdf(d2) * strike *
            std::exp(-risk_free_rate * time_to_expiry));
}

inline double naive_black_scholes::put_price(
    double underlying, double strike, double risk_free_rate, double volatility,
    double time_to_expiry)
{
    double sigma_root_t = volatility * std::sqrt(time_to_expiry);
    double d1 = naive_math_helper::d1(
        sigma_root_t, underlying, strike, risk_free_rate, time_to_expiry);
    double d2 = naive_math_helper::d2(d1, sigma_root_t);

    return (naive_math_helper::normal_cdf(-d2) * strike *
            std::exp(-risk_free_rate * time_to_expiry)) -
           (naive_math_helper::normal_cdf(-d1) * underlying);
}

inline double naive_black_scholes::call_delta(
    double underlying, double strike, double risk_free_rate, double volatility,
    double time_to_expiry)
{
    double sigma_root_t = volatility * std::sqrt(time_to_expiry);
    return naive_math_helper::d1(
        sigma_root_t, underlying, strike, risk_free_rate, time_to_expiry);
}

inline double naive_black_scholes::put_delta(
    double underlying, double strike, double risk_free_rate, double volatility,
    double time_to_expiry)
{
    double sigma_root_t = volatility * std::sqrt(time_to_expiry);
    return 1.0 - naive_math_helper::d1(
                     sigma_root_t, underlying, strike, risk_free_rate,
                     time_to_expiry);
}

}  // namespace fast_pricing