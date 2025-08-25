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

#ifndef _OCTK_NV12_BUFFER_HPP
#define _OCTK_NV12_BUFFER_HPP

#include <octk_video_frame_buffer.hpp>
#include <octk_aligned_malloc.hpp>

#include <cstddef>
#include <cstdint>
#include <memory>

OCTK_BEGIN_NAMESPACE

// NV12 is a biplanar encoding format, with full-resolution Y and
// half-resolution interleved UV. More information can be found at
// http://msdn.microsoft.com/library/windows/desktop/dd206750.aspx#nv12.
class OCTK_MEDIA_API NV12Buffer : public NV12BufferInterface
{
public:
    NV12Buffer(int width, int height);
    NV12Buffer(int width, int height, int stride_y, int stride_uv);
    ~NV12Buffer() override;

    static std::shared_ptr<NV12Buffer> Create(int width, int height);
    static std::shared_ptr<NV12Buffer> Create(int width,
                                              int height,
                                              int stride_y,
                                              int stride_uv);
    static std::shared_ptr<NV12Buffer> Copy(const I420BufferInterface &i420_buffer);

    std::shared_ptr<I420BufferInterface> ToI420() override;

    int width() const override;
    int height() const override;

    int StrideY() const override;
    int StrideUV() const override;

    const uint8_t *DataY() const override;
    const uint8_t *DataUV() const override;

    uint8_t *MutableDataY();
    uint8_t *MutableDataUV();

    // Sets all three planes to all zeros. Used to work around for
    // quirks in memory checkers
    // (https://bugs.chromium.org/p/libyuv/issues/detail?id=377) and
    // ffmpeg (http://crbug.com/390941).
    // TODO(https://crbug.com/390941): Deprecated. Should be deleted if/when those
    // issues are resolved in a better way. Or in the mean time, use SetBlack.
    void InitializeData();

    // Scale the cropped area of `src` to the size of `this` buffer, and
    // write the result into `this`.
    void CropAndScaleFrom(const NV12BufferInterface &src,
                          int offsetX,
                          int offsetY,
                          int cropWidth,
                          int cropHeight);

private:
    size_t UVOffset() const;

    const int width_;
    const int height_;
    const int stride_y_;
    const int stride_uv_;
    const std::unique_ptr<uint8_t, AlignedFreeDeleter> data_;
};

OCTK_END_NAMESPACE

#endif  // _OCTK_NV12_BUFFER_HPP
