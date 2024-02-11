//
// Created by Karolis Spukas on 10/2/2024.
//

#pragma once

#include <vector>

namespace fast_option_pricer {

template <typename T>
struct OptionPricing
{
    OptionPricing(
        const std::vector<T>& underlyings, const std::vector<T>& strikes,
        const std::vector<T>& risk_free_rates,
        const std::vector<T>& volatilities,
        const std::vector<T>& times_to_expiry,
        const std::vector<T>& dividend_yields)
        : num_options(underlyings.size()),
          underlyings(underlyings),
          strikes(strikes),
          risk_free_rates(risk_free_rates),
          volatilities(volatilities),
          times_to_expiry(times_to_expiry),
          dividend_yields(dividend_yields),
          prices(std::vector<T>(underlyings.size(), 0)),
          deltas(std::vector<T>(underlyings.size(), 0))
    {
        assert(num_options == strikes.size());
        assert(num_options == risk_free_rates.size());
        assert(num_options == volatilities.size());
        assert(num_options == times_to_expiry.size());
        assert(num_options == dividend_yields.size());
    }

    const size_t num_options;
    const std::vector<T> underlyings;
    const std::vector<T> strikes;
    const std::vector<T> risk_free_rates;
    const std::vector<T> volatilities;
    const std::vector<T> times_to_expiry;
    const std::vector<T> dividend_yields;
    std::vector<T> prices;
    std::vector<T> deltas;
};

template <typename T>
concept IsFloatOrDouble =
    std::is_same<T, float>::value || std::is_same<T, double>::value;

}  // namespace fast_option_pricer
