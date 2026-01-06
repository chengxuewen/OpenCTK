/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2026~Present ChengXueWen.
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

#include <octk_task_queue_thread.hpp>
#include <octk_date_time.hpp>
#include <octk_timestamp.hpp>
#include <octk_mutex.hpp>

#include <set>
#include <map>
#include <thread>

OCTK_BEGIN_NAMESPACE

class TaskQueueThreadPrivate
{
    OCTK_DEFINE_PPTR(TaskQueueThread)
    OCTK_DECLARE_PUBLIC(TaskQueueThread)
    OCTK_DISABLE_COPY_MOVE(TaskQueueThreadPrivate)
public:
    struct PendingTask
    {
        Task::Id id;
        Task::SharedPtr task;
        struct Compare
        {
            bool operator()(const PendingTask &lhs, const PendingTask &rhs) const { return lhs.id < rhs.id; }
        };
    };
    using PendingTasksSet = std::set<PendingTask, PendingTask::Compare>;
    struct DelayedTask
    {
        Task::Id id;
        int64_t timestamp; // usecs
        Task::SharedPtr task;
        struct Compare
        {
            bool operator()(const DelayedTask &lhs, const DelayedTask &rhs) const
            {
                return std::tie(lhs.timestamp, lhs.id) < std::tie(rhs.timestamp, rhs.id);
            }
        };
    };
    using DelayedTasksSet = std::set<DelayedTask, DelayedTask::Compare>;

    explicit TaskQueueThreadPrivate(TaskQueueThread *p);
    ~TaskQueueThreadPrivate() = default;

    void init();

    std::thread mThread;
    RecursiveMutex mMutex;
    RecursiveMutex::Condition mTaskReadyCondition;
    bool mQuit OCTK_ATTRIBUTE_GUARDED_BY(mMutex) = false;
    Task::Id mTaskIdCounter OCTK_ATTRIBUTE_GUARDED_BY(mMutex) = 0;
    PendingTasksSet mPendingTasks OCTK_ATTRIBUTE_GUARDED_BY(mMutex);
    DelayedTasksSet mDelayedTasks OCTK_ATTRIBUTE_GUARDED_BY(mMutex);
};

TaskQueueThreadPrivate::TaskQueueThreadPrivate(TaskQueueThread *p)
    : mPPtr(p)
{
}

void TaskQueueThreadPrivate::init()
{
    RecursiveMutex::Condition started;
    RecursiveMutex::UniqueLock lock(mMutex);
    mThread = std::thread(
        [this, &started]()
        {
            OCTK_LOGGING_TRACE(OCTK_TASK_QUEUE_LOGGER(), "TaskQueueThreadPrivate: thread started");
            TaskQueueThread::CurrentSetter currentSetter(mPPtr);
            started.notify_all();
            mPPtr->processTasks();
            OCTK_LOGGING_TRACE(OCTK_TASK_QUEUE_LOGGER(), "TaskQueueThreadPrivate: thread finished");
        });
    started.wait(lock);
    OCTK_LOGGING_TRACE(OCTK_TASK_QUEUE_LOGGER(), "TaskQueueThreadPrivate: constructor done");
}

TaskQueueThread::TaskQueueThread()
    : mDPtr(new TaskQueueThreadPrivate(this))
{
    mDPtr->init();
}

TaskQueueThread::SharedPtr TaskQueueThread::makeShared()
{
    return SharedPtr(new TaskQueueThread, [](TaskQueueThread *thread) { thread->destroy(); });
}

TaskQueueThread::UniquePtr TaskQueueThread::makeUnique()
{
    return UniquePtr(new TaskQueueThread);
}

TaskQueueThread::~TaskQueueThread()
{
}

void TaskQueueThread::destroy()
{
    OCTK_D(TaskQueueThread);
    OCTK_LOGGING_TRACE(OCTK_TASK_QUEUE_LOGGER(), "TaskQueueThread::destroy()");
    OCTK_ASSERT(!this->isCurrent());
    {
        RecursiveMutex::Lock lock(d->mMutex);
        d->mQuit = true;
        OCTK_LOGGING_TRACE(OCTK_TASK_QUEUE_LOGGER(), "TaskQueueThread::destroy() notify_all");
        d->mTaskReadyCondition.notify_all();
    }
    if (d->mThread.joinable())
    {
        d->mThread.join();
    }
    OCTK_LOGGING_TRACE(OCTK_TASK_QUEUE_LOGGER(), "TaskQueueThread::destroy() delete");
    delete this;
}

bool TaskQueueThread::cancelTask(const Task *task)
{
    OCTK_D(TaskQueueThread);
    RecursiveMutex::Lock lock(d->mMutex);
    bool canceled = false;
    for (auto iter = d->mPendingTasks.begin(); iter != d->mPendingTasks.end();)
    {
        if (iter->task.get() == task)
        {
            iter = d->mPendingTasks.erase(iter);
            canceled = true;
        }
        else
        {
            ++iter;
        }
    }
    for (auto iter = d->mDelayedTasks.begin(); iter != d->mDelayedTasks.end();)
    {
        if (iter->task.get() == task)
        {
            iter = d->mDelayedTasks.erase(iter);
            canceled = true;
        }
        else
        {
            ++iter;
        }
    }
    return canceled;
}

void TaskQueueThread::postTask(const Task::SharedPtr &task, const SourceLocation &location)
{
    OCTK_D(TaskQueueThread);
    RecursiveMutex::Lock lock(d->mMutex);
    d->mPendingTasks.insert({++d->mTaskIdCounter, std::move(task)});
    d->mTaskReadyCondition.notify_one();
}

void TaskQueueThread::postDelayedTask(const Task::SharedPtr &task,
                                      const TimeDelta &delay,
                                      const SourceLocation &location)
{
    OCTK_D(TaskQueueThread);
    RecursiveMutex::Lock lock(d->mMutex);
    d->mDelayedTasks.insert({++d->mTaskIdCounter, DateTime::steadyTimeUSecs() + delay.us(), std::move(task)});
    d->mTaskReadyCondition.notify_one();
    OCTK_LOGGING_TRACE(OCTK_TASK_QUEUE_LOGGER(), "TaskQueueThread: postDelayedTask");
}

TaskQueueThread::NextTask TaskQueueThread::popNextTask()
{
    OCTK_D(TaskQueueThread);
    OCTK_LOGGING_TRACE(OCTK_TASK_QUEUE_LOGGER(), "TaskQueueThread::popNextTask()");
    NextTask result;
    const int64_t tickUSecs = DateTime::steadyTimeUSecs();
    RecursiveMutex::Lock lock(d->mMutex);
    if (d->mQuit)
    {
        result.finalTask = true;
        return result;
    }

    if (!d->mDelayedTasks.empty())
    {
        auto delayedTask = d->mDelayedTasks.begin();
        const auto delayedTaskTimestamp = delayedTask->timestamp;
        if (tickUSecs >= delayedTaskTimestamp)
        {
            if (!d->mPendingTasks.empty())
            {
                auto pendingTask = d->mPendingTasks.begin();
                if (pendingTask->id < delayedTask->id)
                {
                    result.runTask = std::move(pendingTask->task);
                    d->mPendingTasks.erase(pendingTask);
                    return result;
                }
            }

            result.runTask = std::move(delayedTask->task);
            d->mDelayedTasks.erase(delayedTask);
            return result;
        }

        result.sleepTime = TimeDelta::Millis(DivideRoundUp(delayedTaskTimestamp - tickUSecs, 1'000));
    }

    if (!d->mPendingTasks.empty())
    {
        auto pendingTask = d->mPendingTasks.begin();
        result.runTask = std::move(pendingTask->task);
        d->mPendingTasks.erase(pendingTask);
    }

    return result;
}

void TaskQueueThread::processTasks()
{
    OCTK_D(TaskQueueThread);
    RecursiveMutex::UniqueLock lock(d->mMutex);
    lock.unlock();
    while (true)
    {
        OCTK_LOGGING_TRACE(OCTK_TASK_QUEUE_LOGGER(), "TaskQueueThread::processTasks() loop");
        const auto nextTask = this->popNextTask();
        if (nextTask.finalTask)
        {
            break;
        }

        if (nextTask.runTask)
        {
            OCTK_LOGGING_TRACE(OCTK_TASK_QUEUE_LOGGER(),
                               "TaskQueueThread::processTasks() runTask:%p",
                               nextTask.runTask.get());
            // process entry immediately then try again
            nextTask.runTask->run();
            // Attempt to run more tasks before going to sleep.
            continue;
        }

        lock.lock();
        OCTK_LOGGING_TRACE(OCTK_TASK_QUEUE_LOGGER(),
                           "TaskQueueThread::processTasks() wait %d us",
                           nextTask.sleepTime.us());
        const auto deadline = std::chrono::steady_clock::now() +
                              std::chrono::microseconds(std::min(nextTask.sleepTime.us(), 1'000'000LL));
        d->mTaskReadyCondition.wait_until(lock, deadline);
        lock.unlock();
    }
    OCTK_LOGGING_TRACE(OCTK_TASK_QUEUE_LOGGER(), "TaskQueueThread::processTasks() break loop");
    lock.lock();
    // Ensure remaining deleted tasks are destroyed with Current() set up to this task queue.
    d->mPendingTasks.clear();
    OCTK_LOGGING_TRACE(OCTK_TASK_QUEUE_LOGGER(), "TaskQueueThread::processTasks() done");
}

OCTK_END_NAMESPACE
