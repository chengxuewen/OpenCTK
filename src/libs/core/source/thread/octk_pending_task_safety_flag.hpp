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

#ifndef _OCTK_PENDING_TASK_SAFETY_FLAG_HPP
#define _OCTK_PENDING_TASK_SAFETY_FLAG_HPP

#include <octk_sequence_checker.hpp>
#include <octk_scoped_refptr.hpp>
#include <octk_move_wrapper.hpp>
#include <octk_nullability.hpp>
#include <octk_task_queue.hpp>
#include <octk_ref_count.hpp>

OCTK_BEGIN_NAMESPACE

// The PendingTaskSafetyFlag and the ScopedTaskSafety are designed to address
// the issue where you have a task to be executed later that has references,
// but cannot guarantee that the referenced object is alive when the task is
// executed.

// This mechanism can be used with tasks that are created and destroyed
// on a single thread / task queue, and with tasks posted to the same
// thread/task queue, but tasks can be posted from any thread/TQ.

// Typical usage:
// When posting a task, post a copy (capture by-value in a lambda) of the flag
// reference and before performing the work, check the `alive()` state. Abort if
// alive() returns `false`:
//
// class ExampleClass {
// ....
//    ScopedRefPtr<PendingTaskSafetyFlag> flag = safety_flag_;
//    my_task_queue_->PostTask(
//        [flag = std::move(flag), this] {
//          // Now running on the main thread.
//          if (!flag->alive())
//            return;
//          MyMethod();
//        });
//   ....
//   ~ExampleClass() {
//     safety_flag_->SetNotAlive();
//   }
//   scoped_refptr<PendingTaskSafetyFlag> safety_flag_
//        = PendingTaskSafetyFlag::Create();
// }
//
// SafeTask makes this check automatic:
//
//   my_task_queue_->PostTask(SafeTask(safety_flag_, [this] { MyMethod(); }));
//
class OCTK_CORE_API PendingTaskSafetyFlag final : public RefCountedNonVirtual<PendingTaskSafetyFlag>
{
public:
    static ScopedRefPtr<PendingTaskSafetyFlag> Create();

// Creates a flag, but with its SequenceChecker initially detached. Hence, it
// may be created on a different thread than the flag will be used on.
    static ScopedRefPtr<PendingTaskSafetyFlag> CreateDetached();

// Creates a flag, but with its SequenceChecker explicitly initialized for
// a given task queue and the `alive()` flag specified.
    static ScopedRefPtr<PendingTaskSafetyFlag> CreateAttachedToTaskQueue(bool alive,
                                                                         Nonnull<TaskQueue *> attached_queue);

// Same as `CreateDetached()` except the initial state of the returned flag
// will be `!alive()`.
    static ScopedRefPtr<PendingTaskSafetyFlag> CreateDetachedInactive();

    ~
    PendingTaskSafetyFlag() = default;

    void SetNotAlive();
// The SetAlive method is intended to support Start/Stop/Restart usecases.
// When a class has called SetNotAlive on a flag used for posted tasks, and
// decides it wants to post new tasks and have them run, there are two
// reasonable ways to do that:
//
// (i) Use the below SetAlive method. One subtlety is that any task posted
//     prior to SetNotAlive, and still in the queue, is resurrected and will
//     run.
//
// (ii) Create a fresh flag, and just drop the reference to the old one. This
//      avoids the above problem, and ensures that tasks poster prior to
//      SetNotAlive stay cancelled. Instead, there's a potential data race on
//      the flag pointer itself. Some synchronization is required between the
//      thread overwriting the flag pointer, and the threads that want to post
//      tasks and therefore read that same pointer.
    void SetAlive();
    bool alive() const;

protected:
    explicit PendingTaskSafetyFlag(bool alive) : alive_(alive) {}
    PendingTaskSafetyFlag(bool alive, Nonnull<TaskQueue *> attached_queue)
        : alive_(alive), main_sequence_(attached_queue) {}

private:
    static ScopedRefPtr<PendingTaskSafetyFlag> CreateInternal(bool alive);

    bool alive_ = true;
    OCTK_ATTRIBUTE_NO_UNIQUE_ADDRESS SequenceChecker main_sequence_;
};

// The ScopedTaskSafety makes using PendingTaskSafetyFlag very simple.
// It does automatic PTSF creation and signalling of destruction when the
// ScopedTaskSafety instance goes out of scope.
//
// Example usage:
//
//     my_task_queue->PostTask(SafeTask(scoped_task_safety.flag(),
//        [this] {
//             // task goes here
//        }
//
// This should be used by the class that wants tasks dropped after destruction.
// The requirement is that the instance has to be constructed and destructed on
// the same thread as the potentially dropped tasks would be running on.
class OCTK_CORE_API ScopedTaskSafety final
{
public:
    ScopedTaskSafety() = default;
    explicit ScopedTaskSafety(ScopedRefPtr<PendingTaskSafetyFlag> flag) : flag_(std::move(flag)) {}
    ~ScopedTaskSafety() { flag_->SetNotAlive(); }

// Returns a new reference to the safety flag.
    ScopedRefPtr<PendingTaskSafetyFlag> flag() const { return flag_; }

// Marks the current flag as not-alive and attaches to a new one.
    void reset(ScopedRefPtr<PendingTaskSafetyFlag> new_flag =
    PendingTaskSafetyFlag::Create())
    {
        flag_->SetNotAlive();
        flag_ = std::move(new_flag);
    }

private:
    ScopedRefPtr<PendingTaskSafetyFlag> flag_ = PendingTaskSafetyFlag::Create();
};

// Like ScopedTaskSafety, but allows construction on a different thread than
// where the flag will be used.
class OCTK_CORE_API ScopedTaskSafetyDetached final
{
public:
    ScopedTaskSafetyDetached() = default;
    ~ScopedTaskSafetyDetached() { flag_->SetNotAlive(); }

// Returns a new reference to the safety flag.
    ScopedRefPtr<PendingTaskSafetyFlag> flag() const { return flag_; }

private:
    ScopedRefPtr<PendingTaskSafetyFlag> flag_ = PendingTaskSafetyFlag::CreateDetached();
};

inline TaskQueue::Task SafeTask(ScopedRefPtr<PendingTaskSafetyFlag> flag,
                                TaskQueue::Task task)
{
    auto moveFlag = utils::makeMoveWrapper(std::move(flag));
    auto moveTask = utils::makeMoveWrapper(std::move(task));
    return [moveFlag, moveTask]() mutable {
        if (moveFlag.move()->alive())
        {
            moveTask.move()();
        }
    };
}
OCTK_END_NAMESPACE

#endif // _OCTK_PENDING_TASK_SAFETY_FLAG_HPP
