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

#include <octk_task_queue_factory.hpp>
#include <octk_platform_thread.hpp>
#include <octk_task_queue_old.hpp>
#include <octk_task_event.hpp>
#include <octk_date_time.hpp>
#include <octk_memory.hpp>
#include <octk_mutex.hpp>

#include <map>
#include <queue>

OCTK_BEGIN_NAMESPACE

namespace
{

ThreadPriority TaskQueuePriorityToThreadPriority(TaskQueueFactory::Priority priority)
{
    switch (priority)
    {
        case TaskQueueFactory::Priority::HIGH: return ThreadPriority::kRealtime;
        case TaskQueueFactory::Priority::LOW: return ThreadPriority::kLow;
        case TaskQueueFactory::Priority::NORMAL: return ThreadPriority::kNormal;
    }
    return ThreadPriority::kNormal;
}

class TaskQueueStdlib final : public TaskQueueOld
{
public:
    TaskQueueStdlib(StringView queue_name, ThreadPriority priority);
    ~TaskQueueStdlib() override = default;

    void Delete() override;

protected:
    void PostTaskImpl(Task task, const PostTaskTraits &traits, const SourceLocation &location) override;
    void PostDelayedTaskImpl(Task task,
                             TimeDelta delay,
                             const PostDelayedTaskTraits &traits,
                             const SourceLocation &location) override;

private:
    using OrderId = uint64_t;

    struct DelayedEntryTimeout
    {
        // TODO(bugs.webrtc.org/13756): Migrate to Timestamp.
        int64_t next_fire_at_us{};
        OrderId order{};

        bool operator<(const DelayedEntryTimeout &o) const
        {
            return std::tie(next_fire_at_us, order) < std::tie(o.next_fire_at_us, o.order);
        }
    };

    struct NextTask
    {
        bool final_task = false;
        Task run_task;
        TimeDelta sleep_time = Event::foreverDuration();
    };

    static PlatformThread InitializeThread(TaskQueueStdlib *me, StringView queue_name, ThreadPriority priority);

    NextTask GetNextTask();

    void ProcessTasks();

    void NotifyWake();

    // Signaled whenever a new task is pending.
    Event flag_notify_;

    Mutex pending_lock_;

    // Indicates if the worker thread needs to shutdown now.
    bool thread_should_quit_ OCTK_ATTRIBUTE_GUARDED_BY(pending_lock_) = false;

    // Holds the next order to use for the next task to be
    // put into one of the pending queues.
    OrderId thread_posting_order_ OCTK_ATTRIBUTE_GUARDED_BY(pending_lock_) = 0;

    // The list of all pending tasks that need to be processed in the
    // FIFO queue ordering on the worker thread.
    std::queue<std::pair<OrderId, Task>> pending_queue_ OCTK_ATTRIBUTE_GUARDED_BY(pending_lock_);

    // The list of all pending tasks that need to be processed at a future
    // time based upon a delay. On the off change the delayed task should
    // happen at exactly the same time interval as another task then the
    // task is processed based on FIFO ordering. std::priority_queue was
    // considered but rejected due to its inability to extract the
    // move-only value out of the queue without the presence of a hack.
    std::map<DelayedEntryTimeout, Task> delayed_queue_ OCTK_ATTRIBUTE_GUARDED_BY(pending_lock_);

    // Contains the active worker thread assigned to processing
    // tasks (including delayed tasks).
    // Placing this last ensures the thread doesn't touch uninitialized attributes
    // throughout it's lifetime.
    PlatformThread thread_;
};

TaskQueueStdlib::TaskQueueStdlib(StringView queue_name, ThreadPriority priority)
    : flag_notify_(/*manual_reset=*/false, /*initially_signaled=*/false)
    //, thread_(InitializeThread(this, queue_name, priority))
{
}

// static
PlatformThread TaskQueueStdlib::InitializeThread(TaskQueueStdlib *me, StringView queue_name, ThreadPriority priority)
{
    Event started;
    #if 0
    auto thread = PlatformThread::SpawnJoinable(
        [&started, me] {
            CurrentTaskQueueSetter set_current(me);
            started.Set();
            me->ProcessTasks();
        },
        queue_name, ThreadAttributes().SetPriority(priority));
    started.Wait(Event::foreverDuration());
    return thread;
    // #endif
// }

void TaskQueueStdlib::Delete()
{
    OCTK_DCHECK(!IsCurrent());

    {
        Mutex::Locker locker(&pending_lock_);
        thread_should_quit_ = true;
    }

    NotifyWake();

    delete this;
}

void TaskQueueStdlib::PostTaskImpl(Task task, const PostTaskTraits &traits, const SourceLocation &location)
{
    {
        Mutex::Locker locker(&pending_lock_);
        pending_queue_.push(std::make_pair(++thread_posting_order_, std::move(task)));
    }

    NotifyWake();
}

void TaskQueueStdlib::PostDelayedTaskImpl(Task task,
                                          TimeDelta delay,
                                          const PostDelayedTaskTraits &traits,
                                          const SourceLocation &location)
{
    DelayedEntryTimeout delayed_entry;
    delayed_entry.next_fire_at_us = DateTime::TimeMicros() + delay.us();

    {
        Mutex::Locker locker(&pending_lock_);
        delayed_entry.order = ++thread_posting_order_;
        delayed_queue_[delayed_entry] = std::move(task);
    }

    NotifyWake();
}

TaskQueueStdlib::NextTask TaskQueueStdlib::GetNextTask()
{
    NextTask result;

    const int64_t tick_us = DateTime::TimeMicros();

    Mutex::Locker locker(&pending_lock_);

    if (thread_should_quit_)
    {
        result.final_task = true;
        return result;
    }

    if (!delayed_queue_.empty())
    {
        auto delayed_entry = delayed_queue_.begin();
        const auto &delay_info = delayed_entry->first;
        auto &delay_run = delayed_entry->second;
        if (tick_us >= delay_info.next_fire_at_us)
        {
            if (!pending_queue_.empty())
            {
                auto &entry = pending_queue_.front();
                auto &entry_order = entry.first;
                auto &entry_run = entry.second;
                if (entry_order < delay_info.order)
                {
                    result.run_task = std::move(entry_run);
                    pending_queue_.pop();
                    return result;
                }
            }

            result.run_task = std::move(delay_run);
            delayed_queue_.erase(delayed_entry);
            return result;
        }

        result.sleep_time = TimeDelta::Millis(DivideRoundUp(delay_info.next_fire_at_us - tick_us, 1000));
    }

    if (!pending_queue_.empty())
    {
        auto &entry = pending_queue_.front();
        result.run_task = std::move(entry.second);
        pending_queue_.pop();
    }

    return result;
}

void TaskQueueStdlib::ProcessTasks()
{
    while (true)
    {
        auto task = GetNextTask();

        if (task.final_task)
        {
            break;
        }

        if (task.run_task)
        {
            // process entry immediately then try again
            std::move(task.run_task)();

            // Attempt to run more tasks before going to sleep.
            continue;
        }

        flag_notify_.Wait(task.sleep_time, task.sleep_time);
    }

    // Ensure remaining deleted tasks are destroyed with Current() set up to this
    // task queue.
    std::queue<std::pair<OrderId, Task>> pending_queue;
    {
        Mutex::Locker locker(&pending_lock_);
        pending_queue_.swap(pending_queue);
    }
    pending_queue = {};
#if OCTK_DCHECK_IS_ON
    Mutex::Locker locker(&pending_lock_);
    OCTK_DCHECK(pending_queue_.empty());
#endif
}

void TaskQueueStdlib::NotifyWake()
{
    // The queue holds pending tasks to complete. Either tasks are to be
    // executed immediately or tasks are to be run at some future delayed time.
    // For immediate tasks the task queue's thread is busy running the task and
    // the thread will not be waiting on the flag_notify_ event. If no immediate
    // tasks are available but a delayed task is pending then the thread will be
    // waiting on flag_notify_ with a delayed time-out of the nearest timed task
    // to run. If no immediate or pending tasks are available, the thread will
    // wait on flag_notify_ until signaled that a task has been added (or the
    // thread to be told to shutdown).

    // In all cases, when a new immediate task, delayed task, or request to
    // shutdown the thread is added the flag_notify_ is signaled after. If the
    // thread was waiting then the thread will wake up immediately and re-assess
    // what task needs to be run next (i.e. run a task now, wait for the nearest
    // timed delayed task, or shutdown the thread). If the thread was not waiting
    // then the thread will remained signaled to wake up the next time any
    // attempt to wait on the flag_notify_ event occurs.

    // Any immediate or delayed pending task (or request to shutdown the thread)
    // must always be added to the queue prior to signaling flag_notify_ to wake
    // up the possibly sleeping thread. This prevents a race condition where the
    // thread is notified to wake up but the task queue's thread finds nothing to
    // do so it waits once again to be signaled where such a signal may never
    // happen.
    flag_notify_.Set();
}

class TaskQueueStdlibFactory final : public TaskQueueFactory
{
public:
    std::unique_ptr<TaskQueueOld, TaskQueueDeleter> CreateTaskQueue(StringView name, Priority priority) const override
    {
        return std::unique_ptr<TaskQueueOld, TaskQueueDeleter>(
            new TaskQueueStdlib(name, TaskQueuePriorityToThreadPriority(priority)));
    }
};
} // namespace

namespace utils
{
std::unique_ptr<TaskQueueFactory> createDefaultTaskQueueFactory()
{
    return utils::make_unique<TaskQueueStdlibFactory>();
}
} // namespace utils
OCTK_END_NAMESPACE