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

#include <octk_optional.hpp>
#include <octk_array_view.hpp>

#include <stddef.h>
#include <stdint.h>

OCTK_BEGIN_NAMESPACE

// A class for parsing out picture parameter set (PPS) data from a H264 NALU.
class PpsParser
{
public:
    // The parsed state of the PPS. Only some select values are stored.
    // Add more as they are actually needed.
    struct PpsState
    {
        PpsState() = default;

        bool bottom_field_pic_order_in_frame_present_flag = false;
        bool weighted_pred_flag = false;
        bool entropy_coding_mode_flag = false;
        uint32_t num_ref_idx_l0_default_active_minus1 = 0;
        uint32_t num_ref_idx_l1_default_active_minus1 = 0;
        uint32_t weighted_bipred_idc = false;
        uint32_t redundant_pic_cnt_present_flag = 0;
        int pic_init_qp_minus26 = 0;
        uint32_t id = 0;
        uint32_t sps_id = 0;
    };

    struct SliceHeader
    {
        SliceHeader() = default;

        uint32_t first_mb_in_slice = 0;
        uint32_t pic_parameter_set_id = 0;
    };

    // Unpack RBSP and parse PPS state from the supplied buffer.
    static Optional<PpsState> ParsePps(ArrayView<const uint8_t> data);
    // TODO: bugs.webrtc.org/42225170 - Deprecate.
    static inline Optional<PpsState> ParsePps(const uint8_t *data, size_t length)
    {
        return ParsePps(utils::makeArrayView(data, length));
    }

    static bool ParsePpsIds(ArrayView<const uint8_t> data, uint32_t *pps_id, uint32_t *sps_id);

    static Optional<SliceHeader> ParseSliceHeader(ArrayView<const uint8_t> data);

protected:
    // Parse the PPS state, for a buffer where RBSP decoding has already been
    // performed.
    static Optional<PpsState> ParseInternal(ArrayView<const uint8_t> buffer);
};

OCTK_END_NAMESPACE
