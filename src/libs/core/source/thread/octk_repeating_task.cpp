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

#include <octk_repeating_task.hpp>
#include <octk_invocable.hpp>
#include <octk_logging.hpp>

OCTK_BEGIN_NAMESPACE

namespace
{

class RepeatingTask
{
public:
    RepeatingTask(TaskQueue *task_queue,
                  TaskQueue::DelayPrecision precision,
                  TimeDelta first_delay,
                  Invocable<TimeDelta()> task,
                  Clock *clock,
                  SharedRefPtr<PendingTaskSafetyFlag> alive_flag,
                  const SourceLocation &location);
    RepeatingTask(RepeatingTask &&) = default;
    RepeatingTask &operator=(RepeatingTask &&) = delete;
    ~RepeatingTask() = default;

    void operator()() &&;

private:
    TaskQueue *const mTaskQueue;
    const TaskQueue::DelayPrecision precision_;
    Clock *const clock_;
    const SourceLocation location_;
    Invocable<TimeDelta()> task_;
    // This is always finite.
    Timestamp next_run_time_ OCTK_ATTRIBUTE_GUARDED_BY(mTaskQueue);
    SharedRefPtr<PendingTaskSafetyFlag> alive_flag_
    OCTK_ATTRIBUTE_GUARDED_BY(mTaskQueue);
};

RepeatingTask::RepeatingTask(TaskQueue *task_queue,
                             TaskQueue::DelayPrecision precision,
                             TimeDelta first_delay,
                             Invocable<TimeDelta()> task,
                             Clock *clock,
                             SharedRefPtr<PendingTaskSafetyFlag> alive_flag,
                             const SourceLocation &location)
    : mTaskQueue(task_queue), precision_(precision), clock_(clock), location_(location), task_(std::move(task))
    , next_run_time_(clock_->CurrentTime() + first_delay), alive_flag_(std::move(alive_flag)) {}

void RepeatingTask::operator()() &&
{
    OCTK_DCHECK_RUN_ON(mTaskQueue);
    if (!alive_flag_->alive())
    {
        return;
    }

    detail::RepeatingTaskImplDTraceProbeRun();
    TimeDelta delay = task_();
    OCTK_DCHECK_GE(delay, TimeDelta::Zero());

    // A delay of +infinity means that the task should not be run again.
    // Alternatively, the closure might have stopped this task.
    if (delay.IsPlusInfinity() || !alive_flag_->alive())
    {
        return;
    }

    TimeDelta lost_time = clock_->CurrentTime() - next_run_time_;
    next_run_time_ += delay;
    delay -= lost_time;
    delay = std::max(delay, TimeDelta::Zero());

    mTaskQueue->PostDelayedTaskWithPrecision(precision_, std::move(*this), delay,
                                              location_);
}
}  // namespace

RepeatingTaskHandle RepeatingTaskHandle::Start(TaskQueue *task_queue,
                                               Invocable<TimeDelta()> closure,
                                               TaskQueue::DelayPrecision precision,
                                               Clock *clock,
                                               const SourceLocation &location)
{
    auto alive_flag = PendingTaskSafetyFlag::CreateDetached();
    detail::RepeatingTaskHandleDTraceProbeStart();
    task_queue->PostTask(RepeatingTask(task_queue, precision, TimeDelta::Zero(),
                                       std::move(closure), clock, alive_flag, location),
                         location);
    return RepeatingTaskHandle(std::move(alive_flag));
}

// DelayedStart is equivalent to Start except that the first invocation of the
// closure will be delayed by the given amount.
RepeatingTaskHandle RepeatingTaskHandle::DelayedStart(TaskQueue *task_queue,
                                                      TimeDelta first_delay,
                                                      Invocable<TimeDelta()> closure,
                                                      TaskQueue::DelayPrecision precision,
                                                      Clock *clock,
                                                      const SourceLocation &location)
{
    auto alive_flag = PendingTaskSafetyFlag::CreateDetached();
    detail::RepeatingTaskHandleDTraceProbeDelayedStart();
    task_queue->PostDelayedTaskWithPrecision(precision,
                                             RepeatingTask(task_queue, precision, first_delay, std::move(closure),
                                                           clock, alive_flag, location),
                                             first_delay, location);
    return RepeatingTaskHandle(std::move(alive_flag));
}

void RepeatingTaskHandle::Stop()
{
    if (repeating_task_)
    {
        repeating_task_->SetNotAlive();
        repeating_task_ = nullptr;
    }
}

bool RepeatingTaskHandle::Running() const
{
    return repeating_task_ != nullptr;
}

namespace detail
{
// These methods are empty, but can be externally equipped with actions using
// dtrace.
void RepeatingTaskHandleDTraceProbeStart() {}
void RepeatingTaskHandleDTraceProbeDelayedStart() {}
void RepeatingTaskImplDTraceProbeRun() {}
}  // namespace detail
OCTK_END_NAMESPACE
