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
    static inline auto normal_cdf(const auto& x, const auto& d)
    {
        auto res = hn::Mul(hn::Set(d, -1), x);

        // Highway math-inl.h does not have erf or erfc
        std::array<T, Lanes> tmp;
        hn::Store(res, d, tmp.data());
        for (auto& el : tmp) {
            el = std::erfc(el);
        }

        res = hn::Mul(hn::Set(d, 0.5), res);
        return res;
    }
};

}  // namespace fast_option_pricer