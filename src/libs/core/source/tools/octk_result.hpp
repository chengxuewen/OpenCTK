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

#ifndef _OCTK_RESULT_HPP
#define _OCTK_RESULT_HPP

#include <octk_optional.hpp>

OCTK_BEGIN_NAMESPACE

struct OkResult final
{
    // clang-format off
    struct init { };
    explicit constexpr OkResult(init /*unused*/) noexcept { }
    // clang-format off
};
namespace utils
{
constexpr OkResult okResult{ OkResult::init{} };
} // namespace utils

template <typename T = std::string>
class Result
{
    template <typename U> friend class Result;
public:
    using ErrorType = T;

    Result() noexcept { }
    Result(OkResult /*unused*/) noexcept { }

    template <typename U>
    Result(const Result<U> &other)
        : mError(other.mError)
    {
    }
    Result(const Result &error)
        : mError(error.mError)
    {
    }
    Result(Result &&error)
        : mError(std::move(error.mError))
    {
    }
    Result(const T &value)
        : mError(value)
    {
    }
    Result(T &&value)
        : mError(std::move(value))
    {
    }

    template <typename U> Result &operator=(const Result<U> &other)
    {
        mError = other.mError;
        return *this;
    }
    template <typename U> Result &operator=(Result<U> &&other)
    {
        mError = std::move(other.mError);
        return *this;
    }
    Result &operator=(const Result &other)
    {
        mError = other.mError;
        return *this;
    }
    Result &operator=(Result &&other)
    {
        mError = std::move(other.mError);
        return *this;
    }

    bool ok() const { return !mError.has_value(); }
    bool isOk() const { return !mError.has_value(); }
    bool success() const { return !mError.has_value(); }
    bool isSuccess() const { return !mError.has_value(); }

    const T &error() const { return mError.value(); }

    operator bool() const { return !mError.has_value(); }

private:
    Optional<T> mError;
};

using ResultS = Result<std::string>;

OCTK_END_NAMESPACE

#endif // _OCTK_RESULT_HPP