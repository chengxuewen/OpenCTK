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

#ifndef _RGBA_BUFFER_HPP
#define _RGBA_BUFFER_HPP

#include <octk_video_frame_buffer.hpp>
#include <octk_aligned_malloc.hpp>

#include <cstddef>
#include <cstdint>
#include <memory>

OCTK_BEGIN_NAMESPACE

class OCTK_MEDIA_API RGBABuffer : public RGBABufferInterface
{
public:
    RGBABuffer(int width, int height);
    ~RGBABuffer() override;

    static std::shared_ptr<RGBABuffer> create(int width, int height);
    static std::shared_ptr<RGBABuffer> copy(const I420BufferInterface &i420Buffer);
    static std::shared_ptr<RGBABuffer> copy(const RGBABufferInterface &rgbaBuffer);

    std::shared_ptr<I420BufferInterface> toI420() override;
    std::shared_ptr<RGBABufferInterface> toRGBA() override;

    void InitializeData();

    int width() const override;
    int height() const override;

    int stride() const override;

    uint8_t *mutableData();
    const uint8_t *data() const override;

    // Scale the cropped area of `src` to the size of `this` buffer, and write the result into `this`.
    void cropAndScaleFrom(const RGBABufferInterface &src, int offsetX, int offsetY, int cropWidth, int cropHeight);

private:
    const int mWidth;
    const int mHeight;
    const std::unique_ptr<uint8_t, AlignedFreeDeleter> mData;
};

OCTK_END_NAMESPACE

#endif // _RGBA_BUFFER_HPP
