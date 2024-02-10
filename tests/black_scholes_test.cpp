//
// Created by Karolis Spukas on 8/2/2024.
//

#include <benchmark/benchmark.h>
#include <gtest/gtest.h>
#include <hwy/highway.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#include "common.h"
#include "fast_black_scholes.h"
#include "naive_black_scholes.h"

namespace fast_option_pricer {

namespace hn = hwy::HWY_NAMESPACE;

template <typename T>
struct RandomInput
{
    RandomInput()
        : underlyings(num_options, 0),
          strikes(num_options, 0),
          risk_free_rates(num_options, 0),
          volatilities(num_options, 0),
          times_to_expiry(num_options, 0),
          dividend_yields(num_options, 0)
    {
        for (size_t i = 0; i < num_options; ++i) {
            underlyings[i] = rng(500.0);
            strikes[i] = rng(500.0);
            risk_free_rates[i] = rng(0.10);
            volatilities[i] = rng(0.4);
            times_to_expiry[i] = rng(251.0) / 252.0;
            dividend_yields[i] = rng(0.10);
        }
    }

    [[nodiscard]] T rng(float HI)
    {
        return static_cast<T>(std::rand()) / (static_cast<T>(RAND_MAX / HI));
    }

    size_t num_options{10000000};
    std::vector<T> underlyings;
    std::vector<T> strikes;
    std::vector<T> risk_free_rates;
    std::vector<T> volatilities;
    std::vector<T> times_to_expiry;
    std::vector<T> dividend_yields;
};

class BlackScholesTest : public ::testing::Test
{
   public:
    using T = double;

    RandomInput<T> r;
};

TEST_F(BlackScholesTest, ComparePrice)
{
    // Assign
    /*std::vector<T> spot{110.0, 110.0, 110.0, 110.0};
    std::vector<T> strike{120.0, 120.0, 120.0, 120.0};
    std::vector<T> years_to_expiry(spot.size(), 25.0 / 252.0);
    std::vector<T> risk_free_rate(spot.size(), 0.02);
    std::vector<T> volatility{0.15, 0.16, 0.17, 0.18};
    std::vector<T> dividend_yield{0.05, 0.05, 0.05, 0.05};*/

    // Act
    OptionPricing<T> fast_op_call(
        r.underlyings, r.strikes, r.risk_free_rates, r.volatilities,
        r.times_to_expiry, r.dividend_yields);
    OptionPricing<T> fast_op_put(
        r.underlyings, r.strikes, r.risk_free_rates, r.volatilities,
        r.times_to_expiry, r.dividend_yields);
    OptionPricing<T> naive_op_call(
        r.underlyings, r.strikes, r.risk_free_rates, r.volatilities,
        r.times_to_expiry, r.dividend_yields);
    OptionPricing<T> naive_op_put(
        r.underlyings, r.strikes, r.risk_free_rates, r.volatilities,
        r.times_to_expiry, r.dividend_yields);

    // Assert
    FastBlackScholes<T, hn::ScalableTag<T>>::price<true>(fast_op_call);
    NaiveBlackScholes::price<true>(naive_op_call);
    EXPECT_EQ(fast_op_call.prices.size(), naive_op_call.prices.size());
    EXPECT_EQ(fast_op_call.deltas.size(), naive_op_call.deltas.size());
    for (auto i = 0; i < fast_op_call.prices.size(); ++i) {
        // std::cout << "Elem: " << i << ", " << fast_op_call.prices[i] << ", "
        // << fast_op_call.deltas[i] << std::endl;
        EXPECT_NEAR(fast_op_call.prices[i], naive_op_call.prices[i], 1e-5);
        EXPECT_NEAR(fast_op_call.deltas[i], naive_op_call.deltas[i], 1e-5);
    }

    FastBlackScholes<T, hn::ScalableTag<T>>::price<false>(fast_op_put);
    NaiveBlackScholes::price<false>(naive_op_put);
    EXPECT_EQ(fast_op_put.prices.size(), naive_op_put.prices.size());
    EXPECT_EQ(fast_op_put.deltas.size(), naive_op_put.deltas.size());
    for (auto i = 0; i < fast_op_put.prices.size(); ++i) {
        // std::cout << "Elem: " << i << std::endl;
        EXPECT_NEAR(fast_op_put.prices[i], naive_op_put.prices[i], 1e-5);
        EXPECT_NEAR(fast_op_put.deltas[i], naive_op_put.deltas[i], 1e-5);
    }
}

static void BM_NaivePrice(benchmark::State& state)
{
    // Perform setup here
    using T = double;
    RandomInput<T> r;
    OptionPricing<T> naive_op(
        r.underlyings, r.strikes, r.risk_free_rates, r.volatilities,
        r.times_to_expiry, r.dividend_yields);

    for (auto _ : state) {
        // This code gets timed
        NaiveBlackScholes::price<true>(naive_op);
        NaiveBlackScholes::price<false>(naive_op);
    }
}

BENCHMARK(BM_NaivePrice);

static void BM_FastPrice(benchmark::State& state)
{
    // Perform setup here
    using T = double;
    RandomInput<T> r;
    OptionPricing<T> fast_op(
        r.underlyings, r.strikes, r.risk_free_rates, r.volatilities,
        r.times_to_expiry, r.dividend_yields);

    for (auto _ : state) {
        // This code gets timed
        FastBlackScholes<T, hn::ScalableTag<T>>::price<true>(fast_op);
        FastBlackScholes<T, hn::ScalableTag<T>>::price<false>(fast_op);
    }
}

BENCHMARK(BM_FastPrice);

TEST_F(BlackScholesTest, Benchmarks)
{
    ::benchmark::RunSpecifiedBenchmarks();
}

}  // namespace fast_option_pricer