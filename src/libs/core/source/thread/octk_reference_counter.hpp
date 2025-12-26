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

#include <octk_global.hpp>

#include <atomic>

OCTK_BEGIN_NAMESPACE

class ReferenceCounter
{
public:
    using Value = std::atomic<int>;

    ReferenceCounter(int value) noexcept { mValue = value; }
    ReferenceCounter() noexcept = default;
    ~ReferenceCounter() = default;

    bool ref() noexcept { return this->ref(mValue); }
    bool deref() noexcept { return this->deref(mValue); }
    int loadRelaxed() const noexcept { return this->loadRelaxed(mValue); }
    int loadAcquire() const noexcept { return this->loadAcquire(mValue); }

    static bool ref(Value &value) noexcept { return ++value != 0; }
    static bool deref(Value &value) noexcept { return --value != 0; }
    static int loadRelaxed(Value &value) noexcept { return value.load(std::memory_order_relaxed); }
    static int loadAcquire(Value &value) noexcept { return value.load(std::memory_order_acquire); }

private:
    mutable Value mValue{0};
    OCTK_DISABLE_COPY_MOVE(ReferenceCounter)
};

OCTK_END_NAMESPACE