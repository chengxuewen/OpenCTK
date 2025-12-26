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

#ifndef _OCTK_PLATFORM_THREAD_P_HPP
#define _OCTK_PLATFORM_THREAD_P_HPP

#include <octk_reference_counter.hpp>
#include <octk_platform_thread.hpp>
#include <octk_mutex.hpp>

OCTK_BEGIN_NAMESPACE

namespace detail
{
static int idealConcurrencyThreadCount() noexcept;
} // namespace detail


class PlatformThreadData
{
    mutable ReferenceCounter mRefCounter;

    ~PlatformThreadData()
    {
        OCTK_ASSERT(mRefCounter.loadRelaxed() != 0);
        delete thread.exchange(nullptr, std::memory_order_acq_rel);
    }

public:
    PlatformThreadData(int initialRefCount = 1)
        : mRefCounter(initialRefCount)
    {
    }

    static PlatformThreadData *current(PlatformThreadPrivate *thread);
    static PlatformThreadData *current(bool createIfNecessary = true); // impl
    static void clearCurrent();                                        // impl

    void ref()
    {
        mRefCounter.ref();
        OCTK_ASSERT(mRefCounter.loadRelaxed() != 0);
    }
    void deref()
    {
        if (!mRefCounter.deref())
        {
            delete this;
        }
    }

    bool quitNow{false};
    bool canWait{true};
    bool isAdopted{false};

    int loopLevel{0};
    int scopeLevel{0};


    std::vector<void *> tls;
    std::atomic<PlatformThread *> thread;
    std::atomic<PlatformThread::Handle> threadHandle;
};

class OCTK_CORE_API PlatformThreadPrivate
{
public:
    using Priority = PlatformThread::Priority;
    using Handle = PlatformThread::Handle;

    PlatformThreadPrivate(PlatformThread *p, PlatformThreadData *data = nullptr);
    virtual ~PlatformThreadPrivate();

    static PlatformThreadPrivate *get(PlatformThread *p) { return p->dFunc(); }
    static PlatformThread *get(PlatformThreadPrivate *d) { return d->pFunc(); }

    void setPriority(Priority priority); // impl
    bool start(Priority priority);       // impl
    void exit(int code);                 // impl

    virtual void onFinished() { }
    virtual void onStarted() { }
    virtual void run() { }

    mutable Mutex mMutex;
    mutable std::condition_variable mDoneCondition;

    int mReturnCode{-1};
    std::atomic<bool> mExited{false};
    std::atomic<bool> mRunning{false};
    std::atomic<bool> mFinished{true};
    std::atomic<bool> mInFinish{false}; //when in finish
    std::atomic<bool> mInterruptionRequested{false};

    std::string mName;
    uint mStackSize{0};
    Priority mPriority{Priority::kInherit};

    PlatformThreadData *mData{nullptr};
#if defined(OCTK_OS_WIN)
#else
    pthread_t mThread{};
#endif

protected:
    OCTK_DEFINE_PPTR(PlatformThread)
    OCTK_DECLARE_PUBLIC(PlatformThread)
    OCTK_DISABLE_COPY_MOVE(PlatformThreadPrivate)
};

OCTK_END_NAMESPACE

#endif // _OCTK_PLATFORM_THREAD_P_HPP
