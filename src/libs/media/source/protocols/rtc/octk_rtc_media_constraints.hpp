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

#include <octk_shared_pointer.hpp>
#include <octk_string.hpp>

OCTK_BEGIN_NAMESPACE

class RtcMediaConstraints
{
public:
    // These keys are google specific.
    static const char *kGoogEchoCancellation; // googEchoCancellation

    static const char *kExtendedFilterEchoCancellation; // googEchoCancellation2
    static const char *kDAEchoCancellation;             // googDAEchoCancellation
    static const char *kAutoGainControl;                // googAutoGainControl
    static const char *kNoiseSuppression;               // googNoiseSuppression
    static const char *kHighpassFilter;                 // googHighpassFilter
    static const char *kAudioMirroring;                 // googAudioMirroring
    static const char *kAudioNetworkAdaptorConfig;      // goodAudioNetworkAdaptorConfig

    // Constraint keys for CreateOffer / CreateAnswer
    // Specified by the W3C PeerConnection spec
    static const char *kOfferToReceiveVideo;    // OfferToReceiveVideo
    static const char *kOfferToReceiveAudio;    // OfferToReceiveAudio
    static const char *kVoiceActivityDetection; // VoiceActivityDetection
    static const char *kIceRestart;             // IceRestart
                                                // These keys are google specific.
    static const char *kUseRtpMux;              // googUseRtpMUX

    // Constraints values.
    static const char *kValueTrue;  // true
    static const char *kValueFalse; // false

    // PeerConnection constraint keys.
    // Temporary pseudo-constraints used to enable DataChannels
    static const char *kEnableRtpDataChannels; // Enable RTP DataChannels
                                               // Google-specific constraint keys.
                                               // Temporary pseudo-constraint for enabling DSCP through JS.
    static const char *kEnableDscp;            // googDscp
                                               // Constraint to enable IPv6 through JS.
    static const char *kEnableIPv6;            // googIPv6
                                               // Temporary constraint to enable suspend below min bitrate feature.
    static const char *kEnableVideoSuspendBelowMinBitrate;
    // googSuspendBelowMinBitrate
    // Constraint to enable combined audio+video bandwidth estimation.
    // static const char*
    //    kCombinedAudioVideoBwe;  // googCombinedAudioVideoBwe
    static const char *kScreencastMinBitrate; // googScreencastMinBitrate
    static const char *kCpuOveruseDetection;  // googCpuOveruseDetection

    // Specifies number of simulcast layers for all video tracks
    // with a Plan B offer/answer
    // (see RTCOfferAnswerOptions::num_simulcast_layers).
    static const char *kNumSimulcastLayers;

public:
    static SharedPointer<RtcMediaConstraints> Create();

    virtual void AddMandatoryConstraint(const String key, const String value) = 0;

    virtual void AddOptionalConstraint(const String key, const String value) = 0;

protected:
    virtual ~RtcMediaConstraints() { }
};


OCTK_END_NAMESPACE
