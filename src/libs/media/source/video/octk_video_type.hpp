/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
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

#define OCTK_FOURCC(a, b, c, d) \
    ((static_cast<uint32_t>(a)) | (static_cast<uint32_t>(b) << 8) | \
    (static_cast<uint32_t>(c) << 16) | /* NOLINT */                \
    (static_cast<uint32_t>(d) << 24))  /* NOLINT */

OCTK_BEGIN_NAMESPACE

enum class VideoType : uint32_t
{
    // 13 Primary RGB formats: 4 32 bpp, 2 24 bpp, 3 16 bpp, 1 10 bpc 2 64 bpp
    kRGB565 = OCTK_FOURCC('R', 'G', 'B', 'P'),  // rgb565 LE.
    kRGB24 = OCTK_FOURCC('R', 'G', 'B', ' '),
    kBGR24 = OCTK_FOURCC('B', 'G', 'R', ' '),
    kARGB = OCTK_FOURCC('A', 'R', 'G', 'B'),
    kBGRA = OCTK_FOURCC('B', 'G', 'R', 'A'),
    kABGR = OCTK_FOURCC('A', 'B', 'G', 'R'),
    kRGBA = OCTK_FOURCC('R', 'G', 'B', 'A'),
    kRAW = OCTK_FOURCC('R', 'A', 'W', ' '),

    // 10 Primary YUV formats: 5 planar, 2 biplanar, 2 packed.
    kI420 = OCTK_FOURCC('I', '4', '2', '0'),
    kI422 = OCTK_FOURCC('I', '4', '2', '2'),
    kI444 = OCTK_FOURCC('I', '4', '4', '4'),
    kI400 = OCTK_FOURCC('I', '4', '0', '0'),
    kNV21 = OCTK_FOURCC('N', 'V', '2', '1'),
    kNV12 = OCTK_FOURCC('N', 'V', '1', '2'),
    kYUY2 = OCTK_FOURCC('Y', 'U', 'Y', '2'),
    kYV12 = OCTK_FOURCC('Y', 'V', '1', '2'),
    kUYVY = OCTK_FOURCC('U', 'Y', 'V', 'Y'),
    kI010 = OCTK_FOURCC('I', '0', '1', '0'),
    kI210 = OCTK_FOURCC('I', '2', '1', '0'),

    // 1 Primary Compressed YUV format.
    kMJPG = OCTK_FOURCC('M', 'J', 'P', 'G'),
    // Match any fourcc.
    kANY = OCTK_FOURCC('A', 'N', 'Y', ' '),
};

namespace utils
{
/**
 * @brief Calculate the required buffer size.
 * @param type      - The type of the designated video frame
 * @param width     - frame width in pixels.
 * @param height    - frame height in pixels.
 * @return The required size in bytes to accommodate the specified video frame.
 */
OCTK_MEDIA_API size_t videoTypeBufferSize(VideoType type, int width, int height);
} // namespace utils

OCTK_END_NAMESPACE
