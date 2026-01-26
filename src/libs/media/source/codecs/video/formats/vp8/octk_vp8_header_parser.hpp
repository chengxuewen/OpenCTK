/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
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

#include <octk_media_global.hpp>

#include <stdint.h>
#include <stdio.h>

OCTK_BEGIN_NAMESPACE

namespace vp8
{

typedef struct VP8BitReader VP8BitReader;
struct VP8BitReader
{
    // Boolean decoder.
    uint32_t value_; // Current value (2 bytes).
    uint32_t range_; // Current range (always in [128..255] interval).
    int bits_;       // Number of bits shifted out of value, at most 7.
    // Read buffer.
    const uint8_t *buf_;     // Next byte to be read.
    const uint8_t *buf_end_; // End of read buffer.
};

// Gets the QP, QP range: [0, 127].
// Returns true on success, false otherwise.
bool GetQp(const uint8_t *buf, size_t length, int *qp);

} // namespace vp8

OCTK_END_NAMESPACE
