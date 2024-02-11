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
    template <typename VecT, typename T, unsigned long Lanes, typename D, D d>
    [[nodiscard]] static inline VecT normal_cdf(const VecT& x)
    {
        VecT res = hn::Div(
            hn::Mul(hn::Set(d, static_cast<T>(-1.0)), x),
            hn::Sqrt(hn::Set(d, static_cast<T>(2.0))));

        // Highway math-inl.h does not have erf or erfc
        std::array<T, Lanes> tmp;
        hn::Store(res, d, tmp.data());
        for (auto& el : tmp) {
            el = std::erfc(el);
        }

        res = hn::Mul(hn::Set(d, 0.5), hn::Load(d, tmp.data()));
        return res;
    }

    template <typename VecT, typename T, typename D, D d>
    [[nodiscard]] static inline VecT normal_pdf(const VecT& x)
    {
        return hn::Mul(
            hn::Set(d, static_cast<T>(0.3989422804014327)),
            hn::Exp(
                d, hn::Mul(hn::Set(d, static_cast<T>(-0.5)), hn::Mul(x, x))));
    }
};

}  // namespace fast_option_pricer