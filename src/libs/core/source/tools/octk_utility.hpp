/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
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

#ifndef _OCTK_UTILITY_HPP
#define _OCTK_UTILITY_HPP

#include <octk_type_traits.hpp>

OCTK_BEGIN_NAMESPACE
namespace utils
{
/***********************************************************************************************************************
  * like std::conjunction
***********************************************************************************************************************/
// this adds const to non-const objects (like std::as_const)
template <typename T> constexpr typename std::add_const<T>::type &asConst(T &t) noexcept { return t; }
// prevent rvalue arguments:
template <typename T> void asConst(const T &&) = delete;

/***********************************************************************************************************************
  * like cxx14 std::exchange
***********************************************************************************************************************/
#if OCTK_CC_CPP14_OR_GREATER
using std::exchange;
#else
template <typename T, typename U = T>
OCTK_CXX14_CONSTEXPR T exchange(T &t, U &&newValue) noexcept(
    Conjunction<std::is_nothrow_move_constructible<T>, std::is_nothrow_assignable<T &, U>>::value)
{
    T old = std::move(t);
    t = std::forward<U>(newValue);
    return old;
}
#endif

/***********************************************************************************************************************
  * like cxx20 std::identity
***********************************************************************************************************************/
struct identity
{
    template <typename T> constexpr T &&operator()(T &&t) const noexcept { return std::forward<T>(t); }
    using is_transparent = void;
};

/***********************************************************************************************************************
  * like cxx23 std::to_underlying
***********************************************************************************************************************/
template <typename Enum> constexpr typename std::underlying_type<Enum>::type toUnderlying(Enum e) noexcept
{
    return static_cast<typename std::underlying_type<Enum>::type>(e);
}
} // namespace utils
OCTK_END_NAMESPACE

#endif // _OCTK_UTILITY_HPP
