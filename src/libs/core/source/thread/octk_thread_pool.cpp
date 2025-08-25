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
#include <octk_memory.hpp>

#include <thread>

OCTK_BEGIN_NAMESPACE

ThreadPoolPrivate::ThreadPoolPrivate(ThreadPool *p)
    : mPPtr(p)
    , mQuit(false)
{
}

ThreadPoolPrivate::~ThreadPoolPrivate()
{
    // Signal to dispatch threads that it's time to wrap up
    std::unique_lock<std::mutex> lock(mMutex);
    mQuit = true;
    lock.unlock();
    mCondition.notify_all();

    // Wait for threads to finish before we exit
    for (size_t i = 0; i < mThreads.size(); i++)
    {
        if (mThreads[i].joinable())
        {
            mThreads[i].join();
        }
    }
}

void ThreadPoolPrivate::dispatch(void)
{
    std::unique_lock<std::mutex> lock(mMutex);
    do
    {
        //Wait until we have data or a quit signal
        mCondition.wait(lock, [this] {
            return (mQueue.size() || mQuit);
        });

        //after wait, we own the lock
        if (!mQuit && mQueue.size())
        {
            auto task = std::move(mQueue.front());
            mQueue.pop();

            //unlock now that we're done messing with the queue
            lock.unlock();
            task();
            lock.lock();
        }
    } while (!mQuit);
}

ThreadPool::ThreadPool(const std::string &name, size_t count)
    : mDPtr(utils::makeUnique<ThreadPoolPrivate>(this))
{
    mDPtr->mName = name;
    mDPtr->mThreads.resize(count);
    for (size_t i = 0; i < mDPtr->mThreads.size(); i++)
    {
        mDPtr->mThreads[i] = std::thread(&ThreadPoolPrivate::dispatch, mDPtr.get());
    }
}

ThreadPool::ThreadPool(ThreadPoolPrivate *d)
    : mDPtr(d)
{
}

ThreadPool::~ThreadPool()
{
}

ThreadPool *ThreadPool::defaultInstance()
{
    static std::once_flag once;
    static ThreadPool *instance;
    std::call_once(once, [=](){ instance = new ThreadPool("default", std::thread::hardware_concurrency()); });
    return instance;
}

void ThreadPool::runTask(const TaskFunc &task)
{
    OCTK_D(ThreadPool);
    std::unique_lock<std::mutex> lock(d->mMutex);
    d->mQueue.push(task);

    // Manual unlocking is done before notifying, to avoid waking up
    // the waiting thread only to block again (see notify_one for details)
    lock.unlock();
    d->mCondition.notify_one();
}

void ThreadPool::runTask(TaskFunc &&task)
{
    OCTK_D(ThreadPool);
    std::unique_lock<std::mutex> lock(d->mMutex);
    d->mQueue.push(std::move(task));

    // Manual unlocking is done before notifying, to avoid waking up
    // the waiting thread only to block again (see notify_one for details)
    lock.unlock();
    d->mCondition.notify_one();
}

void ThreadPool::removePending()
{
    OCTK_D(ThreadPool);
    std::unique_lock<std::mutex> lock(d->mMutex);
    d->mQueue = { };
}

OCTK_END_NAMESPACE