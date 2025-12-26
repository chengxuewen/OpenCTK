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

#include <private/octk_platform_thread_p.hpp>
#include <octk_exception.hpp>
#include <octk_logging.hpp>
#include <octk_checks.hpp>

#include <thread>

OCTK_BEGIN_NAMESPACE

PlatformThreadData *PlatformThreadData::current(PlatformThreadPrivate *thread)
{
    OCTK_ASSERT(nullptr != thread);
    return thread->mData;
}

PlatformThreadPrivate::PlatformThreadPrivate(PlatformThread *p, PlatformThreadData *data)
    : mPPtr(p)
    , mData(data ? data : new PlatformThreadData)
{
}

PlatformThreadPrivate::~PlatformThreadPrivate() { mData->deref(); }

PlatformThread::PlatformThread()
    : mDPtr(new PlatformThreadPrivate(this))
{
    mDPtr->mData->thread.store(this, std::memory_order_relaxed);
    this->setName("PlatformThread", this);
}

PlatformThread::PlatformThread(PlatformThreadPrivate *d)
    : mDPtr(d)
{
    mDPtr->mData->thread.store(this, std::memory_order_relaxed);
    this->setName("PlatformThread", this);
}

PlatformThread::~PlatformThread()
{
    OCTK_D(PlatformThread);
    Mutex::Locker locker(d->mMutex);
    if (d->mInFinish)
    {
        locker.unlock();
        this->wait();
        locker.relock();
    }
    if (d->mRunning && !d->mFinished && !d->mData->isAdopted)
    {
        OCTK_FATAL("PlatformThread: Destroyed while thread is still running");
        d->mData->thread.store(nullptr, std::memory_order_release);
    }
}

bool PlatformThread::isInterruptionRequested() const
{
    OCTK_D(const PlatformThread);
    // fast path: check that the flag is not set:
    if (!d->mInterruptionRequested.load(std::memory_order_relaxed))
    {
        return false;
    }
    // slow path: if the flag is set, take into account run status:
    Mutex::Locker locker(d->mMutex);
    return d->mRunning && !d->mFinished && !d->mInFinish;
}

Status PlatformThread::requestInterruption()
{
    OCTK_D(PlatformThread);
    Mutex::Locker locker(d->mMutex);
    if (!d->mRunning || d->mFinished || d->mInFinish)
    {
        return "Thread is not running or finished";
    }
    d->mInterruptionRequested.store(true, std::memory_order_relaxed);
    return okStatus;
}

const std::string &PlatformThread::name() const
{
    OCTK_D(const PlatformThread);
    Mutex::Locker locker(d->mMutex);
    return d->mName;
}

Status PlatformThread::setName(const StringView name, const void *obj)
{
    OCTK_D(PlatformThread);
    Mutex::Locker locker(d->mMutex);
    if (d->mRunning)
    {
        return "cannot set name while the thread is running";
    }
    d->mName = std::string(name);
    if (obj)
    {
        // The %p specifier typically produce at most 16 hex digits, possibly with a 0x prefix.
        // But format is implementation defined, so add some margin.
        char buf[30];
        std::snprintf(buf, sizeof(buf), " 0x%p", obj);
        d->mName += buf;
    }
    return okStatus;
}

PlatformThread::Priority PlatformThread::priority() const
{
    OCTK_D(const PlatformThread);
    Mutex::Locker locker(d->mMutex);
    return d->mPriority;
}

Status PlatformThread::setPriority(Priority priority)
{
    if (priority == Priority::kInherit)
    {
        return "Argument cannot be InheritPriority";
    }
    OCTK_D(PlatformThread);
    Mutex::Locker locker(d->mMutex);
    if (!d->mRunning)
    {
        return "Cannot set priority, thread is not running";
    }
    d->setPriority(priority);
    return okStatus;
}

uint PlatformThread::stackSize() const
{
    OCTK_D(const PlatformThread);
    Mutex::Locker locker(d->mMutex);
    return d->mStackSize;
}

Status PlatformThread::setStackSize(uint stackSize)
{
    OCTK_D(PlatformThread);
    Mutex::Locker locker(d->mMutex);
    if (d->mRunning)
    {
        return "cannot change stack size while the thread is running";
    }
    d->mStackSize = stackSize;
    return okStatus;
}

bool PlatformThread::isFinished() const
{
    OCTK_D(const PlatformThread);
    return d->mFinished.load(std::memory_order_relaxed);
}

bool PlatformThread::isRunning() const
{
    OCTK_D(const PlatformThread);
    return d->mRunning.load(std::memory_order_relaxed) && !d->mInFinish.load(std::memory_order_relaxed);
}

PlatformThread::Id PlatformThread::id() const
{
    OCTK_D(const PlatformThread);
    return d->mData->threadHandle.load();
}

void PlatformThread::start(Priority priority)
{
    OCTK_D(PlatformThread);
    Mutex::UniqueLocker locker(d->mMutex);

    if (d->mInFinish)
    {
        d->mDoneCondition.wait(locker, [d]() { return d->mFinished.load(); });
    }
    if (d->mRunning)
    {
        return;
    }

    d->mRunning = true;
    d->mFinished = false;
    d->mReturnCode = 0;
    d->mExited = false;
    d->mInterruptionRequested = false;
    d->mPriority = priority;
    if (!d->start(priority))
    {
        OCTK_WARNING("PlatformThread::start: Thread creation error");
        d->mRunning = false;
        d->mFinished = false;
        d->mData->threadHandle.store(nullptr, std::memory_order_relaxed);
    }
}

void PlatformThread::terminate() { }

bool PlatformThread::wait(unsigned long msecs)
{
    OCTK_D(PlatformThread);
    if (this->currentThreadId() == PlatformThread::currentThreadId())
    {
        OCTK_WARNING("PlatformThread::wait: Thread tried to wait on itself");
        return false;
    }

    Mutex::UniqueLocker locker(d->mMutex);
    if (d->mFinished || !d->mRunning)
    {
        return true;
    }

    while (d->mRunning)
    {
        if (std::cv_status::timeout == d->mDoneCondition.wait_for(locker, std::chrono::milliseconds(msecs)))
        {
            return false;
        }
    }
    return true;
}

PlatformThread *PlatformThread::currentThread() noexcept { return PlatformThreadData::current()->thread.load(); }

PlatformThread::Id PlatformThread::currentThreadId() noexcept
{
    return PlatformThreadData::current()->threadHandle.load();
}

// int PlatformThread::idealConcurrencyCount() noexcept { return detail::idealConcurrencyThreadCount(); }

void PlatformThread::yieldCurrentThread() { std::this_thread::yield(); }

void PlatformThread::usleep(unsigned long usecs) { std::this_thread::sleep_for(std::chrono::microseconds(usecs)); }

void PlatformThread::msleep(unsigned long msecs) { std::this_thread::sleep_for(std::chrono::milliseconds(msecs)); }

void PlatformThread::sleep(unsigned long secs) { std::this_thread::sleep_for(std::chrono::seconds(secs)); }

void PlatformThread::exit(int code)
{
    auto thread = currentThread();
    if (thread)
    {
        Mutex::UniqueLocker locker(thread->dFunc()->mMutex);
        if (thread->dFunc()->mRunning)
        {
            thread->dFunc()->exit(code);
        }
    }
}

void PlatformThread::setTerminationEnabled(bool enabled) { }

OCTK_END_NAMESPACE
