//
// Created by cxw on 25-8-15.
//

#include <octk_ref_counted_object.hpp>
#include <octk_rtc_event_log.hpp>
#include <octk_field_trials.hpp>
#include <octk_rtc_context.hpp>
#include <octk_ref_count.hpp>
#include <octk_memory.hpp>

OCTK_BEGIN_NAMESPACE

namespace
{

template <typename T> void Store(Nonnull<std::unique_ptr<T>> value, SharedRefPtr<const RefCountedBase> &leaf)
{
    class StorageNode : public RefCountedBase
    {
    public:
        StorageNode(SharedRefPtr<const RefCountedBase> parent, Nonnull<std::unique_ptr<T>> value)
            : parent_(std::move(parent))
            , value_(std::move(value))
        {
        }

        StorageNode(const StorageNode &) = delete;
        StorageNode &operator=(const StorageNode &) = delete;

        ~StorageNode() override = default;

    private:
        SharedRefPtr<const RefCountedBase> parent_;
        Nonnull<std::unique_ptr<T>> value_;
    };

    // Utilities provided with ownership form a tree:
    // Root is nullptr, each node keeps an ownership of one utility.
    // Each child node has a link to the parent, but parent is unaware of its
    // children. Each `RtcContextFactory` and `RtcContext` keep a reference to a
    // 'leaf_' - node with the last provided utility. This way `RtcContext` keeps
    // ownership of a single branch of the storage tree with each used utiltity
    // owned by one of the nodes on that branch.
    leaf = utils::makeRefCounted<StorageNode>(std::move(leaf), std::move(value));
}

} // namespace

RtcContextFactory::RtcContextFactory(const RtcContext &env)
    : leaf_(env.storage_)
    , field_trials_(env.field_trials_)
    , clock_(env.clock_)
    , task_queue_factory_(env.task_queue_factory_)
    , event_log_(env.event_log_)
{
}

void RtcContextFactory::Set(Nullable<std::unique_ptr<const FieldTrialsView>> utility)
{
    if (utility != nullptr)
    {
        field_trials_ = utility.get();
        Store(std::move(utility), leaf_);
    }
}

void RtcContextFactory::Set(Nullable<std::unique_ptr<Clock>> utility)
{
    if (utility != nullptr)
    {
        clock_ = utility.get();
        Store(std::move(utility), leaf_);
    }
}

void RtcContextFactory::Set(Nullable<std::unique_ptr<TaskQueueFactory>> utility)
{
    if (utility != nullptr)
    {
        task_queue_factory_ = utility.get();
        Store(std::move(utility), leaf_);
    }
}

void RtcContextFactory::Set(Nullable<std::unique_ptr<RtcEventLog>> utility)
{
    if (utility != nullptr)
    {
        event_log_ = utility.get();
        Store(std::move(utility), leaf_);
    }
}

RtcContext RtcContextFactory::CreateWithDefaults() &&
{
    if (field_trials_ == nullptr)
    {
        Set(utils::make_unique<FieldTrialBasedConfig>());
    }
    if (clock_ == nullptr)
    {
        Set(Clock::GetRealTimeClock());
    }
    if (task_queue_factory_ == nullptr)
    {
        // Set(utils::createDefaultTaskQueueFactory(field_trials_));
        Set(utils::createDefaultTaskQueueFactory());
    }
    if (event_log_ == nullptr)
    {
        Set(utils::make_unique<RtcEventLogNull>());
    }

    OCTK_DCHECK(field_trials_ != nullptr);
    OCTK_DCHECK(clock_ != nullptr);
    OCTK_DCHECK(task_queue_factory_ != nullptr);
    OCTK_DCHECK(event_log_ != nullptr);
    return RtcContext(std::move(leaf_), //
                      field_trials_,
                      clock_,
                      task_queue_factory_,
                      event_log_);
}

RtcContext RtcContextFactory::Create() const
{
    // Create a temporary copy to avoid mutating `this` with default utilities.
    return RtcContextFactory(*this).CreateWithDefaults();
}

OCTK_END_NAMESPACE
