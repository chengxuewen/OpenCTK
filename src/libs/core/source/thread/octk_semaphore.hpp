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

#include <octk_assert.hpp>
#include <octk_global.hpp>

OCTK_BEGIN_NAMESPACE

template <std::ptrdiff_t LeastMaxValue = std::numeric_limits<std::ptrdiff_t>::max()> class OCTK_CORE_API Semaphore
{
public:
    explicit Semaphore(std::ptrdiff_t desired = 0)
        : mCounter(desired)
    {
        OCTK_ASSERT(desired >= 0 && desired <= max());
    }
    ~Semaphore() = default;

    static constexpr std::ptrdiff_t max() noexcept
    {
        static_assert(LeastMaxValue >= 0, "LeastMaxValue shall be non-negative");
        return LeastMaxValue;
    }

    void acquire()
    {
        std::unique_lock<decltype(mMutex)> lock{mMutex};
        mCondition.wait(lock, [&]() { return mCounter > 0; });
        --mCounter;
    }
    bool tryAcquire()
    {
        std::unique_lock<decltype(mMutex)> lock{mMutex};
        if (mCounter <= 0)
        {
            return false;
        }
        --mCounter;
        return true;
    }
    template <typename Rep, typename Period> bool tryAcquireFor(const std::chrono::duration<Rep, Period> &relTime)
    {
        const auto timeout_time = std::chrono::steady_clock::now() + relTime;
        return this->tryAcquireWait(timeout_time);
    }

    template <typename Clock, typename Duration>
    bool tryAcquireUntil(const std::chrono::time_point<Clock, Duration> &abs_time)
    {
        return this->tryAcquireWait(abs_time);
    }

    void release(std::ptrdiff_t update = 1)
    {
        {
            std::lock_guard<decltype(mMutex)> lock{mMutex};
            OCTK_ASSERT(update >= 0 && update <= max() - mCounter);
            mCounter += update;
            if (mCounter <= 0)
            {
                return;
            }
        } // avoid hurry up and wait
        mCondition.notify_all();
    }


    // int available() const;


protected:
    template <typename Clock, typename Duration>
    bool tryAcquireWait(const std::chrono::time_point<Clock, Duration> &timeout_time)
    {
        std::unique_lock<decltype(mMutex)> lock{mMutex};
        if (!mCondition.wait_until(lock, timeout_time, [&]() { return mCounter > 0; }))
        {
            return false;
        }
        --mCounter;
        return true;
    }

private:
    OCTK_DISABLE_COPY_MOVE(Semaphore)
    std::mutex mMutex;
    std::ptrdiff_t mCounter{0};
    std::condition_variable mCondition;
};

OCTK_END_NAMESPACE