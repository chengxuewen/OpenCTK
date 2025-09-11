//
// Created by cxw on 25-8-15.
//

#ifndef _OCTK_FAKE_RESOURCE_HPP
#define _OCTK_FAKE_RESOURCE_HPP

#include <octk_ref_counted_object.hpp>
#include <octk_string_view.hpp>
#include <octk_resource.hpp>

OCTK_BEGIN_NAMESPACE

// Fake resource used for testing.
class FakeResource : public Resource
{
public:
    static ScopedRefPtr<FakeResource> Create(StringView name) { return utils::makeRefCounted<FakeResource>(name); }

    explicit FakeResource(StringView name)
        : Resource()
        , mName(name)
        , listener_(nullptr)
    {
    }
    ~FakeResource() override { }

    void SetUsageState(ResourceUsageState usage_state)
    {
        if (listener_)
        {
            listener_->OnResourceUsageStateMeasured(ScopedRefPtr<Resource>(this), usage_state);
        }
    }

    // Resource implementation.
    std::string Name() const override { return mName; }
    void SetResourceListener(ResourceListener *listener) override { listener_ = listener; }

private:
    const std::string mName;
    ResourceListener *listener_;
};

OCTK_END_NAMESPACE

#endif // _OCTK_FAKE_RESOURCE_HPP
