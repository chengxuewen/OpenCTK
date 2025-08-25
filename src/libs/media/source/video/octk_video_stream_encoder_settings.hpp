/*
*  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source
*  tree. An additional intellectual property rights grant can be found
*  in the file PATENTS.  All contributing project authors may
*  be found in the AUTHORS file in the root of the source tree.
*/

#ifndef _OCTK_VIDEO_STREAM_ENCODER_SETTINGS_HPP
#define _OCTK_VIDEO_STREAM_ENCODER_SETTINGS_HPP

#include <octk_video_bitrate_allocator_factory.hpp>
#include <octk_video_encoder_factory.hpp>
#include <octk_sdp_video_format.hpp>
#include <octk_video_encoder.hpp>

OCTK_BEGIN_NAMESPACE

class EncoderSwitchRequestCallback
{
public:
    virtual ~EncoderSwitchRequestCallback() { }

    // Requests switch to next negotiated encoder.
    virtual void RequestEncoderFallback() = 0;

    // Requests switch to a specific encoder. If the encoder is not available and
    // `allow_default_fallback` is `true` the default fallback is invoked.
    virtual void RequestEncoderSwitch(const SdpVideoFormat &format, bool allow_default_fallback) = 0;
};

struct VideoStreamEncoderSettings
{
    explicit VideoStreamEncoderSettings(const VideoEncoder::Capabilities &capabilities)
        : capabilities(capabilities)
    {
    }

    // Enables the new method to estimate the cpu load from encoding, used for
    // cpu adaptation.
    bool experiment_cpu_load_estimator = false;

    // Ownership stays with WebrtcVideoEngine (delegated from PeerConnection).
    VideoEncoderFactory *encoder_factory = nullptr;

    // Requests the WebRtcVideoChannel to perform a codec switch.
    EncoderSwitchRequestCallback *encoder_switch_request_callback = nullptr;

    // Ownership stays with WebrtcVideoEngine (delegated from PeerConnection).
    VideoBitrateAllocatorFactory *bitrate_allocator_factory = nullptr;

    // Negotiated capabilities which the VideoEncoder may expect the other
    // side to use.
    VideoEncoder::Capabilities capabilities;

    // Enables the frame instrumentation generator that is required for automatic
    // corruption detection.
    bool enable_frame_instrumentation_generator = false;
};

OCTK_END_NAMESPACE

#endif // _OCTK_VIDEO_STREAM_ENCODER_SETTINGS_HPP
