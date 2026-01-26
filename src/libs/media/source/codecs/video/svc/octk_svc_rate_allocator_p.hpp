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

#include <octk_inlined_vector.hpp>
#include <octk_field_trials_view.hpp>
#include <octk_data_rate.hpp>
#include <octk_video_bitrate_allocator.hpp>
#include <octk_video_bitrate_allocation.hpp>
#include <octk_video_codec_constants.hpp>
#include <octk_video_codec.hpp>
#include <private/octk_stable_target_rate_experiment_p.hpp>

#include <stddef.h>

#include <vector>

OCTK_BEGIN_NAMESPACE

class SvcRateAllocator : public VideoBitrateAllocator
{
public:
    SvcRateAllocator(const VideoCodec &codec, const FieldTrialsView &field_trials);

    VideoBitrateAllocation Allocate(VideoBitrateAllocationParameters parameters) override;

    static DataRate GetMaxBitrate(const VideoCodec &codec);
    static DataRate GetPaddingBitrate(const VideoCodec &codec);
    static InlinedVector<DataRate, kMaxSpatialLayers> GetLayerStartBitrates(const VideoCodec &codec);

private:
    struct NumLayers
    {
        size_t spatial = 1;
        size_t temporal = 1;
    };

    static NumLayers GetNumLayers(const VideoCodec &codec);
    std::vector<DataRate> DistributeAllocationToSpatialLayersNormalVideo(DataRate total_bitrate,
                                                                         size_t first_active_layer,
                                                                         size_t num_spatial_layers) const;

    std::vector<DataRate> DistributeAllocationToSpatialLayersScreenSharing(DataRate total_bitrate,
                                                                           size_t first_active_layer,
                                                                           size_t num_spatial_layers) const;

    // Returns the number of layers that are active and have enough bitrate to
    // actually be enabled.
    size_t FindNumEnabledLayers(DataRate target_rate) const;

    const VideoCodec codec_;
    const NumLayers num_layers_;
    const StableTargetRateExperiment experiment_settings_;
    const InlinedVector<DataRate, kMaxSpatialLayers> cumulative_layer_start_bitrates_;
    size_t last_active_layer_count_;
};

OCTK_END_NAMESPACE