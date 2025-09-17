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

#ifndef _OCTK_I444_BUFFER_HPP
#define _OCTK_I444_BUFFER_HPP

#include <octk_video_frame_buffer.hpp>
#include <octk_aligned_malloc.hpp>
#include <octk_video_rotation.hpp>

#include <stdint.h>
#include <memory>

OCTK_BEGIN_NAMESPACE

// Plain I444 buffer in standard memory.
// I444 represents an image with in YUV format withouth any chroma subsampling.
// https://en.wikipedia.org/wiki/Chroma_subsampling#4:4:4
class OCTK_MEDIA_API I444Buffer : public I444BufferInterface
{
public:
    I444Buffer(int width, int height);
    I444Buffer(int width, int height, int stride_y, int stride_u, int stride_v);
    ~I444Buffer() override;

    static std::shared_ptr<I444Buffer> Create(int width, int height);
    static std::shared_ptr<I444Buffer> Create(int width,
                                              int height,
                                              int stride_y,
                                              int stride_u,
                                              int stride_v);

    // Create a new buffer and copy the pixel data.
    static std::shared_ptr<I444Buffer> Copy(const I444BufferInterface &buffer);

    static std::shared_ptr<I444Buffer> Copy(int width,
                                            int height,
                                            const uint8_t *data_y,
                                            int stride_y,
                                            const uint8_t *data_u,
                                            int stride_u,
                                            const uint8_t *data_v,
                                            int stride_v);

    // Returns a rotated copy of |src|.
    static std::shared_ptr<I444Buffer> Rotate(const I444BufferInterface &src,
                                              VideoRotation rotation);

    std::shared_ptr<I420BufferInterface> toI420() final;
    const I420BufferInterface *getI420() const final { return nullptr; }

    // Sets all three planes to all zeros. Used to work around for
    // quirks in memory checkers
    // (https://bugs.chromium.org/p/libyuv/issues/detail?id=377) and
    // ffmpeg (http://crbug.com/390941).
    // TODO(https://crbug.com/390941): Deprecated. Should be deleted if/when those
    // issues are resolved in a better way. Or in the mean time, use SetBlack.
    void InitializeData();

    int width() const override;
    int height() const override;
    const uint8_t *dataY() const override;
    const uint8_t *dataU() const override;
    const uint8_t *dataV() const override;

    int strideY() const override;
    int strideU() const override;
    int strideV() const override;

    uint8_t *MutableDataY();
    uint8_t *MutableDataU();
    uint8_t *MutableDataV();

    // Scale the cropped area of |src| to the size of |this| buffer, and
    // write the result into |this|.
    void cropAndScaleFrom(const I444BufferInterface &src,
                          int offsetX,
                          int offsetY,
                          int cropWidth,
                          int cropHeight);

private:
    const int width_;
    const int height_;
    const int stride_y_;
    const int stride_u_;
    const int stride_v_;
    const std::unique_ptr<uint8_t, AlignedFreeDeleter> data_;
};

OCTK_END_NAMESPACE

#endif  // _OCTK_I444_BUFFER_HPP
