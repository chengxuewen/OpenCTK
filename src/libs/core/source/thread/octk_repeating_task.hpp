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

#ifndef _OCTK_REPEATING_TASK_HPP
#define _OCTK_REPEATING_TASK_HPP

#include <octk_pending_task_safety_flag.hpp>
#include <octk_task_queue.hpp>
#include <octk_time_delta.hpp>
#include <octk_clock.hpp>

#include <type_traits>
#include <utility>
#include <memory>

OCTK_BEGIN_NAMESPACE

namespace detail
{
// Methods simplifying external tracing of RepeatingTaskHandle operations.
void RepeatingTaskHandleDTraceProbeStart();
void RepeatingTaskHandleDTraceProbeDelayedStart();
void RepeatingTaskImplDTraceProbeRun();
}  // namespace detail

// Allows starting tasks that repeat themselves on a TaskQueue indefinately
// until they are stopped or the TaskQueue is destroyed. It allows starting and
// stopping multiple times, but you must stop one task before starting another
// and it can only be stopped when in the running state. The public interface is
// not thread safe.
class RepeatingTaskHandle
{
public:
    RepeatingTaskHandle() = default;
    ~RepeatingTaskHandle() = default;
    RepeatingTaskHandle(RepeatingTaskHandle &&other) = default;
    RepeatingTaskHandle &operator=(RepeatingTaskHandle &&other) = default;
    RepeatingTaskHandle(const RepeatingTaskHandle &) = delete;
    RepeatingTaskHandle &operator=(const RepeatingTaskHandle &) = delete;

    // Start can be used to start a task that will be reposted with a delay
    // determined by the return value of the provided closure. The actual task is
    // owned by the TaskQueue and will live until it has been stopped or the
    // TaskQueue deletes it. It's perfectly fine to destroy the handle while the
    // task is running, since the repeated task is owned by the TaskQueue.
    // The tasks are scheduled onto the task queue using the specified precision.
    static RepeatingTaskHandle Start(TaskQueue *task_queue,
                                     Invocable<TimeDelta()> closure,
                                     TaskQueue::DelayPrecision precision =
                                     TaskQueue::DelayPrecision::kLow,
                                     Clock *clock = Clock::GetRealTimeClock(),
                                     const SourceLocation &location = SourceLocation::current());

    // DelayedStart is equivalent to Start except that the first invocation of the
    // closure will be delayed by the given amount.
    static RepeatingTaskHandle DelayedStart(TaskQueue *task_queue,
                                            TimeDelta first_delay,
                                            Invocable<TimeDelta()> closure,
                                            TaskQueue::DelayPrecision precision =
                                            TaskQueue::DelayPrecision::kLow,
                                            Clock *clock = Clock::GetRealTimeClock(),
                                            const SourceLocation &location = SourceLocation::current());

    // Stops future invocations of the repeating task closure. Can only be called
    // from the TaskQueue where the task is running. The closure is guaranteed to
    // not be running after Stop() returns unless Stop() is called from the
    // closure itself.
    void Stop();

    // Returns true until Stop() was called.
    // Can only be called from the TaskQueue where the task is running.
    bool Running() const;

private:
    explicit RepeatingTaskHandle(SharedRefPtr<PendingTaskSafetyFlag> alive_flag)
        : repeating_task_(std::move(alive_flag)) {}
    SharedRefPtr<PendingTaskSafetyFlag> repeating_task_;
};
OCTK_END_NAMESPACE

#endif // _OCTK_REPEATING_TASK_HPP
