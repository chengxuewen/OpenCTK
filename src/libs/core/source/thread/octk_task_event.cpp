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

#include <octk_task_event.hpp>
#include <octk_optional.hpp>
#if 0
OCTK_BEGIN_NAMESPACE

Event::Event() : Event(false, false) {}

Event::Event(bool manualReset, bool initiallySignaled)
    : mIsManualReset(manualReset), mEventStatus(initiallySignaled) {}

Event::~Event() {}

void Event::Set()
{
    mEventMutex.lock();
    mEventStatus = true;
    mEventCondition.notify_all();
    mEventMutex.unlock();
}

void Event::Reset()
{
    mEventMutex.lock();
    mEventStatus = false;
    mEventMutex.unlock();
}

bool Event::Wait(TimeDelta give_up_after, TimeDelta warn_after)
{
    // Instant when we'll log a warning message (because we've been waiting so
    // long it might be a bug), but not yet give up waiting. nullopt if we
    // shouldn't log a warning.
    const Optional<TimeDelta> warnTime = warn_after >= give_up_after
                                         ? utils::nullopt
                                         : utils::make_optional(warn_after);

    // Instant when we'll stop waiting and return an error. nullopt if we should
    // never give up.
    const Optional<TimeDelta> giveupTime = give_up_after.IsPlusInfinity()
                                           ? utils::nullopt
                                           : utils::make_optional(give_up_after);

    ScopedYieldPolicy::YieldExecution();

    // OCTK_DEBUG() << "mEventMutex.try_lock()::" << mEventMutex.try_lock();
    // mEventMutex.unlock();
    std::unique_lock<std::mutex> lock(mEventMutex);
    // pthread_mutex_lock(&event_mutex_);

    // Wait for `event_cond_` to trigger and `event_status_` to be set, with the
    // given timeout (or without a timeout if none is given).
    const auto wait = [&](const Optional<TimeDelta> timeout) {
        std::cv_status status = std::cv_status::no_timeout;
        while (!mEventStatus && std::cv_status::no_timeout == status)
        {
            if (timeout == utils::nullopt)
            {
                mEventCondition.wait(lock);
            }
            else
            {
                status = mEventCondition.wait_for(lock, std::chrono::microseconds(timeout->us()));
            }
        }
        return status;
    };

    std::cv_status status = std::cv_status::no_timeout;
    if (warnTime == utils::nullopt)
    {
        status = wait(giveupTime);
    }
    else
    {
        status = wait(warnTime);
        if (std::cv_status::timeout == status)
        {
            // WarnThatTheCurrentThreadIsProbablyDeadlocked();
            status = wait(giveupTime);
        }
    }

    // NOTE(liulk): Exactly one thread will auto-reset this event. All
    // the other threads will think it's unsignaled.  This seems to be
    // consistent with auto-reset events in OCTK_OS_WIN
    if (std::cv_status::no_timeout == status && !mIsManualReset)
    {
        mEventStatus = false;
    }

    return std::cv_status::no_timeout == status;
}
OCTK_END_NAMESPACE
#endif
