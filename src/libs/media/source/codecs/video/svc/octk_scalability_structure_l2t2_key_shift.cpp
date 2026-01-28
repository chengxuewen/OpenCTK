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

#include <private/octk_scalability_structure_l2t2_key_shift_p.hpp>
#include <private/octk_scalable_video_controller_p.hpp>
#include <private/octk_dependency_descriptor_p.hpp>
#include <octk_generic_frame_info.hpp>
#include <octk_logging.hpp>
#include <octk_checks.hpp>

#include <utility>
#include <vector>

OCTK_BEGIN_NAMESPACE

namespace
{

DecodeTargetIndication Dti(int sid, int tid, const ScalableVideoController::LayerFrameConfig &config)
{
    if (config.IsKeyframe())
    {
        OCTK_DCHECK_EQ(config.TemporalId(), 0);
        return sid < config.SpatialId() ? DecodeTargetIndication::kNotPresent : DecodeTargetIndication::kSwitch;
    }

    if (sid != config.SpatialId() || tid < config.TemporalId())
    {
        return DecodeTargetIndication::kNotPresent;
    }
    if (tid == config.TemporalId() && tid > 0)
    {
        return DecodeTargetIndication::kDiscardable;
    }
    return DecodeTargetIndication::kSwitch;
}

} // namespace

constexpr int ScalabilityStructureL2T2KeyShift::kNumSpatialLayers;
constexpr int ScalabilityStructureL2T2KeyShift::kNumTemporalLayers;

ScalabilityStructureL2T2KeyShift::~ScalabilityStructureL2T2KeyShift() = default;

ScalableVideoController::StreamLayersConfig ScalabilityStructureL2T2KeyShift::StreamConfig() const
{
    StreamLayersConfig result;
    result.num_spatial_layers = 2;
    result.num_temporal_layers = 2;
    result.scaling_factor_num[0] = 1;
    result.scaling_factor_den[0] = 2;
    result.uses_reference_scaling = true;
    return result;
}

FrameDependencyStructure ScalabilityStructureL2T2KeyShift::DependencyStructure() const
{
    FrameDependencyStructure structure;
    structure.num_decode_targets = 4;
    structure.num_chains = 2;
    structure.decode_target_protected_by_chain = {0, 0, 1, 1};
    structure.templates.resize(7);
    auto &templates = structure.templates;
    templates[0].S(0).T(0).Dtis("SSSS").ChainDiffs({0, 0});
    templates[1].S(0).T(0).Dtis("SS--").ChainDiffs({2, 1}).FrameDiffs({2});
    templates[2].S(0).T(0).Dtis("SS--").ChainDiffs({4, 1}).FrameDiffs({4});
    templates[3].S(0).T(1).Dtis("-D--").ChainDiffs({2, 3}).FrameDiffs({2});
    templates[4].S(1).T(0).Dtis("--SS").ChainDiffs({1, 1}).FrameDiffs({1});
    templates[5].S(1).T(0).Dtis("--SS").ChainDiffs({3, 4}).FrameDiffs({4});
    templates[6].S(1).T(1).Dtis("---D").ChainDiffs({1, 2}).FrameDiffs({2});
    return structure;
}

std::vector<ScalableVideoController::LayerFrameConfig> ScalabilityStructureL2T2KeyShift::NextFrameConfig(bool restart)
{
    std::vector<LayerFrameConfig> configs;
    configs.reserve(2);
    if (restart)
    {
        next_pattern_ = kKey;
    }

    // Buffer0 keeps latest S0T0 frame,
    // Buffer1 keeps latest S1T0 frame.
    switch (next_pattern_)
    {
        case kKey:
            if (DecodeTargetIsActive(/*sid=*/0, /*tid=*/0))
            {
                configs.emplace_back();
                configs.back().S(0).T(0).Update(0).Keyframe();
            }
            if (DecodeTargetIsActive(/*sid=*/1, /*tid=*/0))
            {
                configs.emplace_back();
                configs.back().S(1).T(0).Update(1);
                if (DecodeTargetIsActive(/*sid=*/0, /*tid=*/0))
                {
                    configs.back().Reference(0);
                }
                else
                {
                    configs.back().Keyframe();
                }
            }
            next_pattern_ = kDelta0;
            break;
        case kDelta0:
            if (DecodeTargetIsActive(/*sid=*/0, /*tid=*/0))
            {
                configs.emplace_back();
                configs.back().S(0).T(0).ReferenceAndUpdate(0);
            }
            if (DecodeTargetIsActive(/*sid=*/1, /*tid=*/1))
            {
                configs.emplace_back();
                configs.back().S(1).T(1).Reference(1);
            }
            if (configs.empty() && DecodeTargetIsActive(/*sid=*/1, /*tid=*/0))
            {
                configs.emplace_back();
                configs.back().S(1).T(0).ReferenceAndUpdate(1);
            }
            next_pattern_ = kDelta1;
            break;
        case kDelta1:
            if (DecodeTargetIsActive(/*sid=*/0, /*tid=*/1))
            {
                configs.emplace_back();
                configs.back().S(0).T(1).Reference(0);
            }
            if (DecodeTargetIsActive(/*sid=*/1, /*tid=*/0))
            {
                configs.emplace_back();
                configs.back().S(1).T(0).ReferenceAndUpdate(1);
            }
            if (configs.empty() && DecodeTargetIsActive(/*sid=*/0, /*tid=*/0))
            {
                configs.emplace_back();
                configs.back().S(0).T(0).ReferenceAndUpdate(0);
            }
            next_pattern_ = kDelta0;
            break;
    }

    OCTK_DCHECK(!configs.empty() || active_decode_targets_.none());
    return configs;
}

GenericFrameInfo ScalabilityStructureL2T2KeyShift::OnEncodeDone(const LayerFrameConfig &config)
{
    GenericFrameInfo frame_info;
    frame_info.spatial_id = config.SpatialId();
    frame_info.temporal_id = config.TemporalId();
    frame_info.encoder_buffers = config.Buffers();
    for (int sid = 0; sid < kNumSpatialLayers; ++sid)
    {
        for (int tid = 0; tid < kNumTemporalLayers; ++tid)
        {
            frame_info.decode_target_indications.push_back(Dti(sid, tid, config));
        }
    }
    if (config.IsKeyframe())
    {
        frame_info.part_of_chain = {true, true};
    }
    else if (config.TemporalId() == 0)
    {
        frame_info.part_of_chain = {config.SpatialId() == 0, config.SpatialId() == 1};
    }
    else
    {
        frame_info.part_of_chain = {false, false};
    }
    return frame_info;
}

void ScalabilityStructureL2T2KeyShift::OnRatesUpdated(const VideoBitrateAllocation &bitrates)
{
    for (int sid = 0; sid < kNumSpatialLayers; ++sid)
    {
        // Enable/disable spatial layers independetely.
        bool active = bitrates.GetBitrate(sid, /*tid=*/0) > 0;
        if (!DecodeTargetIsActive(sid, /*tid=*/0) && active)
        {
            // Key frame is required to reenable any spatial layer.
            next_pattern_ = kKey;
        }

        SetDecodeTargetIsActive(sid, /*tid=*/0, active);
        SetDecodeTargetIsActive(sid, /*tid=*/1, active && bitrates.GetBitrate(sid, /*tid=*/1) > 0);
    }
}

OCTK_END_NAMESPACE
