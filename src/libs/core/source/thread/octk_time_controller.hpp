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

#ifndef _OCTK_TIME_CONTROLLER_HPP
#define _OCTK_TIME_CONTROLLER_HPP

#include <octk_task_queue_factory.hpp>
#include <octk_time_delta.hpp>
#include <octk_clock.hpp>
#if 0
OCTK_BEGIN_NAMESPACE

// Interface for controlling time progress. This allows us to execute test code
// in either real time or simulated time by using different implementation of
// this interface.
class TimeController
{
public:
    virtual ~TimeController() = default;
    // Provides a clock instance that follows implementation defined time
    // progress.
    virtual Clock *GetClock() = 0;
    // The returned factory will created task queues that runs in implementation
    // defined time domain.
    virtual TaskQueueFactory *GetTaskQueueFactory() = 0;
    // Simple helper to create an owned factory that can be used as a parameter
    // for PeerConnectionFactory. Note that this might depend on the underlying
    // time controller and therfore must be destroyed before the time controller
    // is destroyed.
    std::unique_ptr<TaskQueueFactory> CreateTaskQueueFactory();

    // Creates an TaskThread instance. If `socket_server` is nullptr, a default
    // noop socket server is created.
    // Returned thread is not null and started.
    // virtual std::unique_ptr<TaskThread> CreateThread(const std::string &name,
    //                                                  std::unique_ptr<SocketServer> socket_server = nullptr) = 0;

    // Creates an TaskThread instance that ensure that it's set as the current
    // thread.
    // virtual TaskThread *GetMainThread() = 0;
    // Allow task queues and process threads created by this instance to execute
    // for the given `duration`.
    virtual void AdvanceTime(TimeDelta duration) = 0;

    // Waits until condition() == true, polling condition() in small time
    // intervals.
    // Returns true if condition() was evaluated to true before `max_duration`
    // elapsed and false otherwise.
    bool Wait(const std::function<bool()> &condition, TimeDelta max_duration = TimeDelta::Seconds(5));
};

// Interface for telling time, scheduling an event to fire at a particular time,
// and waiting for time to pass.
class ControlledAlarmClock
{
public:
    virtual ~ControlledAlarmClock() = default;

    // Gets a clock that tells the alarm clock's notion of time.
    virtual Clock *GetClock() = 0;

    // Schedules the alarm to fire at `deadline`.
    // An alarm clock only supports one deadline. Calls to `ScheduleAlarmAt` with
    // an earlier deadline will reset the alarm to fire earlier.Calls to
    // `ScheduleAlarmAt` with a later deadline are ignored. Returns true if the
    // deadline changed, false otherwise.
    virtual bool ScheduleAlarmAt(Timestamp deadline) = 0;

    // Sets the callback that should be run when the alarm fires.
    virtual void SetCallback(std::function<void()> callback) = 0;

    // Waits for `duration` to pass, according to the alarm clock.
    virtual void Sleep(TimeDelta duration) = 0;
};
OCTK_END_NAMESPACE
#endif
#endif // _OCTK_TIME_CONTROLLER_HPP
