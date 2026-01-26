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

#include "octk_simulated_thread_p.hpp"
#include <octk_memory.hpp>

OCTK_BEGIN_NAMESPACE

namespace
{

// A socket server that does nothing. It's different from NullSocketServer in
// that it does allow sleep/wakeup. This avoids usage of an Event instance which
// otherwise would cause issues with the simulated Yeild behavior.
class DummySocketServer : public SocketServer
{
public:
    Socket *CreateSocket(int family, int type) override
    {
        OCTK_DCHECK_NOTREACHED();
        return nullptr;
    }
    bool Wait(TimeDelta max_wait_duration, bool process_io) override
    {
        OCTK_CHECK(max_wait_duration.IsZero());
        return true;
    }
    void WakeUp() override {}
};
}  // namespace

SimulatedThread::SimulatedThread(sim_time_impl::SimulatedTimeControllerImpl *handler,
                                 StringView name,
                                 std::unique_ptr<SocketServer> socket_server)
    : TaskThread(socket_server ? std::move(socket_server)
                               : utils::make_unique<DummySocketServer>()), handler_(handler), mName(
    new char[name.size()])
{
    std::copy_n(name.begin(), name.size(), mName);
}

SimulatedThread::~SimulatedThread()
{
    handler_->Unregister(this);
    delete[] mName;
}

void SimulatedThread::RunReady(Timestamp at_time)
{
    CurrentThreadSetter set_current(this);
    ProcessMessages(0);
    int delay_ms = GetDelay();
    Mutex::Lock locker(lock_);
    if (delay_ms == kForever)
    {
        next_run_time_ = Timestamp::PlusInfinity();
    }
    else
    {
        next_run_time_ = at_time + TimeDelta::Millis(delay_ms);
    }
}

void SimulatedThread::BlockingCallImpl(FunctionView<void()> functor,
                                       const SourceLocation & /*location*/)
{
    if (IsQuitting())
    {
        return;
    }

    if (IsCurrent())
    {
        functor();
    }
    else
    {
        TaskQueueOld *yielding_from = TaskQueueOld::Current();
        handler_->StartYield(yielding_from);
        RunReady(Timestamp::MinusInfinity());
        CurrentThreadSetter set_current(this);
        functor();
        handler_->StopYield(yielding_from);
    }
}

void SimulatedThread::PostTaskImpl(Invocable<void() &&> task,
                                   const PostTaskTraits &traits,
                                   const SourceLocation &location)
{
    TaskThread::PostTaskImpl(std::move(task), traits, location);
    Mutex::Lock locker(lock_);
    next_run_time_ = Timestamp::MinusInfinity();
}

void SimulatedThread::PostDelayedTaskImpl(Invocable<void() &&> task,
                                          TimeDelta delay,
                                          const PostDelayedTaskTraits &traits,
                                          const SourceLocation &location)
{
    TaskThread::PostDelayedTaskImpl(std::move(task), delay, traits, location);
    Mutex::Lock locker(lock_);
    next_run_time_ = std::min(next_run_time_, Timestamp::Millis(DateTime::TimeMillis()) + delay);
}

void SimulatedThread::Stop()
{
    TaskThread::Quit();
}

SimulatedMainThread::SimulatedMainThread(sim_time_impl::SimulatedTimeControllerImpl *handler)
    : SimulatedThread(handler, "main", nullptr), current_setter_(this) {}

SimulatedMainThread::~SimulatedMainThread()
{
    // Removes pending tasks in case they keep shared pointer references to
    // objects whose destructor expects to run before the TaskThread destructor.
    Stop();
    DoDestroy();
}
OCTK_END_NAMESPACE
