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

//#include <octk_dependency_descriptor.hpp>
#include <octk_generic_frame_info.hpp>
#include <private/octk_scalability_structure_p.hpp>
#include <private/octk_scalable_video_controller_p.hpp>

#include <bitset>
#include <vector>

OCTK_BEGIN_NAMESPACE

class ScalabilityStructureFullSvc : public ScalableVideoController
{
public:
    struct ScalingFactor
    {
        int num = 1;
        int den = 2;
    };
    ScalabilityStructureFullSvc(int num_spatial_layers, int num_temporal_layers, ScalingFactor resolution_factor);
    ~ScalabilityStructureFullSvc() override;

    StreamLayersConfig StreamConfig() const override;

    std::vector<LayerFrameConfig> NextFrameConfig(bool restart) override;
    GenericFrameInfo OnEncodeDone(const LayerFrameConfig &config) override;
    void OnRatesUpdated(const VideoBitrateAllocation &bitrates) override;

private:
    enum FramePattern
    {
        kNone,
        kKey,
        kDeltaT2A,
        kDeltaT1,
        kDeltaT2B,
        kDeltaT0,
    };
    static constexpr StringView kFramePatternNames[] = {"None", "Key", "DeltaT2A", "DeltaT1", "DeltaT2B", "DeltaT0"};
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
    static DecodeTargetIndication Dti(int sid, int tid, const LayerFrameConfig &frame);

    const int num_spatial_layers_;
    const int num_temporal_layers_;
    const ScalingFactor resolution_factor_;

    FramePattern last_pattern_ = kNone;
    std::bitset<kMaxNumSpatialLayers> can_reference_t0_frame_for_spatial_id_ = 0;
    std::bitset<kMaxNumSpatialLayers> can_reference_t1_frame_for_spatial_id_ = 0;
    std::bitset<32> active_decode_targets_;
};

// T1       0   0
//         /   /   / ...
// T0     0---0---0--
// Time-> 0 1 2 3 4
class ScalabilityStructureL1T2 : public ScalabilityStructureFullSvc
{
public:
    explicit ScalabilityStructureL1T2(ScalingFactor resolution_factor = {})
        : ScalabilityStructureFullSvc(1, 2, resolution_factor)
    {
    }
    ~ScalabilityStructureL1T2() override = default;

    FrameDependencyStructure DependencyStructure() const override;
};

// T2       0   0   0   0
//          |  /    |  /
// T1       / 0     / 0  ...
//         |_/     |_/
// T0     0-------0------
// Time-> 0 1 2 3 4 5 6 7
class ScalabilityStructureL1T3 : public ScalabilityStructureFullSvc
{
public:
    explicit ScalabilityStructureL1T3(ScalingFactor resolution_factor = {})
        : ScalabilityStructureFullSvc(1, 3, resolution_factor)
    {
    }
    ~ScalabilityStructureL1T3() override = default;

    FrameDependencyStructure DependencyStructure() const override;
};

// S1  0--0--0-
//     |  |  | ...
// S0  0--0--0-
class ScalabilityStructureL2T1 : public ScalabilityStructureFullSvc
{
public:
    explicit ScalabilityStructureL2T1(ScalingFactor resolution_factor = {})
        : ScalabilityStructureFullSvc(2, 1, resolution_factor)
    {
    }
    ~ScalabilityStructureL2T1() override = default;

    FrameDependencyStructure DependencyStructure() const override;
};

// S1T1     0   0
//         /|  /|  /
// S1T0   0-+-0-+-0
//        | | | | | ...
// S0T1   | 0 | 0 |
//        |/  |/  |/
// S0T0   0---0---0--
// Time-> 0 1 2 3 4
class ScalabilityStructureL2T2 : public ScalabilityStructureFullSvc
{
public:
    explicit ScalabilityStructureL2T2(ScalingFactor resolution_factor = {})
        : ScalabilityStructureFullSvc(2, 2, resolution_factor)
    {
    }
    ~ScalabilityStructureL2T2() override = default;

    FrameDependencyStructure DependencyStructure() const override;
};

// S1T2      4    ,8
// S1T1    / |  6' |
// S1T0   2--+-'+--+-...
//        |  |  |  |
// S0T2   |  3  | ,7
// S0T1   | /   5'
// S0T0   1----'-----...
// Time-> 0  1  2  3
class ScalabilityStructureL2T3 : public ScalabilityStructureFullSvc
{
public:
    explicit ScalabilityStructureL2T3(ScalingFactor resolution_factor = {})
        : ScalabilityStructureFullSvc(2, 3, resolution_factor)
    {
    }
    ~ScalabilityStructureL2T3() override = default;

    FrameDependencyStructure DependencyStructure() const override;
};

// S2     0-0-0-
//        | | |
// S1     0-0-0-...
//        | | |
// S0     0-0-0-
// Time-> 0 1 2
class ScalabilityStructureL3T1 : public ScalabilityStructureFullSvc
{
public:
    explicit ScalabilityStructureL3T1(ScalingFactor resolution_factor = {})
        : ScalabilityStructureFullSvc(3, 1, resolution_factor)
    {
    }
    ~ScalabilityStructureL3T1() override = default;

    FrameDependencyStructure DependencyStructure() const override;
};

// https://www.w3.org/TR/webrtc-svc/#L3T2*
class ScalabilityStructureL3T2 : public ScalabilityStructureFullSvc
{
public:
    explicit ScalabilityStructureL3T2(ScalingFactor resolution_factor = {})
        : ScalabilityStructureFullSvc(3, 2, resolution_factor)
    {
    }
    ~ScalabilityStructureL3T2() override = default;

    FrameDependencyStructure DependencyStructure() const override;
};

// https://www.w3.org/TR/webrtc-svc/#L3T3*
class ScalabilityStructureL3T3 : public ScalabilityStructureFullSvc
{
public:
    explicit ScalabilityStructureL3T3(ScalingFactor resolution_factor = {})
        : ScalabilityStructureFullSvc(3, 3, resolution_factor)
    {
    }
    ~ScalabilityStructureL3T3() override = default;

    FrameDependencyStructure DependencyStructure() const override;
};

OCTK_END_NAMESPACE