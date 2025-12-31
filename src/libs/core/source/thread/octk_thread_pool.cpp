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

#include <private/octk_thread_pool_p.hpp>
#include <octk_elapsed_timer.hpp>
#include <octk_scope_guard.hpp>
#include <octk_exception.hpp>
#include <octk_logging.hpp>
#include <octk_memory.hpp>
#include <octk_assert.hpp>

#include <thread>

OCTK_BEGIN_NAMESPACE

namespace detail
{
class ThreadPoolFunctionTask : public ThreadPool::Task
{
    std::function<void()> mFunction;

public:
    ThreadPoolFunctionTask(std::function<void()> &&function)
        : mFunction(std::move(function))
    {
    }

protected:
    void run() override
    {
        OCTK_TRACE("ThreadPoolFunctionTask::run()");
        mFunction();
    }
};
} // namespace detail

void ThreadPoolThread::start()
{
    OCTK_ASSERT_X(mFinished.load(), "ThreadPoolThread::start", "not finished");
    if (mThread.joinable())
    {
        mThread.join();
    }
    mThread = std::thread(&ThreadPoolThread::run, this);
}

void ThreadPoolThread::exitWait()
{
    OCTK_TRACE("thread %p exitWait", this);
    mExit.store(true);
    if (mThread.joinable())
    {
        OCTK_TRACE("thread %p exitWait join", this);
        mThread.join();
    }
}

void ThreadPoolThread::wake()
{
    OCTK_TRACE("thread %p wake", this);
    mTaskReadyCondition.notify_one();
}

void ThreadPoolThread::wakeAll()
{
    OCTK_TRACE("thread %p wake all", this);
    mTaskReadyCondition.notify_all();
}

void ThreadPoolThread::run()
{
    auto finishedScopeGuard = utils::makeScopeGuard(
        [=]()
        {
            OCTK_TRACE("thread %p run enter", this);
            mExit.store(false);
            mFinished.store(false);
        },
        [=]()
        {
            OCTK_TRACE("thread %p run exit", this);
            mFinished.store(true);
        });
    std::unique_lock<std::mutex> lock(mManager->mMutex);
    while (!mExit.load())
    {
        auto task = std::move(mTask);
        do
        {
            OCTK_TRACE("thread %p do", this);
            if (task)
            {
                lock.unlock();
                OCTK_TRY
                {
                    OCTK_TRACE("");
                    task->run();
                }
                OCTK_CATCH(...)
                {
                    OCTK_WARNING("OCTK Concurrent has caught an exception thrown from a worker thread.\n"
                                 "This is not supported, exceptions thrown in worker threads must be\n"
                                 "caught before control returns to OCTK Concurrent.");
                    this->registerThreadInactive();
                    throw;
                }
                lock.lock();
            }

            // if too many threads are active, exit do task loop
            if (mManager->isTooManyThreadsActive())
            {
                OCTK_TRACE("thread %p do isTooManyThreadsActive true", this);
                break;
            }
            // if task queue is empty, exit do task loop
            task = mManager->mTaskQueue.tryPop();
            if (!task)
            {
                OCTK_TRACE("thread %p do TaskQueue empty", this);
                break;
            }
        } while (!mExit.load());

        // if too many threads are active or exit flag is set, expire this thread
        bool expired = mManager->isTooManyThreadsActive() || mExit.load();
        if (!expired)
        {
            OCTK_TRACE("thread %p isTooManyThreadsActive false", this);
            // start enter waiting state
            mManager->mWaitingThreads.push_back(this);
            this->registerThreadInactive();
            if (mExit.load())
            {
                OCTK_TRACE("thread %p is exit set expired", this);
                expired = true;
            }
            else
            {
                // wait for work, exiting after the expiry timeout is reached
                OCTK_TRACE("thread %p TaskReadyCondition start wait, expiry timeout: %d ms, joinable:%d",
                           this,
                           mManager->mExpiryTimeout,
                           mThread.joinable());
                mTaskReadyCondition.wait_for(lock, std::chrono::milliseconds(mManager->mExpiryTimeout));
                OCTK_TRACE("thread %p TaskReadyCondition finish wait, expiry timeout: %d ms",
                           this,
                           mManager->mExpiryTimeout);
                // start exit waiting state
                ++mManager->mActiveThreadCount;
            }
            // erase if this thread is still in the waiting list
            {
                const auto iter = std::find(mManager->mWaitingThreads.begin(), mManager->mWaitingThreads.end(), this);
                if (mManager->mWaitingThreads.end() != iter)
                {
                    OCTK_TRACE("thread %p is still in the waiting list", this);
                    mManager->mWaitingThreads.erase(iter);
                    expired = true;
                }
            }
            // check if this thread is no longer in the all threads list (manager maybe reset)
            {
                const auto iter = std::find_if(mManager->mAllThreads.begin(),
                                               mManager->mAllThreads.end(),
                                               [this](const SharedPtr &thread) { return thread.get() == this; });
                if (mManager->mAllThreads.end() == iter)
                {
                    OCTK_TRACE("thread %p is not in the all threads list", this);
                    expired = true;
                }
            }
        }
        if (expired)
        {
            OCTK_TRACE("thread %p is expired", this);
            mManager->mExpiredThreads.push_back(this);
            this->registerThreadInactive();
            break;
        }
    }
}

void ThreadPoolThread::registerThreadInactive()
{
    OCTK_ASSERT_X(mManager->mActiveThreadCount > 0,
                  "ThreadPoolThread::registerThreadInactive()",
                  "mActiveThreadCount must be greater than 0");
    OCTK_TRACE("thread %p registerThreadInactive", this);
    if (--mManager->mActiveThreadCount == 0)
    {
        OCTK_TRACE("thread %p registerThreadInactive mNoActiveThreadsCondition", this);
        mManager->mNoActiveThreadsCondition.notify_all();
    }
}

ThreadPoolPrivate::ThreadPoolPrivate(ThreadPool *p)
    : mPPtr(p)
{
}

ThreadPoolPrivate::~ThreadPoolPrivate() { }

void ThreadPoolPrivate::enqueueTask(const Task::SharedPtr &task, Priority priority)
{
    OCTK_ASSERT(nullptr != task);
    mTaskQueue.push(task, priority);
}

void ThreadPoolPrivate::startThread(const Task::SharedPtr &task)
{
    OCTK_ASSERT(nullptr != task.get());
    ThreadPoolThread::SharedPtr thread(new ThreadPoolThread(this));
    // if this assert hits, we have an ABA problem (deleted threads don't get removed here)
    OCTK_ASSERT(mAllThreads.find(thread) == mAllThreads.end());
    thread->setName(("Thread (pooled)"));
    mAllThreads.insert(thread);
    ++mActiveThreadCount;
    thread->setTask(task);
    thread->start();
}

bool ThreadPoolPrivate::tryStart(const Task::SharedPtr &task)
{
    OCTK_ASSERT(task != nullptr);

    if (mAllThreads.empty())
    {
        // always create at least one thread
        this->startThread(task);
        return true;
    }

    // can't do anything if we're over the limit
    if (this->activeThreadCount() >= mMaxThreadCount)
    {
        return false;
    }

    if (mWaitingThreads.size() > 0)
    {
        // recycle an available thread
        this->enqueueTask(task, Priority::kHighest);
        auto thread = mWaitingThreads.front();
        OCTK_ASSERT(!thread->task().get());
        mWaitingThreads.pop_front();
        thread->wake();
        return true;
    }

    if (!mExpiredThreads.empty())
    {
        // restart an expired thread
        auto thread = mExpiredThreads.front();
        OCTK_ASSERT(!thread->task().get());
        mExpiredThreads.pop_front();
        ++mActiveThreadCount;
        thread->setTask(task);
        thread->start();
        return true;
    }

    // start a new thread
    this->startThread(task);
    return true;
}

void ThreadPoolPrivate::tryToStartMoreThreads()
{
    // try to push tasks on the queue to any available threads
    while (!mTaskQueue.empty())
    {
        if (!this->tryStart(mTaskQueue.first()))
        {
            break;
        }
        mTaskQueue.pop();
    }
}

bool ThreadPoolPrivate::isTooManyThreadsActive() const
{
    const int activeThreadCount = this->activeThreadCount();
    return activeThreadCount > mMaxThreadCount && (activeThreadCount - mReservedThreadCount) > 1;
}

int ThreadPoolPrivate::activeThreadCount() const
{
    return mAllThreads.size() - mExpiredThreads.size() - mWaitingThreads.size() + mReservedThreadCount;
}

bool ThreadPoolPrivate::isDone() const { return mTaskQueue.empty() && 0 == mActiveThreadCount; }

void ThreadPoolPrivate::reset()
{
    const auto allThreads = std::move(mAllThreads);
    mExpiredThreads.clear();
    mWaitingThreads.clear();
    mMutex.unlock();
    for (auto &thread : allThreads)
    {
        if (!thread->isFinished())
        {
            OCTK_TRACE("thread %p is not finished, wake and exitWait", thread.get());
            thread->wakeAll();
            thread->exitWait();
            OCTK_TRACE("thread %p exitWait done", thread.get());
        }
    }
    mMutex.lock();
    OCTK_TRACE("reset done");
}

ThreadPool::ThreadPool()
    : ThreadPool(new ThreadPoolPrivate(this))
{
}

ThreadPool::ThreadPool(ThreadPoolPrivate *d)
    : mDPtr(d)
{
}

ThreadPool::~ThreadPool() { this->waitForDone(); }

ThreadPool *ThreadPool::defaultInstance()
{
    static std::once_flag once;
    static ThreadPool *instance;
    std::call_once(once, [=]() { instance = new ThreadPool; });
    return instance;
}

void ThreadPool::start(std::function<void()> function, Priority priority)
{
    if (function)
    {
        this->start(Task::create(std::move(function)), priority);
    }
}

bool ThreadPool::tryStart(std::function<void()> function)
{
    if (!function)
    {
        return false;
    }

    OCTK_D(ThreadPool);
    std::unique_lock<std::mutex> lock(d->mMutex);
    if (!d->mAllThreads.empty() && d->activeThreadCount() >= d->mMaxThreadCount)
    {
        return false;
    }

    auto task = Task::create(std::move(function));
    if (!d->tryStart(task))
    {
        return false;
    }

    return true;
}

void ThreadPool::start(const Task::SharedPtr &task, Priority priority)
{
    if (task)
    {
        OCTK_D(ThreadPool);
        std::unique_lock<std::mutex> lock(d->mMutex);
        if (!d->tryStart(task))
        {
            if (!d->mWaitingThreads.empty())
            {
                auto thread = d->mWaitingThreads.front();
                OCTK_ASSERT(!thread->task().get());
                d->mWaitingThreads.pop_front();
                thread->setTask(task);
                thread->wake();
            }
            else
            {
                d->enqueueTask(task, priority);
            }
        }
    }
}

bool ThreadPool::tryStart(const Task::SharedPtr &task)
{
    if (!task)
    {
        return false;
    }

    OCTK_D(ThreadPool);
    std::unique_lock<std::mutex> lock(d->mMutex);
    if (!d->mAllThreads.empty() && d->activeThreadCount() >= d->mMaxThreadCount)
    {
        return false;
    }

    if (!d->tryStart(task))
    {
        return false;
    }

    return true;
}

int ThreadPool::maxThreadCount() const
{
    OCTK_D(const ThreadPool);
    std::lock_guard<std::mutex> lock(d->mMutex);
    return d->mMaxThreadCount;
}

void ThreadPool::setMaxThreadCount(int count)
{
    OCTK_D(ThreadPool);
    std::lock_guard<std::mutex> lock(d->mMutex);
    if (count != d->mMaxThreadCount)
    {
        d->mMaxThreadCount = count;
        d->tryToStartMoreThreads();
    }
}

int ThreadPool::expiryTimeout() const
{
    OCTK_D(const ThreadPool);
    std::lock_guard<std::mutex> lock(d->mMutex);
    return d->mExpiryTimeout;
}

void ThreadPool::setExpiryTimeout(int msecs)
{
    OCTK_D(ThreadPool);
    std::lock_guard<std::mutex> lock(d->mMutex);
    if (msecs != d->mExpiryTimeout)
    {
        d->mExpiryTimeout = msecs;
    }
}

bool ThreadPool::waitForDone(unsigned long msecs)
{
    OCTK_D(ThreadPool);
    ElapsedTimer timer;
    std::unique_lock<std::mutex> lock(d->mMutex);
    do
    {
        OCTK_TRACE("waitForDone() do");
        while (!d->isDone() && timer.elapsed() < msecs)
        {
            if (kWaitForeverMSecs == msecs)
            {
                OCTK_TRACE("waitForDone() do wait forever");
                d->mNoActiveThreadsCondition.wait(lock);
            }
            else
            {
                OCTK_TRACE("waitForDone() do wait %d ms", msecs);
                d->mNoActiveThreadsCondition.wait_for(lock, std::chrono::milliseconds(msecs));
            }
        }
        if (!d->isDone())
        {
            OCTK_TRACE("waitForDone() do !isDone return false");
            return false;
        }
        d->reset();
        // More threads can be started during reset(), in that case continue waiting if we still have time left.
    } while (!d->isDone() && timer.elapsed() < msecs);
    OCTK_TRACE("waitForDone() do finish:%d", d->isDone());
    return d->isDone();
}

// bool ThreadPool::contains(Task *task) const
// {
//
// }

// void ThreadPool::cancel(Task *task)
// {
//     if (this->ake(runnable) && runnable->autoDelete() && !runnable->ref) // tryTake already deref'ed
//         delete runnable;
// }
//
// bool ThreadPool::take(Task *task)
// {
//
// }

void ThreadPool::clear()
{
    OCTK_D(ThreadPool);
    while (!d->mTaskQueue.empty())
    {
        d->mTaskQueue.pop();
    }
}

int ThreadPool::maxQueueSize() const
{
    OCTK_D(const ThreadPool);
    return d->mMaxQueueSize.load();
}

ThreadPool::Task::SharedPtr ThreadPool::Task::create(std::function<void()> function)
{
    return SharedPtr(new detail::ThreadPoolFunctionTask(std::move(function)), Deleter{true});
}

ThreadPool::Task::SharedPtr ThreadPool::Task::makeShared(Task *task, bool autoDelete)
{
    return SharedPtr(task, Deleter{autoDelete});
}

OCTK_END_NAMESPACE