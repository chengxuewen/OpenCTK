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

#ifndef _OCTK_YIELD_POLICY_HPP
#define _OCTK_YIELD_POLICY_HPP

#include <octk_global.hpp>

#include <memory>

OCTK_BEGIN_NAMESPACE

class YieldInterface
{
public:
    virtual ~YieldInterface() = default;
    virtual void YieldExecution() = 0;
};

// Sets the current thread-local yield policy while it's in scope and reverts
// to the previous policy when it leaves the scope.
class ScopedYieldPolicy final
{
public:
    explicit ScopedYieldPolicy(YieldInterface *policy);
    ScopedYieldPolicy(const ScopedYieldPolicy &) = delete;
    ScopedYieldPolicy &operator=(const ScopedYieldPolicy &) = delete;
    ~ScopedYieldPolicy();
    // Will yield as specified by the currently active thread-local yield policy
    // (which by default is a no-op).
    static void YieldExecution();

private:
    YieldInterface *const previous_;
};
OCTK_END_NAMESPACE

#endif // _OCTK_YIELD_POLICY_HPP
