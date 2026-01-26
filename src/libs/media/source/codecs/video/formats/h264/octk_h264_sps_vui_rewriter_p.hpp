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

#include <private/octk_h264_sps_parser_p.hpp>
#include <octk_color_space.hpp>
#include <octk_buffer.hpp>

#include <stddef.h>
#include <stdint.h>

OCTK_BEGIN_NAMESPACE

// A class that can parse an SPS+VUI and if necessary creates a copy with
// updated parameters.
// The rewriter disables frame buffering. This should force decoders to deliver
// decoded frame immediately and, thus, reduce latency.
// The rewriter updates video signal type parameters if external parameters are
// provided.
class SpsVuiRewriter : private SpsParser
{
public:
    enum class ParseResult
    {
        kFailure,
        kVuiOk,
        kVuiRewritten
    };
    enum class Direction
    {
        kIncoming,
        kOutgoing
    };

    // Parses an SPS block and if necessary copies it and rewrites the VUI.
    // Returns kFailure on failure, kParseOk if parsing succeeded and no update
    // was necessary and kParsedAndModified if an updated copy of buffer was
    // written to destination. destination may be populated with some data even if
    // no rewrite was necessary, but the end offset should remain unchanged.
    // Unless parsing fails, the sps parameter will be populated with the parsed
    // SPS state. This function assumes that any previous headers
    // (NALU start, type, Stap-A, etc) have already been parsed and that RBSP
    // decoding has been performed.
    static ParseResult ParseAndRewriteSps(ArrayView<const uint8_t> buffer,
                                          Optional<SpsParser::SpsState> *sps,
                                          const ColorSpace *color_space,
                                          Buffer *destination,
                                          Direction Direction);

    // Parses NAL units from `buffer`, strips AUD blocks and rewrites VUI in SPS
    // blocks if necessary.
    static Buffer ParseOutgoingBitstreamAndRewrite(ArrayView<const uint8_t> buffer, const ColorSpace *color_space);

private:
    static ParseResult ParseAndRewriteSps(ArrayView<const uint8_t> buffer,
                                          Optional<SpsParser::SpsState> *sps,
                                          const ColorSpace *color_space,
                                          Buffer *destination);

    static void UpdateStats(ParseResult result, Direction direction);
};

OCTK_END_NAMESPACE