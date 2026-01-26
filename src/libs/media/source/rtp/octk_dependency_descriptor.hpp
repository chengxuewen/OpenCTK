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

#include <octk_render_resolution.hpp>
#include <octk_string_view.hpp>
#include <octk_optional.hpp>
#include <octk_types.hpp>

#include <initializer_list>
#include <memory>
#include <vector>

OCTK_BEGIN_NAMESPACE
// Structures to build and parse dependency descriptor as described in
// https://aomediacodec.github.io/av1-rtp-spec/#dependency-descriptor-rtp-header-extension

// Relationship of a frame to a Decode target.
enum class DecodeTargetIndication
{
    kNotPresent = 0,  // DecodeTargetInfo symbol '-'
    kDiscardable = 1, // DecodeTargetInfo symbol 'D'
    kSwitch = 2,      // DecodeTargetInfo symbol 'S'
    kRequired = 3     // DecodeTargetInfo symbol 'R'
};

struct FrameDependencyTemplate
{
    // Setters are named briefly to chain them when building the template.
    FrameDependencyTemplate &S(int spatial_layer);
    FrameDependencyTemplate &T(int temporal_layer);
    FrameDependencyTemplate &Dtis(StringView dtis);
    FrameDependencyTemplate &FrameDiffs(::std::initializer_list<int> diffs);
    FrameDependencyTemplate &ChainDiffs(::std::initializer_list<int> diffs);

    friend bool operator==(const FrameDependencyTemplate &lhs, const FrameDependencyTemplate &rhs)
    {
        return lhs.spatial_id == rhs.spatial_id && lhs.temporal_id == rhs.temporal_id &&
               lhs.decode_target_indications == rhs.decode_target_indications && lhs.frame_diffs == rhs.frame_diffs &&
               lhs.chain_diffs == rhs.chain_diffs;
    }

    int spatial_id = 0;
    int temporal_id = 0;
    //    absl::InlinedVector<DecodeTargetIndication, 10> decode_target_indications;
    //    absl::InlinedVector<int, 4> frame_diffs;
    //    absl::InlinedVector<int, 4> chain_diffs;
    std::vector<DecodeTargetIndication> decode_target_indications;
    std::vector<int> frame_diffs;
    std::vector<int> chain_diffs;
};

struct FrameDependencyStructure
{
    friend bool operator==(const FrameDependencyStructure &lhs, const FrameDependencyStructure &rhs)
    {
        return lhs.num_decode_targets == rhs.num_decode_targets && lhs.num_chains == rhs.num_chains &&
               lhs.decode_target_protected_by_chain == rhs.decode_target_protected_by_chain &&
               lhs.resolutions == rhs.resolutions && lhs.templates == rhs.templates;
    }

    int structure_id = 0;
    int num_decode_targets = 0;
    int num_chains = 0;
    // If chains are used (num_chains > 0), maps decode target index into index of
    // the chain protecting that target.
    //    absl::InlinedVector<int, 10> decode_target_protected_by_chain;
    //    absl::InlinedVector<RenderResolution, 4> resolutions;
    std::vector<int> decode_target_protected_by_chain;
    std::vector<RenderResolution> resolutions;
    std::vector<FrameDependencyTemplate> templates;
};

class DependencyDescriptorMandatory
{
public:
    void set_frame_number(int frame_number) { frame_number_ = frame_number; }
    int frame_number() const { return frame_number_; }

    void set_template_id(int template_id) { template_id_ = template_id; }
    int template_id() const { return template_id_; }

    void set_first_packet_in_frame(bool first) { first_packet_in_frame_ = first; }
    bool first_packet_in_frame() const { return first_packet_in_frame_; }

    void set_last_packet_in_frame(bool last) { last_packet_in_frame_ = last; }
    bool last_packet_in_frame() const { return last_packet_in_frame_; }

private:
    int frame_number_;
    int template_id_;
    bool first_packet_in_frame_;
    bool last_packet_in_frame_;
};

struct DependencyDescriptor
{
    static constexpr int kMaxSpatialIds = 4;
    static constexpr int kMaxTemporalIds = 8;
    static constexpr int kMaxDecodeTargets = 32;
    static constexpr int kMaxTemplates = 64;

    bool first_packet_in_frame = true;
    bool last_packet_in_frame = true;
    int frame_number = 0;
    FrameDependencyTemplate frame_dependencies;
    Optional<RenderResolution> resolution;
    Optional<uint32_t> active_decode_targets_bitmask;
    std::unique_ptr<FrameDependencyStructure> attached_structure;
};

// Below are implementation details.
namespace detail
{
std::vector<DecodeTargetIndication> StringToDecodeTargetIndications(StringView indication_symbols);
} // namespace detail

inline FrameDependencyTemplate &FrameDependencyTemplate::S(int spatial_layer)
{
    this->spatial_id = spatial_layer;
    return *this;
}
inline FrameDependencyTemplate &FrameDependencyTemplate::T(int temporal_layer)
{
    this->temporal_id = temporal_layer;
    return *this;
}
inline FrameDependencyTemplate &FrameDependencyTemplate::Dtis(StringView dtis)
{
    this->decode_target_indications = detail::StringToDecodeTargetIndications(dtis);
    return *this;
}
inline FrameDependencyTemplate &FrameDependencyTemplate::FrameDiffs(std::initializer_list<int> diffs)
{
    this->frame_diffs.assign(diffs.begin(), diffs.end());
    return *this;
}
inline FrameDependencyTemplate &FrameDependencyTemplate::ChainDiffs(std::initializer_list<int> diffs)
{
    this->chain_diffs.assign(diffs.begin(), diffs.end());
    return *this;
}

OCTK_END_NAMESPACE