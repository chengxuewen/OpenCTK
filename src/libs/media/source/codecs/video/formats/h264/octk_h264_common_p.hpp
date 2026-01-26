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
#include <octk_buffer.hpp>

#include <stddef.h>
#include <stdint.h>

#include <vector>

OCTK_BEGIN_NAMESPACE

namespace h264
{
// The size of a full NALU start sequence {0 0 0 1}, used for the first NALU
// of an access unit, and for SPS and PPS blocks.
const size_t kNaluLongStartSequenceSize = 4;

// The size of a shortened NALU start sequence {0 0 1}, that may be used if
// not the first NALU of an access unit or an SPS or PPS block.
const size_t kNaluShortStartSequenceSize = 3;

// The size of the NALU type byte (1).
const size_t kNaluTypeSize = 1;

// Maximum reference index for reference pictures.
constexpr int kMaxReferenceIndex = 31;

enum NaluType : uint8_t
{
    kSlice = 1,
    kIdr = 5,
    kSei = 6,
    kSps = 7,
    kPps = 8,
    kAud = 9,
    kEndOfSequence = 10,
    kEndOfStream = 11,
    kFiller = 12,
    kPrefix = 14,
    kStapA = 24,
    kFuA = 28
};

enum SliceType : uint8_t
{
    kP = 0,
    kB = 1,
    kI = 2,
    kSp = 3,
    kSi = 4
};

struct NaluIndex
{
    // Start index of NALU, including start sequence.
    size_t start_offset;
    // Start index of NALU payload, typically type header.
    size_t payload_start_offset;
    // Length of NALU payload, in bytes, counting from payload_start_offset.
    size_t payload_size;
};

// Returns a vector of the NALU indices in the given buffer.
OCTK_MEDIA_API std::vector<NaluIndex> FindNaluIndices(ArrayView<const uint8_t> buffer);

// Get the NAL type from the header byte immediately following start sequence.
OCTK_MEDIA_API NaluType ParseNaluType(uint8_t data);

// Methods for parsing and writing RBSP. See section 7.4.1 of the H264 spec.
//
// The following sequences are illegal, and need to be escaped when encoding:
// 00 00 00 -> 00 00 03 00
// 00 00 01 -> 00 00 03 01
// 00 00 02 -> 00 00 03 02
// And things in the source that look like the emulation byte pattern (00 00 03)
// need to have an extra emulation byte added, so it's removed when decoding:
// 00 00 03 -> 00 00 03 03
//
// Decoding is simply a matter of finding any 00 00 03 sequence and removing
// the 03 emulation byte.

// Parse the given data and remove any emulation byte escaping.
std::vector<uint8_t> ParseRbsp(ArrayView<const uint8_t> data);

// TODO: bugs.webrtc.org/42225170 - Deprecate.
inline std::vector<uint8_t> ParseRbsp(const uint8_t *data, size_t length)
{
    return ParseRbsp(utils::makeArrayView(data, length));
}

// Write the given data to the destination buffer, inserting and emulation
// bytes in order to escape any data the could be interpreted as a start
// sequence.
void WriteRbsp(ArrayView<const uint8_t> bytes, Buffer *destination);

// TODO: bugs.webrtc.org/42225170 - Deprecate.
inline void WriteRbsp(const uint8_t *bytes, size_t length, Buffer *destination)
{
    WriteRbsp(utils::makeArrayView(bytes, length), destination);
}

} // namespace h264

OCTK_END_NAMESPACE