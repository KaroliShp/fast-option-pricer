//
// Created by Karolis Spukas on 10/2/2024.
//

#pragma once

#include <hwy/highway.h>
#include "math-inl.h"

namespace fast_option_pricer {

namespace hn = hwy::HWY_NAMESPACE;

class FastMathHelper
{
   public:
    template <typename T, unsigned long Lanes>
    [[nodiscard]] static inline auto normal_cdf(const auto& x, const auto& d)
    {
        auto res = hn::Div(hn::Mul(hn::Set(d, -1), x), hn::Sqrt(hn::Set(d, 2)));

        // Highway math-inl.h does not have erf or erfc
        std::array<T, Lanes> tmp;
        hn::Store(res, d, tmp.data());
        for (auto& el : tmp) {
            el = std::erfc(el);
        }

        res = hn::Mul(hn::Set(d, 0.5), hn::Load(d, tmp.data()));
        return res;
    }

    template <typename T>
    [[nodiscard]] static inline auto normal_pdf(const auto& x, const auto& d)
    {
        return hn::Mul(
            hn::Set(d, static_cast<T>(0.3989422804014327)),
            hn::Exp(
                d, hn::Mul(hn::Set(d, static_cast<T>(-0.5)), hn::Mul(x, x))));
    }
};

}  // namespace fast_option_pricer