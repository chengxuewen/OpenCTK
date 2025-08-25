/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
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

#include <octk_encoded_frame.hpp>

#include <cstddef>
#include <cstdint>

OCTK_BEGIN_NAMESPACE

Optional<Timestamp> EncodedFrame::ReceivedTimestamp() const
{
    return ReceivedTime() >= 0 ? utils::make_optional(Timestamp::Millis(ReceivedTime())) : utils::nullopt;
}

Optional<Timestamp> EncodedFrame::RenderTimestamp() const
{
    return RenderTimeMs() >= 0 ? utils::make_optional(Timestamp::Millis(RenderTimeMs())) : utils::nullopt;
}

bool EncodedFrame::delayed_by_retransmission() const { return false; }

void EncodedFrame::CopyCodecSpecific(const RTPVideoHeader *header)
{
    if (header)
    {
        switch (header->codec)
        {
            case kVideoCodecVP8:
            {
                const auto &vp8_header = octk::get<RTPVideoHeaderVP8>(header->video_type_header);
                if (_codecSpecificInfo.codecType != kVideoCodecVP8)
                {
                    // This is the first packet for this frame.
                    _codecSpecificInfo.codecSpecific.VP8.temporalIdx = 0;
                    _codecSpecificInfo.codecSpecific.VP8.layerSync = false;
                    _codecSpecificInfo.codecSpecific.VP8.keyIdx = -1;
                    _codecSpecificInfo.codecType = kVideoCodecVP8;
                }
                _codecSpecificInfo.codecSpecific.VP8.nonReference = vp8_header.nonReference;
                if (vp8_header.temporalIdx != kNoTemporalIdx)
                {
                    _codecSpecificInfo.codecSpecific.VP8.temporalIdx = vp8_header.temporalIdx;
                    _codecSpecificInfo.codecSpecific.VP8.layerSync = vp8_header.layerSync;
                }
                if (vp8_header.keyIdx != kNoKeyIdx)
                {
                    _codecSpecificInfo.codecSpecific.VP8.keyIdx = vp8_header.keyIdx;
                }
                break;
            }
            case kVideoCodecVP9:
            {
                const auto &vp9_header = octk::get<RTPVideoHeaderVP9>(header->video_type_header);
                if (_codecSpecificInfo.codecType != kVideoCodecVP9)
                {
                    // This is the first packet for this frame.
                    _codecSpecificInfo.codecSpecific.VP9.temporal_idx = 0;
                    _codecSpecificInfo.codecSpecific.VP9.gof_idx = 0;
                    _codecSpecificInfo.codecSpecific.VP9.inter_layer_predicted = false;
                    _codecSpecificInfo.codecType = kVideoCodecVP9;
                }
                _codecSpecificInfo.codecSpecific.VP9.inter_pic_predicted = vp9_header.inter_pic_predicted;
                _codecSpecificInfo.codecSpecific.VP9.flexible_mode = vp9_header.flexible_mode;
                _codecSpecificInfo.codecSpecific.VP9.num_ref_pics = vp9_header.num_ref_pics;
                for (uint8_t r = 0; r < vp9_header.num_ref_pics; ++r)
                {
                    _codecSpecificInfo.codecSpecific.VP9.p_diff[r] = vp9_header.pid_diff[r];
                }
                _codecSpecificInfo.codecSpecific.VP9.ss_data_available = vp9_header.ss_data_available;
                if (vp9_header.temporal_idx != kNoTemporalIdx)
                {
                    _codecSpecificInfo.codecSpecific.VP9.temporal_idx = vp9_header.temporal_idx;
                    _codecSpecificInfo.codecSpecific.VP9.temporal_up_switch = vp9_header.temporal_up_switch;
                }
                if (vp9_header.spatial_idx != kNoSpatialIdx)
                {
                    _codecSpecificInfo.codecSpecific.VP9.inter_layer_predicted = vp9_header.inter_layer_predicted;
                    this->setSpatialIndex(vp9_header.spatial_idx);
                }
                if (vp9_header.gof_idx != kNoGofIdx)
                {
                    _codecSpecificInfo.codecSpecific.VP9.gof_idx = vp9_header.gof_idx;
                }
                if (vp9_header.ss_data_available)
                {
                    auto &vp9 = _codecSpecificInfo.codecSpecific.VP9;
                    vp9.num_spatial_layers = vp9_header.num_spatial_layers;
                    vp9.spatial_layer_resolution_present = vp9_header.spatial_layer_resolution_present;
                    if (vp9_header.spatial_layer_resolution_present)
                    {
                        for (size_t i = 0; i < vp9_header.num_spatial_layers; ++i)
                        {
                            _codecSpecificInfo.codecSpecific.VP9.width[i] = vp9_header.width[i];
                            _codecSpecificInfo.codecSpecific.VP9.height[i] = vp9_header.height[i];
                        }
                    }
                    _codecSpecificInfo.codecSpecific.VP9.gof.CopyGofInfoVP9(vp9_header.gof);
                }
                break;
            }
            case kVideoCodecH264:
            {
                _codecSpecificInfo.codecType = kVideoCodecH264;
                break;
            }
            case kVideoCodecAV1:
            {
                _codecSpecificInfo.codecType = kVideoCodecAV1;
                break;
            }
            default:
            {
                _codecSpecificInfo.codecType = kVideoCodecGeneric;
                break;
            }
        }
    }
}

OCTK_END_NAMESPACE
