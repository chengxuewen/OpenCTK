//
// Created by cxw on 25-8-15.
//

#ifndef _OCTK_MOCK_RESOURCE_LISTENER_HPP
#define _OCTK_MOCK_RESOURCE_LISTENER_HPP

#include <octk_resource.hpp>
#include <octk_shared_ref_ptr.hpp>

#include <gmock/gmock.h>

OCTK_BEGIN_NAMESPACE

class MockResourceListener : public ResourceListener
{
public:
    MOCK_METHOD(void,
                OnResourceUsageStateMeasured,
                (SharedRefPtr<Resource> resource, ResourceUsageState usage_state),
                (override));
};

OCTK_END_NAMESPACE

#endif // _OCTK_MOCK_RESOURCE_LISTENER_HPP
