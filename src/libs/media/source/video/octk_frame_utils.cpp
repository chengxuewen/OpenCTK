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

#include <octk_frame_utils.hpp>
#include <octk_i420_buffer.hpp>
#include <octk_nv12_buffer.hpp>
#include <octk_video_frame.hpp>

OCTK_BEGIN_NAMESPACE

namespace utils
{

bool EqualPlane(const uint8_t *data1,
                const uint8_t *data2,
                int stride1,
                int stride2,
                int width,
                int height)
{
    for (int y = 0; y < height; ++y)
    {
        if (memcmp(data1, data2, width) != 0)
        {
            return false;
        }
        data1 += stride1;
        data2 += stride2;
    }
    return true;
}

bool FramesEqual(const VideoFrame &f1, const VideoFrame &f2)
{
    if (f1.rtpTimestamp() != f2.rtpTimestamp() ||
        f1.ntpTimeMSecs() != f2.ntpTimeMSecs() ||
        f1.renderTimeMSecs() != f2.renderTimeMSecs())
    {
        return false;
    }
    return FrameBufsEqual(f1.videoFrameBuffer(), f2.videoFrameBuffer());
}

bool FrameBufsEqual(const std::shared_ptr<VideoFrameBuffer> &f1,
                    const std::shared_ptr<VideoFrameBuffer> &f2)
{
    if (f1 == f2)
    {
        return true;
    }
    // Exlude nullptr (except if both are nullptr, as above)
    if (!f1 || !f2)
    {
        return false;
    }

    if (f1->width() != f2->width() || f1->height() != f2->height() ||
        f1->type() != f2->type())
    {
        return false;
    }

    std::shared_ptr<I420BufferInterface> f1_i420 = f1->toI420();
    std::shared_ptr<I420BufferInterface> f2_i420 = f2->toI420();
    return EqualPlane(f1_i420->dataY(), f2_i420->dataY(), f1_i420->strideY(),
                      f2_i420->strideY(), f1_i420->width(), f1_i420->height()) &&
           EqualPlane(f1_i420->dataU(), f2_i420->dataU(), f1_i420->strideU(),
                      f2_i420->strideU(), f1_i420->chromaWidth(),
                      f1_i420->chromaHeight()) &&
           EqualPlane(f1_i420->dataV(), f2_i420->dataV(), f1_i420->strideV(),
                      f2_i420->strideV(), f1_i420->chromaWidth(),
                      f1_i420->chromaHeight());
}

std::shared_ptr<I420Buffer> ReadI420Buffer(int width, int height, FILE *f)
{
    int half_width = (width + 1) / 2;
    std::shared_ptr<I420Buffer> buffer(
        // Explicit stride, no padding between rows.
        I420Buffer::create(width, height, width, half_width, half_width));
    size_t size_y = static_cast<size_t>(width) * height;
    size_t size_uv = static_cast<size_t>(half_width) * ((height + 1) / 2);

    if (fread(buffer->MutableDataY(), 1, size_y, f) < size_y)
    {
        return nullptr;
    }
    if (fread(buffer->MutableDataU(), 1, size_uv, f) < size_uv)
    {
        return nullptr;
    }
    if (fread(buffer->MutableDataV(), 1, size_uv, f) < size_uv)
    {
        return nullptr;
    }
    return buffer;
}

std::shared_ptr<NV12Buffer> ReadNV12Buffer(int width, int height, FILE *f)
{
    std::shared_ptr<NV12Buffer> buffer(NV12Buffer::create(width, height));
    size_t size_y = static_cast<size_t>(width) * height;
    size_t size_uv = static_cast<size_t>(width + width % 2) * ((height + 1) / 2);

    if (fread(buffer->MutableDataY(), 1, size_y, f) < size_y)
    {
        return nullptr;
    }
    if (fread(buffer->MutableDataUV(), 1, size_uv, f) < size_uv)
    {
        return nullptr;
    }
    return buffer;
}
} // namespace utils
OCTK_END_NAMESPACE
