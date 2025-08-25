/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present chengxuewen.
** Copyright 2016 The WebRTC Project Authors.
**
** License: MIT License
**
** Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
** documentation files (the "Software"), to deal in the Software without restriction, including without limitation
** the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
** and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in all copies or substantial portions
** of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
** TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
** CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
** IN THE SOFTWARE.
**
***********************************************************************************************************************/

#ifndef _OCTK_DIVIDE_ROUND_HPP
#define _OCTK_DIVIDE_ROUND_HPP

#include <octk_global.hpp>
#include <octk_checks.hpp>
#include <octk_macros.hpp>

OCTK_BEGIN_NAMESPACE

namespace detail
{

/**
 * @brief long double > double > float > unsigned int > int > short > char
 * @tparam T1
 * @tparam T2
 */
template <typename T1, typename T2>
struct DivideRoundResult
{
    template <typename T> struct Int
    {
        using Type = typename std::conditional<(sizeof(T) < sizeof(int)), int, T>::type;
    };
    template <typename T> struct TypeInfo
    {
        static constexpr bool isUnsigned = std::is_unsigned<T>::value;
        static constexpr size_t size = sizeof(T);
        using Type = T;
    };
    using TI1Type = typename Int<T1>::Type;
    using TI2Type = typename Int<T2>::Type;
    using TI1 = TypeInfo<TI1Type>;
    using TI2 = TypeInfo<TI2Type>;

    using IntType = typename std::conditional<(TI1::size > TI2::size), TI1Type,
                                              typename std::conditional<(TI1::size < TI2::size), TI2Type,
                                                                        typename std::conditional<(TI1::isUnsigned),
                                                                                                  TI1Type,
                                                                                                  TI2Type>::type>::type>::type;

    template <typename R1, typename R2>
    struct IsSameConditional
    {
        using Type = typename std::conditional<(std::is_same<T1, R1>::value || std::is_same<T2, R1>::value), R1,
                                               R2>::type;
    };
    template <typename R1, typename R2>
    using IsSameConditionalType = typename IsSameConditional<R1, R2>::Type;

    using Type = IsSameConditionalType<long double,
                                       IsSameConditionalType<double,
                                                             IsSameConditionalType<float, IntType>>>;
};
template <typename T1, typename T2>
using DivideRoundResultType = typename DivideRoundResult<T1, T2>::Type;
}

template <typename Dividend, typename Divisor>
// inline detail::DivideRoundResultType<Dividend, Divisor> OCTK_CXX14_CONSTEXPR
inline auto OCTK_CXX14_CONSTEXPR
DivideRoundUp(Dividend dividend, Divisor divisor) -> decltype(dividend / divisor)
{
    static_assert(std::is_integral<Dividend>(), "");
    static_assert(std::is_integral<Divisor>(), "");
    OCTK_DCHECK_GE(dividend, 0);
    OCTK_DCHECK_GT(divisor, 0);

    auto quotient = dividend / divisor;
    auto remainder = dividend % divisor;
    return quotient + (remainder > 0 ? 1 : 0);
}

template <typename Dividend, typename Divisor>
// inline detail::DivideRoundResultType<Dividend, Divisor> OCTK_CXX14_CONSTEXPR
inline auto OCTK_CXX14_CONSTEXPR
DivideRoundToNearest(Dividend dividend, Divisor divisor) -> decltype(dividend / divisor)
{
    static_assert(std::is_integral<Dividend>(), "");
    static_assert(std::is_integral<Divisor>(), "");
    OCTK_DCHECK_GT(divisor, 0);

    if (dividend < Dividend{0})
    {
        auto half_of_divisor = divisor / 2;
        auto quotient = dividend / divisor;
        auto remainder = dividend % divisor;
        if (SafeGt(-remainder, half_of_divisor))
        {
            --quotient;
        }
        return quotient;
    }

    auto half_of_divisor = (divisor - 1) / 2;
    auto quotient = dividend / divisor;
    auto remainder = dividend % divisor;
    if (SafeGt(remainder, half_of_divisor))
    {
        ++quotient;
    }
    return quotient;
}
OCTK_END_NAMESPACE

#endif // _OCTK_DIVIDE_ROUND_HPP
