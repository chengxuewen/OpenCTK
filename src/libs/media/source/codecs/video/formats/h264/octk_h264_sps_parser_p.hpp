/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2026~Present ChengXueWen.
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

#include <octk_media_global.hpp>
#include <octk_bit_buffer.hpp>
#include <octk_optional.hpp>

OCTK_BEGIN_NAMESPACE

// A class for parsing out sequence parameter set (SPS) data from an H264 NALU.
class OCTK_MEDIA_API SpsParser
{
public:
    // The parsed state of the SPS. Only some select values are stored.
    // Add more as they are actually needed.
    struct OCTK_MEDIA_API SpsState
    {
        SpsState();
        SpsState(const SpsState &);
        ~SpsState();

        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t delta_pic_order_always_zero_flag = 0;
        uint32_t chroma_format_idc = 1;
        uint32_t separate_colour_plane_flag = 0;
        uint32_t frame_mbs_only_flag = 0;
        uint32_t log2_max_frame_num = 4;         // Smallest valid value.
        uint32_t log2_max_pic_order_cnt_lsb = 4; // Smallest valid value.
        uint32_t pic_order_cnt_type = 0;
        uint32_t max_num_ref_frames = 0;
        uint32_t vui_params_present = 0;
        uint32_t id = 0;
    };

    // Unpack RBSP and parse SPS state from the supplied buffer.
    static Optional<SpsState> ParseSps(ArrayView<const uint8_t> data);

protected:
    // Parse the SPS state, up till the VUI part, for a buffer where RBSP
    // decoding has already been performed.
    static Optional<SpsState> ParseSpsUpToVui(BitBufferReader &reader);
};

OCTK_END_NAMESPACE
