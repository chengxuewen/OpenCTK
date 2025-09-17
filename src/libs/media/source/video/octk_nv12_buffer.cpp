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

#include <octk_nv12_buffer.hpp>
#include <octk_i420_buffer.hpp>
#include <octk_checks.hpp>

#include <libyuv.h>

#include <cstddef>
#include <cstdint>
#include <cstring>

OCTK_BEGIN_NAMESPACE

namespace
{

static const int kBufferAlignment = 64;

int NV12DataSize(int height, int stride_y, int stride_uv)
{
    return stride_y * height + stride_uv * ((height + 1) / 2);
}
}  // namespace

NV12Buffer::NV12Buffer(int width, int height)
    : NV12Buffer(width, height, width, width + width % 2) {}

NV12Buffer::NV12Buffer(int width, int height, int stride_y, int stride_uv)
    : width_(width), height_(height), stride_y_(stride_y), stride_uv_(stride_uv)
    , data_(static_cast<uint8_t *>(utils::alignedMalloc(NV12DataSize(height_, stride_y_, stride_uv),
                                                        kBufferAlignment)))
{
    OCTK_DCHECK_GT(width, 0);
    OCTK_DCHECK_GT(height, 0);
    OCTK_DCHECK_GE(stride_y, width);
    OCTK_DCHECK_GE(stride_uv, (width + width % 2));
}

NV12Buffer::~NV12Buffer() = default;

// static
std::shared_ptr<NV12Buffer> NV12Buffer::create(int width, int height)
{
    return std::make_shared<NV12Buffer>(width, height);
}

// static
std::shared_ptr<NV12Buffer> NV12Buffer::create(int width,
                                               int height,
                                               int stride_y,
                                               int stride_uv)
{
    return std::make_shared<NV12Buffer>(width, height, stride_y, stride_uv);
}

// static
std::shared_ptr<NV12Buffer> NV12Buffer::Copy(const I420BufferInterface &i420_buffer)
{
    std::shared_ptr<NV12Buffer> buffer = NV12Buffer::create(i420_buffer.width(), i420_buffer.height());
    libyuv::I420ToNV12(i420_buffer.dataY(), i420_buffer.strideY(),
                       i420_buffer.dataU(), i420_buffer.strideU(),
                       i420_buffer.dataV(), i420_buffer.strideV(),
                       buffer->MutableDataY(), buffer->strideY(),
                       buffer->MutableDataUV(), buffer->strideUV(),
                       buffer->width(), buffer->height());
    return buffer;
}

std::shared_ptr<I420BufferInterface> NV12Buffer::toI420()
{
    std::shared_ptr<I420Buffer> i420_buffer = I420Buffer::create(width(), height());
    libyuv::NV12ToI420(dataY(), strideY(),
                       dataUV(), strideUV(),
                       i420_buffer->MutableDataY(), i420_buffer->strideY(),
                       i420_buffer->MutableDataU(), i420_buffer->strideU(),
                       i420_buffer->MutableDataV(), i420_buffer->strideV(),
                       width(), height());
    return i420_buffer;
}

int NV12Buffer::width() const
{
    return width_;
}
int NV12Buffer::height() const
{
    return height_;
}

int NV12Buffer::strideY() const
{
    return stride_y_;
}
int NV12Buffer::strideUV() const
{
    return stride_uv_;
}

const uint8_t *NV12Buffer::dataY() const
{
    return data_.get();
}

const uint8_t *NV12Buffer::dataUV() const
{
    return data_.get() + UVOffset();
}

uint8_t *NV12Buffer::MutableDataY()
{
    return data_.get();
}

uint8_t *NV12Buffer::MutableDataUV()
{
    return data_.get() + UVOffset();
}

size_t NV12Buffer::UVOffset() const
{
    return stride_y_ * height_;
}

void NV12Buffer::InitializeData()
{
    memset(data_.get(), 0, NV12DataSize(height_, stride_y_, stride_uv_));
}

void NV12Buffer::cropAndScaleFrom(const NV12BufferInterface &src,
                                  int offsetX,
                                  int offsetY,
                                  int cropWidth,
                                  int cropHeight)
{
    OCTK_CHECK_LE(cropWidth, src.width());
    OCTK_CHECK_LE(cropHeight, src.height());
    OCTK_CHECK_LE(cropWidth + offsetX, src.width());
    OCTK_CHECK_LE(cropHeight + offsetY, src.height());
    OCTK_CHECK_GE(offsetX, 0);
    OCTK_CHECK_GE(offsetY, 0);

    // Make sure offset is even so that u/v plane becomes aligned.
    const int uv_offset_x = offsetX / 2;
    const int uv_offset_y = offsetY / 2;
    offsetX = uv_offset_x * 2;
    offsetY = uv_offset_y * 2;

    const uint8_t *yPlane = src.dataY() + src.strideY() * offsetY + offsetX;
    const uint8_t *uv_plane = src.dataUV() + src.strideUV() * uv_offset_y + uv_offset_x * 2;

    int res = libyuv::NV12Scale(yPlane, src.strideY(),
                                uv_plane, src.strideUV(),
                                cropWidth, cropHeight,
                                MutableDataY(), strideY(),
                                MutableDataUV(), strideUV(),
                                width(), height(),
                                libyuv::kFilterBox);

    OCTK_DCHECK_EQ(res, 0);
}

OCTK_END_NAMESPACE