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

#ifndef _OCTK_TASK_QUEUE_FOR_TEST_HPP
#define _OCTK_TASK_QUEUE_FOR_TEST_HPP

#include <octk_task_queue_factory.hpp>
#include <octk_function_view.hpp>
#include <octk_string_view.hpp>
#include <octk_scope_guard.hpp>
#include <octk_task_queue_old.hpp>
#include <octk_task_event.hpp>
#include <octk_checks.hpp>

#include <utility>

OCTK_BEGIN_NAMESPACE

namespace utils
{
inline void sendTask(TaskQueueOld *task_queue,
                     FunctionView<void()> task)
{
    if (task_queue->IsCurrent())
    {
        task();
        return;
    }

    Event event;
    task_queue->PostTask([task, &event] {
        auto cleanup = utils::makeScopeGuard([&event] { event.Set(); });
        task();
    });
    OCTK_CHECK(event.Wait(/*give_up_after=*/Event::foreverDuration(),/*warn_after=*/TimeDelta::Seconds(10)));
}
} // namespace utils

class TaskQueueForTest
{
public:
    explicit TaskQueueForTest(std::unique_ptr<TaskQueueOld, TaskQueueDeleter> task_queue)
        : impl_(std::move(task_queue)) {}
    explicit TaskQueueForTest(StringView name = "TestQueue",
                              TaskQueueFactory::Priority priority = TaskQueueFactory::Priority::NORMAL)
        : impl_(utils::createDefaultTaskQueueFactory()->CreateTaskQueue(name, priority)) {}
    TaskQueueForTest(const TaskQueueForTest &) = delete;
    TaskQueueForTest &operator=(const TaskQueueForTest &) = delete;
    ~TaskQueueForTest()
    {
        // Stop the TaskQueueOld before invalidating impl_ pointer so that tasks that
        // race with the TaskQueueForTest destructor could still use TaskQueueForTest
        // functions like 'IsCurrent'.
        impl_.get_deleter()(impl_.get());
        impl_.release();
    }

    bool IsCurrent() const { return impl_->IsCurrent(); }

    // Returns non-owning pointer to the task queue implementation.
    TaskQueueOld *Get() { return impl_.get(); }

    void PostTask(TaskQueueOld::Task task,
                  const SourceLocation &location = SourceLocation::current())
    {
        impl_->PostTask(std::move(task), location);
    }
    void PostDelayedTask(TaskQueueOld::Task task,
                         TimeDelta delay,
                         const SourceLocation &location = SourceLocation::current())
    {
        impl_->PostDelayedTask(std::move(task), delay, location);
    }
    void PostDelayedHighPrecisionTask(TaskQueueOld::Task task,
                                      TimeDelta delay,
                                      const SourceLocation &location = SourceLocation::current())
    {
        impl_->PostDelayedHighPrecisionTask(std::move(task), delay, location);
    }

    // A convenience, test-only method that blocks the current thread while
    // a task executes on the task queue.
    void SendTask(FunctionView<void()> task)
    {
        utils::sendTask(Get(), task);
    }

    // Wait for the completion of all tasks posted prior to the
    // WaitForPreviouslyPostedTasks() call.
    void WaitForPreviouslyPostedTasks()
    {
        OCTK_DCHECK(!Get()->IsCurrent());
        // Post an empty task on the queue and wait for it to finish, to ensure
        // that all already posted tasks on the queue get executed.
        SendTask([]() {});
    }

private:
    std::unique_ptr<TaskQueueOld, TaskQueueDeleter> impl_;
};
OCTK_END_NAMESPACE

#endif // _OCTK_TASK_QUEUE_FOR_TEST_HPP
