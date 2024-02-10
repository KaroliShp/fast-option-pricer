//
// Created by Karolis Spukas on 10/2/2024.
//

#pragma once

#include <vector>

namespace fast_option_pricer {

template <typename T>
struct OptionPricing
{
    OptionPricing() = default;

    OptionPricing(
        std::vector<T> underlyings, std::vector<T> strikes,
        std::vector<T> risk_free_rates, std::vector<T> volatilities,
        std::vector<T> times_to_expiry, std::vector<T> dividend_yields)
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

    size_t num_options;
    std::vector<T> underlyings;
    std::vector<T> strikes;
    std::vector<T> risk_free_rates;
    std::vector<T> volatilities;
    std::vector<T> times_to_expiry;
    std::vector<T> dividend_yields;
    std::vector<T> prices;
    std::vector<T> deltas;
};

}  // namespace fast_option_pricer
