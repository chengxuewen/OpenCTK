/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
** Copyright (c) 2020 The WebRTC project authors. All Rights Reserved.
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

#include <octk_dependency_descriptor.hpp>
#include <octk_video_bitrate_allocation.hpp>
#include <octk_generic_frame_info.hpp>
#include <private/octk_scalable_video_controller_p.hpp>

#include <vector>

OCTK_BEGIN_NAMESPACE

// S1T1     0   0
//         /   /   /
// S1T0   0---0---0
//        |        ...
// S0T1   |   0   0
//        |  /   /
// S0T0   0-0---0--
// Time-> 0 1 2 3 4
class ScalabilityStructureL2T2KeyShift : public ScalableVideoController
{
public:
    ~ScalabilityStructureL2T2KeyShift() override;

    StreamLayersConfig StreamConfig() const override;
    FrameDependencyStructure DependencyStructure() const override;

    std::vector<LayerFrameConfig> NextFrameConfig(bool restart) override;
    GenericFrameInfo OnEncodeDone(const LayerFrameConfig &config) override;
    void OnRatesUpdated(const VideoBitrateAllocation &bitrates) override;

private:
    enum FramePattern
    {
        kKey,
        kDelta0,
        kDelta1,
    };

    static constexpr int kNumSpatialLayers = 2;
    static constexpr int kNumTemporalLayers = 2;

    bool DecodeTargetIsActive(int sid, int tid) const { return active_decode_targets_[sid * kNumTemporalLayers + tid]; }
    void SetDecodeTargetIsActive(int sid, int tid, bool value)
    {
        active_decode_targets_.set(sid * kNumTemporalLayers + tid, value);
    }

    FramePattern next_pattern_ = kKey;
    std::bitset<32> active_decode_targets_ = 0b1111;
};

OCTK_END_NAMESPACE