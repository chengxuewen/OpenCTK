/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
** Copyright (c) 2021 The WebRTC project authors. All Rights Reserved.
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

// Scalability structure with multiple independent spatial layers each with the
// same temporal layering.
class ScalabilityStructureSimulcast : public ScalableVideoController
{
public:
    struct ScalingFactor
    {
        int num = 1;
        int den = 2;
    };
    ScalabilityStructureSimulcast(int num_spatial_layers, int num_temporal_layers, ScalingFactor resolution_factor);
    ~ScalabilityStructureSimulcast() override;

    StreamLayersConfig StreamConfig() const override;
    std::vector<LayerFrameConfig> NextFrameConfig(bool restart) override;
    GenericFrameInfo OnEncodeDone(const LayerFrameConfig &config) override;
    void OnRatesUpdated(const VideoBitrateAllocation &bitrates) override;

private:
    enum FramePattern
    {
        kNone,
        kDeltaT2A,
        kDeltaT1,
        kDeltaT2B,
        kDeltaT0,
    };
    static constexpr int kMaxNumSpatialLayers = 3;
    static constexpr int kMaxNumTemporalLayers = 3;

    // Index of the buffer to store last frame for layer (`sid`, `tid`)
    int BufferIndex(int sid, int tid) const { return tid * num_spatial_layers_ + sid; }
    bool DecodeTargetIsActive(int sid, int tid) const
    {
        return active_decode_targets_[sid * num_temporal_layers_ + tid];
    }
    void SetDecodeTargetIsActive(int sid, int tid, bool value)
    {
        active_decode_targets_.set(sid * num_temporal_layers_ + tid, value);
    }
    FramePattern NextPattern() const;
    bool TemporalLayerIsActive(int tid) const;

    const int num_spatial_layers_;
    const int num_temporal_layers_;
    const ScalingFactor resolution_factor_;

    FramePattern last_pattern_ = kNone;
    std::bitset<kMaxNumSpatialLayers> can_reference_t0_frame_for_spatial_id_ = 0;
    std::bitset<kMaxNumSpatialLayers> can_reference_t1_frame_for_spatial_id_ = 0;
    std::bitset<32> active_decode_targets_;
};

// S1  0--0--0-
//             ...
// S0  0--0--0-
class ScalabilityStructureS2T1 : public ScalabilityStructureSimulcast
{
public:
    explicit ScalabilityStructureS2T1(ScalingFactor resolution_factor = {})
        : ScalabilityStructureSimulcast(2, 1, resolution_factor)
    {
    }
    ~ScalabilityStructureS2T1() override = default;

    FrameDependencyStructure DependencyStructure() const override;
};

class ScalabilityStructureS2T2 : public ScalabilityStructureSimulcast
{
public:
    explicit ScalabilityStructureS2T2(ScalingFactor resolution_factor = {})
        : ScalabilityStructureSimulcast(2, 2, resolution_factor)
    {
    }
    ~ScalabilityStructureS2T2() override = default;

    FrameDependencyStructure DependencyStructure() const override;
};

// S1T2       3   7
//            |  /
// S1T1       / 5
//           |_/
// S1T0     1-------9...
//
// S0T2       2   6
//            |  /
// S0T1       / 4
//           |_/
// S0T0     0-------8...
// Time->   0 1 2 3 4
class ScalabilityStructureS2T3 : public ScalabilityStructureSimulcast
{
public:
    explicit ScalabilityStructureS2T3(ScalingFactor resolution_factor = {})
        : ScalabilityStructureSimulcast(2, 3, resolution_factor)
    {
    }
    ~ScalabilityStructureS2T3() override = default;

    FrameDependencyStructure DependencyStructure() const override;
};

class ScalabilityStructureS3T1 : public ScalabilityStructureSimulcast
{
public:
    explicit ScalabilityStructureS3T1(ScalingFactor resolution_factor = {})
        : ScalabilityStructureSimulcast(3, 1, resolution_factor)
    {
    }
    ~ScalabilityStructureS3T1() override = default;

    FrameDependencyStructure DependencyStructure() const override;
};

class ScalabilityStructureS3T2 : public ScalabilityStructureSimulcast
{
public:
    explicit ScalabilityStructureS3T2(ScalingFactor resolution_factor = {})
        : ScalabilityStructureSimulcast(3, 2, resolution_factor)
    {
    }
    ~ScalabilityStructureS3T2() override = default;

    FrameDependencyStructure DependencyStructure() const override;
};

class ScalabilityStructureS3T3 : public ScalabilityStructureSimulcast
{
public:
    explicit ScalabilityStructureS3T3(ScalingFactor resolution_factor = {})
        : ScalabilityStructureSimulcast(3, 3, resolution_factor)
    {
    }
    ~ScalabilityStructureS3T3() override = default;

    FrameDependencyStructure DependencyStructure() const override;
};

OCTK_END_NAMESPACE