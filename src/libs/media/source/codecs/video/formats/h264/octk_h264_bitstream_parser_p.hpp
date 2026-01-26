/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2026~Present ChengXueWen.
** Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
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

#include <private/octk_video_bitstream_parser_p.hpp>
#include <private/octk_h264_pps_parser_p.hpp>
#include <private/octk_h264_sps_parser_p.hpp>

#include <stddef.h>
#include <stdint.h>

OCTK_BEGIN_NAMESPACE

// Stateful H264 bitstream parser (due to SPS/PPS). Used to parse out QP values
// from the bitstream.
// TODO(pbos): Unify with RTP SPS parsing and only use one H264 parser.
// TODO(pbos): If/when this gets used on the receiver side CHECKs must be
// removed and gracefully abort as we have no control over receive-side
// bitstreams.
class H264BitStreamParser : public BitStreamParser
{
public:
    H264BitStreamParser();
    ~H264BitStreamParser() override;

    void ParseBitstream(ArrayView<const uint8_t> bitstream) override;
    Optional<int> GetLastSliceQp() const override;

protected:
    enum Result
    {
        kOk,
        kInvalidStream,
        kUnsupportedStream,
    };
    void ParseSlice(ArrayView<const uint8_t> slice);
    Result ParseNonParameterSetNalu(ArrayView<const uint8_t> source, uint8_t nalu_type);

    // SPS/PPS state, updated when parsing new SPS/PPS, used to parse slices.
    Optional<SpsParser::SpsState> sps_;
    Optional<PpsParser::PpsState> pps_;

    // Last parsed slice QP.
    Optional<int32_t> last_slice_qp_delta_;
};

OCTK_END_NAMESPACE
