/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
** Copyright (c) 2024 The WebRTC project authors. All Rights Reserved.
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

#include <octk_video_codec.hpp>
#include <octk_encoded_image.hpp>
#include <octk_simulcast_stream.hpp>
#include <octk_codec_specific_info.hpp>
#include <octk_video_bitrate_allocation.hpp>
#include <octk_generic_frame_info.hpp>
#include <private/octk_scalable_video_controller_p.hpp>

#include <stddef.h>

#include <memory>
#include <vector>

OCTK_BEGIN_NAMESPACE

class OCTK_MEDIA_API SimulcastToSvcConverter
{
public:
    explicit SimulcastToSvcConverter(const VideoCodec &);
    SimulcastToSvcConverter(SimulcastToSvcConverter &&) = default;

    SimulcastToSvcConverter(const SimulcastToSvcConverter &) = delete;
    SimulcastToSvcConverter &operator=(const SimulcastToSvcConverter &) = delete;
    SimulcastToSvcConverter &operator=(SimulcastToSvcConverter &&) = default;

    ~SimulcastToSvcConverter() = default;

    static bool IsConfigSupported(const VideoCodec &codec);

    VideoCodec GetConfig() const;

    void EncodeStarted(bool force_keyframe);

    bool ConvertFrame(EncodedImage &encoded_image, CodecSpecificInfo &codec_specific);

private:
    struct LayerState
    {
        LayerState(ScalabilityMode scalability_mode, int num_temporal_layers);
        ~LayerState() = default;
        LayerState(const LayerState &) = delete;
        LayerState(LayerState &&) = default;

        std::unique_ptr<ScalableVideoController> video_controller;
        ScalableVideoController::LayerFrameConfig layer_config;
        bool awaiting_frame;
    };

    VideoCodec config_;

    std::vector<LayerState> layers_;
};

OCTK_END_NAMESPACE