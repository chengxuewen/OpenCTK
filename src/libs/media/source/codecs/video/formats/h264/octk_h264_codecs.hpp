/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2026~Present ChengXueWen.
** Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
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

#include <octk_nullability.hpp>
#include <octk_media_context.hpp>
#include <private/octk_h264_common_p.hpp>
#include <octk_h264_profile.hpp>
#include <octk_scalability_mode.hpp>
#include <octk_sdp_video_format.hpp>
#include <octk_video_encoder.hpp>
#include <octk_video_decoder.hpp>

#include <memory>
#include <string>
#include <vector>

// #include "absl/base/nullability.h"
// #include "api/environment/environment.h"
// #include "api/video_codecs/h264_profile_level_id.h"
// #include "api/video_codecs/scalability_mode.h"
// #include "api/video_codecs/sdp_video_format.h"
// #include "api/video_codecs/video_decoder.h"
// #include "api/video_codecs/video_encoder.h"
// #include "modules/video_coding/codecs/h264/include/h264_globals.h"
// #include "rtc_base/system/rtc_export.h"

OCTK_BEGIN_NAMESPACE

// Creates an H264 SdpVideoFormat entry with specified paramters.
OCTK_MEDIA_API SdpVideoFormat CreateH264Format(H264Profile profile,
                                               H264Level level,
                                               const std::string &packetization_mode,
                                               bool add_scalability_modes = false);

// Set to disable the H.264 encoder/decoder implementations that are provided if
// `rtc_use_h264` build flag is true (if false, this function does nothing).
// This function should only be called before or during WebRTC initialization
// and is not thread-safe.
OCTK_MEDIA_API void DisableRtcUseH264();

// Returns a vector with all supported internal H264 encode profiles that we can
// negotiate in SDP, in order of preference.
std::vector<SdpVideoFormat> SupportedH264Codecs(bool add_scalability_modes = false);

// Returns a vector with all supported internal H264 decode profiles that we can
// negotiate in SDP, in order of preference. This will be available for receive
// only connections.
std::vector<SdpVideoFormat> SupportedH264DecoderCodecs();

class OCTK_MEDIA_API H264Encoder
{
public:
    // If H.264 is supported (any implementation).
    static bool IsSupported();
    static bool SupportsScalabilityMode(ScalabilityMode scalability_mode);
};

struct H264EncoderSettings
{
    // Use factory function rather than constructor to allow to create
    // `H264EncoderSettings` with designated initializers.
    static H264EncoderSettings Parse(const SdpVideoFormat &format);

    H264PacketizationMode packetization_mode = H264PacketizationMode::NonInterleaved;
};
Nonnull<std::unique_ptr<VideoEncoder>> CreateH264Encoder(const MediaContext &env, H264EncoderSettings settings = {});

class OCTK_MEDIA_API H264Decoder : public VideoDecoder
{
public:
    static std::unique_ptr<H264Decoder> Create();
    static bool IsSupported();

    ~H264Decoder() override { }
};

OCTK_END_NAMESPACE