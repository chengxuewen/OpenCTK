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

#include "octk_simulated_task_queue.hpp"

OCTK_BEGIN_NAMESPACE

SimulatedTaskQueue::SimulatedTaskQueue(sim_time_impl::SimulatedTimeControllerImpl *handler,
                                       StringView name)
    : handler_(handler), mName(new char[name.size()])
{
    std::copy_n(name.begin(), name.size(), mName);
}

SimulatedTaskQueue::~SimulatedTaskQueue()
{
    handler_->Unregister(this);
    delete[] mName;
}

void SimulatedTaskQueue::Delete()
{
    // Need to destroy the tasks outside of the lock because task destruction
    // can lead to re-entry in SimulatedTaskQueue via custom destructors.
    std::deque<TaskQueueOld::Task> ready_tasks;
    std::map<Timestamp, std::vector<TaskQueueOld::Task>> delayed_tasks;
    {
        Mutex::Lock locker(lock_);
        ready_tasks_.swap(ready_tasks);
        delayed_tasks_.swap(delayed_tasks);
    }
    ready_tasks.clear();
    delayed_tasks.clear();
    delete this;
}

void SimulatedTaskQueue::RunReady(Timestamp at_time)
{
    Mutex::Lock locker(lock_);
    for (auto it = delayed_tasks_.begin();
         it != delayed_tasks_.end() && it->first <= at_time;
         it = delayed_tasks_.erase(it))
    {
        for (auto &task: it->second)
        {
            ready_tasks_.push_back(std::move(task));
        }
    }
    CurrentTaskQueueSetter set_current(this);
    while (!ready_tasks_.empty())
    {
        TaskQueueOld::Task ready = std::move(ready_tasks_.front());
        ready_tasks_.pop_front();
        lock_.unlock();
        std::move(ready)();
        ready = nullptr;
        lock_.lock();
    }
    if (!delayed_tasks_.empty())
    {
        next_run_time_ = delayed_tasks_.begin()->first;
    }
    else
    {
        next_run_time_ = Timestamp::PlusInfinity();
    }
}

void SimulatedTaskQueue::PostTaskImpl(TaskQueueOld::Task task,
                                      const PostTaskTraits & /*traits*/,
                                      const SourceLocation & /*location*/)
{
    Mutex::Lock locker(lock_);
    ready_tasks_.push_back(std::move(task));
    next_run_time_ = Timestamp::MinusInfinity();
}

void SimulatedTaskQueue::PostDelayedTaskImpl(TaskQueueOld::Task task,
                                             TimeDelta delay,
                                             const PostDelayedTaskTraits & /*traits*/,
                                             const SourceLocation & /*location*/)
{
    Mutex::Lock locker(lock_);
    Timestamp target_time = handler_->CurrentTime() + delay;
    delayed_tasks_[target_time].push_back(std::move(task));
    next_run_time_ = std::min(next_run_time_, target_time);
}
OCTK_END_NAMESPACE
