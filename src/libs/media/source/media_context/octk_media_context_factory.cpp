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

#include <private/octk_field_trials_registry_p.hpp>
#include <private/octk_field_trials_p.hpp>
#include <octk_media_context_factory.hpp>
#include <octk_field_trials_view.hpp>
#include <octk_checks.hpp>
#include <octk_clock.hpp>

#include <memory>
#include <utility>

OCTK_BEGIN_NAMESPACE

namespace
{

template <typename T>
void Store(Nonnull<std::unique_ptr<T>> value, MediaContext::Storage &leaf)
{
    class StorageNode
    {
    public:
        StorageNode(MediaContext::Storage parent, Nonnull<std::unique_ptr<T>> value)
            : parent_(std::move(parent))
            , value_(std::move(value))
        {
        }

        StorageNode(const StorageNode &) = delete;
        StorageNode &operator=(const StorageNode &) = delete;

        ~StorageNode() = default;

    private:
        MediaContext::Storage parent_;
        Nonnull<std::unique_ptr<T>> value_;
    };

    // Utilities provided with ownership form a tree:
    // Root is nullptr, each node keeps an ownership of one utility.
    // Each child node has a link to the parent, but parent is unaware of its
    // children. Each `MediaContextFactory` and `MediaContext` keep a reference to a
    // 'leaf_' - node with the last provided utility. This way `MediaContext` keeps
    // ownership of a single branch of the storage tree with each used utility
    // owned by one of the nodes on that branch.
    leaf = std::make_shared<StorageNode>(std::move(leaf), std::move(value));
}

} // namespace

MediaContextFactory::MediaContextFactory(const MediaContext &context)
    : leaf_(context.storage_)
    , field_trials_(context.field_trials_)
    , clock_(context.clock_)
    , task_queue_factory_(context.task_queue_factory_)
    , event_log_(context.event_log_)
{
}

void MediaContextFactory::Set(Nullable<std::unique_ptr<const FieldTrialsView>> utility)
{
    if (utility)
    {
        field_trials_ = utility.get();
        Store(std::move(utility), leaf_);
    }
}

void MediaContextFactory::Set(Nullable<std::unique_ptr<Clock>> utility)
{
    if (utility)
    {
        clock_ = utility.get();
        Store(std::move(utility), leaf_);
    }
}

void MediaContextFactory::Set(Nullable<std::unique_ptr<TaskQueueFactory>> utility)
{
    if (utility != nullptr)
    {
        task_queue_factory_ = utility.get();
        Store(std::move(utility), leaf_);
    }
}

void MediaContextFactory::Set(Nullable<std::unique_ptr<MediaEventLog>> utility)
{
    if (utility != nullptr)
    {
        event_log_ = utility.get();
        Store(std::move(utility), leaf_);
    }
}

MediaContext MediaContextFactory::CreateWithDefaults() &&
{
    if (!field_trials_)
    {
        Set(std::make_unique<FieldTrials>(""));
    }
    if (!clock_)
    {
        Set(Clock::GetRealTimeClock());
    }
    if (task_queue_factory_ == nullptr)
    {
        Set(std::move(TaskQueueFactory::CreateDefault()));
    }
    if (event_log_ == nullptr)
    {
        Set(std::make_unique<MediaEventLogNull>());
    }

    OCTK_DCHECK(field_trials_ != nullptr);
    OCTK_DCHECK(clock_ != nullptr);
    OCTK_DCHECK(task_queue_factory_ != nullptr);
    OCTK_DCHECK(event_log_ != nullptr);
    return MediaContext(std::move(leaf_), //
                        field_trials_,
                        clock_,
                        task_queue_factory_,
                        event_log_);
}

MediaContext MediaContextFactory::Create() const
{
    // Create a temporary copy to avoid mutating `this` with default utilities.
    return MediaContextFactory(*this).CreateWithDefaults();
}

OCTK_END_NAMESPACE
