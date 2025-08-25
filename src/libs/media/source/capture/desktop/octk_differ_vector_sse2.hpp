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

// This header file is used only differ_block.h. It defines the SSE2 rountines
// for finding vector difference.

#ifndef _OCTK_DESKTOP_CAPTURE_DIFFER_VECTOR_SSE2_HPP
#define _OCTK_DESKTOP_CAPTURE_DIFFER_VECTOR_SSE2_HPP

#include <octk_media_global.hpp>

#include <stdint.h>

OCTK_BEGIN_NAMESPACE

// Find vector difference of dimension 16.
extern bool VectorDifference_SSE2_W16(const uint8_t *image1,
                                      const uint8_t *image2);

// Find vector difference of dimension 32.
extern bool VectorDifference_SSE2_W32(const uint8_t *image1,
                                      const uint8_t *image2);
OCTK_END_NAMESPACE

#endif  // _OCTK_DESKTOP_CAPTURE_DIFFER_VECTOR_SSE2_HPP
