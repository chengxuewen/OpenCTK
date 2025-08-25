/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
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

#ifndef _OCTK_RTC_EVENT_HPP
#define _OCTK_RTC_EVENT_HPP

#include <octk_media_global.hpp>

OCTK_BEGIN_NAMESPACE

// This class allows us to store unencoded RTC events. Subclasses of this class
// store the actual information. This allows us to keep all unencoded events,
// even when their type and associated information differ, in the same buffer.
// Additionally, it prevents dependency leaking - a module that only logs
// events of type RtcEvent_A doesn't need to know about anything associated
// with events of type RtcEvent_B.
class RtcEvent
{
public:
    // Subclasses of this class have to associate themselves with a unique value
    // of Type. This leaks the information of existing subclasses into the
    // superclass, but the *actual* information - rtclog::StreamConfig, etc. -
    // is kept separate.
    enum class Type : uint32_t
    {
        AlrStateEvent,
        RouteChangeEvent,
        RemoteEstimateEvent,
        AudioNetworkAdaptation,
        AudioPlayout,
        AudioReceiveStreamConfig,
        AudioSendStreamConfig,
        BweUpdateDelayBased,
        BweUpdateLossBased,
        DtlsTransportState,
        DtlsWritableState,
        IceCandidatePairConfig,
        IceCandidatePairEvent,
        ProbeClusterCreated,
        ProbeResultFailure,
        ProbeResultSuccess,
        RtcpPacketIncoming,
        RtcpPacketOutgoing,
        RtpPacketIncoming,
        RtpPacketOutgoing,
        VideoReceiveStreamConfig,
        VideoSendStreamConfig,
        GenericPacketSent,
        GenericPacketReceived,
        GenericAckReceived,
        FrameDecoded,
        NetEqSetMinimumDelay,
        BeginV3Log = 0x2501580,
        EndV3Log = 0x2501581,
        FakeEvent, // For unit testing.
    };

    RtcEvent();
    virtual ~RtcEvent() = default;

    virtual Type GetType() const = 0;

    virtual bool IsConfigEvent() const = 0;

    // Events are grouped by Type before being encoded.
    // Optionally, `GetGroupKey` can be overloaded to group the
    // events by a secondary key (in addition to the event type.)
    // This can, in some cases, improve compression efficiency
    // e.g. by grouping events by SSRC.
    virtual uint32_t GetGroupKey() const { return 0; }

    int64_t timestamp_ms() const { return timestamp_us_ / 1000; }
    int64_t timestamp_us() const { return timestamp_us_; }

protected:
    // clang-format off
    explicit RtcEvent(int64_t timestamp_us) : timestamp_us_(timestamp_us) { }
    // clang-format off
    const int64_t timestamp_us_;
};
OCTK_END_NAMESPACE

#endif // _OCTK_RTC_EVENT_HPP
