//
// Created by cxw on 25-8-15.
//

#include "octk_broadcast_resource_listener.hpp"
#include <octk_ref_counted_object.hpp>
#include <octk_string_view.hpp>
#include <octk_checks.hpp>

OCTK_BEGIN_NAMESPACE


// The AdapterResource redirects resource usage measurements from its parent to
// a single ResourceListener.
class BroadcastResourceListener::AdapterResource : public Resource
{
public:
    explicit AdapterResource(StringView name)
        : name_(std::move(name))
    {
    }
    ~AdapterResource() override { OCTK_DCHECK(!listener_); }

    // The parent is letting us know we have a usage neasurement.
    void OnResourceUsageStateMeasured(ResourceUsageState usage_state)
    {
        Mutex::Locker locker(&lock_);
        if (!listener_)
            return;
        listener_->OnResourceUsageStateMeasured(ScopedRefPtr<Resource>(this), usage_state);
    }

    // Resource implementation.
    std::string Name() const override { return name_; }
    void SetResourceListener(ResourceListener *listener) override
    {
        Mutex::Locker locker(&lock_);
        OCTK_DCHECK(!listener_ || !listener);
        listener_ = listener;
    }

private:
    const std::string name_;
    Mutex lock_;
    ResourceListener *listener_ OCTK_ATTRIBUTE_GUARDED_BY(lock_) = nullptr;
};

BroadcastResourceListener::BroadcastResourceListener(ScopedRefPtr<Resource> source_resource)
    : source_resource_(source_resource)
    , is_listening_(false)
{
    OCTK_DCHECK(source_resource_);
}

BroadcastResourceListener::~BroadcastResourceListener() { OCTK_DCHECK(!is_listening_); }

ScopedRefPtr<Resource> BroadcastResourceListener::SourceResource() const { return source_resource_; }

void BroadcastResourceListener::StartListening()
{
    Mutex::Locker locker(&lock_);
    OCTK_DCHECK(!is_listening_);
    source_resource_->SetResourceListener(this);
    is_listening_ = true;
}

void BroadcastResourceListener::StopListening()
{
    Mutex::Locker locker(&lock_);
    OCTK_DCHECK(is_listening_);
    OCTK_DCHECK(adapters_.empty());
    source_resource_->SetResourceListener(nullptr);
    is_listening_ = false;
}

ScopedRefPtr<Resource> BroadcastResourceListener::CreateAdapterResource()
{
    Mutex::Locker locker(&lock_);
    OCTK_DCHECK(is_listening_);
    ScopedRefPtr<AdapterResource> adapter =
        utils::makeRefCounted<AdapterResource>(source_resource_->Name() + "Adapter");
    adapters_.push_back(adapter);
    return adapter;
}

void BroadcastResourceListener::RemoveAdapterResource(ScopedRefPtr<Resource> resource)
{
    Mutex::Locker locker(&lock_);
    auto it = std::find(adapters_.begin(), adapters_.end(), resource);
    OCTK_DCHECK(it != adapters_.end());
    adapters_.erase(it);
}

std::vector<ScopedRefPtr<Resource>> BroadcastResourceListener::GetAdapterResources()
{
    std::vector<ScopedRefPtr<Resource>> resources;
    Mutex::Locker locker(&lock_);
    for (const auto &adapter : adapters_)
    {
        resources.push_back(adapter);
    }
    return resources;
}

void BroadcastResourceListener::OnResourceUsageStateMeasured(ScopedRefPtr<Resource> resource,
                                                             ResourceUsageState usage_state)
{
    OCTK_DCHECK_EQ(resource, source_resource_);
    Mutex::Locker locker(&lock_);
    for (const auto &adapter : adapters_)
    {
        adapter->OnResourceUsageStateMeasured(usage_state);
    }
}

OCTK_END_NAMESPACE