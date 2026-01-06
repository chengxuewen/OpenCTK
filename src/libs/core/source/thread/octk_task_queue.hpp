/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2026~Present ChengXueWen.
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

#pragma once

#include <octk_source_location.hpp>
#include <octk_time_delta.hpp>
#include <octk_logging.hpp>
#include <octk_task.hpp>

OCTK_BEGIN_NAMESPACE

class OCTK_CORE_API TaskQueueBase
{
public:
    struct Deleter final
    {
        void operator()(TaskQueueBase *taskQueue) const { taskQueue->destroy(); }
    };
    using SharedPtr = std::shared_ptr<TaskQueueBase>;
    using UniquePtr = std::unique_ptr<TaskQueueBase, Deleter>;

    class OCTK_CORE_API CurrentSetter final
    {
        TaskQueueBase *const mPrevious;
        OCTK_DISABLE_COPY_MOVE(CurrentSetter)
    public:
        explicit CurrentSetter(TaskQueueBase *taskQueue);
        ~CurrentSetter();
    };

    class OCTK_CORE_API SafetyFlag final
    {
        std::atomic<bool> mAlive{true};

        explicit SafetyFlag(bool alive)
            : mAlive(alive) { };

    public:
        using SharedPtr = std::shared_ptr<SafetyFlag>;

        static SharedPtr create() { return SharedPtr(new SafetyFlag(true)); }
        static SharedPtr createDetached()
        {
            return SharedPtr(new SafetyFlag(true));
        } // TODO::impl main_sequence_.Detach();

        ~SafetyFlag() { }

        bool isAlive() const { return mAlive.load(std::memory_order_acquire); }
        void setNotAlive() { mAlive.store(false, std::memory_order_release); }
        void setAlive() { mAlive.store(true, std::memory_order_release); }
    };

    virtual ~TaskQueueBase() = default;

    virtual void destroy() = 0;
    virtual bool cancelTask(const Task *task) = 0;

    void postTask(Task *task, bool autoDelete, const SourceLocation &location = SourceLocation::current())
    {
        this->postTask(Task::makeShared(task, autoDelete), location);
    }
    void postTask(UniqueFunction<void() &&> function, const SourceLocation &location = SourceLocation::current())
    {
        this->postTask(Task::create(std::move(function)), location);
    }
    virtual void postTask(const Task::SharedPtr &task, const SourceLocation &location = SourceLocation::current()) = 0;


    void postDelayedTask(Task *task,
                         bool autoDelete,
                         const TimeDelta &delay,
                         const SourceLocation &location = SourceLocation::current())
    {
        this->postDelayedTask(Task::makeShared(task, autoDelete), delay, location);
    }
    void postDelayedTask(UniqueFunction<void() &&> function,
                         const TimeDelta &delay,
                         const SourceLocation &location = SourceLocation::current())
    {
        this->postDelayedTask(Task::create(std::move(function)), delay, location);
    }
    virtual void postDelayedTask(const Task::SharedPtr &task,
                                 const TimeDelta &delay,
                                 const SourceLocation &location = SourceLocation::current()) = 0;


    bool isCurrent() const { return this->current() == this; }

    static TaskQueueBase *current();
};

OCTK_END_NAMESPACE

OCTK_DECLARE_LOGGER(OCTK_CORE_API, OCTK_TASK_QUEUE_LOGGER)