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

#ifndef _OCTK_VIDEO_FILTER_SETTINGS_HPP
#define _OCTK_VIDEO_FILTER_SETTINGS_HPP

#include <octk_media_global.hpp>

#include <stdint.h>

OCTK_BEGIN_NAMESPACE

// Filter settings for automatic corruption detection. See
// http://www.webrtc.org/experiments/rtp-hdrext/corruption-detection for more
// information.
struct CorruptionDetectionFilterSettings
{
    // Size of the blur kernel used.
    double std_dev = 0.0;
    // Allowed error thresholds (maps to `Y err` and `UV err` respectively).
    int luma_error_threshold = 0;
    int chroma_error_threshold = 0;
};
OCTK_END_NAMESPACE

#endif  // _OCTK_VIDEO_FILTER_SETTINGS_HPP
