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
#include <octk_assert.hpp>

#if !defined(OCTK_OS_WIN)

#    include <pthread.h>
#    include <sched.h> // SCHED_IDLE
#    include <unistd.h>
#    include <sys/types.h>
#    include <sys/syscall.h>

#    if defined(OCTK_OS_LINUX)
#        include <sys/prctl.h> // PR_SET_NAME
#        if !defined(OCTK_LINUXBASE)
#            include <sys/prctl.h>
#        endif
#        if !defined(SCHED_IDLE)
#            define SCHED_IDLE 5 // from linux/sched.h
#        endif
#    elif defined(OCTK_OS_MAC)
#        include <sys/sysctl.h> // sysctlbyname
#        if !defined(OCTK_OS_IOS)
#            include <CoreServices/CoreServices.h>
#        endif // !defined(OCTK_OS_IOS)
#    elif defined(OCTK_OS_BSD4)
#        include <sys/sysctl.h>
#    elif defined(OCTK_OS_HPUX)
#        include <sys/pstat.h>
#    elif defined(OCTK_OS_VXWORKS)
#        if (_WRS_VXWORKS_MAJOR > 6) || ((_WRS_VXWORKS_MAJOR == 6) && (_WRS_VXWORKS_MINOR >= 6))
#            include <vxCpuLib.h>
#            include <cpuset.h>
#            define OCTK_VXWORKS_HAS_CPUSET
#        endif
#    endif

#    if defined(OCTK_OS_DARWIN) || !defined(OCTK_OS_ANDROID) && !defined(OCTK_OS_OPENBSD) &&                           \
                                       defined(_POSIX_THREAD_PRIORITY_SCHEDULING) &&                                   \
                                       (_POSIX_THREAD_PRIORITY_SCHEDULING - 0 >= 0)
#        define OCTK_HAS_THREAD_PRIORITY_SCHEDULING
#    endif

OCTK_BEGIN_NAMESPACE

namespace detail
{
namespace thread
{
static void finish(void *arg);
} // namespace thread

static_assert(sizeof(pthread_t) <= sizeof(PlatformThread::Handle), "PlatformThread::Handle size error");
OCTK_STATIC_CONSTANT_NUMBER(kThreadPriorityResetFlag, 0x80000000)

/* Handle help functions */
template <typename T>
static typename std::enable_if<std::is_integral<T>::value, PlatformThread::Handle>::type toHandle(T ref)
{
    return reinterpret_cast<PlatformThread::Handle>(static_cast<intptr_t>(ref));
}
template <typename T>
static typename std::enable_if<std::is_pointer<T>::value, PlatformThread::Handle>::type toHandle(T ref)
{
    return ref;
}
template <typename T>
static typename std::enable_if<std::is_integral<T>::value, T>::type fromHandle(const PlatformThread::Handle &handle)
{
    return static_cast<T>(reinterpret_cast<intptr_t>(handle));
}
template <typename T>
static typename std::enable_if<std::is_pointer<T>::value, T>::type fromHandle(const PlatformThread::Handle &handle)
{
    return static_cast<T>(handle);
}

/* tls */
namespace tls
{
static thread_local PlatformThreadData *currentThreadData = nullptr;
static pthread_once_t currentThreadDataOnce = PTHREAD_ONCE_INIT;
static pthread_key_t currentThreadDataKey;
static void destroyCurrentThreadData(void *p)
{
    auto data = static_cast<PlatformThreadData *>(p);
    // thread_local variables are set to zero before calling this destructor function,
    // if they are internally using pthread-specific data management,
    // so we need to set it back to the right value...
    currentThreadData = data;
    if (data->isAdopted)
    {
        auto thread = data->thread.load(std::memory_order_acquire);
        OCTK_ASSERT(thread);
        auto threadPrivate = static_cast<PlatformThreadPrivate *>(PlatformThreadPrivate::get(thread));
        OCTK_ASSERT(!threadPrivate->mFinished);
        thread::finish(threadPrivate);
    }
    data->deref();

    // ... but we must reset it to zero before returning so we aren't leaving a dangling pointer.
    currentThreadData = nullptr;
}
static void createCurrentThreadDataKey()
{
    // create key
    pthread_key_create(&currentThreadDataKey, destroyCurrentThreadData);
}
OCTK_DESTRUCTOR_FUNCTION(destroyCurrentThreadDataKey)
static void destroyCurrentThreadDataKey()
{
    pthread_once(&currentThreadDataOnce, createCurrentThreadDataKey);
    pthread_key_delete(currentThreadDataKey);

    // Reset currentThreadDataOnce in case we end up recreating the thread-data in the rare case of
    // PlatformThreadData construction after destroying the PlatformThreadData.
    pthread_once_t pthreadOnceInit = PTHREAD_ONCE_INIT;
    currentThreadDataOnce = pthreadOnceInit;
}
} // namespace tls
// Utility functions for getting, setting and clearing thread specific data.
static PlatformThreadData *currentThreadData() { return tls::currentThreadData; }
static void setThreadData(PlatformThreadData *data)
{
    tls::currentThreadData = data;
    pthread_once(&tls::currentThreadDataOnce, tls::createCurrentThreadDataKey);
    pthread_setspecific(tls::currentThreadDataKey, data);
}
static void clearThreadData()
{
    tls::currentThreadData = nullptr;
    pthread_setspecific(tls::currentThreadDataKey, nullptr);
}

/* thread */
namespace thread
{
#    if defined(OCTK_HAS_THREAD_PRIORITY_SCHEDULING)
#        if defined(OCTK_OS_QNX)
static bool calculatePriority(PlatformThread::Priority priority, int *sched_policy, int *sched_priority)
{
    // On QNX, NormalPriority is mapped to 10.  A QNX system could use a value different
    // than 10 for the "normal" priority but it's difficult to achieve this so we'll
    // assume that no one has ever created such a system.  This makes the mapping from
    // Qt priorities to QNX priorities lopsided.   There's usually more space available
    // to map into above the "normal" priority than below it.  QNX also has a privileged
    // priority range (for threads that assist the kernel).  We'll assume that no Qt
    // thread needs to use priorities in that range.
    int priority_norm = 10;
    // _sched_info::priority_priv isn't documented.  You'd think that it's the start of the
    // privileged priority range but it's actually the end of the unpriviledged range.
    struct _sched_info info;
    if (SchedInfo_r(0, *sched_policy, &info) != EOK)
    {
        return false;
    }

    if (priority == PlatformThread::Priority::kIdle)
    {
        *sched_priority = info.priority_min;
        return true;
    }

    if (priority_norm < info.priority_min)
    {
        priority_norm = info.priority_min;
    }
    if (priority_norm > info.priority_priv)
    {
        priority_norm = info.priority_priv;
    }

    int to_min, to_max;
    int from_min, from_max;
    int prio;
    if (priority < PlatformThread::Priority::kNormal)
    {
        to_min = info.priority_min;
        to_max = priority_norm;
        from_min = QThread::LowestPriority;
        from_max = PlatformThread::Priority::kNormal;
    }
    else
    {
        to_min = priority_norm;
        to_max = info.priority_priv;
        from_min = PlatformThread::Priority::kNormal;
        from_max = QThread::TimeCriticalPriority;
    }

    prio = ((priority - from_min) * (to_max - to_min)) / (from_max - from_min) + to_min;
    prio = qBound(to_min, prio, to_max);

    *sched_priority = prio;
    return true;
}
#        else
// Does some magic and calculate the Unix scheduler priorities sched_policy is IN/OUT:
// it must be set to a valid policy before calling this function sched_priority is OUT only
static bool calculatePriority(PlatformThread::Priority priority, int *sched_policy, int *sched_priority)
{
#            ifdef SCHED_IDLE
    if (priority == PlatformThread::Priority::kIdle)
    {
        *sched_policy = SCHED_IDLE;
        *sched_priority = 0;
        return true;
    }
    const auto lowestPriority = PlatformThread::Priority::kLowest;
#            else
    const auto lowestPriority = PlatformThread::Priority::kIdle;
#            endif // SCHED_IDLE
    const auto highestPriority = PlatformThread::Priority::kTimeCritical;

    int prio_min;
    int prio_max;
#            if defined(OCTK_OS_VXWORKS) && defined(VXWORKS_DKM)
    // for other scheduling policies than SCHED_RR or SCHED_FIFO
    prio_min = SCHED_FIFO_LOW_PRI;
    prio_max = SCHED_FIFO_HIGH_PRI;

    if ((*sched_policy == SCHED_RR) || (*sched_policy == SCHED_FIFO))
#            endif // defined(OCTK_OS_VXWORKS) && defined(VXWORKS_DKM)
    {
        prio_min = sched_get_priority_min(*sched_policy);
        prio_max = sched_get_priority_max(*sched_policy);
    }

    if (prio_min == -1 || prio_max == -1)
    {
        return false;
    }

    int prio;
    // crudely scale our priority enum values to the prio_min/prio_max
    prio = (((int)priority - (int)lowestPriority) * (prio_max - prio_min) / (int)highestPriority) + prio_min;
    prio = std::max(prio_min, std::min(prio_max, prio));

    *sched_priority = prio;
    return true;
}
#        endif     // defined(OCTK_OS_QNX)
#    endif         // OCTK_HAS_THREAD_PRIORITY_SCHEDULING
#    if (defined(OCTK_OS_LINUX) || defined(OCTK_OS_MAC) || defined(OCTK_OS_QNX))
static void setCurrentName(const char *name)
{
#        if defined(OCTK_OS_LINUX) && !defined(OCTK_LINUXBASE)
    prctl(PR_SET_NAME, (unsigned long)name, 0, 0, 0);
#        elif defined(OCTK_OS_MAC)
    pthread_setname_np(name);
#        elif defined(OCTK_OS_QNX)
    pthread_setname_np(pthread_self(), name);
#        endif
}
#    endif // (defined(OCTK_OS_LINUX) || defined(OCTK_OS_MAC) || defined(OCTK_OS_QNX))
static void finish(void *arg)
{
    OCTK_TRY
    {
        auto threadPrivate = reinterpret_cast<PlatformThreadPrivate *>(arg);
        auto thread = PlatformThreadPrivate::get(threadPrivate);
        Mutex::UniqueLocker locker(threadPrivate->mMutex);
        threadPrivate->mInFinish = true;
        threadPrivate->mPriority = PlatformThread::Priority::kInherit;
        void *data = &threadPrivate->mData->tls;
        locker.unlock();
        threadPrivate->onFinished();
        // Application::sendPostedEvents(nullptr, Event::DeferredDelete);
        // ThreadStorageData::finish((void **)data);
        locker.lock();

        // AbstractEventDispatcher *eventDispatcher = d->data->eventDispatcher.loadRelaxed();
        // if (eventDispatcher)
        // {
        //     d->data->eventDispatcher = nullptr;
        //     locker.unlock();
        //     eventDispatcher->closingDown();
        //     delete eventDispatcher;
        //     locker.relock();
        // }

        threadPrivate->mRunning = false;
        threadPrivate->mFinished = true;
        threadPrivate->mInterruptionRequested = false;

        threadPrivate->mInFinish = false;
        threadPrivate->mDoneCondition.notify_all();
    }
#    if OCTK_HAS_EXCEPTIONS
#        ifdef __GLIBCXX__
    // POSIX thread cancellation under glibc is implemented by throwing an exception
    // of this type. Do what libstdc++ is doing and handle it specially in order not to
    // abort the application if user's code calls a cancellation function.
    OCTK_CATCH(const abi::__forced_unwind &) { throw; }
#        endif // __GLIBCXX__
    OCTK_CATCH(...) { OCTK_ASSERT(false); }
#    endif // OCTK_HAS_EXCEPTIONS
}
static void *start(void *arg)
{
#    if !defined(OCTK_OS_ANDROID)
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, nullptr);
#    endif // !defined(OCTK_OS_ANDROID)
    pthread_cleanup_push(thread::finish, arg);

    OCTK_TRY
    {
        auto threadPrivate = reinterpret_cast<PlatformThreadPrivate *>(arg);
        auto threadData = PlatformThreadData::current(threadPrivate);
        auto thread = PlatformThreadPrivate::get(threadPrivate);
        {
            Mutex::UniqueLocker locker(threadPrivate->mMutex);

            // do we need to reset the thread priority?
            if (int(threadPrivate->mPriority) & kThreadPriorityResetFlag)
            {
                threadPrivate->setPriority(
                    PlatformThread::Priority((int)threadPrivate->mPriority & ~kThreadPriorityResetFlag));
            }
            threadData->threadHandle.store(toHandle(pthread_self()), std::memory_order_relaxed);
            setThreadData(threadData);
        }

        // data->ensureEventDispatcher();

#    if (defined(OCTK_OS_LINUX) || defined(OCTK_OS_MAC) || defined(OCTK_OS_QNX))
        {
            // Sets the name of the current thread. We can only do this when the thread is starting, as we don't
            // have a cross platform way of setting the name of an arbitrary thread.
            if (threadPrivate->mName.empty())
            {
                setCurrentName(threadPrivate->mName.c_str());
            }
        }
#    endif
        threadPrivate->onStarted();
#    if !defined(OCTK_OS_ANDROID)
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, nullptr);
        pthread_testcancel();
#    endif
        threadPrivate->run();
    }
#    if OCTK_HAS_EXCEPTIONS
#        ifdef __GLIBCXX__
    // POSIX thread cancellation under glibc is implemented by throwing an exception
    // of this type. Do what libstdc++ is doing and handle it specially in order not to
    // abort the application if user's code calls a cancellation function.
    OCTK_CATCH(const abi::__forced_unwind &) { throw; }
#        endif // __GLIBCXX__
    OCTK_CATCH(...) { OCTK_ASSERT(false); }
#    endif // OCTK_HAS_EXCEPTIONS

    // This pop runs finish() below. It's outside the try/catch (and has its own try/catch) to prevent finish()
    // to be run in case an exception is thrown.
    pthread_cleanup_pop(1);
    return nullptr;
}
} // namespace thread
} // namespace detail

PlatformThreadData *PlatformThreadData::current(bool createIfNecessary)
{
    auto *threadData = detail::currentThreadData();
    if (!threadData && createIfNecessary)
    {
        threadData = new PlatformThreadData;
        OCTK_TRY
        {
            detail::setThreadData(threadData);
            // threadData->thread = new QAdoptedThread(threadData);
        }
        OCTK_CATCH(...)
        {
            detail::clearThreadData();
            threadData->deref();
            threadData = nullptr;
            OCTK_RETHROW;
        }
        threadData->deref();
        threadData->isAdopted = true;
        threadData->threadHandle.store(detail::toHandle(pthread_self()), std::memory_order_relaxed);
        // if (!CoreApplicationPrivate::theMainThread.loadAcquire())
        // CoreApplicationPrivate::theMainThread.storeRelease(data->thread.loadRelaxed());
    }
    return threadData;
}

void PlatformThreadData::clearCurrent() { detail::clearThreadData(); }

void PlatformThreadPrivate::setPriority(Priority priority)
{
    mPriority = priority;
    // copied from start() with a few modifications:

#    ifdef OCTK_HAS_THREAD_PRIORITY_SCHEDULING
    int sched_policy;
    sched_param param;

    if (pthread_getschedparam(detail::fromHandle<pthread_t>(mData->threadHandle.load(std::memory_order_relaxed)),
                              &sched_policy,
                              &param) != 0)
    {
        // failed to get the scheduling policy, don't bother setting the priority
        OCTK_WARNING("PlatformThread::setPriority: Cannot get scheduler parameters");
        return;
    }

    int prio;
    if (!detail::thread::calculatePriority(priority, &sched_policy, &prio))
    {
        // failed to get the scheduling parameters, don't bother setting the priority
        OCTK_WARNING("PlatformThread::setPriority: Cannot determine scheduler priority range");
        return;
    }

    param.sched_priority = prio;
    int status = pthread_setschedparam(
        detail::fromHandle<pthread_t>(mData->threadHandle.load(std::memory_order_relaxed)),
        sched_policy,
        &param);

#        ifdef SCHED_IDLE
    // were we trying to set to idle priority and failed?
    if (status == -1 && sched_policy == SCHED_IDLE && errno == EINVAL)
    {
        // reset to lowest priority possible
        pthread_getschedparam(from_HANDLE<pthread_t>(data->threadId.loadRelaxed()), &sched_policy, &param);
        param.sched_priority = sched_get_priority_min(sched_policy);
        pthread_setschedparam(from_HANDLE<pthread_t>(data->threadId.loadRelaxed()), sched_policy, &param);
    }
#        else
    OCTK_UNUSED(status);
#        endif // SCHED_IDLE
#    endif
}

bool PlatformThreadPrivate::start(Priority priority)
{
    mPriority = priority;

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

#    if defined(OCTK_HAS_THREAD_PRIORITY_SCHEDULING)
    switch (priority)
    {
        case Priority::kInherit:
        {
            pthread_attr_setinheritsched(&attr, PTHREAD_INHERIT_SCHED);
            break;
        }
        default:
        {
            int sched_policy;
            if (pthread_attr_getschedpolicy(&attr, &sched_policy) != 0)
            {
                // failed to get the scheduling policy, don't bother setting the priority
                OCTK_WARNING("PlatformThread::start: Cannot determine default scheduler policy");
                break;
            }
            int prio;
            if (!detail::thread::calculatePriority(priority, &sched_policy, &prio))
            {
                // failed to get the scheduling parameters, don't bother setting the priority
                OCTK_WARNING("PlatformThread::start: Cannot determine scheduler priority range");
                break;
            }

            sched_param sp;
            sp.sched_priority = prio;
            if (pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED) != 0 ||
                pthread_attr_setschedpolicy(&attr, sched_policy) != 0 || pthread_attr_setschedparam(&attr, &sp) != 0)
            {
                // could not set scheduling hints, fallback to inheriting them we'll try again from inside the thread
                pthread_attr_setinheritsched(&attr, PTHREAD_INHERIT_SCHED);
                mPriority = Priority((int)priority | detail::kThreadPriorityResetFlag);
            }
            break;
        }
    }
#    endif // OCTK_HAS_THREAD_PRIORITY_SCHEDULING
    if (mStackSize > 0)
    {
#    if defined(_POSIX_THREAD_ATTR_STACKSIZE) && (_POSIX_THREAD_ATTR_STACKSIZE - 0 > 0)
        int code = pthread_attr_setstacksize(&attr, mStackSize);
#    else
        int code = ENOSYS; // stack size not supported, automatically fail
#    endif // _POSIX_THREAD_ATTR_STACKSIZE
        if (code)
        {
            OCTK_WARNING("PlatformThread::start: Thread stack size error");
            // we failed to set the stacksize, and as the documentation states, the thread will fail to run...
            mRunning = false;
            mFinished = false;
            return false;
        }
    }

    pthread_t pthread;
    int code = pthread_create(&pthread, &attr, detail::thread::start, this);
    if (code == EPERM)
    {
        // caller does not have permission to set the scheduling parameters/policy
#    if defined(OCTK_HAS_THREAD_PRIORITY_SCHEDULING)
        pthread_attr_setinheritsched(&attr, PTHREAD_INHERIT_SCHED);
#    endif // defined(OCTK_HAS_THREAD_PRIORITY_SCHEDULING)
        code = pthread_create(&pthread, &attr, detail::thread::start, this);
    }
    mData->threadHandle = detail::toHandle(pthread);
    pthread_attr_destroy(&attr);
    return 0 == code;
}

void PlatformThreadPrivate::exit(int code) { }

bool PlatformThread::Id::isEqual(const Id &other) const noexcept
{
    return pthread_equal(detail::fromHandle<pthread_t>(mHandle), detail::fromHandle<pthread_t>(other.mHandle));
}

OCTK_END_NAMESPACE

#endif // !defined(OCTK_OS_WIN)