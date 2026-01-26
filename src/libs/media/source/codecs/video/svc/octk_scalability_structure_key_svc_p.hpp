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

#include <octk_dependency_descriptor.hpp>
#include <octk_video_bitrate_allocation.hpp>
#include <octk_generic_frame_info.hpp>
#include <private/octk_scalable_video_controller_p.hpp>

#include <bitset>
#include <vector>

OCTK_BEGIN_NAMESPACE

class ScalabilityStructureKeySvc : public ScalableVideoController
{
public:
    ScalabilityStructureKeySvc(int num_spatial_layers, int num_temporal_layers);
    ~ScalabilityStructureKeySvc() override;

    StreamLayersConfig StreamConfig() const override;

    std::vector<LayerFrameConfig> NextFrameConfig(bool restart) override;
    GenericFrameInfo OnEncodeDone(const LayerFrameConfig &config) override;
    void OnRatesUpdated(const VideoBitrateAllocation &bitrates) override;

private:
    enum FramePattern : int
    {
        kNone,
        kKey,
        kDeltaT0,
        kDeltaT2A,
        kDeltaT1,
        kDeltaT2B,
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
    bool TemporalLayerIsActive(int tid) const;
    static DecodeTargetIndication Dti(int sid, int tid, const LayerFrameConfig &config);

    std::vector<LayerFrameConfig> KeyframeConfig();
    std::vector<LayerFrameConfig> T0Config();
    std::vector<LayerFrameConfig> T1Config();
    std::vector<LayerFrameConfig> T2Config(FramePattern pattern);

    FramePattern NextPattern(FramePattern last_pattern) const;

    const int num_spatial_layers_;
    const int num_temporal_layers_;

    FramePattern last_pattern_ = kNone;
    std::bitset<kMaxNumSpatialLayers> spatial_id_is_enabled_;
    std::bitset<kMaxNumSpatialLayers> can_reference_t1_frame_for_spatial_id_;
    std::bitset<32> active_decode_targets_;
};

// S1  0--0--0-
//     |       ...
// S0  0--0--0-
class ScalabilityStructureL2T1Key : public ScalabilityStructureKeySvc
{
public:
    ScalabilityStructureL2T1Key()
        : ScalabilityStructureKeySvc(2, 1)
    {
    }
    ~ScalabilityStructureL2T1Key() override;

    FrameDependencyStructure DependencyStructure() const override;
};

// S1T1     0   0
//         /   /   /
// S1T0   0---0---0
//        |         ...
// S0T1   | 0   0
//        |/   /   /
// S0T0   0---0---0
// Time-> 0 1 2 3 4
class ScalabilityStructureL2T2Key : public ScalabilityStructureKeySvc
{
public:
    ScalabilityStructureL2T2Key()
        : ScalabilityStructureKeySvc(2, 2)
    {
    }
    ~ScalabilityStructureL2T2Key() override;

    FrameDependencyStructure DependencyStructure() const override;
};

class ScalabilityStructureL2T3Key : public ScalabilityStructureKeySvc
{
public:
    ScalabilityStructureL2T3Key()
        : ScalabilityStructureKeySvc(2, 3)
    {
    }
    ~ScalabilityStructureL2T3Key() override;

    FrameDependencyStructure DependencyStructure() const override;
};

class ScalabilityStructureL3T1Key : public ScalabilityStructureKeySvc
{
public:
    ScalabilityStructureL3T1Key()
        : ScalabilityStructureKeySvc(3, 1)
    {
    }
    ~ScalabilityStructureL3T1Key() override;

    FrameDependencyStructure DependencyStructure() const override;
};

class ScalabilityStructureL3T2Key : public ScalabilityStructureKeySvc
{
public:
    ScalabilityStructureL3T2Key()
        : ScalabilityStructureKeySvc(3, 2)
    {
    }
    ~ScalabilityStructureL3T2Key() override;

    FrameDependencyStructure DependencyStructure() const override;
};

class ScalabilityStructureL3T3Key : public ScalabilityStructureKeySvc
{
public:
    ScalabilityStructureL3T3Key()
        : ScalabilityStructureKeySvc(3, 3)
    {
    }
    ~ScalabilityStructureL3T3Key() override;

    FrameDependencyStructure DependencyStructure() const override;
};

OCTK_END_NAMESPACE