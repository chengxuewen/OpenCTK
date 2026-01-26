/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
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

// This file contains codec dependent definitions that are needed in
// order to compile the WebRTC codebase, even if this codec is not used.

#pragma once

#include <octk_codecs_constants.hpp>

OCTK_BEGIN_NAMESPACE

struct RTPVideoHeaderVP8
{
    void InitRTPVideoHeaderVP8()
    {
        pictureId = media::codecs::kNoPictureId;
        tl0PicIdx = media::codecs::kNoTl0PicIdx;
        temporalIdx = media::codecs::kNoTemporalIdx;
        keyIdx = media::codecs::kNoKeyIdx;
        beginningOfPartition = false;
        nonReference = false;
        layerSync = false;
        partitionId = 0;
    }

    friend bool operator==(const RTPVideoHeaderVP8 &lhs, const RTPVideoHeaderVP8 &rhs)
    {
        return lhs.nonReference == rhs.nonReference && lhs.pictureId == rhs.pictureId &&
               lhs.tl0PicIdx == rhs.tl0PicIdx && lhs.temporalIdx == rhs.temporalIdx && lhs.layerSync == rhs.layerSync &&
               lhs.keyIdx == rhs.keyIdx && lhs.partitionId == rhs.partitionId &&
               lhs.beginningOfPartition == rhs.beginningOfPartition;
    }

    friend bool operator!=(const RTPVideoHeaderVP8 &lhs, const RTPVideoHeaderVP8 &rhs) { return !(lhs == rhs); }

    bool nonReference; // Frame is discardable.
    int16_t pictureId; // Picture ID index, 15 bits;
    // kNoPictureId if PictureID does not exist.
    int16_t tl0PicIdx; // TL0PIC_IDX, 8 bits;
    // kNoTl0PicIdx means no value provided.
    uint8_t temporalIdx; // Temporal layer index, or kNoTemporalIdx.
    bool layerSync;      // This frame is a layer sync frame.
    // Disabled if temporalIdx == kNoTemporalIdx.
    int keyIdx;                // 5 bits; kNoKeyIdx means not used.
    int partitionId;           // VP8 partition ID
    bool beginningOfPartition; // True if this packet is the first
    // in a VP8 partition. Otherwise false
};

OCTK_END_NAMESPACE