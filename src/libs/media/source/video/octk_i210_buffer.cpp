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

#include <octk_i210_buffer.hpp>
#include <octk_i422_buffer.hpp>
#include <octk_i420_buffer.hpp>
#include <octk_checks.hpp>

#include <libyuv.h>

#include <cstdint>
#include <utility>

// Aligning pointer to 64 bytes for improved performance, e.g. use SIMD.
static const int kBufferAlignment = 64;
static const int kBytesPerPixel = 2;

OCTK_BEGIN_NAMESPACE

namespace
{

int I210DataSize(int height, int stride_y, int stride_u, int stride_v)
{
    return kBytesPerPixel * (stride_y * height + stride_u * height + stride_v * height);
}
}  // namespace

I210Buffer::I210Buffer(int width,
                       int height,
                       int stride_y,
                       int stride_u,
                       int stride_v)
    : width_(width), height_(height), stride_y_(stride_y), stride_u_(stride_u), stride_v_(stride_v)
    , data_(static_cast<uint16_t *>(utils::alignedMalloc(I210DataSize(height, stride_y, stride_u, stride_v),
                                                         kBufferAlignment)))
{
    OCTK_DCHECK_GT(width, 0);
    OCTK_DCHECK_GT(height, 0);
    OCTK_DCHECK_GE(stride_y, width);
    OCTK_DCHECK_GE(stride_u, (width + 1) / 2);
    OCTK_DCHECK_GE(stride_v, (width + 1) / 2);
}

I210Buffer::~I210Buffer() {}

// static
std::shared_ptr<I210Buffer> I210Buffer::Create(int width, int height)
{
    return std::make_shared<I210Buffer>(width, height, width, (width + 1) / 2, (width + 1) / 2);
}

// static
std::shared_ptr<I210Buffer> I210Buffer::Copy(const I210BufferInterface &source)
{
    const int width = source.width();
    const int height = source.height();
    std::shared_ptr<I210Buffer> buffer = Create(width, height);
    OCTK_CHECK_EQ(0, libyuv::I210Copy(source.dataY(), source.strideY(),
                                      source.dataU(), source.strideU(),
                                      source.dataV(), source.strideV(),
                                      buffer->MutableDataY(), buffer->strideY(),
                                      buffer->MutableDataU(), buffer->strideU(),
                                      buffer->MutableDataV(), buffer->strideV(),
                                      width, height));
    return buffer;
}

// static
std::shared_ptr<I210Buffer> I210Buffer::Copy(const I420BufferInterface &source)
{
    const int width = source.width();
    const int height = source.height();
    auto i422buffer = I422Buffer::Copy(source);
    std::shared_ptr<I210Buffer> buffer = Create(width, height);
    OCTK_CHECK_EQ(0, libyuv::I422ToI210(i422buffer->dataY(), i422buffer->strideY(),
                                        i422buffer->dataU(), i422buffer->strideU(),
                                        i422buffer->dataV(), i422buffer->strideV(),
                                        buffer->MutableDataY(), buffer->strideY(),
                                        buffer->MutableDataU(), buffer->strideU(),
                                        buffer->MutableDataV(), buffer->strideV(),
                                        width, height));
    return buffer;
}

// static
std::shared_ptr<I210Buffer> I210Buffer::Rotate(const I210BufferInterface &src, VideoRotation rotation)
{
    OCTK_CHECK(src.dataY());
    OCTK_CHECK(src.dataU());
    OCTK_CHECK(src.dataV());

    int rotated_width = src.width();
    int rotated_height = src.height();
    if (rotation == VideoRotation::Angle90 || rotation == VideoRotation::Angle270)
    {
        std::swap(rotated_width, rotated_height);
    }

    std::shared_ptr<I210Buffer> buffer = I210Buffer::Create(rotated_width, rotated_height);

    OCTK_CHECK_EQ(0, libyuv::I210Rotate(src.dataY(), src.strideY(),
                                        src.dataU(), src.strideU(),
                                        src.dataV(), src.strideV(),
                                        buffer->MutableDataY(), buffer->strideY(),
                                        buffer->MutableDataU(), buffer->strideU(),
                                        buffer->MutableDataV(), buffer->strideV(),
                                        src.width(), src.height(),
                                        static_cast<libyuv::RotationMode>(rotation)));

    return buffer;
}

std::shared_ptr<I420BufferInterface> I210Buffer::toI420()
{
    std::shared_ptr<I420Buffer> i420_buffer = I420Buffer::create(width(), height());
    libyuv::I210ToI420(dataY(), strideY(),
                       dataU(), strideU(),
                       dataV(), strideV(),
                       i420_buffer->MutableDataY(), i420_buffer->strideY(),
                       i420_buffer->MutableDataU(), i420_buffer->strideU(),
                       i420_buffer->MutableDataV(), i420_buffer->strideV(),
                       width(), height());
    return i420_buffer;
}

int I210Buffer::width() const
{
    return width_;
}

int I210Buffer::height() const
{
    return height_;
}

const uint16_t *I210Buffer::dataY() const
{
    return data_.get();
}
const uint16_t *I210Buffer::dataU() const
{
    return data_.get() + stride_y_ * height_;
}
const uint16_t *I210Buffer::dataV() const
{
    return data_.get() + stride_y_ * height_ + stride_u_ * height_;
}

int I210Buffer::strideY() const
{
    return stride_y_;
}
int I210Buffer::strideU() const
{
    return stride_u_;
}
int I210Buffer::strideV() const
{
    return stride_v_;
}

uint16_t *I210Buffer::MutableDataY()
{
    return const_cast<uint16_t *>(dataY());
}
uint16_t *I210Buffer::MutableDataU()
{
    return const_cast<uint16_t *>(dataU());
}
uint16_t *I210Buffer::MutableDataV()
{
    return const_cast<uint16_t *>(dataV());
}

void I210Buffer::cropAndScaleFrom(const I210BufferInterface &src,
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
    OCTK_CHECK_GE(cropWidth, 0);
    OCTK_CHECK_GE(cropHeight, 0);

    // Make sure offset is even so that u/v plane becomes aligned.
    const int uv_offset_x = offsetX / 2;
    const int uv_offset_y = offsetY;
    offsetX = uv_offset_x * 2;

    const uint16_t *yPlane = src.dataY() + src.strideY() * offsetY + offsetX;
    const uint16_t *uPlane = src.dataU() + src.strideU() * uv_offset_y + uv_offset_x;
    const uint16_t *vPlane = src.dataV() + src.strideV() * uv_offset_y + uv_offset_x;
    int res = libyuv::I422Scale_16(yPlane, src.strideY(),
                                   uPlane, src.strideU(),
                                   vPlane, src.strideV(),
                                   cropWidth, cropHeight,
                                   MutableDataY(), strideY(),
                                   MutableDataU(), strideU(),
                                   MutableDataV(), strideV(),
                                   width(), height(),
                                   libyuv::kFilterBox);

    OCTK_DCHECK_EQ(res, 0);
}

void I210Buffer::scaleFrom(const I210BufferInterface &src)
{
    cropAndScaleFrom(src, 0, 0, src.width(), src.height());
}
OCTK_END_NAMESPACE