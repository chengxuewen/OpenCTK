/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2026~Present ChengXueWen.
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

#include <octk_rtc_audio_track.hpp>

OCTK_BEGIN_NAMESPACE

class RtcIceTransport
{
public:
    virtual RtcIceTransport *internal() = 0;
};

#if 0
class RtcIceTransportInit final
{
public:
    RtcIceTransportInit() = default;
    RtcIceTransportInit(const RtcIceTransportInit &) = delete;
    RtcIceTransportInit(RtcIceTransportInit &&) = default;
    RtcIceTransportInit &operator=(const RtcIceTransportInit &) = delete;
    RtcIceTransportInit &operator=(RtcIceTransportInit &&) = default;

    webrtc::PortAllocator *port_allocator() { return port_allocator_; }
    void set_port_allocator(webrtc::PortAllocator *port_allocator) { port_allocator_ = port_allocator; }

    AsyncDnsResolverFactoryInterface *async_dns_resolver_factory() { return async_dns_resolver_factory_; }
    void set_async_dns_resolver_factory(AsyncDnsResolverFactoryInterface *async_dns_resolver_factory)
    {
        RTC_DCHECK(!async_resolver_factory_);
        async_dns_resolver_factory_ = async_dns_resolver_factory;
    }
    AsyncResolverFactory *async_resolver_factory() { return async_resolver_factory_; }
    ABSL_DEPRECATED("bugs.webrtc.org/12598")
    void set_async_resolver_factory(AsyncResolverFactory *async_resolver_factory)
    {
        RTC_DCHECK(!async_dns_resolver_factory_);
        async_resolver_factory_ = async_resolver_factory;
    }

    RtcEventLog *event_log() { return event_log_; }
    void set_event_log(RtcEventLog *event_log) { event_log_ = event_log; }
};
#endif

class RtcIceTransportFactory
{
public:
    virtual ~RtcIceTransportFactory() = default;

    // virtual const SharedPointer<RtcIceTransport> CreateIceTransport(const std::string &transport_name,
    //                                                              int component,
    //                                                              RtcIceTransportInit init) = 0;
};

OCTK_END_NAMESPACE
