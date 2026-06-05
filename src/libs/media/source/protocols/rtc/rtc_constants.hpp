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

#include <openctk/media/media_global.hpp>

OCTK_BEGIN_NAMESPACE

namespace constants
{
namespace rtc
{

namespace sdp
{
OCTK_STATIC_CONSTANT_STRING(kOffer, "offer")
OCTK_STATIC_CONSTANT_STRING(kAnswer, "answer")
OCTK_STATIC_CONSTANT_STRING(kPrAnswer, "pranswer")
OCTK_STATIC_CONSTANT_STRING(kRollback, "rollback")
} // namespace sdp

OCTK_STATIC_CONSTANT_STRING(kBackendNameWebRTC, "WebRTC");
OCTK_STATIC_CONSTANT_STRING(kBackendNameGStreamer, "GStreamer");

// Constraint keys for CreateOffer / CreateAnswer
// Specified by the W3C PeerConnection spec
// static const char *kOfferToReceiveVideo;    // OfferToReceiveVideo
OCTK_STATIC_CONSTANT_STRING(kOfferToReceiveVideo, "OfferToReceiveVideo")
// static const char *kOfferToReceiveAudio;    // OfferToReceiveAudio
OCTK_STATIC_CONSTANT_STRING(kOfferToReceiveAudio, "OfferToReceiveAudio")
// static const char *kVoiceActivityDetection; // VoiceActivityDetection
OCTK_STATIC_CONSTANT_STRING(kVoiceActivityDetection, "VoiceActivityDetection")
// static const char *kIceRestart;             // IceRestart
OCTK_STATIC_CONSTANT_STRING(kIceRestart, "IceRestart")
// These keys are google specific.

// static const char *kUseRtpMux;              // googUseRtpMUX
OCTK_STATIC_CONSTANT_STRING(kUseRtpMux, "googUseRtpMUX")
// static const char *kEnableDscp;            // googDscp
OCTK_STATIC_CONSTANT_STRING(kEnableDscp, "googDscp")

OCTK_STATIC_CONSTANT_STRING(kEnableVideoSuspendBelowMinBitrate, "googSuspendBelowMinBitrate")
// googSuspendBelowMinBitrate
// Constraint to enable combined audio+video bandwidth estimation.
// static const char*
//    kCombinedAudioVideoBwe;  // googCombinedAudioVideoBwe
OCTK_STATIC_CONSTANT_STRING(kScreencastMinBitrate, "googScreencastMinBitrate")
OCTK_STATIC_CONSTANT_STRING(kCpuOveruseDetection, "googCpuOveruseDetection")

// Specifies number of simulcast layers for all video tracks with a Plan B offer/answer
OCTK_STATIC_CONSTANT_STRING(kNumSimulcastLayers, "googNumSimulcastLayers")
OCTK_STATIC_CONSTANT_STRING(kRawPacketizationForVideoEnabled, "googRawPacketizationForVideoEnabled")
} // namespace rtc
} // namespace constants

OCTK_END_NAMESPACE