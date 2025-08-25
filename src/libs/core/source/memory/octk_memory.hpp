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

#ifndef _OCTK_MEMORY_HPP
#define _OCTK_MEMORY_HPP

#include <octk_global.hpp>

#include <memory>

OCTK_BEGIN_NAMESPACE

namespace utils
{
namespace detail
{
// helper to construct a non-array unique_ptr
template <typename T> struct make_unique_helper
{
    typedef std::unique_ptr<T> unique_ptr;
    template <typename... Args> static inline unique_ptr make(Args &&...args)
    {
        return unique_ptr(new T(std::forward<Args>(args)...));
    }
};

// helper to construct an array unique_ptr
template <typename T> struct make_unique_helper<T[]>
{
    typedef std::unique_ptr<T[]> unique_ptr;

    template <typename... Args> static inline unique_ptr make(Args &&...args)
    {
        return unique_ptr(new T[sizeof...(Args)]{std::forward<Args>(args)...});
    }
};

// helper to construct an array unique_ptr with specified extent
template <typename T, std::size_t N> struct make_unique_helper<T[N]>
{
    typedef std::unique_ptr<T[]> unique_ptr;

    template <typename... Args> static inline unique_ptr make(Args &&...args)
    {
        static_assert(N >= sizeof...(Args), "For make_unique<T[N]> N must be as largs as the number of arguments");
        return unique_ptr(new T[N]{std::forward<Args>(args)...});
    }

#if __GNUC__ == 4 && __GNUC_MINOR__ <= 6
    // G++ 4.6 has an ICE when you have no arguments
        static inline unique_ptr make() { return unique_ptr(new T[N]); }
#endif
};

} // namespace detail

template <typename T, typename... Args>
inline typename detail::make_unique_helper<T>::unique_ptr makeUnique(Args &&...args)
{
    return detail::make_unique_helper<T>::make(std::forward<Args>(args)...);
}

template <typename T, typename... Args>
inline typename detail::make_unique_helper<T>::unique_ptr make_unique(Args &&...args)
{
    return detail::make_unique_helper<T>::make(std::forward<Args>(args)...);
}

} // namespace utils

OCTK_END_NAMESPACE

#endif // _OCTK_MEMORY_HPP
