/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
** Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
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

#include <octk_vp9_constants.hpp>
#include <octk_array_view.hpp>
#include <octk_optional.hpp>

#include <stddef.h>
#include <stdint.h>

#include <array>
#include <bitset>
#include <optional>
#include <string>

OCTK_BEGIN_NAMESPACE

namespace vp9
{

// Gets the QP, QP range: [0, 255].
// Returns true on success, false otherwise.
bool GetQp(const uint8_t *buf, size_t length, int *qp);

} // namespace vp9

// Bit depth per channel. Support varies by profile.
enum class Vp9BitDept : uint8_t
{
    k8Bit = 8,
    k10Bit = 10,
    k12Bit = 12,
};

enum class Vp9ColorSpace : uint8_t
{
    CS_UNKNOWN = 0,   // Unknown (in this case the color space must be signaled
                      // outside the VP9 bitstream).
    CS_BT_601 = 1,    // CS_BT_601 Rec. ITU-R BT.601-7
    CS_BT_709 = 2,    // Rec. ITU-R BT.709-6
    CS_SMPTE_170 = 3, // SMPTE-170
    CS_SMPTE_240 = 4, // SMPTE-240
    CS_BT_2020 = 5,   // Rec. ITU-R BT.2020-2
    CS_RESERVED = 6,  // Reserved
    CS_RGB = 7,       // sRGB (IEC 61966-2-1)
};

enum class Vp9ColorRange
{
    kStudio, // Studio swing:
             // For BitDepth equals 8:
             //     Y is between 16 and 235 inclusive.
             //     U and V are between 16 and 240 inclusive.
             // For BitDepth equals 10:
             //     Y is between 64 and 940 inclusive.
             //     U and V are between 64 and 960 inclusive.
             // For BitDepth equals 12:
             //     Y is between 256 and 3760.
             //     U and V are between 256 and 3840 inclusive.
    kFull    // Full swing; no restriction on Y, U, V values.
};

enum class Vp9YuvSubsampling
{
    k444,
    k440,
    k422,
    k420,
};

enum Vp9ReferenceFrame : int
{
    kNone = -1,
    kIntra = 0,
    kLast = 1,
    kGolden = 2,
    kAltref = 3,
};

enum class Vp9InterpolationFilter : uint8_t
{
    kEightTap = 0,
    kEightTapSmooth = 1,
    kEightTapSharp = 2,
    kBilinear = 3,
    kSwitchable = 4
};

struct Vp9UncompressedHeader
{
    int profile = 0; // Profiles 0-3 are valid.
    Optional<uint8_t> show_existing_frame;
    bool is_keyframe = false;
    bool show_frame = false;
    bool error_resilient = false;
    Vp9BitDept bit_detph = Vp9BitDept::k8Bit;
    Optional<Vp9ColorSpace> color_space;
    Optional<Vp9ColorRange> color_range;
    Optional<Vp9YuvSubsampling> sub_sampling;
    int frame_width = 0;
    int frame_height = 0;
    int render_width = 0;
    int render_height = 0;
    // Width/height of the tiles used (in units of 8x8 blocks).
    size_t tile_cols_log2 = 0; // tile_cols = 1 << tile_cols_log2
    size_t tile_rows_log2 = 0; // tile_rows = 1 << tile_rows_log2
    Optional<size_t> render_size_offset_bits;
    // Number of bits from the start of the frame header to where the loop filter
    // parameters are located.
    Optional<size_t> loop_filter_params_offset_bits;
    Vp9InterpolationFilter interpolation_filter = Vp9InterpolationFilter::kEightTap;
    bool allow_high_precision_mv = false;
    int base_qp = 0;
    bool is_lossless = false;
    uint8_t frame_context_idx = 0;

    bool segmentation_enabled = false;
    Optional<std::array<uint8_t, 7>> segmentation_tree_probs;
    Optional<std::array<uint8_t, 3>> segmentation_pred_prob;
    bool segmentation_is_delta = false;
    std::array<std::array<Optional<int>, kVp9SegLvlMax>, kVp9MaxSegments> segmentation_features;

    // Which of the 8 reference buffers may be used as references for this frame.
    // -1 indicates not used (e.g. {-1, -1, -1} for intra-only frames).
    std::array<int, kVp9RefsPerFrame> reference_buffers = {-1, -1, -1};
    // Sign bias corresponding to reference buffers, where the index is a
    // ReferenceFrame.
    // false/0 indidate backwards reference, true/1 indicate forwards reference).
    std::bitset<kVp9MaxRefFrames> reference_buffers_sign_bias = 0;

    // Indicates which reference buffer [0,7] to infer the frame size from.
    Optional<int> infer_size_from_reference;
    // Which of the 8 reference buffers are updated by this frame.
    std::bitset<kVp9NumRefFrames> updated_buffers = 0;

    // Header sizes, in bytes.
    uint32_t uncompressed_header_size = 0;
    uint32_t compressed_header_size = 0;

    bool is_intra_only() const
    {
        return reference_buffers[0] == -1 && reference_buffers[1] == -1 && reference_buffers[2] == -1;
    }

    std::string ToString() const;
};

// Parses the uncompressed header and populates (most) values in a
// UncompressedHeader struct. Returns nullopt on failure.
Optional<Vp9UncompressedHeader> ParseUncompressedVp9Header(ArrayView<const uint8_t> buf);

OCTK_END_NAMESPACE