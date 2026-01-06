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

#ifndef _OCTK_TASK_QUEUE_HPP
#define _OCTK_TASK_QUEUE_HPP

#include <octk_source_location.hpp>
#include <octk_time_delta.hpp>
#include <octk_invocable.hpp>

#include <functional>

OCTK_BEGIN_NAMESPACE

enum class ThreadPriority
{
    kLow = 1,
    kNormal,
    kHigh,
    kRealtime,
};

#if 1

// Asynchronously executes tasks in a way that guarantees that they're executed
// in FIFO order and that tasks never overlap. Tasks may always execute on the
// same worker thread and they may not. To DCHECK that tasks are executing on a
// known task queue, use IsCurrent().
class OCTK_ATTRIBUTE_LOCKABLE OCTK_CORE_API TaskQueueOld
{
public:
    using Task = Invocable<void() &&>;

    enum class DelayPrecision
    {
        // This may include up to a 17 ms leeway in addition to OS timer precision.
        // See PostDelayedTask() for more information.
        kLow,
        // This does not have the additional delay that kLow has, but it is still
        // limited by OS timer precision. See PostDelayedHighPrecisionTask() for
        // more information.
        kHigh,
    };

    // Starts destruction of the task queue.
    // On return ensures no task are running and no new tasks are able to start
    // on the task queue.
    // Responsible for deallocation. Deallocation may happen synchronously during
    // Delete or asynchronously after Delete returns.
    // Code not running on the TaskQueueOld should not make any assumption when
    // TaskQueueOld is deallocated and thus should not call any methods after Delete.
    // Code running on the TaskQueueOld should not call Delete, but can assume
    // TaskQueueOld still exists and may call other methods, e.g. PostTask.
    // Should be called on the same task queue or thread that this task queue
    // was created on.
    virtual void Delete() = 0;

    // Schedules a `task` to execute. Tasks are executed in FIFO order.
    // When a TaskQueueOld is deleted, pending tasks will not be executed but they
    // will be deleted.
    //
    // As long as tasks are not posted from task destruction, posted tasks are
    // guaranteed to be destroyed with current() pointing to the task queue they
    // were posted to, whether they're executed or not. That means SequenceChecker
    // works during task destruction, a fact that can be used to guarantee
    // thread-compatible object deletion happening on a particular task queue
    // which can simplify class design.
    // Note that this guarantee does not apply to delayed tasks.
    //
    // May be called on any thread or task queue, including this task queue.
    void PostTask(Task task, const SourceLocation &location = SourceLocation::current())
    {
        PostTaskImpl(std::move(task), PostTaskTraits{}, location);
    }

    // Prefer PostDelayedTask() over PostDelayedHighPrecisionTask() whenever
    // possible.
    //
    // Schedules a `task` to execute a specified `delay` from when the call is
    // made, using "low" precision. All scheduling is affected by OS-specific
    // leeway and current workloads which means that in terms of precision there
    // are no hard guarantees, but in addition to the OS induced leeway, "low"
    // precision adds up to a 17 ms additional leeway. The purpose of this leeway
    // is to achieve more efficient CPU scheduling and reduce Idle Wake Up
    // frequency.
    //
    // The task may execute with [-1, 17 + OS induced leeway) ms additional delay.
    //
    // Avoid making assumptions about the precision of the OS scheduler. On macOS,
    // the OS induced leeway may be 10% of sleep interval. On Windows, 1 ms
    // precision timers may be used but there are cases, such as when running on
    // battery, when the timer precision can be as poor as 15 ms.
    //
    // "Low" precision is not implemented everywhere yet. Where not yet
    // implemented, PostDelayedTask() has "high" precision. See
    // https://crbug.com/webrtc/13583 for more information.
    //
    // May be called on any thread or task queue, including this task queue.
    void PostDelayedTask(Task task, TimeDelta delay, const SourceLocation &location = SourceLocation::current())
    {
        PostDelayedTaskImpl(std::move(task), delay, PostDelayedTaskTraits{}, location);
    }

    // Prefer PostDelayedTask() over PostDelayedHighPrecisionTask() whenever
    // possible.
    //
    // Schedules a `task` to execute a specified `delay` from when the call is
    // made, using "high" precision. All scheduling is affected by OS-specific
    // leeway and current workloads which means that in terms of precision there
    // are no hard guarantees.
    //
    // The task may execute with [-1, OS induced leeway] ms additional delay.
    //
    // Avoid making assumptions about the precision of the OS scheduler. On macOS,
    // the OS induced leeway may be 10% of sleep interval. On Windows, 1 ms
    // precision timers may be used but there are cases, such as when running on
    // battery, when the timer precision can be as poor as 15 ms.
    //
    // May be called on any thread or task queue, including this task queue.
    void PostDelayedHighPrecisionTask(Task task,
                                      TimeDelta delay,
                                      const SourceLocation &location = SourceLocation::current())
    {
        PostDelayedTaskTraits traits;
        traits.high_precision = true;
        PostDelayedTaskImpl(std::move(task), delay, traits, location);
    }

    // As specified by `precision`, calls either PostDelayedTask() or
    // PostDelayedHighPrecisionTask().
    void PostDelayedTaskWithPrecision(DelayPrecision precision,
                                      Task task,
                                      TimeDelta delay,
                                      const SourceLocation &location = SourceLocation::current())
    {
        switch (precision)
        {
            case DelayPrecision::kLow: PostDelayedTask(std::move(task), delay, location); break;
            case DelayPrecision::kHigh: PostDelayedHighPrecisionTask(std::move(task), delay, location); break;
        }
    }

    // Returns the task queue that is running the current thread.
    // Returns nullptr if this thread is not associated with any task queue.
    // May be called on any thread or task queue, including this task queue.
    static TaskQueueOld *Current();
    bool IsCurrent() const { return Current() == this; }

protected:
    // This is currently only present here to simplify introduction of future
    // planned task queue changes.
    struct PostTaskTraits
    {
    };

    struct PostDelayedTaskTraits
    {
        // If `high_precision` is false, tasks may execute within up to a 17 ms
        // leeway in addition to OS timer precision. Otherwise the task should be
        // limited to OS timer precision. See PostDelayedTask() and
        // PostDelayedHighPrecisionTask() for more information.
        bool high_precision = false;
    };

    class OCTK_CORE_API CurrentTaskQueueSetter
    {
    public:
        explicit CurrentTaskQueueSetter(TaskQueueOld *task_queue);
        CurrentTaskQueueSetter(const CurrentTaskQueueSetter &) = delete;
        CurrentTaskQueueSetter &operator=(const CurrentTaskQueueSetter &) = delete;
        ~CurrentTaskQueueSetter();

    private:
        TaskQueueOld *const previous_;
    };

    // Subclasses should implement this method to support the behavior defined in
    // the PostTask and PostTaskTraits docs above.
    virtual void PostTaskImpl(Task task, const PostTaskTraits &traits, const SourceLocation &location) = 0;

    // Subclasses should implement this method to support the behavior defined in
    // the PostDelayedTask/PostHighPrecisionDelayedTask and PostDelayedTaskTraits
    // docs above.
    virtual void PostDelayedTaskImpl(Task task,
                                     TimeDelta delay,
                                     const PostDelayedTaskTraits &traits,
                                     const SourceLocation &location) = 0;

    // Users of the TaskQueueOld should call Delete instead of directly deleting this object.
    virtual ~TaskQueueOld() = default;
};

struct TaskQueueDeleter
{
    void operator()(TaskQueueOld *task_queue) const { task_queue->Delete(); }
};

#endif
OCTK_END_NAMESPACE

#endif // _OCTK_TASK_QUEUE_HPP
