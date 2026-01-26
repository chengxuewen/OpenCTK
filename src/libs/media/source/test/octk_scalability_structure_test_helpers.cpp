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

#if 0
#include <private/octk_scalability_structure_test_helpers_p.hpp>

#include <stdint.h>

#include <utility>
#include <vector>

#include <octk_array_view.hpp>
#include <octk_dependency_descriptor.hpp>
#include <octk_video_bitrate_allocation.hpp>
#include <private/octk_scalable_video_controller_p.hpp>

#include "test/gtest.h"

OCTK_BEGIN_NAMESPACE

VideoBitrateAllocation EnableTemporalLayers(int s0, int s1, int s2)
{
    VideoBitrateAllocation bitrate;
    for (int tid = 0; tid < s0; ++tid)
    {
        bitrate.SetBitrate(0, tid, 1'000'000);
    }
    for (int tid = 0; tid < s1; ++tid)
    {
        bitrate.SetBitrate(1, tid, 1'000'000);
    }
    for (int tid = 0; tid < s2; ++tid)
    {
        bitrate.SetBitrate(2, tid, 1'000'000);
    }
    return bitrate;
}

void ScalabilityStructureWrapper::GenerateFrames(int num_temporal_units, std::vector<GenericFrameInfo> &frames)
{
    for (int i = 0; i < num_temporal_units; ++i)
    {
        for (auto &layer_frame : structure_controller_.NextFrameConfig(/*restart=*/false))
        {
            int64_t frame_id = ++frame_id_;
            bool is_keyframe = layer_frame.IsKeyframe();

            GenericFrameInfo frame_info = structure_controller_.OnEncodeDone(layer_frame);
            if (is_keyframe)
            {
                chain_diff_calculator_.Reset(frame_info.part_of_chain);
            }
            frame_info.chain_diffs = chain_diff_calculator_.From(frame_id, frame_info.part_of_chain);
            for (int64_t base_frame_id : frame_deps_calculator_.FromBuffersUsage(frame_id, frame_info.encoder_buffers))
            {
                frame_info.frame_diffs.push_back(frame_id - base_frame_id);
            }

            frames.push_back(std::move(frame_info));
        }
    }
}

bool ScalabilityStructureWrapper::FrameReferencesAreValid(rtc::ArrayView<const GenericFrameInfo> frames) const
{
    bool valid = true;
    // VP9 and AV1 supports up to 8 buffers. Expect no more buffers are not used.
    std::bitset<8> buffer_contains_frame;
    for (size_t i = 0; i < frames.size(); ++i)
    {
        const GenericFrameInfo &frame = frames[i];
        for (const CodecBufferUsage &buffer_usage : frame.encoder_buffers)
        {
            if (buffer_usage.id < 0 || buffer_usage.id >= 8)
            {
                ADD_FAILURE() << "Invalid buffer id " << buffer_usage.id << " for frame#" << i
                              << ". Up to 8 buffers are supported.";
                valid = false;
                continue;
            }
            if (buffer_usage.referenced && !buffer_contains_frame[buffer_usage.id])
            {
                ADD_FAILURE() << "buffer " << buffer_usage.id << " for frame#" << i << " was reference before updated.";
                valid = false;
            }
            if (buffer_usage.updated)
            {
                buffer_contains_frame.set(buffer_usage.id);
            }
        }
        for (int fdiff : frame.frame_diffs)
        {
            if (fdiff <= 0 || static_cast<size_t>(fdiff) > i)
            {
                ADD_FAILURE() << "Invalid frame diff " << fdiff << " for frame#" << i;
                valid = false;
            }
        }
    }
    return valid;
}

OCTK_END_NAMESPACE
#endif