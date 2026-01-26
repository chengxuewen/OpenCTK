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

#include <test/octk_simulated_time_controller_p.hpp>
#include <test/octk_simulated_task_queue_p.hpp>
#include <test/octk_simulated_thread_p.hpp>
#include <octk_memory.hpp>

#include <algorithm>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include <deque>
#include <list>

#if 0
OCTK_BEGIN_NAMESPACE

namespace
{
// Helper function to remove from a std container by value.
template <class C>
bool RemoveByValue(C *vec, typename C::value_type val)
{
    auto it = std::find(vec->begin(), vec->end(), val);
    if (it == vec->end())
    {
        return false;
    }
    vec->erase(it);
    return true;
}
}  // namespace

namespace sim_time_impl
{

SimulatedTimeControllerImpl::SimulatedTimeControllerImpl(Timestamp start_time)
    : thread_id_(PlatformThread::currentThreadId()), current_time_(start_time) {}

SimulatedTimeControllerImpl::~SimulatedTimeControllerImpl() = default;

std::unique_ptr<TaskQueueOld, TaskQueueDeleter>
SimulatedTimeControllerImpl::CreateTaskQueue(StringView name,
                                             TaskQueueFactory::Priority priority) const
{
    // TODO(srte): Remove the const cast when the interface is made mutable.
    auto mutable_this = const_cast<SimulatedTimeControllerImpl *>(this);
    auto task_queue = std::unique_ptr<SimulatedTaskQueue, TaskQueueDeleter>(
        new SimulatedTaskQueue(mutable_this, name));
    mutable_this->Register(task_queue.get());
    return task_queue;
}

std::unique_ptr<TaskThread> SimulatedTimeControllerImpl::CreateThread(const std::string &name,
                                                                      std::unique_ptr<SocketServer> socket_server)
{
    auto thread = utils::make_unique<SimulatedThread>(this, name, std::move(socket_server));
    Register(thread.get());
    return thread;
}

void SimulatedTimeControllerImpl::YieldExecution()
{
    if (PlatformThread::currentThreadId() == thread_id_)
    {
        TaskQueueOld *yielding_from = TaskQueueOld::Current();
        // Since we might continue execution on a process thread, we should reset
        // the thread local task queue reference. This ensures that thread checkers
        // won't think we are executing on the yielding task queue. It also ensure
        // that TaskQueueOld::Current() won't return the yielding task queue.
        TokenTaskQueue::CurrentTaskQueueSetter reset_queue(nullptr);
        // When we yield, we don't want to risk executing further tasks on the
        // currently executing task queue. If there's a ready task that also yields,
        // it's added to this set as well and only tasks on the remaining task
        // queues are executed.
        auto inserted = yielded_.insert(yielding_from);
        OCTK_DCHECK(inserted.second);
        RunReadyRunners();
        yielded_.erase(inserted.first);
    }
}

void SimulatedTimeControllerImpl::RunReadyRunners()
{
    // Using a dummy thread rather than nullptr to avoid implicit thread creation
    // by Thread::Current().
    SimulatedThread::CurrentThreadSetter set_current(dummy_thread_.get());
    Mutex::Lock locker(lock_);
    OCTK_DCHECK_EQ(PlatformThread::currentThreadId(), thread_id_);
    Timestamp current_time = CurrentTime();
    // Clearing `ready_runners_` in case this is a recursive call:
    // RunReadyRunners -> Run -> Event::Wait -> Yield ->RunReadyRunners
    ready_runners_.clear();

    // We repeat until we have no ready left to handle tasks posted by ready
    // runners.
    while (true)
    {
        for (auto *runner: runners_)
        {
            if (yielded_.find(runner->GetAsTaskQueue()) == yielded_.end() &&
                runner->GetNextRunTime() <= current_time)
            {
                ready_runners_.push_back(runner);
            }
        }
        if (ready_runners_.empty())
        {
            break;
        }
        while (!ready_runners_.empty())
        {
            auto *runner = ready_runners_.front();
            ready_runners_.pop_front();
            lock_.unlock();
            // Note that the RunReady function might indirectly cause a call to
            // Unregister() which will grab `lock_` again to remove items from
            // `ready_runners_`.
            runner->RunReady(current_time);
            lock_.lock();
        }
    }
}

Timestamp SimulatedTimeControllerImpl::CurrentTime() const
{
    Mutex::Lock locker(time_lock_);
    return current_time_;
}

Timestamp SimulatedTimeControllerImpl::NextRunTime() const
{
    Timestamp current_time = CurrentTime();
    Timestamp next_time = Timestamp::PlusInfinity();
    Mutex::Lock locker(lock_);
    for (auto *runner: runners_)
    {
        Timestamp next_run_time = runner->GetNextRunTime();
        if (next_run_time <= current_time)
        {
            return current_time;
        }
        next_time = std::min(next_time, next_run_time);
    }
    return next_time;
}

void SimulatedTimeControllerImpl::AdvanceTime(Timestamp target_time)
{
    Mutex::Lock time_lock(time_lock_);
    OCTK_DCHECK_GE(target_time, current_time_);
    current_time_ = target_time;
}

void SimulatedTimeControllerImpl::Register(SimulatedSequenceRunner *runner)
{
    Mutex::Lock locker(lock_);
    runners_.push_back(runner);
}

void SimulatedTimeControllerImpl::Unregister(SimulatedSequenceRunner *runner)
{
    Mutex::Lock locker(lock_);
    bool removed = RemoveByValue(&runners_, runner);
    OCTK_CHECK(removed);
    RemoveByValue(&ready_runners_, runner);
}

void SimulatedTimeControllerImpl::StartYield(TaskQueueOld *yielding_from)
{
    auto inserted = yielded_.insert(yielding_from);
    OCTK_DCHECK(inserted.second);
}

void SimulatedTimeControllerImpl::StopYield(TaskQueueOld *yielding_from)
{
    yielded_.erase(yielding_from);
}
}  // namespace sim_time_impl

GlobalSimulatedTimeController::GlobalSimulatedTimeController(
    Timestamp start_time)
    : sim_clock_(start_time.us()), impl_(start_time), yield_policy_(&impl_)
{
    global_clock_.SetTime(start_time);
    auto main_thread = utils::make_unique<SimulatedMainThread>(&impl_);
    impl_.Register(main_thread.get());
    main_thread_ = std::move(main_thread);
}

GlobalSimulatedTimeController::~GlobalSimulatedTimeController() = default;

Clock *GlobalSimulatedTimeController::GetClock()
{
    return &sim_clock_;
}

TaskQueueFactory *GlobalSimulatedTimeController::GetTaskQueueFactory()
{
    return &impl_;
}

std::unique_ptr<TaskThread> GlobalSimulatedTimeController::CreateThread(const std::string &name,
                                                                        std::unique_ptr<SocketServer> socket_server)
{
    return impl_.CreateThread(name, std::move(socket_server));
}

TaskThread *GlobalSimulatedTimeController::GetMainThread()
{
    return main_thread_.get();
}

void GlobalSimulatedTimeController::AdvanceTime(TimeDelta duration)
{
    ScopedYieldPolicy yield_policy(&impl_);
    Timestamp current_time = impl_.CurrentTime();
    Timestamp target_time = current_time + duration;
    OCTK_DCHECK_EQ(current_time.us(), DateTime::TimeMicros());
    while (current_time < target_time)
    {
        impl_.RunReadyRunners();
        Timestamp next_time = std::min(impl_.NextRunTime(), target_time);
        impl_.AdvanceTime(next_time);
        auto delta = next_time - current_time;
        current_time = next_time;
        sim_clock_.AdvanceTimeMicroseconds(delta.us());
        global_clock_.AdvanceTime(delta);
    }
    // After time has been simulated up until `target_time` we also need to run
    // tasks meant to be executed at `target_time`.
    impl_.RunReadyRunners();
}

void GlobalSimulatedTimeController::SkipForwardBy(TimeDelta duration)
{
    ScopedYieldPolicy yield_policy(&impl_);
    Timestamp current_time = impl_.CurrentTime();
    Timestamp target_time = current_time + duration;
    impl_.AdvanceTime(target_time);
    sim_clock_.AdvanceTimeMicroseconds(duration.us());
    global_clock_.AdvanceTime(duration);
}

void GlobalSimulatedTimeController::Register(sim_time_impl::SimulatedSequenceRunner *runner)
{
    impl_.Register(runner);
}

void GlobalSimulatedTimeController::Unregister(sim_time_impl::SimulatedSequenceRunner *runner)
{
    impl_.Unregister(runner);
}
OCTK_END_NAMESPACE
#endif