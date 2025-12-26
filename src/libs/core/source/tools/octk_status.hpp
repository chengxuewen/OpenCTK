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

#pragma once

#include <octk_error.hpp>

OCTK_BEGIN_NAMESPACE

class Status
{
public:
    Status() noexcept { }
    Status(const Status &other)
        : mError(other.mError)
    {
    }
    Status(Status &&other)
        : mError(std::move(other.mError))
    {
    }
    Status(const Error::Domain &domain,
           Error::Id code,
           const StringView message,
           const Error::SharedDataPtr &cause = {})
        : mError(Error::create(domain, code, message, cause))
    {
    }
    Status(const char *message, const Error::SharedDataPtr &cause = {})
        : mError(Error::create(message, cause))
    {
    }
    Status(const StringView message, const Error::SharedDataPtr &cause = {})
        : mError(Error::create(message, cause))
    {
    }
    Status(const Error::SharedDataPtr &error)
        : mError(error)
    {
    }
    Status(Error::SharedDataPtr &&error)
        : mError(std::move(error))
    {
    }
    virtual ~Status() noexcept { }

    Status &operator=(const Status &other)
    {
        mError = other.mError;
        return *this;
    }
    Status &operator=(Status &&other)
    {
        mError = std::move(other.mError);
        return *this;
    }

    bool operator==(const Status &other) const { return mError == other.mError; }
    bool operator!=(const Status &other) const { return mError != other.mError; }

    bool ok() const { return !mError.data(); }
    bool isOk() const { return !mError.data(); }
    bool success() const { return !mError.data(); }
    bool isSuccess() const { return !mError.data(); }
    operator bool() const { return !mError.data(); }

    const Error *error() const { return mError.data(); }
    std::string errorString() const { return mError.data() ? mError.data()->toString() : ""; }

private:
    Error::SharedDataPtr mError{nullptr};
};

inline std::ostream &operator<<(std::ostream &os, const Status &status)
{
    const std::string string = status.isOk() ? "OK" : status.errorString().c_str();
    os << string;
    return os;
}

static const Status okStatus{};

OCTK_END_NAMESPACE