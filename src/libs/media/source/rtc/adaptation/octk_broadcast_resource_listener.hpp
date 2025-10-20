/*
*  Copyright 2020 The WebRTC Project Authors. All rights reserved.
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source
*  tree. An additional intellectual property rights grant can be found
*  in the file PATENTS.  All contributing project authors may
*  be found in the AUTHORS file in the root of the source tree.
*/

#ifndef _OCTK_BROADCAST_RESOURCE_LISTENER_HPP
#define _OCTK_BROADCAST_RESOURCE_LISTENER_HPP

#include <octk_shared_ref_ptr.hpp>
#include <octk_resource.hpp>
#include <octk_mutex.hpp>

OCTK_BEGIN_NAMESPACE

// Responsible for forwarding 1 resource usage measurement to N listeners by
// creating N "adapter" resources.
//
// Example:
// If we have ResourceA, ResourceListenerX and ResourceListenerY we can create a
// BroadcastResourceListener that listens to ResourceA, use CreateAdapter() to
// spawn adapter resources ResourceX and ResourceY and let ResourceListenerX
// listen to ResourceX and ResourceListenerY listen to ResourceY. When ResourceA
// makes a measurement it will be echoed by both ResourceX and ResourceY.
//
// TODO(https://crbug.com/webrtc/11565): When the ResourceAdaptationProcessor is
// moved to call there will only be one ResourceAdaptationProcessor that needs
// to listen to the injected resources. When this is the case, delete this class
// and DCHECK that a Resource's listener is never overwritten.
class BroadcastResourceListener : public ResourceListener
{
public:
    explicit BroadcastResourceListener(SharedRefPtr<Resource> source_resource);
    ~BroadcastResourceListener() override;

    SharedRefPtr<Resource> SourceResource() const;
    void StartListening();
    void StopListening();

    // Creates a Resource that redirects any resource usage measurements that
    // BroadcastResourceListener receives to its listener.
    SharedRefPtr<Resource> CreateAdapterResource();

    // Unregister the adapter from the BroadcastResourceListener; it will no
    // longer receive resource usage measurement and will no longer be referenced.
    // Use this to prevent memory leaks of old adapters.
    void RemoveAdapterResource(SharedRefPtr<Resource> resource);
    std::vector<SharedRefPtr<Resource>> GetAdapterResources();

    // ResourceListener implementation.
    void OnResourceUsageStateMeasured(SharedRefPtr<Resource> resource, ResourceUsageState usage_state) override;

private:
    class AdapterResource;
    friend class AdapterResource;

    const SharedRefPtr<Resource> source_resource_;
    Mutex lock_;
    bool is_listening_ OCTK_ATTRIBUTE_GUARDED_BY(lock_);
    // The AdapterResource unregisters itself prior to destruction, guaranteeing
    // that these pointers are safe to use.
    std::vector<SharedRefPtr<AdapterResource>> adapters_ OCTK_ATTRIBUTE_GUARDED_BY(lock_);
};

OCTK_END_NAMESPACE

#endif // _OCTK_BROADCAST_RESOURCE_LISTENER_HPP
