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

#include <private/octk_scalable_video_controller_no_layering_p.hpp>

#include <utility>
#include <vector>

#include <octk_dependency_descriptor.hpp>
#include <octk_checks.hpp>

OCTK_BEGIN_NAMESPACE

ScalableVideoControllerNoLayering::~ScalableVideoControllerNoLayering() = default;

ScalableVideoController::StreamLayersConfig ScalableVideoControllerNoLayering::StreamConfig() const
{
    StreamLayersConfig result;
    result.num_spatial_layers = 1;
    result.num_temporal_layers = 1;
    result.uses_reference_scaling = false;
    return result;
}

FrameDependencyStructure ScalableVideoControllerNoLayering::DependencyStructure() const
{
    FrameDependencyStructure structure;
    structure.num_decode_targets = 1;
    structure.num_chains = 1;
    structure.decode_target_protected_by_chain = {0};

    FrameDependencyTemplate key_frame;
    key_frame.decode_target_indications = {DecodeTargetIndication::kSwitch};
    key_frame.chain_diffs = {0};
    structure.templates.push_back(key_frame);

    FrameDependencyTemplate delta_frame;
    delta_frame.decode_target_indications = {DecodeTargetIndication::kSwitch};
    delta_frame.chain_diffs = {1};
    delta_frame.frame_diffs = {1};
    structure.templates.push_back(delta_frame);

    return structure;
}

std::vector<ScalableVideoController::LayerFrameConfig> ScalableVideoControllerNoLayering::NextFrameConfig(bool restart)
{
    if (!enabled_)
    {
        return {};
    }
    std::vector<LayerFrameConfig> result(1);
    if (restart || start_)
    {
        result[0].Id(0).Keyframe().Update(0);
    }
    else
    {
        result[0].Id(0).ReferenceAndUpdate(0);
    }
    start_ = false;
    return result;
}

GenericFrameInfo ScalableVideoControllerNoLayering::OnEncodeDone(const LayerFrameConfig &config)
{
    OCTK_DCHECK_EQ(config.Id(), 0);
    GenericFrameInfo frame_info;
    frame_info.encoder_buffers = config.Buffers();
    if (config.IsKeyframe())
    {
        for (auto &buffer : frame_info.encoder_buffers)
        {
            buffer.referenced = false;
        }
    }
    frame_info.decode_target_indications = {DecodeTargetIndication::kSwitch};
    frame_info.part_of_chain = {true};
    return frame_info;
}

void ScalableVideoControllerNoLayering::OnRatesUpdated(const VideoBitrateAllocation &bitrates)
{
    enabled_ = bitrates.GetBitrate(0, 0) > 0;
}

OCTK_END_NAMESPACE
