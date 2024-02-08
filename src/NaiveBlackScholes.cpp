//
// Created by Karolis Spukas on 8/2/2024.
//

#include "NaiveBlackScholes.h"

namespace fast_pricing {

inline double NaiveBlackScholes::call_price(
    double underlying, double strike, double risk_free_rate, double volatility,
    double time_to_expiry)
{
    double sigma_root_t = volatility * std::sqrt(time_to_expiry);
    double d1 = NaiveMathHelper::d1(
        sigma_root_t, underlying, strike, risk_free_rate, time_to_expiry);
    double d2 = NaiveMathHelper::d2(d1, sigma_root_t);

    return (NaiveMathHelper::normal_cdf(d1) * underlying) -
           (NaiveMathHelper::normal_cdf(d2) * strike *
            std::exp(-risk_free_rate * time_to_expiry));
}

inline double NaiveBlackScholes::put_price(
    double underlying, double strike, double risk_free_rate, double volatility,
    double time_to_expiry)
{
    double sigma_root_t = volatility * std::sqrt(time_to_expiry);
    double d1 = NaiveMathHelper::d1(
        sigma_root_t, underlying, strike, risk_free_rate, time_to_expiry);
    double d2 = NaiveMathHelper::d2(d1, sigma_root_t);

    return (NaiveMathHelper::normal_cdf(-d2) * strike *
            std::exp(-risk_free_rate * time_to_expiry)) -
           (NaiveMathHelper::normal_cdf(-d1) * underlying);
}

inline double NaiveBlackScholes::call_delta(
    double underlying, double strike, double risk_free_rate, double volatility,
    double time_to_expiry)
{
    double sigma_root_t = volatility * std::sqrt(time_to_expiry);
    return NaiveMathHelper::d1(
        sigma_root_t, underlying, strike, risk_free_rate, time_to_expiry);
}

inline double NaiveBlackScholes::put_delta(
    double underlying, double strike, double risk_free_rate, double volatility,
    double time_to_expiry)
{
    double sigma_root_t = volatility * std::sqrt(time_to_expiry);
    return 1.0 - NaiveMathHelper::d1(
                     sigma_root_t, underlying, strike, risk_free_rate,
                     time_to_expiry);
}

}  // namespace fast_pricing