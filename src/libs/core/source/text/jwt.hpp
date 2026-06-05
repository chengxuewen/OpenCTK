/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2026~Present ChengXueWen.
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

#pragma once

#include <openctk/core/exception.hpp>

#include <openctk/3rdparty/jwt-cpp/jwt.h>
#include <openctk/3rdparty/jwt-cpp/traits/nlohmann-json/traits.h>

// sea https://thalhammer.github.io/jwt-cpp/index.html

OCTK_BEGIN_NAMESPACE

namespace utils
{
namespace jwt
{
using JsonTraits = ::jwt::traits::nlohmann_json;
using DefaultClock = ::jwt::default_clock;

using DecodedJwt = ::jwt::decoded_jwt<JsonTraits>;
using Claim = ::jwt::basic_claim<JsonTraits>;
using Payload = ::jwt::payload<JsonTraits>;

template <typename Clock>
using Builder = ::jwt::builder<Clock, JsonTraits>;

Builder<DefaultClock> create(DefaultClock c = {})
{
    return Builder<DefaultClock>(c);
}
template <typename Clock>
Builder<Clock> create(Clock c)
{
    return Builder<Clock>(c);
}

DecodedJwt decode(const std::string &token)
{
    return ::jwt::decode<JsonTraits>(token);
}

} // namespace jwt
} // namespace utils

OCTK_END_NAMESPACE
