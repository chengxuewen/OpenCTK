/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
** Copyright (c) 2023 The WebRTC project authors. All Rights Reserved.
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

// This header file provides wrapper for common OpenCTK media utilities.
// Different applications may need different implementations of these utilities.
// The main purpose of the `MediaContext` class below is to propagate references
// to those utilities to all OpenCTK media classes that need them.

#pragma once

#include <octk_task_queue_factory.hpp>
#include <octk_media_event_log.hpp>
#include <octk_nullability.hpp>

#include <utility>

OCTK_BEGIN_NAMESPACE

// These classes are forward declared to keep MediaContext dependencies
// lightweight. Users who need any of the types below should include their
// header explicitly.
class Clock;
// class TaskQueueFactory;
class FieldTrialsView;
// class MediaEventLog;

// Contains references to OpenCTK media utilities. Object of this class should be
// passed as a construction parameter and saved by value in each class that
// needs it. Most classes shouldn't create a new instance of the `MediaContext`,
// but instead should use a propagated copy.
// Usually MediaContext should be the first parameter in a constructor or a
// factory, and the first member in the class. Keeping MediaContext as the first
// member in the class ensures utilities (e.g. clock) are still valid during
// destruction of other members.
//
// Example:
//    class PeerConnection {
//     public:
//      PeerConnection(const MediaContext& context, ...)
//          : context_(context),
//            log_duration_on_destruction_(&context_.clock()),
//            rtp_manager_(context_, ...),
//            ...
//
//      const FieldTrialsView& trials() const { return context_.field_trials(); }
//
//      scoped_refptr<RtpTransceiverInterface> AddTransceiver(...) {
//        return make_ref_counted<RtpTransceiverImpl>(context_, ...);
//      }
//
//     private:
//      const MediaContext context_;
//      Stats log_duration_on_destruction_;
//      RtpTransmissionManager rtp_manager_;
//    };
// This class is thread safe.
class OCTK_MEDIA_API MediaContext final
{
public:
    using Storage = std::shared_ptr<void>;

    // Default constructor is deleted in favor of creating this object using
    // `MediaContextFactory`. To create the default media context use
    // `MediaContextFactory().Create()` or `CreateMediaContext()`.
    MediaContext() = delete;

    MediaContext(const MediaContext &) = default;
    MediaContext(MediaContext &&) = default;
    MediaContext &operator=(const MediaContext &) = default;
    MediaContext &operator=(MediaContext &&) = default;

    ~MediaContext() = default;

    // Provides means to alter behavior, mostly for A/B testing new features.
    const FieldTrialsView &field_trials() const;

    inline const FieldTrialsView *field_trials_ptr() const { return field_trials_; }

    // Provides an interface to query current time.
    Clock &clock() const;

    // Provides a factory for task queues, OpenCTK threading primitives.
    TaskQueueFactory &task_queue_factory() const;

    // Provides an interface for collecting structured logs.
    MediaEventLog &event_log() const;

private:
    friend class MediaContextFactory;
    MediaContext(const Storage &storage,
                 Nonnull<const FieldTrialsView *> field_trials,
                 Nonnull<Clock *> clock,
                 Nonnull<TaskQueueFactory *> task_queue_factory,
                 Nonnull<MediaEventLog *> event_log)
        : storage_(storage)
        , field_trials_(field_trials)
        , clock_(clock)
        , task_queue_factory_(task_queue_factory)
        , event_log_(event_log)
    {
    }

    // Container that keeps ownership of the utilities below.
    // Defining this as a RefCountedBase allows `MediaContext` to share this
    // storage with another `MediaContext`, in particular allows `MediaContext` to
    // be copyable. It is up to the `MediaContextFactory` to provide an object that
    // ensures references to utilities below are valid while object in the
    // `storage_` is alive.
    Storage storage_;

    Nonnull<const FieldTrialsView *> field_trials_;
    Nonnull<Clock *> clock_;
    Nonnull<TaskQueueFactory *> task_queue_factory_;
    Nonnull<MediaEventLog *> event_log_;
};

//------------------------------------------------------------------------------
// Implementation details follow
//------------------------------------------------------------------------------

inline const FieldTrialsView &MediaContext::field_trials() const
{
    return *field_trials_;
}

inline Clock &MediaContext::clock() const
{
    return *clock_;
}

inline TaskQueueFactory &MediaContext::task_queue_factory() const
{
    return *task_queue_factory_;
}

inline MediaEventLog &MediaContext::event_log() const
{
    return *event_log_;
}

OCTK_END_NAMESPACE