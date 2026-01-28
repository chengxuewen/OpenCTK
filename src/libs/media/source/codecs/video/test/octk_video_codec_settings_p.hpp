/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
** Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
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

#include <octk_video_encoder.hpp>

OCTK_BEGIN_NAMESPACE

namespace test
{

const uint16_t kTestWidth = 352;
const uint16_t kTestHeight = 288;
const uint32_t kTestFrameRate = 30;
const unsigned int kTestMinBitrateKbps = 30;
const unsigned int kTestStartBitrateKbps = 300;
const uint8_t kTestPayloadType = 100;
const int64_t kTestTimingFramesDelayMs = 200;
const uint16_t kTestOutlierFrameSizePercent = 250;

static void CodecSettings(VideoCodecType codec_type, VideoCodec *settings)
{
    *settings = {};

    settings->width = kTestWidth;
    settings->height = kTestHeight;

    settings->startBitrate = kTestStartBitrateKbps;
    settings->maxBitrate = 0;
    settings->minBitrate = kTestMinBitrateKbps;

    settings->maxFramerate = kTestFrameRate;

    settings->active = true;

    settings->qpMax = 56; // See webrtcvideoengine.h.
    settings->numberOfSimulcastStreams = 0;

    settings->timing_frame_thresholds = {
        kTestTimingFramesDelayMs,
        kTestOutlierFrameSizePercent,
    };

    settings->codecType = codec_type;
    switch (codec_type)
    {
        case kVideoCodecVP8: *(settings->VP8()) = VideoEncoder::GetDefaultVp8Settings(); return;
        case kVideoCodecVP9: *(settings->VP9()) = VideoEncoder::GetDefaultVp9Settings(); return;
        case kVideoCodecH264:
            // TODO(brandtr): Set `qpMax` here, when the OpenH264 wrapper supports it.
            *(settings->H264()) = VideoEncoder::getDefaultH264Settings();
            return;
        default: return;
    }
}
} // namespace test

OCTK_END_NAMESPACE