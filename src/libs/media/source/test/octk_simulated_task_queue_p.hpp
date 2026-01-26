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

#ifndef _OCTK_SIMULATED_TASK_QUEUE_P_HPP
#define _OCTK_SIMULATED_TASK_QUEUE_P_HPP

#include <test/octk_simulated_time_controller_p.hpp>
#include <octk_task_queue.hpp>
#include <octk_time_delta.hpp>
#include <octk_mutex.hpp>

#include <memory>
#include <vector>
#include <deque>
#include <map>

OCTK_BEGIN_NAMESPACE

// TODO: move to core thread?
class SimulatedTaskQueue : public TaskQueueOld, public sim_time_impl::SimulatedSequenceRunner
{
public:
    SimulatedTaskQueue(sim_time_impl::SimulatedTimeControllerImpl *handler, StringView name);

    ~SimulatedTaskQueue();

    void RunReady(Timestamp at_time) override;

    Timestamp GetNextRunTime() const override
    {
        Mutex::Lock locker(lock_);
        return next_run_time_;
    }
    TaskQueueOld *GetAsTaskQueue() override { return this; }

    // TaskQueueOld interface
    void Delete() override;
    void PostTaskImpl(TaskQueueOld::Task task, const PostTaskTraits &traits, const SourceLocation &location) override;
    void PostDelayedTaskImpl(TaskQueueOld::Task task,
                             TimeDelta delay,
                             const PostDelayedTaskTraits &traits,
                             const SourceLocation &location) override;

private:
    sim_time_impl::SimulatedTimeControllerImpl *const handler_;
    // Using char* to be debugger friendly.
    char *mName;

    mutable Mutex lock_;

    std::deque<TaskQueueOld::Task> ready_tasks_ OCTK_ATTRIBUTE_GUARDED_BY(lock_);
    std::map<Timestamp, std::vector<TaskQueueOld::Task>> delayed_tasks_ OCTK_ATTRIBUTE_GUARDED_BY(lock_);

    Timestamp next_run_time_ OCTK_ATTRIBUTE_GUARDED_BY(lock_) = Timestamp::PlusInfinity();
};
OCTK_END_NAMESPACE

#endif // _OCTK_SIMULATED_TASK_QUEUE_P_HPP
