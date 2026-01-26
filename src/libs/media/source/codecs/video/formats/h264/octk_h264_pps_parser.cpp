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

#include <private/octk_h264_pps_parser_p.hpp>
#include <private/octk_h264_common_p.hpp>
#include <octk_bit_buffer.hpp>
#include <octk_checks.hpp>
#include <octk_bits.hpp>

#include <limits>
#include <vector>

OCTK_BEGIN_NAMESPACE

namespace
{
constexpr int kMaxPicInitQpDeltaValue = 25;
constexpr int kMinPicInitQpDeltaValue = -26;
} // namespace

// General note: this is based off the 02/2014 version of the H.264 standard.
// You can find it on this page:
// http://www.itu.int/rec/T-REC-H.264

Optional<PpsParser::PpsState> PpsParser::ParsePps(ArrayView<const uint8_t> data)
{
    // First, parse out rbsp, which is basically the source buffer minus emulation
    // bytes (the last byte of a 0x00 0x00 0x03 sequence). RBSP is defined in
    // section 7.3.1 of the H.264 standard.
    return ParseInternal(h264::ParseRbsp(data));
}

bool PpsParser::ParsePpsIds(ArrayView<const uint8_t> data, uint32_t *pps_id, uint32_t *sps_id)
{
    OCTK_DCHECK(pps_id);
    OCTK_DCHECK(sps_id);
    // First, parse out rbsp, which is basically the source buffer minus emulation
    // bytes (the last byte of a 0x00 0x00 0x03 sequence). RBSP is defined in
    // section 7.3.1 of the H.264 standard.
    std::vector<uint8_t> unpacked_buffer = h264::ParseRbsp(data);
    BitBufferReader reader(unpacked_buffer);
    *pps_id = reader.ReadExponentialGolomb();
    *sps_id = reader.ReadExponentialGolomb();
    return reader.Ok();
}

Optional<PpsParser::SliceHeader> PpsParser::ParseSliceHeader(ArrayView<const uint8_t> data)
{
    std::vector<uint8_t> unpacked_buffer = h264::ParseRbsp(data);
    BitBufferReader slice_reader(unpacked_buffer);
    PpsParser::SliceHeader slice_header;

    // first_mb_in_slice: ue(v)
    slice_header.first_mb_in_slice = slice_reader.ReadExponentialGolomb();
    // slice_type: ue(v)
    slice_reader.ReadExponentialGolomb();
    // pic_parameter_set_id: ue(v)
    slice_header.pic_parameter_set_id = slice_reader.ReadExponentialGolomb();

    // The rest of the slice header requires information from the SPS to parse.

    if (!slice_reader.Ok())
    {
        return utils::nullopt;
    }
    return slice_header;
}

Optional<PpsParser::PpsState> PpsParser::ParseInternal(ArrayView<const uint8_t> buffer)
{
    BitBufferReader reader(buffer);
    PpsState pps;
    pps.id = reader.ReadExponentialGolomb();
    pps.sps_id = reader.ReadExponentialGolomb();

    // entropy_coding_mode_flag: u(1)
    pps.entropy_coding_mode_flag = reader.Read<bool>();
    // bottom_field_pic_order_in_frame_present_flag: u(1)
    pps.bottom_field_pic_order_in_frame_present_flag = reader.Read<bool>();

    // num_slice_groups_minus1: ue(v)
    uint32_t num_slice_groups_minus1 = reader.ReadExponentialGolomb();
    if (num_slice_groups_minus1 > 0)
    {
        // slice_group_map_type: ue(v)
        uint32_t slice_group_map_type = reader.ReadExponentialGolomb();
        if (slice_group_map_type == 0)
        {
            for (uint32_t i_group = 0; i_group <= num_slice_groups_minus1 && reader.Ok(); ++i_group)
            {
                // run_length_minus1[iGroup]: ue(v)
                reader.ReadExponentialGolomb();
            }
        }
        else if (slice_group_map_type == 1)
        {
            // TODO(sprang): Implement support for dispersed slice group map type.
            // See 8.2.2.2 Specification for dispersed slice group map type.
        }
        else if (slice_group_map_type == 2)
        {
            for (uint32_t i_group = 0; i_group <= num_slice_groups_minus1 && reader.Ok(); ++i_group)
            {
                // top_left[iGroup]: ue(v)
                reader.ReadExponentialGolomb();
                // bottom_right[iGroup]: ue(v)
                reader.ReadExponentialGolomb();
            }
        }
        else if (slice_group_map_type == 3 || slice_group_map_type == 4 || slice_group_map_type == 5)
        {
            // slice_group_change_direction_flag: u(1)
            reader.ConsumeBits(1);
            // slice_group_change_rate_minus1: ue(v)
            reader.ReadExponentialGolomb();
        }
        else if (slice_group_map_type == 6)
        {
            // pic_size_in_map_units_minus1: ue(v)
            uint32_t pic_size_in_map_units = reader.ReadExponentialGolomb() + 1;
            int slice_group_id_bits = 1 + utils::bit_width(num_slice_groups_minus1);

            // slice_group_id: array of size pic_size_in_map_units, each element
            // is represented by ceil(log2(num_slice_groups_minus1 + 1)) bits.
            int64_t bits_to_consume = int64_t{slice_group_id_bits} * pic_size_in_map_units;
            if (!reader.Ok() || bits_to_consume > std::numeric_limits<int>::max())
            {
                return utils::nullopt;
            }
            reader.ConsumeBits(bits_to_consume);
        }
    }
    // num_ref_idx_l0_default_active_minus1: ue(v)
    pps.num_ref_idx_l0_default_active_minus1 = reader.ReadExponentialGolomb();
    // num_ref_idx_l1_default_active_minus1: ue(v)
    pps.num_ref_idx_l1_default_active_minus1 = reader.ReadExponentialGolomb();
    if (pps.num_ref_idx_l0_default_active_minus1 > h264::kMaxReferenceIndex ||
        pps.num_ref_idx_l1_default_active_minus1 > h264::kMaxReferenceIndex)
    {
        return utils::nullopt;
    }
    // weighted_pred_flag: u(1)
    pps.weighted_pred_flag = reader.Read<bool>();
    // weighted_bipred_idc: u(2)
    pps.weighted_bipred_idc = reader.ReadBits(2);

    // pic_init_qp_minus26: se(v)
    pps.pic_init_qp_minus26 = reader.ReadSignedExponentialGolomb();
    // Sanity-check parsed value
    if (!reader.Ok() || pps.pic_init_qp_minus26 > kMaxPicInitQpDeltaValue ||
        pps.pic_init_qp_minus26 < kMinPicInitQpDeltaValue)
    {
        return utils::nullopt;
    }
    // pic_init_qs_minus26: se(v)
    reader.ReadExponentialGolomb();
    // chroma_qp_index_offset: se(v)
    reader.ReadExponentialGolomb();
    // deblocking_filter_control_present_flag: u(1)
    // constrained_intra_pred_flag: u(1)
    reader.ConsumeBits(2);
    // redundant_pic_cnt_present_flag: u(1)
    pps.redundant_pic_cnt_present_flag = reader.ReadBit();
    if (!reader.Ok())
    {
        return utils::nullopt;
    }

    return pps;
}

OCTK_END_NAMESPACE
