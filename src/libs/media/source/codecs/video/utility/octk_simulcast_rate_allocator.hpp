/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2019~Present ChengXueWen.
** Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
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

#include <private/octk_stable_target_rate_experiment_p.hpp>
#include <private/octk_rate_control_settings_p.hpp>
#include <octk_media_context.hpp>
#include <octk_video_bitrate_allocator.hpp>
#include <octk_video_codec.hpp>

#include <stddef.h>
#include <stdint.h>

#include <vector>

OCTK_BEGIN_NAMESPACE

class SimulcastRateAllocator : public VideoBitrateAllocator
{
public:
    SimulcastRateAllocator(const MediaContext &mediaContext, const VideoCodec &codec);
    ~SimulcastRateAllocator() override;

    SimulcastRateAllocator(const SimulcastRateAllocator &) = delete;
    SimulcastRateAllocator &operator=(const SimulcastRateAllocator &) = delete;

    VideoBitrateAllocation Allocate(VideoBitrateAllocationParameters parameters) override;
    const VideoCodec &GetCodec() const;

    static float GetTemporalRateAllocation(int num_layers, int temporal_id, bool base_heavy_tl3_alloc);

    void SetLegacyConferenceMode(bool mode) override;

private:
    void DistributeAllocationToSimulcastLayers(DataRate total_bitrate,
                                               DataRate stable_bitrate,
                                               VideoBitrateAllocation *allocated_bitrates);
    void DistributeAllocationToTemporalLayers(VideoBitrateAllocation *allocated_bitrates) const;
    std::vector<uint32_t> DefaultTemporalLayerAllocation(int bitrate_kbps,
                                                         int max_bitrate_kbps,
                                                         int simulcast_id) const;
    std::vector<uint32_t> ScreenshareTemporalLayerAllocation(int bitrate_kbps,
                                                             int max_bitrate_kbps,
                                                             int simulcast_id) const;
    int NumTemporalStreams(size_t simulcast_id) const;

    const VideoCodec codec_;
    const StableTargetRateExperiment stable_rate_settings_;
    const RateControlSettings rate_control_settings_;
    std::vector<bool> stream_enabled_;
    bool legacy_conference_mode_;
};

OCTK_END_NAMESPACE