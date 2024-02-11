# Fast Option Pricing [WIP]

Portable SIMD-based C++ option pricing library (Black-Scholes).

I was curious to see how much performance there is to be gained by using a relatively straightforward SIMD implementation compared to the "naive" scalar implementation. I wanted a portable solution as I'd never tried writing SIMD code on an Apple chip (M2), hence the choice of `highway` (https://github.com/google/highway) library. It currently lacks some of the necessary maths operations though, most notably an `std::erf` equivalent for standard normal CDF.

I made some basic optimizations to the data layout and the order of arithmetic operations, but much more work needs to be done for the SIMD implementation.

For now, calculates option prices (call/put), as well as the main greeks (Delta, Gamma, Vega, Theta, Rho).

## Installation

Written in C++20, compiled with Apple clang 14.0.3. Dependencies (through `vcpkg`):
- highway (1.0.7)

Additional dependencies for tests:
- gtest (1.14.0)
- benchmark (1.8.3)

## Results

10,000,000 randomized call and put options for both `floats` and `doubles`.

### arm64-osx (Apple M2) NEON with Apple clang 14.0.3

Can use at most `Vec128`, giving us 2 doubles or 4 floats per vector.

```bash
----------------------------------------------------------------
Benchmark                      Time             CPU   Iterations
----------------------------------------------------------------
BM_FastPrice<double>   886793667 ns    886623000 ns            1
BM_NaivePrice<double> 1127674666 ns   1127539000 ns            1
BM_FastPrice<float>    673567875 ns    673450000 ns            1
BM_NaivePrice<float>  1162894792 ns   1162567000 ns            1
```

## x86 Avx256

Should be possible to use at most `Vec256`, giving us 4 doubles or 8 floats per vector.

```bash
TODO
```