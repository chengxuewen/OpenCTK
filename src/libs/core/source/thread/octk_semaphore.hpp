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

#if OCTK_CC_CPP20_OR_GREATER
#    include <semaphore>
#endif

OCTK_BEGIN_NAMESPACE

template <std::ptrdiff_t LeastMaxValue = std::numeric_limits<std::ptrdiff_t>::max()>
class OCTK_CORE_API CountingSemaphore
{
public:
    explicit CountingSemaphore(std::ptrdiff_t desired = 0)
        : mCount(desired)
#if OCTK_CC_CPP20_OR_GREATER
        , mSemaphore(desired)
#endif
    {
        OCTK_ASSERT(desired >= 0 && desired <= max());
    }
    virtual ~CountingSemaphore() = default;

    static constexpr std::ptrdiff_t max() noexcept
    {
        static_assert(LeastMaxValue >= 0, "LeastMaxValue shall be non-negative");
        return LeastMaxValue;
    }

    void acquire()
    {
#if OCTK_CC_CPP20_OR_GREATER
        mSemaphore.acquire();
#else
        std::unique_lock<decltype(mMutex)> lock{mMutex};
        mCondition.wait(lock, [&]() { return mCount > 0; });
#endif
        mCount.fetch_sub(1, std::memory_order_relaxed);
    }
    bool tryAcquire()
    {
#if OCTK_CC_CPP20_OR_GREATER
        if (mSemaphore.try_acquire())
        {
            mCount.fetch_sub(1, std::memory_order_relaxed);
            return true;
        }
        return false;
#else
        std::unique_lock<decltype(mMutex)> lock{mMutex};
        if (mCount <= 0)
        {
            return false;
        }
        --mCount;
        return true;
#endif
    }
    template <typename Rep, typename Period> bool tryAcquireFor(const std::chrono::duration<Rep, Period> &relTime)
    {
#if OCTK_CC_CPP20_OR_GREATER
        if (mSemaphore.try_acquire_for(relTime))
        {
            mCount.fetch_sub(1, std::memory_order_relaxed);
            return true;
        }
        return false;
#else
        const auto timeout_time = std::chrono::steady_clock::now() + relTime;
        return this->tryAcquireWait(timeout_time);
#endif
    }

    template <typename Clock, typename Duration>
    bool tryAcquireUntil(const std::chrono::time_point<Clock, Duration> &absTime)
    {
#if OCTK_CC_CPP20_OR_GREATER
        if (mSemaphore.try_acquire_until(absTime))
        {
            mCount.fetch_sub(1, std::memory_order_relaxed);
            return true;
        }
        return false;
#else
        return this->tryAcquireWait(absTime);
#endif
    }

    void release(std::ptrdiff_t update = 1)
    {
#if OCTK_CC_CPP20_OR_GREATER
        mCount.fetch_add(update, std::memory_order_relaxed);
        mSemaphore.release(update);
#else
        {
            std::lock_guard<decltype(mMutex)> lock{mMutex};
            OCTK_ASSERT(update >= 0 && update <= max() - mCount);
            mCount += update;
            if (mCount <= 0)
            {
                return;
            }
        } // avoid hurry up and wait
        mCondition.notify_all();
#endif
    }

    std::ptrdiff_t available() const
    {
#if !OCTK_CC_CPP20_OR_GREATER
        std::lock_guard<decltype(mMutex)> lock{mMutex};
#endif
        return mCount.load();
    }


protected:
#if !OCTK_CC_CPP20_OR_GREATER
    template <typename Clock, typename Duration>
    bool tryAcquireWait(const std::chrono::time_point<Clock, Duration> &timeout_time)
    {
        std::unique_lock<decltype(mMutex)> lock{mMutex};
        if (!mCondition.wait_until(lock, timeout_time, [&]() { return mCount > 0; }))
        {
            return false;
        }
        --mCount;
        return true;
    }
#endif

private:
#if OCTK_CC_CPP20_OR_GREATER
    std::counting_semaphore<LeastMaxValue> mSemaphore;
#else
    mutable std::mutex mMutex;
    // std::ptrdiff_t mCount{0};
    std::condition_variable mCondition;
#endif
    std::atomic<std::ptrdiff_t> mCount{0};
    OCTK_DISABLE_COPY_MOVE(CountingSemaphore)
};
using BinarySemaphore = CountingSemaphore<1>;
using Semaphore = CountingSemaphore<>;

OCTK_END_NAMESPACE