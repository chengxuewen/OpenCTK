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

#include <octk_pending_task_safety_flag.hpp>

OCTK_BEGIN_NAMESPACE

// static
ScopedRefPtr<PendingTaskSafetyFlag>
PendingTaskSafetyFlag::CreateInternal(bool alive)
{
    // Explicit new, to access private constructor.
    return ScopedRefPtr<PendingTaskSafetyFlag>(new PendingTaskSafetyFlag(alive));
}

// static
ScopedRefPtr <PendingTaskSafetyFlag> PendingTaskSafetyFlag::Create()
{
    return CreateInternal(true);
}

ScopedRefPtr <PendingTaskSafetyFlag> PendingTaskSafetyFlag::CreateDetached()
{
    ScopedRefPtr<PendingTaskSafetyFlag> safety_flag = CreateInternal(true);
    safety_flag->main_sequence_.Detach();
    return safety_flag;
}

// Creates a flag, but with its SequenceChecker explicitly initialized for
// a given task queue and the `alive()` flag specified.
ScopedRefPtr <PendingTaskSafetyFlag>
PendingTaskSafetyFlag::CreateAttachedToTaskQueue(bool alive, Nonnull<TaskQueue *> attached_queue)
{
    OCTK_DCHECK(attached_queue) << "Null TaskQueue provided";
    return ScopedRefPtr<PendingTaskSafetyFlag>(new PendingTaskSafetyFlag(alive, attached_queue));
}

ScopedRefPtr <PendingTaskSafetyFlag> PendingTaskSafetyFlag::CreateDetachedInactive()
{
    ScopedRefPtr<PendingTaskSafetyFlag> safety_flag = CreateInternal(false);
    safety_flag->main_sequence_.Detach();
    return safety_flag;
}

void PendingTaskSafetyFlag::SetNotAlive()
{
    OCTK_DCHECK_RUN_ON(&main_sequence_);
    alive_ = false;
}

void PendingTaskSafetyFlag::SetAlive()
{
    OCTK_DCHECK_RUN_ON(&main_sequence_);
    alive_ = true;
}

bool PendingTaskSafetyFlag::alive() const
{
    OCTK_DCHECK_RUN_ON(&main_sequence_);
    return alive_;
}

OCTK_END_NAMESPACE
