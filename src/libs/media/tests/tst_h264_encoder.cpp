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

#include <private/octk_video_encoder_openh264_p.hpp>
#include <octk_media_context_factory.hpp>
#include <octk_video_encoder.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

OCTK_BEGIN_NAMESPACE

namespace
{

const int kMaxPayloadSize = 1024;
const int kNumCores = 1;

const VideoEncoder::Capabilities kCapabilities(false);
const VideoEncoder::Settings kSettings(kCapabilities, kNumCores, kMaxPayloadSize);

void SetDefaultSettings(VideoCodec *codec_settings)
{
    codec_settings->codecType = kVideoCodecH264;
    codec_settings->maxFramerate = 60;
    codec_settings->width = 640;
    codec_settings->height = 480;
    // If frame dropping is false, we get a warning that bitrate can't
    // be controlled for RC_QUALITY_MODE; RC_BITRATE_MODE and RC_TIMESTAMP_MODE
    codec_settings->SetFrameDropEnabled(true);
    codec_settings->startBitrate = 2000;
    codec_settings->maxBitrate = 4000;
}

TEST(VideoEncoderOpenh264Test, CanInitializeWithDefaultParameters)
{
    VideoEncoderOpenh264 encoder(CreateMediaContext(), {});
    VideoCodec codec_settings;
    SetDefaultSettings(&codec_settings);
    EXPECT_EQ(WEBRTC_VIDEO_CODEC_OK, encoder.initEncode(&codec_settings, kSettings));
    EXPECT_EQ(H264PacketizationMode::NonInterleaved, encoder.packetizationMode());
}

TEST(VideoEncoderOpenh264Test, CanInitializeWithNonInterleavedModeExplicitly)
{
    VideoEncoderOpenh264 encoder(CreateMediaContext(), H264PacketizationMode::NonInterleaved);
    VideoCodec codec_settings;
    SetDefaultSettings(&codec_settings);
    EXPECT_EQ(WEBRTC_VIDEO_CODEC_OK, encoder.initEncode(&codec_settings, kSettings));
    EXPECT_EQ(H264PacketizationMode::NonInterleaved, encoder.packetizationMode());
}

TEST(VideoEncoderOpenh264Test, CanInitializeWithSingleNalUnitModeExplicitly)
{
    VideoEncoderOpenh264 encoder(CreateMediaContext(), H264PacketizationMode::SingleNalUnit);
    VideoCodec codec_settings;
    SetDefaultSettings(&codec_settings);
    EXPECT_EQ(WEBRTC_VIDEO_CODEC_OK, encoder.initEncode(&codec_settings, kSettings));
    EXPECT_EQ(H264PacketizationMode::SingleNalUnit, encoder.packetizationMode());
}

} // anonymous namespace

OCTK_END_NAMESPACE