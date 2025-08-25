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

#ifndef _OCTK_FRAME_UTILS_HPP
#define _OCTK_FRAME_UTILS_HPP

#include <octk_nv12_buffer.hpp>

OCTK_BEGIN_NAMESPACE

class I420Buffer;

class VideoFrame;

class VideoFrameBuffer;
namespace utils
{

OCTK_MEDIA_API bool EqualPlane(const uint8_t *data1,
                               const uint8_t *data2,
                               int stride1,
                               int stride2,
                               int width,
                               int height);

static inline bool EqualPlane(const uint8_t *data1,
                              const uint8_t *data2,
                              int stride,
                              int width,
                              int height)
{
    return EqualPlane(data1, data2, stride, stride, width, height);
}

OCTK_MEDIA_API bool FramesEqual(const VideoFrame &f1, const VideoFrame &f2);

OCTK_MEDIA_API bool FrameBufsEqual(const std::shared_ptr<VideoFrameBuffer> &f1,
                                   const std::shared_ptr<VideoFrameBuffer> &f2);

OCTK_MEDIA_API std::shared_ptr<I420Buffer> ReadI420Buffer(int width, int height, FILE *);

OCTK_MEDIA_API std::shared_ptr<NV12Buffer> ReadNV12Buffer(int width, int height, FILE *);
} // namespace utils
OCTK_END_NAMESPACE

#endif // _OCTK_FRAME_UTILS_HPP
