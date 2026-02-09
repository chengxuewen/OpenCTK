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
    RtcIceServer ice_servers[RtcIceServer::kMaxSize];
    RtcIceTransportsType type{RtcIceTransportsType::kAll};
    RtcBundlePolicy bundle_policy{RtcBundlePolicy::kBalanced};
    RtcRtcpMuxPolicy rtcp_mux_policy{RtcRtcpMuxPolicy::kRequire};
    RtcCandidateNetworkPolicy candidate_network_policy{RtcCandidateNetworkPolicy::kAll};
    RtcTcpCandidatePolicy tcp_candidate_policy{RtcTcpCandidatePolicy::kEnabled};

    int ice_candidate_pool_size{0};

    RtcMediaSecurityType srtp_type{RtcMediaSecurityType::kDTLS_SRTP};
    RtcSdpSemantics sdp_semantics{RtcSdpSemantics::kUnifiedPlan};

    bool offer_to_receive_audio{true};
    bool offer_to_receive_video{true};

    bool disable_ipv6{false};
    bool disable_ipv6_on_wifi{false};
    int max_ipv6_networks{5};
    bool disable_link_local_networks{false};
    int screencast_min_bitrate{-1};

    // private
    bool use_rtp_mux{true};
    uint32_t local_audio_bandwidth{128};
    uint32_t local_video_bandwidth{512};
};

OCTK_END_NAMESPACE
