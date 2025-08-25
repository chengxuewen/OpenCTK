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

#ifndef _OCTK_OPTIONAL_HPP
#define _OCTK_OPTIONAL_HPP

#include <octk_global.hpp>
#include <octk_core_config.hpp>

#if OCTK_FEATURE_USE_OPTIONAL_LITE
#   include <nonstd/optional.hpp>
#else
#   include <tl/optional.hpp>
#endif

OCTK_BEGIN_NAMESPACE
#if OCTK_FEATURE_USE_OPTIONAL_LITE

template <typename T> using Optional = nonstd::optional<T>;
using in_place_t = nonstd::in_place_t;
using nullopt_t = nonstd::nullopt_t;

namespace utils
{
using nonstd::nullopt;
using nonstd::in_place;
using nonstd::make_optional;
} // namespace utils

#else

template <typename T> using Optional = tl::optional<T>;
using in_place_t = tl::in_place_t;
using nullopt_t = tl::nullopt_t;

namespace utils
{
using tl::in_place;
using tl::nullopt;
using tl::make_optional;
} // namespace utils

#endif
OCTK_END_NAMESPACE

#endif // _OCTK_OPTIONAL_HPP
