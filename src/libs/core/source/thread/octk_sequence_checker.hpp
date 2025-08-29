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

#ifndef _OCTK_SEQUENCE_CHECKER_HPP
#define _OCTK_SEQUENCE_CHECKER_HPP

#include <octk_platform_thread.hpp>
#include <octk_type_traits.hpp>
#include <octk_task_queue.hpp>
#include <octk_checks.hpp>
#include <octk_mutex.hpp>

OCTK_BEGIN_NAMESPACE

namespace internal
{

// Real implementation of SequenceChecker, for use in debug mode, or
// for temporary use in release mode (e.g. to OCTK_CHECK on a threading issue
// seen only in the wild).
//
// Note: You should almost always use the SequenceChecker class to get the
// right version for your build configuration.
class OCTK_CORE_API SequenceCheckerImpl
{
public:
    explicit SequenceCheckerImpl(bool attach_to_current_thread);
    explicit SequenceCheckerImpl(TaskQueue *attached_queue);
    ~SequenceCheckerImpl() = default;

    bool IsCurrent() const;
    // Changes the task queue or thread that is checked for in IsCurrent. This can
    // be useful when an object may be created on one task queue / thread and then
    // used exclusively on another thread.
    void Detach();

    // Returns a string that is formatted to match with the error string printed
    // by OCTK_CHECK() when a condition is not met.
    // This is used in conjunction with the OCTK_DCHECK_RUN_ON() macro.
    std::string ExpectationToString() const;

private:
    mutable Mutex lock_;
    // These are mutable so that IsCurrent can set them.
    mutable bool attached_ OCTK_ATTRIBUTE_GUARDED_BY(lock_);
    mutable PlatformThread::Ref valid_thread_ OCTK_ATTRIBUTE_GUARDED_BY(lock_);
    mutable const TaskQueue *valid_queue_ OCTK_ATTRIBUTE_GUARDED_BY(lock_);
};

// Do nothing implementation, for use in release mode.
//
// Note: You should almost always use the SequenceChecker class to get the
// right version for your build configuration.
class SequenceCheckerDoNothing
{
public:
    explicit SequenceCheckerDoNothing(bool /* attach_to_current_thread */) { }
    explicit SequenceCheckerDoNothing(TaskQueue * /* attached_queue */) { }
    bool IsCurrent() const { return true; }
    void Detach() { }
};

template <typename ThreadLikeObject>
typename std::enable_if<std::is_base_of<SequenceCheckerImpl, ThreadLikeObject>::value, std::string>::type
ExpectationToString([[maybe_unused]] const ThreadLikeObject *checker)
{
#if OCTK_DCHECK_IS_ON
    return checker->ExpectationToString();
#else
    return std::string();
#endif
}

// Catch-all implementation for types other than explicitly supported above.
template <typename ThreadLikeObject>
typename std::enable_if<!std::is_base_of<SequenceCheckerImpl, ThreadLikeObject>::value, std::string>::type
ExpectationToString(const ThreadLikeObject *)
{
    return std::string();
}
} // namespace internal

// SequenceChecker is a helper class used to help verify that some methods
// of a class are called on the same task queue or thread. A
// SequenceChecker is bound to a a task queue if the object is
// created on a task queue, or a thread otherwise.
//
//
// Example:
// class MyClass {
//  public:
//   void Foo() {
//     OCTK_DCHECK_RUN_ON(&sequence_checker_);
//     ... (do stuff) ...
//   }
//
//  private:
//   SequenceChecker sequence_checker_;
// }
//
// In Release mode, IsCurrent will always return true.
class OCTK_ATTRIBUTE_LOCKABLE SequenceChecker
#if OCTK_DCHECK_IS_ON
    : public internal::SequenceCheckerImpl
{
    using Impl = internal::SequenceCheckerImpl;
#else
    : public internal::SequenceCheckerDoNothing
{
    using Impl = internal::SequenceCheckerDoNothing;
#endif
public:
    enum InitialState : bool
    {
        kDetached = false,
        kAttached = true
    };

    // TODO(tommi): We could maybe join these two ctors and have fewer factory
    // functions. At the moment they're separate to minimize code changes when
    // we added the second ctor as well as avoiding to have unnecessary code at
    // the SequenceChecker which much only run for the SequenceCheckerImpl
    // implementation.
    // In theory we could have something like:
    //
    //  SequenceChecker(InitialState initial_state = kAttached,
    //                  TaskQueue* attached_queue = TaskQueue::Current());
    //
    // But the problem with that is having the call to `Current()` exist for
    // `SequenceCheckerDoNothing`.
    explicit SequenceChecker(InitialState initial_state = kAttached)
        : Impl(initial_state)
    {
    }
    explicit SequenceChecker(TaskQueue *attached_queue)
        : Impl(attached_queue)
    {
    }

    // Returns true if sequence checker is attached to the current sequence.
    bool IsCurrent() const { return Impl::IsCurrent(); }
    // Detaches checker from sequence to which it is attached. Next attempt
    // to do a check with this checker will result in attaching this checker
    // to the sequence on which check was performed.
    void Detach() { Impl::Detach(); }
};

// OCTK_RUN_ON/OCTK_GUARDED_BY/OCTK_DCHECK_RUN_ON macros allows to annotate
// variables are accessed from same thread/task queue.
// Using tools designed to check mutexes, it checks at compile time everywhere
// variable is access, there is a run-time dcheck thread/task queue is correct.
//
// class SequenceCheckerExample {
//  public:
//   int CalledFromPacer() OCTK_RUN_ON(pacer_sequence_checker_) {
//     return var2_;
//   }
//
//   void CallMeFromPacer() {
//     OCTK_DCHECK_RUN_ON(&pacer_sequence_checker_)
//        << "Should be called from pacer";
//     CalledFromPacer();
//   }
//
//  private:
//   int pacer_var_ OCTK_GUARDED_BY(pacer_sequence_checker_);
//   SequenceChecker pacer_sequence_checker_;
// };
//
// class TaskQueueExample {
//  public:
//   class Encoder {
//    public:
//     rtc::TaskQueue& Queue() { return encoder_queue_; }
//     void Encode() {
//       OCTK_DCHECK_RUN_ON(&encoder_queue_);
//       DoSomething(var_);
//     }
//
//    private:
//     rtc::TaskQueue& encoder_queue_;
//     Frame var_ OCTK_GUARDED_BY(encoder_queue_);
//   };
//
//   void Encode() {
//     // Will fail at runtime when DCHECK is enabled:
//     // encoder_->Encode();
//     // Will work:
//     std::shared_ptr<Encoder> encoder = encoder_;
//     encoder_->Queue().PostTask([encoder] { encoder->Encode(); });
//   }
//
//  private:
//   std::shared_ptr<Encoder> encoder_;
// }

// Document if a function expected to be called from same thread/task queue.
#define OCTK_RUN_ON(...) OCTK_ATTRIBUTE_EXCLUSIVE_LOCKS_REQUIRED(__VA_ARGS__)

// Checks current code is running on the desired sequence.
//
// First statement validates it is running on the sequence `x`.
// Second statement annotates for the thread safety analyzer the check was done.
// Such annotation has to be attached to a function, and that function has to be
// called. Thus current implementation creates a noop lambda and calls it.
#define OCTK_DCHECK_RUN_ON(x)                                                                                          \
    OCTK_DCHECK((x)->IsCurrent()) << octk::internal::ExpectationToString(x);                                           \
    []() OCTK_ATTRIBUTE_ASSERT_EXCLUSIVE_LOCK(x) { }()
OCTK_END_NAMESPACE

#endif // _OCTK_SEQUENCE_CHECKER_HPP
