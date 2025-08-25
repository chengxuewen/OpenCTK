/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
**
** License: MIT License
**
** Permission is hereby granted, free of charge, to any person obtaining a copy
*of this software and associated
** documentation files (the "Software"), to deal in the Software without
*restriction, including without limitation
** the rights to use, copy, modify, merge, publish, distribute, sublicense,
*and/or sell copies of the Software,
** and to permit persons to whom the Software is furnished to do so, subject to
*the following conditions:
**
** The above copyright notice and this permission notice shall be included in
*all copies or substantial portions
** of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*IMPLIED, INCLUDING BUT NOT LIMITED
** TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
*NONINFRINGEMENT. IN NO EVENT SHALL
** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*LIABILITY, WHETHER IN AN ACTION OF
** CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
*SOFTWARE OR THE USE OR OTHER DEALINGS
** IN THE SOFTWARE.
**
***********************************************************************************************************************/

#ifndef _OCTK_ICE_TRANSPORT_INTERFACE_HPP
#define _OCTK_ICE_TRANSPORT_INTERFACE_HPP

#include <octk_scoped_refptr.hpp>
#include <octk_media_global.hpp>
#include <octk_ref_count.hpp>

OCTK_BEGIN_NAMESPACE

class ActiveIceControllerFactoryInterface;
class AsyncDnsResolverFactoryInterface;
class IceControllerFactoryInterface;
class IceTransportInternal;
class FieldTrialsView;
class PortAllocator;
class RtcEventLog;

// An ICE transport, as represented to the outside world.
// This object is refcounted, and is therefore alive until the
// last holder has released it.
class IceTransportInterface : public RefCountInterface
{
public:
    // Accessor for the internal representation of an ICE transport.
    // The returned object can only be safely used on the signalling thread.
    // TODO(crbug.com/907849): Add API calls for the functions that have to
    // be exposed to clients, and stop allowing access to the
    // IceTransportInternal API.
    virtual IceTransportInternal *internal() = 0;
};

struct IceTransportInit final
{
public:
    IceTransportInit() = default;
    IceTransportInit(const IceTransportInit &) = delete;
    IceTransportInit(IceTransportInit &&) = default;
    IceTransportInit &operator=(const IceTransportInit &) = delete;
    IceTransportInit &operator=(IceTransportInit &&) = default;

    PortAllocator *port_allocator() { return port_allocator_; }
    void set_port_allocator(PortAllocator *port_allocator) { port_allocator_ = port_allocator; }

    AsyncDnsResolverFactoryInterface *async_dns_resolver_factory() { return async_dns_resolver_factory_; }
    void set_async_dns_resolver_factory(AsyncDnsResolverFactoryInterface *async_dns_resolver_factory)
    {
        async_dns_resolver_factory_ = async_dns_resolver_factory;
    }

    RtcEventLog *event_log() { return event_log_; }
    void set_event_log(RtcEventLog *event_log) { event_log_ = event_log; }

    void set_ice_controller_factory(IceControllerFactoryInterface *ice_controller_factory)
    {
        ice_controller_factory_ = ice_controller_factory;
    }
    IceControllerFactoryInterface *ice_controller_factory() { return ice_controller_factory_; }

    // An active ICE controller actively manages the connection used by an ICE
    // transport, in contrast with a legacy ICE controller that only picks the
    // best connection to use or ping, and lets the transport decide when and
    // whether to switch.
    //
    // Which ICE controller is used is determined as follows:
    //
    //   1. If an active ICE controller factory is supplied, it is used and
    //      the legacy ICE controller factory is not used.
    //   2. If not, a default active ICE controller is used, wrapping over the
    //      supplied or the default legacy ICE controller.
    void set_active_ice_controller_factory(ActiveIceControllerFactoryInterface *active_ice_controller_factory)
    {
        active_ice_controller_factory_ = active_ice_controller_factory;
    }
    ActiveIceControllerFactoryInterface *active_ice_controller_factory() { return active_ice_controller_factory_; }

    const FieldTrialsView *field_trials() { return field_trials_; }
    void set_field_trials(const FieldTrialsView *field_trials) { field_trials_ = field_trials; }

private:
    PortAllocator *port_allocator_ = nullptr;
    AsyncDnsResolverFactoryInterface *async_dns_resolver_factory_ = nullptr;
    RtcEventLog *event_log_ = nullptr;
    IceControllerFactoryInterface *ice_controller_factory_ = nullptr;
    ActiveIceControllerFactoryInterface *active_ice_controller_factory_ = nullptr;
    const FieldTrialsView *field_trials_ = nullptr;
    // TODO(https://crbug.com/webrtc/12657): Redesign to have const members.
};

// TODO(qingsi): The factory interface is defined in this file instead of its
// namesake file ice_transport_factory.h to avoid the extra dependency on p2p/
// introduced there by the p2p/-dependent factory methods. Move the factory
// methods to a different file or rename it.
class IceTransportFactory
{
public:
    virtual ~IceTransportFactory() = default;
    // As a refcounted object, the returned ICE transport may outlive the host
    // construct into which its reference is given, e.g. a peer connection. As a
    // result, the returned ICE transport should not hold references to any object
    // that the transport does not own and that has a lifetime bound to the host
    // construct. Also, assumptions on the thread safety of the returned transport
    // should be clarified by implementations. For example, a peer connection
    // requires the returned transport to be constructed and destroyed on the
    // network thread and an ICE transport factory that intends to work with a
    // peer connection should offer transports compatible with these assumptions.
    virtual ScopedRefPtr<IceTransportInterface>
    CreateIceTransport(const std::string &transport_name, int component, IceTransportInit init) = 0;
};

OCTK_END_NAMESPACE

#endif // _OCTK_ICE_TRANSPORT_INTERFACE_HPP
