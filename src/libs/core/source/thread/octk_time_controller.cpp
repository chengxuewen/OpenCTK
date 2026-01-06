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

#include <octk_time_controller.hpp>
#include <octk_memory.hpp>

OCTK_BEGIN_NAMESPACE

std::unique_ptr<TaskQueueFactory> TimeController::CreateTaskQueueFactory()
{
    class FactoryWrapper final : public TaskQueueFactory
    {
    public:
        explicit FactoryWrapper(TaskQueueFactory *inner_factory) : inner_(inner_factory) {}
        std::unique_ptr<TaskQueueOld, TaskQueueDeleter> CreateTaskQueue(StringView name, Priority priority) const override
        {
            return inner_->CreateTaskQueue(name, priority);
        }

    private:
        TaskQueueFactory *const inner_;
    };
    return utils::make_unique<FactoryWrapper>(GetTaskQueueFactory());
}
bool TimeController::Wait(const std::function<bool()> &condition, TimeDelta max_duration)
{
    // Step size is chosen to be short enough to not significantly affect latency
    // in real time tests while being long enough to avoid adding too much load to
    // the system.
    const auto kStep = TimeDelta::Millis(5);
    for (auto elapsed = TimeDelta::Zero(); elapsed < max_duration; elapsed += kStep)
    {
        if (condition())
        {
            return true;
        }
        AdvanceTime(kStep);
    }
    return condition();
}
OCTK_END_NAMESPACE
