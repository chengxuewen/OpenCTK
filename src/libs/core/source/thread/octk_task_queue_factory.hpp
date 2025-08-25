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

#ifndef _OCTK_TASK_QUEUE_FACTORY_HPP
#define _OCTK_TASK_QUEUE_FACTORY_HPP

#include <octk_task_queue.hpp>
#include <octk_string_view.hpp>

OCTK_BEGIN_NAMESPACE
// The implementation of this interface must be thread-safe.
class TaskQueueFactory
{
public:
    // TaskQueue priority levels. On some platforms these will map to thread
    // priorities, on others such as Mac and iOS, GCD queue priorities.
    enum class Priority { NORMAL = 0, HIGH, LOW };

    virtual ~TaskQueueFactory() = default;
    virtual std::unique_ptr<TaskQueue, TaskQueueDeleter> CreateTaskQueue(StringView name,
                                                                         Priority priority) const = 0;
};

namespace utils
{
// std::unique_ptr<TaskQueueFactory> CreateDefaultTaskQueueFactory(const FieldTrialsView* field_trials = nullptr);
OCTK_CORE_API std::unique_ptr<TaskQueueFactory> createDefaultTaskQueueFactory();
}
OCTK_END_NAMESPACE

#endif // _OCTK_TASK_QUEUE_FACTORY_HPP
