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

#ifndef _OCTK_SIMULATED_THREAD_HPP
#define _OCTK_SIMULATED_THREAD_HPP

#include <test/octk_simulated_time_controller.hpp>
#include <octk_mutex.hpp>

#include <memory>

OCTK_BEGIN_NAMESPACE

class SimulatedThread : public TaskThread, public sim_time_impl::SimulatedSequenceRunner
{
public:
    using CurrentThreadSetter = TaskThread::CurrentTaskThreadSetter;
    SimulatedThread(sim_time_impl::SimulatedTimeControllerImpl *handler,
                    StringView name,
                    std::unique_ptr<SocketServer> socket_server);
    ~SimulatedThread() override;

    void RunReady(Timestamp at_time) override;

    Timestamp GetNextRunTime() const override
    {
        Mutex::Lock locker(lock_);
        return next_run_time_;
    }

    TaskQueue *GetAsTaskQueue() override { return this; }

    // Thread interface
    void BlockingCallImpl(FunctionView<void()> functor,
                          const SourceLocation &location) override;
    void PostTaskImpl(TaskQueue::Task task,
                      const PostTaskTraits &traits,
                      const SourceLocation &location) override;
    void PostDelayedTaskImpl(TaskQueue::Task task,
                             TimeDelta delay,
                             const PostDelayedTaskTraits &traits,
                             const SourceLocation &location) override;

    void Stop() override;

private:
    sim_time_impl::SimulatedTimeControllerImpl *const handler_;
    // Using char* to be debugger friendly.
    char *mName;
    mutable Mutex lock_;
    Timestamp next_run_time_ OCTK_ATTRIBUTE_GUARDED_BY(lock_) = Timestamp::PlusInfinity();
};

class SimulatedMainThread : public SimulatedThread
{
public:
    explicit SimulatedMainThread(sim_time_impl::SimulatedTimeControllerImpl *handler);
    ~SimulatedMainThread();

private:
    CurrentThreadSetter current_setter_;
};
OCTK_END_NAMESPACE

#endif // _OCTK_SIMULATED_THREAD_HPP
