/*
*  Copyright 2023 The WebRTC Project Authors. All rights reserved.
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source
*  tree. An additional intellectual property rights grant can be found
*  in the file PATENTS.  All contributing project authors may
*  be found in the AUTHORS file in the root of the source tree.
*/

#ifndef _OCTK_RTC_CONTEXT_HPP
#define _OCTK_RTC_CONTEXT_HPP

#include <octk_task_queue_factory.hpp>
#include <octk_field_trials_view.hpp>
#include <octk_scoped_refptr.hpp>
#include <octk_nullability.hpp>
#include <octk_ref_count.hpp>
#include <octk_clock.hpp>

OCTK_BEGIN_NAMESPACE

// These classes are forward declared to keep RtcContext dependencies
// lightweight. Users who need any of the types below should include their
// header explicitely.
class Clock;
class RtcEventLog;
class TaskQueueFactory;
// class FieldTrialsView;

// Contains references to WebRTC utilities. Object of this class should be
// passed as a construction parameter and saved by value in each class that
// needs it. Most classes shouldn't create a new instance of the `RtcContext`,
// but instead should use a propagated copy.
// Usually RtcContext should be the first parameter in a constructor or a
// factory, and the first member in the class. Keeping RtcContext as the first
// member in the class ensures utilities (e.g. clock) are still valid during
// destruction of other members.
//
// Example:
//    class PeerConnection {
//     public:
//      PeerConnection(const RtcContext& env, ...)
//          : env_(env),
//            log_duration_on_destruction_(&env_.clock()),
//            rtp_manager_(env_, ...),
//            ...
//
//      const FieldTrialsView& trials() const { return env_.field_trials(); }
//
//      ScopedRefPtr<RtpTransceiverInterface> AddTransceiver(...) {
//        return make_ref_counted<RtpTransceiverImpl>(env_, ...);
//      }
//
//     private:
//      const RtcContext env_;
//      Stats log_duration_on_destruction_;
//      RtpTransmissionManager rtp_manager_;
//    };
// This class is thread safe.
class OCTK_CORE_API RtcContext final
{
public:
    // Default constructor is deleted in favor of creating this object using
    // `RtcContextFactory`. To create the default environment use
    // `RtcContextFactory().Create()` or `CreateRtcContext()`.
    RtcContext() = delete;

    RtcContext(const RtcContext &) = default;
    RtcContext(RtcContext &&) = default;
    RtcContext &operator=(const RtcContext &) = default;
    RtcContext &operator=(RtcContext &&) = default;

    ~RtcContext() = default;

    // Provides means to alter behavior, mostly for A/B testing new features.
    // See ../../g3doc/field-trials.md
    const FieldTrialsView &field_trials() const;

    // Provides an interface to query current time.
    // See ../../g3doc/implementation_basics.md#time
    Clock &clock() const;

    // Provides a factory for task queues, WebRTC threading primitives.
    // See ../../g3doc/implementation_basics.md#threads
    TaskQueueFactory &task_queue_factory() const;

    // Provides an interface for collecting structured logs.
    // See ../../logging/g3doc/rtc_event_log.md
    RtcEventLog &event_log() const;

private:
    friend class RtcContextFactory;
    RtcContext(ScopedRefPtr<const RefCountedBase> storage,
               Nonnull<const FieldTrialsView *> field_trials,
               Nonnull<Clock *> clock,
               Nonnull<TaskQueueFactory *> task_queue_factory,
               Nonnull<RtcEventLog *> event_log)
        : storage_(std::move(storage))
        , field_trials_(field_trials)
        , clock_(clock)
        , task_queue_factory_(task_queue_factory)
        , event_log_(event_log)
    {
    }

    // Container that keeps ownership of the utilities below.
    // Defining this as a RefCountedBase allows `RtcContext` to share this
    // storage with another `RtcContext`, in particular allows `RtcContext` to
    // be copyable. It is up to the `RtcContextFactory` to provide an object that
    // ensures references to utilties below are valid while object in the
    // `storage_` is alive.
    ScopedRefPtr<const RefCountedBase> storage_;

    Nonnull<const FieldTrialsView *> field_trials_;
    Nonnull<Clock *> clock_;
    Nonnull<TaskQueueFactory *> task_queue_factory_;
    Nonnull<RtcEventLog *> event_log_;
};

//------------------------------------------------------------------------------
// Implementation details follow
//------------------------------------------------------------------------------

inline const FieldTrialsView &RtcContext::field_trials() const { return *field_trials_; }

inline Clock &RtcContext::clock() const { return *clock_; }

inline TaskQueueFactory &RtcContext::task_queue_factory() const { return *task_queue_factory_; }

inline RtcEventLog &RtcContext::event_log() const { return *event_log_; }


// IWYU pragma: end_keep
// Constructs `RtcContext`.
// Individual utilities are provided using one of the `Set` functions.
// `Set` functions do nothing when nullptr value is passed.
// Creates default implementations for utilities that are not provided.
//
// Examples:
//    RtcContext default_env = RtcContextFactory().Create();
//
//    RtcContextFactory factory;
//    factory.Set(std::make_unique<CustomTaskQueueFactory>());
//    factory.Set(std::make_unique<CustomFieldTrials>());
//    RtcContext custom_env = factory.Create();
//
class OCTK_CORE_API RtcContextFactory final
{
public:
    RtcContextFactory() = default;
    explicit RtcContextFactory(const RtcContext &env);

    RtcContextFactory(const RtcContextFactory &) = default;
    RtcContextFactory(RtcContextFactory &&) = default;
    RtcContextFactory &operator=(const RtcContextFactory &) = default;
    RtcContextFactory &operator=(RtcContextFactory &&) = default;

    ~RtcContextFactory() = default;

    void Set(Nullable<std::unique_ptr<const FieldTrialsView>> utility);
    void Set(Nullable<std::unique_ptr<Clock>> utility);
    void Set(Nullable<std::unique_ptr<TaskQueueFactory>> utility);
    void Set(Nullable<std::unique_ptr<RtcEventLog>> utility);

    void Set(Nullable<const FieldTrialsView *> utility);
    void Set(Nullable<Clock *> utility);
    void Set(Nullable<TaskQueueFactory *> utility);
    void Set(Nullable<RtcEventLog *> utility);

    RtcContext Create() const;

private:
    RtcContext CreateWithDefaults() &&;

    ScopedRefPtr<const RefCountedBase> leaf_;

    Nullable<const FieldTrialsView *> field_trials_ = nullptr;
    Nullable<Clock *> clock_ = nullptr;
    Nullable<TaskQueueFactory *> task_queue_factory_ = nullptr;
    Nullable<RtcEventLog *> event_log_ = nullptr;
};

// Helper for concise way to create an environment.
// `RtcContext env = CreateRtcContext(utility1, utility2)` is a shortcut to
// `RtcContextFactory factory;
// factory.Set(utility1);
// factory.Set(utility2);
// RtcContext env = factory.Create();`
//
// Examples:
//    RtcContext default_env = CreateRtcContext();
//    RtcContext custom_env =
//        CreateRtcContext(std::make_unique<CustomTaskQueueFactory>(),
//                          std::make_unique<CustomFieldTrials>());
template <typename... Utilities> RtcContext CreateRtcContext(Utilities &&...utilities);

//------------------------------------------------------------------------------
// Implementation details follow
//------------------------------------------------------------------------------

inline void RtcContextFactory::Set(Nullable<const FieldTrialsView *> utility)
{
    if (utility != nullptr)
    {
        field_trials_ = utility;
    }
}

inline void RtcContextFactory::Set(Nullable<Clock *> utility)
{
    if (utility != nullptr)
    {
        clock_ = utility;
    }
}

inline void RtcContextFactory::Set(Nullable<TaskQueueFactory *> utility)
{
    if (utility != nullptr)
    {
        task_queue_factory_ = utility;
    }
}

inline void RtcContextFactory::Set(Nullable<RtcEventLog *> utility)
{
    if (utility != nullptr)
    {
        event_log_ = utility;
    }
}

namespace internal
{

inline void Set(RtcContextFactory & /* factory */) { }

template <typename FirstUtility, typename... Utilities>
void Set(RtcContextFactory &factory, FirstUtility &&first, Utilities &&...utilities)
{
    factory.Set(std::forward<FirstUtility>(first));
    Set(factory, std::forward<Utilities>(utilities)...);
}

} // namespace internal

template <typename... Utilities> RtcContext CreateRtcContext(Utilities &&...utilities)
{
    RtcContextFactory factory;
    internal::Set(factory, std::forward<Utilities>(utilities)...);
    return factory.Create();
}

OCTK_END_NAMESPACE

#endif // _OCTK_RTC_CONTEXT_HPP
