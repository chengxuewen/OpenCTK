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

#include <octk_rtc_types.hpp>

OCTK_BEGIN_NAMESPACE

struct RtcIceServer
{
    String uri;
    String username;
    String password;
    OCTK_STATIC_CONSTANT_NUMBER(kMaxSize, 8)
};

struct RtcConfiguration
{
    RtcIceServer iceServers[RtcIceServer::kMaxSize];
    RtcIceTransportsType type{RtcIceTransportsType::kAll};
    RtcBundlePolicy bundlePolicy{RtcBundlePolicy::kBalanced};
    RtcRtcpMuxPolicy rtcpMuxPolicy{RtcRtcpMuxPolicy::kRequire};
    RtcCandidateNetworkPolicy candidateNetworkPolicy{RtcCandidateNetworkPolicy::kAll};
    RtcTcpCandidatePolicy tcpCandidatePolicy{RtcTcpCandidatePolicy::kEnabled};

    int iceCandidatePoolSize{0};

    RtcMediaSecurityType srtpType{RtcMediaSecurityType::kDTLS_SRTP};
    RtcSdpSemantics sdpSemantics{RtcSdpSemantics::kUnifiedPlan};

    bool offerToReceiveAudio{true};
    bool offerToReceiveVideo{true};

    bool disableIpv6{false};
    bool disableIpv6OnWifi{false};
    int maxIpv6Networks{5};
    bool disableLinkLocalNetworks{false};
    int screencastMinBitrate{-1};

    // private
    bool useRtpMux{true};
    uint32_t localAudioBandWidth{128};
    uint32_t localVideoBandWidth{512};
};

OCTK_END_NAMESPACE
