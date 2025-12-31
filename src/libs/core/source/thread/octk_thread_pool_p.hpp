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

#ifndef _OCTK_THREAD_POOL_P_HPP
#define _OCTK_THREAD_POOL_P_HPP

#include "../tools/octk_assert.hpp"
#include "../tools/octk_logging.hpp"


#include <octk_thread_pool.hpp>
#include <octk_string_view.hpp>

#include <set>
#include <list>
#include <mutex>
#include <queue>
#include <thread>
#include <condition_variable>

OCTK_BEGIN_NAMESPACE

class ThreadPoolThread
{
public:
    using Task = ThreadPool::Task;

    using SharedPtr = std::shared_ptr<ThreadPoolThread>;

    ThreadPoolThread(ThreadPoolPrivate *manager)
        : mManager(manager)
    {
    }
    ~ThreadPoolThread() { this->exitWait(); }

    void setName(const StringView name) { mName = name.data(); }

    void wake();
    void wakeAll();

    void start();
    void exitWait();

    bool isFinished() const { return mFinished.load(); }

    Task::SharedPtr task() { return mTask; }
    void setTask(const Task::SharedPtr &task)
    {
        OCTK_ASSERT(nullptr == mTask.get());
        mTask = task;
    }

protected:
    void run();
    void registerThreadInactive();

private:
    std::string mName;
    std::thread mThread;
    Task::SharedPtr mTask;
    std::atomic<bool> mExit{true};
    std::atomic<bool> mFinished{true};
    ThreadPoolPrivate *const mManager;
    std::condition_variable mTaskReadyCondition;
};

class ThreadPoolTaskQueue
{
public:
    using Task = ThreadPool::Task;
    using Priority = ThreadPool::Priority;

    struct Item
    {
        Task::SharedPtr task;
        uint64_t sequenceId;
        Priority priority;

        bool operator<(const Item &other) const
        {
            if (priority != other.priority)
            {
                // Priority given to values with lower values
                return static_cast<int>(priority) > static_cast<int>(other.priority);
            }
            // When of the same priority, the one with a higher sequence number (inserted later) has a lower priority
            return sequenceId > other.sequenceId;
        }
    };

    ThreadPoolTaskQueue() = default;
    ~ThreadPoolTaskQueue() = default;

    void push(const Task::SharedPtr &task, Priority priority)
    {
        // push item with sequenceId priority
        mQueue.push({task, mIdCounter++, priority});
    }

    Task::SharedPtr first()
    {
        // get first item in queue
        return mQueue.top().task;
    }
    Task::SharedPtr pop()
    {
        auto task = std::move(mQueue.top().task);
        mQueue.pop();
        return task;
    }
    Task::SharedPtr tryPop()
    {
        if (mQueue.empty())
        {
            return nullptr;
        }
        auto task = std::move(mQueue.top().task);
        mQueue.pop();
        return task;
    }
    void clear()
    {
        while (!mQueue.empty())
        {
            mQueue.pop();
        }
    }

    size_t size() const { return mQueue.size(); }
    bool empty() const { return mQueue.empty(); }

private:
    std::priority_queue<Item> mQueue;
    std::atomic<uint64_t> mIdCounter{0};
};

class OCTK_CORE_API ThreadPoolPrivate
{
    friend class ThreadPoolThread;
    OCTK_DEFINE_PPTR(ThreadPool)
    OCTK_DECLARE_PUBLIC(ThreadPool)
    OCTK_DISABLE_COPY_MOVE(ThreadPoolPrivate)
public:
    using Task = ThreadPool::Task;
    using Priority = ThreadPool::Priority;

    explicit ThreadPoolPrivate(ThreadPool *p);
    virtual ~ThreadPoolPrivate();

    void enqueueTask(const Task::SharedPtr &task, Priority priority);
    void startThread(const Task::SharedPtr &task);
    bool tryStart(const Task::SharedPtr &task);
    void tryToStartMoreThreads();

    bool isTooManyThreadsActive() const;
    int activeThreadCount() const;

    bool isDone() const;
    void reset();

    mutable std::mutex mMutex;
    std::condition_variable mCondition;

    ThreadPoolTaskQueue mTaskQueue;
    std::list<ThreadPoolThread *> mWaitingThreads;
    std::list<ThreadPoolThread *> mExpiredThreads;
    std::set<ThreadPoolThread::SharedPtr> mAllThreads;
    std::condition_variable mNoActiveThreadsCondition;

    int mExpiryTimeout = 30000;
    int mActiveThreadCount = 0;
    int mReservedThreadCount = 0;
    std::atomic<int> mMaxQueueSize{256};
    int mMaxThreadCount = std::thread::hardware_concurrency();
};

OCTK_END_NAMESPACE

#endif // _OCTK_THREAD_POOL_P_HPP
