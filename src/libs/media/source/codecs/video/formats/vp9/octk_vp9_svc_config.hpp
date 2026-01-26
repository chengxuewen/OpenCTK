/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
** Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
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

#include <private/octk_scalable_video_controller_p.hpp>
#include <octk_video_codec.hpp>
#include <octk_optional.hpp>

#include <stddef.h>

#include <vector>


// #include "api/video_codecs/spatial_layer.h"
// #include "api/video_codecs/video_codec.h"
// #include "modules/video_coding/svc/scalable_video_controller.h"

OCTK_BEGIN_NAMESPACE

// Uses scalability mode to configure spatial layers.
std::vector<SpatialLayer> GetVp9SvcConfig(VideoCodec &video_codec);

std::vector<SpatialLayer> GetSvcConfig(size_t input_width,
                                       size_t input_height,
                                       float max_framerate_fps,
                                       size_t first_active_layer,
                                       size_t num_spatial_layers,
                                       size_t num_temporal_layers,
                                       bool is_screen_sharing,
                                       Optional<ScalableVideoController::StreamLayersConfig> config = utils::nullopt);

OCTK_END_NAMESPACE