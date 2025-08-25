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

#ifndef _OCTK_DIFFER_BLOCK_HPP
#define _OCTK_DIFFER_BLOCK_HPP

#include <octk_media_global.hpp>

OCTK_BEGIN_NAMESPACE

// Size (in pixels) of each square block used for diffing. This must be a
// multiple of sizeof(uint64)/8.
const int kBlockSize = 32;

// Format: BGRA 32 bit.
const int kBytesPerPixel = 4;

// Low level function to compare 2 vectors of pixels of size kBlockSize. Returns
// whether the blocks differ.
bool VectorDifference(const uint8_t *image1, const uint8_t *image2);

// Low level function to compare 2 blocks of pixels of size
// (kBlockSize, `height`).  Returns whether the blocks differ.
bool BlockDifference(const uint8_t *image1,
                     const uint8_t *image2,
                     int height,
                     int stride);

// Low level function to compare 2 blocks of pixels of size
// (kBlockSize, kBlockSize).  Returns whether the blocks differ.
bool BlockDifference(const uint8_t *image1, const uint8_t *image2, int stride);

OCTK_END_NAMESPACE

#endif  // _OCTK_DIFFER_BLOCK_HPP
