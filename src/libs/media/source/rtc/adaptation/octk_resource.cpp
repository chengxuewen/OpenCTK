//
// Created by cxw on 25-8-15.
//

#include <octk_resource.hpp>
#include <octk_checks.hpp>

OCTK_BEGIN_NAMESPACE

const char *ResourceUsageStateToString(ResourceUsageState usage_state)
{
    switch (usage_state)
    {
        case ResourceUsageState::kOveruse: return "kOveruse";
        case ResourceUsageState::kUnderuse: return "kUnderuse";
    }
    OCTK_CHECK_NOTREACHED();
}

ResourceListener::~ResourceListener() { }

Resource::Resource() { }

Resource::~Resource() { }

OCTK_END_NAMESPACE
