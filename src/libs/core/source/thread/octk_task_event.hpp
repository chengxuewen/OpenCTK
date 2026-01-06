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

#ifndef _OCTK_TASK_EVENT_HPP
#define _OCTK_TASK_EVENT_HPP

#include <octk_time_delta.hpp>
#include <octk_yield_policy.hpp>

#include <mutex>
#include <condition_variable>

#if defined(OCTK_OS_WIN)
#   include <windows.h>
#else
#   include <pthread.h>
#endif

OCTK_BEGIN_NAMESPACE

// OCTK_DISALLOW_WAIT() utility
//
// Sets a stack-scoped flag that disallows use of `rtc::Event::Wait` by means
// of raising a DCHECK when a call to `rtc::Event::Wait()` is made..
// This is useful to guard synchronization-free scopes against regressions.
//
// Example of what this would catch (`ScopeToProtect` calls `Foo`):
//
//  void Foo(TaskQueueOld* tq) {
//    Event event;
//    tq->PostTask([&event]() {
//      event.Set();
//    });
//    event.Wait(Event::kForever);  // <- Will trigger a DCHECK.
//  }
//
//  void ScopeToProtect() {
//    TaskQueueOld* tq = GetSomeTaskQueue();
//    OCTK_DISALLOW_WAIT();  // Policy takes effect.
//    Foo(tq);
//  }
//
#if OCTK_DCHECK_IS_ON
#   define OCTK_DISALLOW_WAIT() ScopedDisallowWait disallow_wait_##__LINE__
#else
#   define OCTK_DISALLOW_WAIT()
#endif

class OCTK_CORE_API Event
{
public:
    Event();
    Event(bool manualReset, bool initiallySignaled);
    Event(const Event &) = delete;
    Event &operator=(const Event &) = delete;
    ~Event();

    void Set();
    void Reset();

    // Waits for the event to become signaled, but logs a warning if it takes more
    // than `warn_after`, and gives up completely if it takes more than
    // `give_up_after`. (If `warn_after >= give_up_after`, no warning will be
    // logged.) Either or both may be `kForever`, which means wait indefinitely.
    //
    // Care is taken so that the underlying OS wait call isn't requested to sleep
    // shorter than `give_up_after`.
    //
    // Returns true if the event was signaled, false if there was a timeout or
    // some other error.
    bool Wait(TimeDelta give_up_after, TimeDelta warn_after);

    // Waits with the given timeout and a reasonable default warning timeout.
    bool Wait(TimeDelta give_up_after)
    {
        return Wait(give_up_after, give_up_after.IsPlusInfinity() ? Event::defaultWarnDuration()
                                                                  : Event::foreverDuration());
    }

    static TimeDelta foreverDuration() { return TimeDelta::PlusInfinity(); }
    static TimeDelta defaultWarnDuration() { return TimeDelta::Seconds(3); }

private:
    std::mutex mEventMutex;
    std::condition_variable mEventCondition;
    const bool mIsManualReset;
    bool mEventStatus;
};

// These classes are provided for compatibility with Chromium.
// The rtc::Event implementation is overriden inside of Chromium for the
// purposes of detecting when threads are blocked that shouldn't be as well as
// to use the more accurate event implementation that's there than is provided
// by default on some platforms (e.g. Windows).
// When building with standalone WebRTC, this class is a noop.
// For further information, please see the
// ScopedAllowBaseSyncPrimitives(ForTesting) classes in Chromium.
class ScopedAllowBaseSyncPrimitives
{
public:
    ScopedAllowBaseSyncPrimitives() {}
    ~ScopedAllowBaseSyncPrimitives() {}
};

class ScopedAllowBaseSyncPrimitivesForTesting
{
public:
    ScopedAllowBaseSyncPrimitivesForTesting() {}
    ~ScopedAllowBaseSyncPrimitivesForTesting() {}
};

#if OCTK_DCHECK_IS_ON

class ScopedDisallowWait
{
public:
    ScopedDisallowWait() = default;

private:
    class DisallowYieldHandler : public YieldInterface
    {
    public:
        void YieldExecution() override { OCTK_DCHECK_NOTREACHED(); }
    } handler_;

    ScopedYieldPolicy policy{&handler_};
};

#endif
OCTK_END_NAMESPACE

#endif // _OCTK_TASK_EVENT_HPP
