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

#include <octk_i422_buffer.hpp>
#include <octk_i420_buffer.hpp>
#include <octk_checks.hpp>

#include <libyuv.h>

#include <algorithm>
#include <cstdint>
#include <utility>

// Aligning pointer to 64 bytes for improved performance, e.g. use SIMD.
static const int kBufferAlignment = 64;

OCTK_BEGIN_NAMESPACE

namespace
{

int I422DataSize(int height, int stride_y, int stride_u, int stride_v)
{
    return stride_y * height + stride_u * height + stride_v * height;
}
}  // namespace

I422Buffer::I422Buffer(int width, int height)
    : I422Buffer(width, height, width, (width + 1) / 2, (width + 1) / 2) {}

I422Buffer::I422Buffer(int width,
                       int height,
                       int stride_y,
                       int stride_u,
                       int stride_v)
    : width_(width), height_(height), stride_y_(stride_y), stride_u_(stride_u), stride_v_(stride_v)
    , data_(static_cast<uint8_t *>(utils::alignedMalloc(I422DataSize(height, stride_y, stride_u, stride_v),
                                                        kBufferAlignment)))
{
    OCTK_DCHECK_GT(width, 0);
    OCTK_DCHECK_GT(height, 0);
    OCTK_DCHECK_GE(stride_y, width);
    OCTK_DCHECK_GE(stride_u, (width + 1) / 2);
    OCTK_DCHECK_GE(stride_v, (width + 1) / 2);
}

I422Buffer::~I422Buffer() {}

// static
std::shared_ptr<I422Buffer> I422Buffer::create(int width, int height)
{
    return std::make_shared<I422Buffer>(width, height);
}

// static
std::shared_ptr<I422Buffer> I422Buffer::create(int width,
                                               int height,
                                               int stride_y,
                                               int stride_u,
                                               int stride_v)
{
    return std::make_shared<I422Buffer>(width, height, stride_y, stride_u, stride_v);
}

// static
std::shared_ptr<I422Buffer> I422Buffer::Copy(const I422BufferInterface &source)
{
    return Copy(source.width(), source.height(),
                source.dataY(), source.strideY(),
                source.dataU(), source.strideU(),
                source.dataV(), source.strideV());
}

// static
std::shared_ptr<I422Buffer> I422Buffer::Copy(const I420BufferInterface &source)
{
    const int width = source.width();
    const int height = source.height();
    std::shared_ptr<I422Buffer> buffer = create(width, height);
    int res = libyuv::I420ToI422(source.dataY(), source.strideY(),
                                 source.dataU(), source.strideU(),
                                 source.dataV(), source.strideV(),
                                 buffer->MutableDataY(), buffer->strideY(),
                                 buffer->MutableDataU(), buffer->strideU(),
                                 buffer->MutableDataV(), buffer->strideV(),
                                 width, height);
    OCTK_DCHECK_EQ(res, 0);

    return buffer;
}

// static
std::shared_ptr<I422Buffer> I422Buffer::Copy(int width,
                                             int height,
                                             const uint8_t *data_y,
                                             int stride_y,
                                             const uint8_t *data_u,
                                             int stride_u,
                                             const uint8_t *data_v,
                                             int stride_v)
{
    // Note: May use different strides than the input data.
    std::shared_ptr<I422Buffer> buffer = create(width, height);
    int res = libyuv::I422Copy(data_y, stride_y,
                               data_u, stride_u,
                               data_v, stride_v,
                               buffer->MutableDataY(), buffer->strideY(),
                               buffer->MutableDataU(), buffer->strideU(),
                               buffer->MutableDataV(), buffer->strideV(),
                               width, height);
    OCTK_DCHECK_EQ(res, 0);

    return buffer;
}

// static
std::shared_ptr<I422Buffer> I422Buffer::Rotate(const I422BufferInterface &src, VideoRotation rotation)
{
    OCTK_CHECK(src.dataY());
    OCTK_CHECK(src.dataU());
    OCTK_CHECK(src.dataV());

    int rotated_width = src.width();
    int rotated_height = src.height();
    if (rotation == VideoRotation::kAngle90 || rotation == VideoRotation::kAngle270)
    {
        std::swap(rotated_width, rotated_height);
    }

    std::shared_ptr<I422Buffer> buffer = I422Buffer::create(rotated_width, rotated_height);

    int res = libyuv::I422Rotate(src.dataY(), src.strideY(),
                                 src.dataU(), src.strideU(),
                                 src.dataV(), src.strideV(),
                                 buffer->MutableDataY(), buffer->strideY(),
                                 buffer->MutableDataU(), buffer->strideU(),
                                 buffer->MutableDataV(), buffer->strideV(),
                                 src.width(), src.height(),
                                 static_cast<libyuv::RotationMode>(rotation));
    OCTK_DCHECK_EQ(res, 0);
    return buffer;
}

std::shared_ptr<I420BufferInterface> I422Buffer::toI420()
{
    std::shared_ptr<I420Buffer> i420_buffer = I420Buffer::create(width(), height());
    int res = libyuv::I422ToI420(dataY(), strideY(),
                                 dataU(), strideU(),
                                 dataV(), strideV(),
                                 i420_buffer->MutableDataY(), i420_buffer->strideY(),
                                 i420_buffer->MutableDataU(), i420_buffer->strideU(),
                                 i420_buffer->MutableDataV(), i420_buffer->strideV(),
                                 width(), height());
    OCTK_DCHECK_EQ(res, 0);

    return i420_buffer;
}

void I422Buffer::InitializeData()
{
    memset(data_.get(), 0,
           I422DataSize(height_, stride_y_, stride_u_, stride_v_));
}

int I422Buffer::width() const
{
    return width_;
}

int I422Buffer::height() const
{
    return height_;
}

const uint8_t *I422Buffer::dataY() const
{
    return data_.get();
}
const uint8_t *I422Buffer::dataU() const
{
    return data_.get() + stride_y_ * height_;
}
const uint8_t *I422Buffer::dataV() const
{
    return data_.get() + stride_y_ * height_ + stride_u_ * height_;
}

int I422Buffer::strideY() const
{
    return stride_y_;
}
int I422Buffer::strideU() const
{
    return stride_u_;
}
int I422Buffer::strideV() const
{
    return stride_v_;
}

uint8_t *I422Buffer::MutableDataY()
{
    return const_cast<uint8_t *>(dataY());
}
uint8_t *I422Buffer::MutableDataU()
{
    return const_cast<uint8_t *>(dataU());
}
uint8_t *I422Buffer::MutableDataV()
{
    return const_cast<uint8_t *>(dataV());
}

void I422Buffer::cropAndScaleFrom(const I422BufferInterface &src,
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
    const int uv_offset_y = offsetY;
    offsetX = uv_offset_x * 2;

    const uint8_t *yPlane = src.dataY() + src.strideY() * offsetY + offsetX;
    const uint8_t *uPlane = src.dataU() + src.strideU() * uv_offset_y + uv_offset_x;
    const uint8_t *vPlane = src.dataV() + src.strideV() * uv_offset_y + uv_offset_x;

    int res = libyuv::I422Scale(yPlane, src.strideY(),
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
OCTK_END_NAMESPACE
