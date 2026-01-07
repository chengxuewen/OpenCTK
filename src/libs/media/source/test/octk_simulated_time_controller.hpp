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

#ifndef _OCTK_SIMULATED_TIME_CONTROLLER_HPP
#define _OCTK_SIMULATED_TIME_CONTROLLER_HPP

#include <octk_sequence_checker.hpp>
#include <octk_time_controller.hpp>
#include <octk_platform_thread.hpp>
#include <octk_yield_policy.hpp>
#include <octk_string_view.hpp>
#include <octk_fake_clock.hpp>
#include <octk_timestamp.hpp>
#include <octk_mutex.hpp>

#include <unordered_set>
#include <utility>
#include <memory>
#include <vector>
#include <list>

OCTK_BEGIN_NAMESPACE

// TODO: move to core thread?
namespace sim_time_impl
{
class SimulatedSequenceRunner
{
public:
    virtual ~SimulatedSequenceRunner() = default;
    // Provides next run time.
    virtual Timestamp GetNextRunTime() const = 0;
    // Runs all ready tasks and modules and updates next run time.
    virtual void RunReady(Timestamp at_time) = 0;

    // All implementations also implements TaskQueueOld in some form, but if we'd
    // inherit from it in this interface we'd run into issues with double
    // inheritance. Therefore we simply allow the implementations to provide a
    // casted pointer to themself.
    virtual TaskQueueOld *GetAsTaskQueue() = 0;
};

class SimulatedTimeControllerImpl : public TaskQueueFactory, public YieldInterface
{
public:
    explicit SimulatedTimeControllerImpl(Timestamp start_time);
    ~SimulatedTimeControllerImpl() override;

    std::unique_ptr<TaskQueueOld, TaskQueueDeleter> CreateTaskQueue(StringView name,
                                                                 Priority priority) const
    OCTK_ATTRIBUTE_LOCKS_EXCLUDED(time_lock_) override;

    // Implements the YieldInterface by running ready tasks on all task queues,
    // except that if this method is called from a task, the task queue running
    // that task is skipped.
    void YieldExecution()
    OCTK_ATTRIBUTE_LOCKS_EXCLUDED(time_lock_, lock_) override;

    // Create thread using provided `socket_server`.
    //std::unique_ptr<TaskThread> CreateThread(const std::string &name,
    //                                         std::unique_ptr<SocketServer> socket_server)
    //OCTK_ATTRIBUTE_LOCKS_EXCLUDED(time_lock_, lock_);

    // Runs all runners in `runners_` that has tasks or modules ready for
    // execution.
    void RunReadyRunners()
    OCTK_ATTRIBUTE_LOCKS_EXCLUDED(time_lock_, lock_);

    // Return `current_time_`.
    Timestamp CurrentTime() const
    OCTK_ATTRIBUTE_LOCKS_EXCLUDED(time_lock_);

    // Return min of runner->GetNextRunTime() for runner in `runners_`.
    Timestamp NextRunTime() const
    OCTK_ATTRIBUTE_LOCKS_EXCLUDED(lock_);

    // Set `current_time_` to `target_time`.
    void AdvanceTime(Timestamp target_time)
    OCTK_ATTRIBUTE_LOCKS_EXCLUDED(time_lock_);

    // Adds `runner` to `runners_`.
    void Register(SimulatedSequenceRunner *runner)
    OCTK_ATTRIBUTE_LOCKS_EXCLUDED(lock_);

    // Removes `runner` from `runners_`.
    void Unregister(SimulatedSequenceRunner *runner)
    OCTK_ATTRIBUTE_LOCKS_EXCLUDED(lock_);

    // Indicates that `yielding_from` is not ready to run.
    void StartYield(TaskQueueOld *yielding_from);
    // Indicates that processing can be continued on `yielding_from`.
    void StopYield(TaskQueueOld *yielding_from);

private:
    const PlatformThread::Id thread_id_;
    const std::unique_ptr<TaskThread> dummy_thread_ = TaskThread::Create();
    mutable Mutex time_lock_;

    Timestamp current_time_
    OCTK_ATTRIBUTE_GUARDED_BY(time_lock_);

    mutable Mutex lock_;

    std::vector<SimulatedSequenceRunner *> runners_
    OCTK_ATTRIBUTE_GUARDED_BY(lock_);
    // Used in RunReadyRunners() to keep track of ready runners that are to be
    // processed in a round robin fashion. the reason it's a member is so that
    // runners can removed from here by Unregister().
    std::list<SimulatedSequenceRunner *> ready_runners_
    OCTK_ATTRIBUTE_GUARDED_BY(lock_);

    // Runners on which YieldExecution has been called.
    std::unordered_set<TaskQueueOld *> yielded_;
};
}  // namespace sim_time_impl

// Used to satisfy sequence checkers for non task queue sequences.
class TokenTaskQueue : public TaskQueueOld
{
public:
    // Promoted to public
    using CurrentTaskQueueSetter = TaskQueueOld::CurrentTaskQueueSetter;

    void Delete() override { OCTK_DCHECK_NOTREACHED(); }
    void PostTaskImpl(TaskQueueOld::Task task,
                      const PostTaskTraits &traits,
                      const SourceLocation &location) override
    {
        OCTK_DCHECK_NOTREACHED();
    }
    void PostDelayedTaskImpl(TaskQueueOld::Task task,
                             TimeDelta delay,
                             const PostDelayedTaskTraits &traits,
                             const SourceLocation &location) override
    {
        OCTK_DCHECK_NOTREACHED();
    }
};

// TimeController implementation using completely simulated time. Task queues
// and process threads created by this controller will run delayed activities
// when AdvanceTime() is called. Overrides the global clock backing
// TimeMillis() and TimeMicros(). Note that this is not thread safe
// since it modifies global state.
class GlobalSimulatedTimeController : public TimeController
{
public:
    explicit GlobalSimulatedTimeController(Timestamp start_time);
    ~GlobalSimulatedTimeController() override;

    Clock *GetClock() override;
    TaskQueueFactory *GetTaskQueueFactory() override;
    std::unique_ptr<TaskThread> CreateThread(const std::string &name,
                                             std::unique_ptr<SocketServer> socket_server) override;
    TaskThread *GetMainThread() override;

    void AdvanceTime(TimeDelta duration) override;

    // Advances time by `duration`and do not run delayed tasks in the meantime.
    // Useful for simulating contention on destination queues.
    void SkipForwardBy(TimeDelta duration);

    // Makes the simulated time controller aware of a custom
    // SimulatedSequenceRunner.
    // TODO(bugs.webrtc.org/11581): remove method once the ModuleRtpRtcpImpl2 unit
    // test stops using it.
    void Register(sim_time_impl::SimulatedSequenceRunner *runner);
    // Removes a previously installed custom SimulatedSequenceRunner from the
    // simulated time controller.
    // TODO(bugs.webrtc.org/11581): remove method once the ModuleRtpRtcpImpl2 unit
    // test stops using it.
    void Unregister(sim_time_impl::SimulatedSequenceRunner *runner);

private:
    ScopedBaseFakeClock global_clock_;
    // Provides simulated CurrentNtpInMilliseconds()
    SimulatedClock sim_clock_;
    sim_time_impl::SimulatedTimeControllerImpl impl_;
    ScopedYieldPolicy yield_policy_;
    std::unique_ptr<TaskThread> main_thread_;
};
OCTK_END_NAMESPACE

#endif // _OCTK_SIMULATED_TIME_CONTROLLER_HPP
