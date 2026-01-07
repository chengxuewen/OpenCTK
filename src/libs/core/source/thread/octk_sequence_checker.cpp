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

#include <octk_sequence_checker.hpp>

#include <sstream>
#if 0
OCTK_BEGIN_NAMESPACE

namespace detail
{

SequenceCheckerImpl::SequenceCheckerImpl(bool attach_to_current_thread)
    : attached_(attach_to_current_thread)
    , valid_thread_(PlatformThread::currentThreadId())
    , valid_queue_(TaskQueueOld::Current())
{
}

SequenceCheckerImpl::SequenceCheckerImpl(TaskQueueOld *attached_queue)
    : attached_(attached_queue != nullptr)
    , valid_thread_(0)
    , valid_queue_(attached_queue)
{
}

bool SequenceCheckerImpl::IsCurrent() const
{
    const TaskQueueOld *const current_queue = TaskQueueOld::Current();
    const PlatformThread::Id current_thread = PlatformThread::currentThreadId();
    Mutex::UniqueLock scoped_lock(lock_);
    if (!attached_)
    { // Previously detached.
        attached_ = true;
        valid_thread_ = current_thread;
        valid_queue_ = current_queue;
        return true;
    }
    if (valid_queue_)
    {
        // return valid_queue_ == current_queue;
    }
    return valid_thread_ == current_thread;
}

void SequenceCheckerImpl::Detach()
{
    Mutex::UniqueLock scoped_lock(lock_);
    attached_ = false;
    // We don't need to touch the other members here, they will be
    // reset on the next call to IsCurrent().
}

#if OCTK_DCHECK_IS_ON
std::string SequenceCheckerImpl::ExpectationToString() const
{
    const TaskQueueOld *const current_queue = TaskQueueOld::Current();
    // const PlatformThread::Ref current_thread = PlatformThread::currentThreadRef();
    // Mutex::Locker scoped_lock(&lock_);
    if (!attached_)
    {
        return "Checker currently not attached.";
    }

    // The format of the string is meant to compliment the one we have inside of
    // FatalLog() (checks.cc).  Example:
    //
    // # Expected: TQ: 0x0 SysQ: 0x7fff69541330 Thread: 0x11dcf6dc0
    // # Actual:   TQ: 0x7fa8f0604190 SysQ: 0x7fa8f0604a30 Thread: 0x700006f1a000
    // TaskQueueOld doesn't match

    // char msgbuf[OCTK_LINE_MAX] = {0};
    // std::snprintf(msgbuf,
    //               OCTK_LINE_MAX,
    //               "# Expected: TQ: %p Thread: %p\n"
    //               "# Actual:   TQ: %p Thread: %p\n",
    //               valid_queue_,
    //               reinterpret_cast<const void *>(valid_thread_),
    //               current_queue,
    //               reinterpret_cast<const void *>(current_thread));
    std::stringstream message;
    // message << msgbuf;
    // if ((valid_queue_ || current_queue) && valid_queue_ != current_queue)
    // {
    //     message << "TaskQueueOld doesn't match\n";
    // }
    // else if (!PlatformThread::isThreadRefEqual(valid_thread_, current_thread))
    // {
    //     message << "Threads don't match\n";
    // }

    return message.str();
}
#endif // OCTK_DCHECK_IS_ON
} // namespace detail
OCTK_END_NAMESPACE
#endif