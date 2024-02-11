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
    explicit RandomInput(unsigned int seed = 1)
        : underlyings(num_options, 0),
          strikes(num_options, 0),
          risk_free_rates(num_options, 0),
          volatilities(num_options, 0),
          times_to_expiry(num_options, 0),
          dividend_yields(num_options, 0)
    {
        std::srand(seed);
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

template <typename T>
static void BM_NaivePrice(benchmark::State& state)
{
    // Perform setup here
    RandomInput<T> r;
    OptionPricing<T> naive_op(
        r.underlyings, r.strikes, r.risk_free_rates, r.volatilities,
        r.times_to_expiry, r.dividend_yields);

    for (auto _ : state) {
        // This code gets timed
        NaiveBlackScholes<T>::template price<true>(naive_op);
        NaiveBlackScholes<T>::template price<false>(naive_op);
    }
}

template <typename T>
static void BM_FastPrice(benchmark::State& state)
{
    // Perform setup here
    RandomInput<T> r;
    OptionPricing<T> fast_op(
        r.underlyings, r.strikes, r.risk_free_rates, r.volatilities,
        r.times_to_expiry, r.dividend_yields);

    for (auto _ : state) {
        // This code gets timed
        FastBlackScholes<T, hn::ScalableTag<T>>::template price<true>(fast_op);
        FastBlackScholes<T, hn::ScalableTag<T>>::template price<false>(fast_op);
    }
}

BENCHMARK(BM_FastPrice<double>);
BENCHMARK(BM_NaivePrice<double>);
BENCHMARK(BM_FastPrice<float>);
BENCHMARK(BM_NaivePrice<float>);

TEST(BlackScholesTestDouble, ComparePrice)
{
    // Assign
    using T = double;
    RandomInput<T> r{};
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

    // Act
    FastBlackScholes<T, hn::ScalableTag<T>>::price<true>(fast_op_call);
    NaiveBlackScholes<T>::price<true>(naive_op_call);
    FastBlackScholes<T, hn::ScalableTag<T>>::price<false>(fast_op_put);
    NaiveBlackScholes<T>::price<false>(naive_op_put);

    // Assert
    EXPECT_EQ(fast_op_call.prices.size(), naive_op_call.prices.size());
    EXPECT_EQ(fast_op_call.deltas.size(), naive_op_call.deltas.size());
    EXPECT_EQ(fast_op_put.prices.size(), naive_op_put.prices.size());
    EXPECT_EQ(fast_op_put.deltas.size(), naive_op_put.deltas.size());
    for (auto i = 0; i < fast_op_call.prices.size(); ++i) {
        EXPECT_NEAR(fast_op_call.prices[i], naive_op_call.prices[i], 1e-5);
        EXPECT_NEAR(fast_op_call.deltas[i], naive_op_call.deltas[i], 1e-5);
        EXPECT_NEAR(fast_op_put.prices[i], naive_op_put.prices[i], 1e-5);
        EXPECT_NEAR(fast_op_put.deltas[i], naive_op_put.deltas[i], 1e-5);
    }
}

TEST(BlackScholesTestFloat, CompareFloatDouble)
{
    // Assign
    RandomInput<double> r1{};
    OptionPricing<double> fast_op_double(
        r1.underlyings, r1.strikes, r1.risk_free_rates, r1.volatilities,
        r1.times_to_expiry, r1.dividend_yields);
    OptionPricing<double> naive_op_double(
        r1.underlyings, r1.strikes, r1.risk_free_rates, r1.volatilities,
        r1.times_to_expiry, r1.dividend_yields);
    RandomInput<float> r2{};
    OptionPricing<float> fast_op_float(
        r2.underlyings, r2.strikes, r2.risk_free_rates, r2.volatilities,
        r2.times_to_expiry, r2.dividend_yields);
    OptionPricing<float> naive_op_float(
        r2.underlyings, r2.strikes, r2.risk_free_rates, r2.volatilities,
        r2.times_to_expiry, r2.dividend_yields);

    // Act
    FastBlackScholes<double, hn::ScalableTag<double>>::price<true>(
        fast_op_double);
    FastBlackScholes<float, hn::ScalableTag<float>>::price<true>(fast_op_float);
    FastBlackScholes<double, hn::ScalableTag<double>>::price<true>(
        naive_op_double);
    FastBlackScholes<float, hn::ScalableTag<float>>::price<true>(
        naive_op_float);

    // Assert
    EXPECT_EQ(fast_op_double.prices.size(), fast_op_float.prices.size());
    EXPECT_EQ(fast_op_double.prices.size(), naive_op_double.prices.size());
    EXPECT_EQ(fast_op_double.prices.size(), naive_op_float.prices.size());
    EXPECT_EQ(fast_op_double.deltas.size(), fast_op_float.deltas.size());
    EXPECT_EQ(fast_op_double.deltas.size(), naive_op_double.deltas.size());
    EXPECT_EQ(fast_op_double.deltas.size(), naive_op_float.deltas.size());
    for (auto i = 0; i < fast_op_double.prices.size(); ++i) {
        EXPECT_NEAR(fast_op_double.prices[i], fast_op_float.prices[i], 1e-2);
        EXPECT_NEAR(fast_op_double.prices[i], naive_op_double.prices[i], 1e-2);
        EXPECT_NEAR(fast_op_double.prices[i], naive_op_float.prices[i], 1e-2);
        EXPECT_NEAR(fast_op_double.deltas[i], fast_op_float.deltas[i], 1e-2);
        EXPECT_NEAR(fast_op_double.deltas[i], naive_op_double.deltas[i], 1e-2);
        EXPECT_NEAR(fast_op_double.deltas[i], naive_op_float.deltas[i], 1e-2);
    }
}

TEST(BlackScholesTestDouble, Benchmarks)
{
    ::benchmark::RunSpecifiedBenchmarks();
}

}  // namespace fast_option_pricer