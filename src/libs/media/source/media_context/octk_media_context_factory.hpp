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

#pragma once

#include <octk_media_context.hpp>

#include <memory>
#include <utility>

OCTK_BEGIN_NAMESPACE

// These classes are forward declared to reduce amount of headers exposed
// through api header.
// IWYU pragma: begin_keep
class Clock;
// class TaskQueueFactory;
class FieldTrialsView;
// class MediaEventLog;
// IWYU pragma: end_keep
// Constructs `MediaContext`.
// Individual utilities are provided using one of the `Set` functions.
// `Set` functions do nothing when nullptr value is passed.
// Creates default implementations for utilities that are not provided.
//
// Examples:
//    MediaContext default_context = MediaContextFactory().Create();
//
//    MediaContextFactory factory;
//    factory.Set(std::make_unique<CustomTaskQueueFactory>());
//    factory.Set(std::make_unique<CustomFieldTrials>());
//    MediaContext custom_context = factory.Create();
//
class OCTK_MEDIA_API MediaContextFactory final
{
public:
    MediaContextFactory() = default;
    explicit MediaContextFactory(const MediaContext &context);

    MediaContextFactory(const MediaContextFactory &) = default;
    MediaContextFactory(MediaContextFactory &&) = default;
    MediaContextFactory &operator=(const MediaContextFactory &) = default;
    MediaContextFactory &operator=(MediaContextFactory &&) = default;

    ~MediaContextFactory() = default;

    void Set(Nonnull<std::unique_ptr<const FieldTrialsView>> utility);
    void Set(Nonnull<std::unique_ptr<Clock>> utility);
    void Set(Nonnull<std::unique_ptr<TaskQueueFactory>> utility);
    void Set(Nonnull<std::unique_ptr<MediaEventLog>> utility);

    void Set(Nonnull<const FieldTrialsView *> utility);
    void Set(Nonnull<Clock *> utility);
    void Set(Nonnull<TaskQueueFactory *> utility);
    void Set(Nonnull<MediaEventLog *> utility);

    MediaContext Create() const;

private:
    MediaContext CreateWithDefaults() &&;

    MediaContext::Storage leaf_;

    Nonnull<const FieldTrialsView *> field_trials_ = nullptr;
    Nonnull<Clock *> clock_ = nullptr;
    Nonnull<TaskQueueFactory *> task_queue_factory_ = nullptr;
    Nonnull<MediaEventLog *> event_log_ = nullptr;
};

// Helper for concise way to create a media context.
// `MediaContext context = CreateMediaContext(utility1, utility2)` is a shortcut to
// `MediaContextFactory factory;
// factory.Set(utility1);
// factory.Set(utility2);
// MediaContext context = factory.Create();`
//
// Examples:
//    MediaContext default_context = CreateMediaContext();
//    MediaContext custom_context =
//        CreateMediaContext(std::make_unique<CustomTaskQueueFactory>(),
//                          std::make_unique<CustomFieldTrials>());
template <typename... Utilities>
MediaContext CreateMediaContext(Utilities &&...utilities);

//------------------------------------------------------------------------------
// Implementation details follow
//------------------------------------------------------------------------------

inline void MediaContextFactory::Set(Nonnull<const FieldTrialsView *> utility)
{
    if (utility != nullptr)
    {
        field_trials_ = utility;
    }
}

inline void MediaContextFactory::Set(Nonnull<Clock *> utility)
{
    if (utility != nullptr)
    {
        clock_ = utility;
    }
}

inline void MediaContextFactory::Set(Nonnull<TaskQueueFactory *> utility)
{
    if (utility != nullptr)
    {
        task_queue_factory_ = utility;
    }
}

inline void MediaContextFactory::Set(Nonnull<MediaEventLog *> utility)
{
    if (utility != nullptr)
    {
        event_log_ = utility;
    }
}

namespace octk_create_media_context_internal
{

inline void Set(MediaContextFactory & /* factory */)
{
}

template <typename FirstUtility, typename... Utilities>
void Set(MediaContextFactory &factory, FirstUtility &&first, Utilities &&...utilities)
{
    factory.Set(std::forward<FirstUtility>(first));
    Set(factory, std::forward<Utilities>(utilities)...);
}

} // namespace octk_create_media_context_internal

template <typename... Utilities>
MediaContext CreateMediaContext(Utilities &&...utilities)
{
    MediaContextFactory factory;
    octk_create_media_context_internal::Set(factory, std::forward<Utilities>(utilities)...);
    return factory.Create();
}

OCTK_END_NAMESPACE