//
// Created by Karolis Spukas on 8/2/2024.
//

#include <benchmark/benchmark.h>
#include <gtest/gtest.h>
#include <hwy/highway.h>
#include <iostream>
#include <vector>
#include "common.h"
#include "fast_black_scholes.h"
#include "naive_black_scholes.h"

namespace fast_option_pricer {

namespace hn = hwy::HWY_NAMESPACE;

class BlackScholesTest : public ::testing::Test
{
   public:
    using T = double;
};

TEST_F(BlackScholesTest, ComparePrice)
{
    // Assign
    std::vector<T> spot{110.0, 110.0, 110.0, 110.0};
    std::vector<T> strike{120.0, 120.0, 120.0, 120.0};
    std::vector<T> years_to_expiry(spot.size(), 25.0 / 252.0);
    std::vector<T> risk_free_rate(spot.size(), 0.02);
    std::vector<T> volatility{0.15, 0.16, 0.17, 0.18};
    std::vector<T> dividend_yield{0.05, 0.05, 0.05, 0.05};

    // Act
    OptionPricing<T> fast_op_call(
        spot, strike, risk_free_rate, volatility, years_to_expiry,
        dividend_yield);
    OptionPricing<T> fast_op_put(
        spot, strike, risk_free_rate, volatility, years_to_expiry,
        dividend_yield);
    OptionPricing<T> naive_op_call(
        spot, strike, risk_free_rate, volatility, years_to_expiry,
        dividend_yield);
    OptionPricing<T> naive_op_put(
        spot, strike, risk_free_rate, volatility, years_to_expiry,
        dividend_yield);

    // Assert
    FastBlackScholes<T, hn::ScalableTag<T>>::price<true>(fast_op_call);
    NaiveBlackScholes::price<true>(naive_op_call);
    EXPECT_EQ(fast_op_call.prices.size(), naive_op_call.prices.size());
    EXPECT_EQ(fast_op_call.deltas.size(), naive_op_call.deltas.size());
    for (auto i = 0; i < fast_op_call.prices.size(); ++i) {
        std::cout << "Elem: " << i << std::endl;
        EXPECT_NEAR(fast_op_call.prices[i], naive_op_call.prices[i], 1e-5);
        EXPECT_NEAR(fast_op_call.deltas[i], naive_op_call.deltas[i], 1e-5);
    }

    FastBlackScholes<T, hn::ScalableTag<T>>::price<false>(fast_op_put);
    NaiveBlackScholes::price<false>(naive_op_put);
    EXPECT_EQ(fast_op_put.prices.size(), naive_op_put.prices.size());
    EXPECT_EQ(fast_op_put.deltas.size(), naive_op_put.deltas.size());
    for (auto i = 0; i < fast_op_put.prices.size(); ++i) {
        std::cout << "Elem: " << i << std::endl;
        EXPECT_NEAR(fast_op_put.prices[i], naive_op_put.prices[i], 1e-5);
        EXPECT_NEAR(fast_op_put.deltas[i], naive_op_put.deltas[i], 1e-5);
    }
}

static void BM_NaivePrice(benchmark::State& state)
{
    // Perform setup here
    using T = double;

    std::vector<T> spot{110.0, 110.0, 110.0, 110.0};
    std::vector<T> strike{120.0, 120.0, 120.0, 120.0};
    std::vector<T> years_to_expiry(spot.size(), 25.0 / 252.0);
    std::vector<T> risk_free_rate(spot.size(), 0.02);
    std::vector<T> volatility{0.15, 0.16, 0.17, 0.18};
    std::vector<T> dividend_yield{0.05, 0.05, 0.05, 0.05};
    OptionPricing<T> naive_op(
        spot, strike, risk_free_rate, volatility, years_to_expiry,
        dividend_yield);

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

    std::vector<T> spot{110.0, 110.0, 110.0, 110.0};
    std::vector<T> strike{120.0, 120.0, 120.0, 120.0};
    std::vector<T> years_to_expiry(spot.size(), 25.0 / 252.0);
    std::vector<T> risk_free_rate(spot.size(), 0.02);
    std::vector<T> volatility{0.15, 0.16, 0.17, 0.18};
    std::vector<T> dividend_yield{0.05, 0.05, 0.05, 0.05};
    OptionPricing<T> fast_op(
        spot, strike, risk_free_rate, volatility, years_to_expiry,
        dividend_yield);

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