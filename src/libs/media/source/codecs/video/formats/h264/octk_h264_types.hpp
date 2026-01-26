/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2026~Present ChengXueWen.
** Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
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

#include <octk_codecs_constants.hpp>
#include <octk_checks.hpp>

#include <algorithm>
#include <string>
#include <vector>

OCTK_BEGIN_NAMESPACE

// The packetization types that we support: single, aggregated, and fragmented.
enum H264PacketizationTypes
{
    kH264SingleNalu, // This packet contains a single NAL unit.
    kH264StapA,      // This packet contains STAP-A (single time
    // aggregation) packets. If this packet has an
    // associated NAL unit type, it'll be for the
    // first such aggregated packet.
    kH264FuA, // This packet contains a FU-A (fragmentation
    // unit) packet, meaning it is a part of a frame
    // that was too large to fit into a single packet.
};

// Packetization modes are defined in RFC 6184 section 6
// Due to the structure containing this being initialized with zeroes
// in some places, and mode 1 being default, mode 1 needs to have the value
// zero. https://crbug.com/webrtc/6803
enum class H264PacketizationMode
{
    NonInterleaved = 0, // Mode 1 - STAP-A, FU-A is allowed
    SingleNalUnit       // Mode 0 - only single NALU allowed
};

// This function is declared inline because it is not clear which
// .cc file it should belong to.
// TODO(hta): Refactor. https://bugs.webrtc.org/6842
// TODO(jonasolsson): Use StringView instead when that's available.
inline std::string toString(H264PacketizationMode mode)
{
    if (mode == H264PacketizationMode::NonInterleaved)
    {
        return "NonInterleaved";
    }
    else if (mode == H264PacketizationMode::SingleNalUnit)
    {
        return "SingleNalUnit";
    }
    OCTK_DCHECK_NOTREACHED();
    return "";
}

struct NaluInfo
{
    uint8_t type;
    int sps_id;
    int pps_id;

    friend bool operator==(const NaluInfo &lhs, const NaluInfo &rhs)
    {
        return lhs.type == rhs.type && lhs.sps_id == rhs.sps_id && lhs.pps_id == rhs.pps_id;
    }

    friend bool operator!=(const NaluInfo &lhs, const NaluInfo &rhs) { return !(lhs == rhs); }
};

struct RTPVideoHeaderH264
{
    // The NAL unit type. If this is a header for a
    // fragmented packet, it's the NAL unit type of
    // the original data. If this is the header for an
    // aggregated packet, it's the NAL unit type of
    // the first NAL unit in the packet.
    uint8_t nalu_type;
    // The packetization type of this buffer - single, aggregated or fragmented.
    H264PacketizationTypes packetization_type;
    std::vector<NaluInfo> nalus;
    // The packetization mode of this transport. Packetization mode
    // determines which packetization types are allowed when packetizing.
    H264PacketizationMode packetization_mode;

    friend bool operator==(const RTPVideoHeaderH264 &lhs, const RTPVideoHeaderH264 &rhs)
    {
        return lhs.nalu_type == rhs.nalu_type && lhs.packetization_type == rhs.packetization_type &&
               lhs.nalus == rhs.nalus && lhs.packetization_mode == rhs.packetization_mode;
    }

    friend bool operator!=(const RTPVideoHeaderH264 &lhs, const RTPVideoHeaderH264 &rhs) { return !(lhs == rhs); }
};

OCTK_END_NAMESPACE