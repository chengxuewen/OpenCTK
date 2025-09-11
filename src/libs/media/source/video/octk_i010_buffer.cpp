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

#include <octk_i010_buffer.hpp>
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

int I010DataSize(int height, int stride_y, int stride_u, int stride_v)
{
    return kBytesPerPixel *
           (stride_y * height + (stride_u + stride_v) * ((height + 1) / 2));
}
}  // namespace

I010Buffer::I010Buffer(int width,
                       int height,
                       int stride_y,
                       int stride_u,
                       int stride_v)
    : width_(width), height_(height), stride_y_(stride_y), stride_u_(stride_u), stride_v_(stride_v)
    , data_(static_cast<uint16_t *>(utils::alignedMalloc(I010DataSize(height, stride_y, stride_u, stride_v),
                                                         kBufferAlignment)))
{
    OCTK_DCHECK_GT(width, 0);
    OCTK_DCHECK_GT(height, 0);
    OCTK_DCHECK_GE(stride_y, width);
    OCTK_DCHECK_GE(stride_u, (width + 1) / 2);
    OCTK_DCHECK_GE(stride_v, (width + 1) / 2);
}

I010Buffer::~I010Buffer() {}

// static
std::shared_ptr<I010Buffer> I010Buffer::Create(int width, int height)
{
    return std::make_shared<I010Buffer>(width, height, width, (width + 1) / 2, (width + 1) / 2);
}

// static
std::shared_ptr<I010Buffer> I010Buffer::Copy(const I010BufferInterface &source)
{
    const int width = source.width();
    const int height = source.height();
    std::shared_ptr<I010Buffer> buffer = Create(width, height);
    int res = libyuv::I010Copy(source.DataY(), source.StrideY(),
                               source.DataU(), source.StrideU(),
                               source.DataV(), source.StrideV(),
                               buffer->MutableDataY(), buffer->StrideY(),
                               buffer->MutableDataU(), buffer->StrideU(),
                               buffer->MutableDataV(), buffer->StrideV(),
                               width, height);
    OCTK_DCHECK_EQ(res, 0);

    return buffer;
}

// static
std::shared_ptr<I010Buffer> I010Buffer::Copy(const I420BufferInterface &source)
{
    const int width = source.width();
    const int height = source.height();
    std::shared_ptr<I010Buffer> buffer = Create(width, height);
    int res = libyuv::I420ToI010(source.DataY(), source.StrideY(),
                                 source.DataU(), source.StrideU(),
                                 source.DataV(), source.StrideV(),
                                 buffer->MutableDataY(), buffer->StrideY(),
                                 buffer->MutableDataU(), buffer->StrideU(),
                                 buffer->MutableDataV(), buffer->StrideV(),
                                 width, height);
    OCTK_DCHECK_EQ(res, 0);

    return buffer;
}

// static
std::shared_ptr<I010Buffer> I010Buffer::Rotate(const I010BufferInterface &src, VideoRotation rotation)
{
    if (rotation == VideoRotation::Angle0)
    {
        return Copy(src);
    }

    OCTK_CHECK(src.DataY());
    OCTK_CHECK(src.DataU());
    OCTK_CHECK(src.DataV());
    int rotated_width = src.width();
    int rotated_height = src.height();
    if (rotation == VideoRotation::Angle90 || rotation == VideoRotation::Angle270)
    {
        std::swap(rotated_width, rotated_height);
    }

    std::shared_ptr<I010Buffer> buffer = Create(rotated_width, rotated_height);

    int res = libyuv::I010Rotate(src.DataY(), src.StrideY(),
                                 src.DataU(), src.StrideU(),
                                 src.DataV(), src.StrideV(),
                                 buffer->MutableDataY(), buffer->StrideY(),
                                 buffer->MutableDataU(), buffer->StrideU(),
                                 buffer->MutableDataV(), buffer->StrideV(),
                                 src.width(), src.height(),
                                 static_cast<libyuv::RotationMode>(rotation));
    OCTK_DCHECK_EQ(res, 0);

    return buffer;
}

std::shared_ptr<I420BufferInterface> I010Buffer::ToI420()
{
    std::shared_ptr<I420Buffer> i420_buffer = I420Buffer::Create(width(), height());
    int res = libyuv::I010ToI420(DataY(), StrideY(),
                                 DataU(), StrideU(),
                                 DataV(), StrideV(),
                                 i420_buffer->MutableDataY(), i420_buffer->StrideY(),
                                 i420_buffer->MutableDataU(), i420_buffer->StrideU(),
                                 i420_buffer->MutableDataV(), i420_buffer->StrideV(),
                                 width(), height());
    OCTK_DCHECK_EQ(res, 0);

    return i420_buffer;
}

int I010Buffer::width() const
{
    return width_;
}

int I010Buffer::height() const
{
    return height_;
}

const uint16_t *I010Buffer::DataY() const
{
    return data_.get();
}
const uint16_t *I010Buffer::DataU() const
{
    return data_.get() + stride_y_ * height_;
}
const uint16_t *I010Buffer::DataV() const
{
    return data_.get() + stride_y_ * height_ + stride_u_ * ((height_ + 1) / 2);
}

int I010Buffer::StrideY() const
{
    return stride_y_;
}
int I010Buffer::StrideU() const
{
    return stride_u_;
}
int I010Buffer::StrideV() const
{
    return stride_v_;
}

uint16_t *I010Buffer::MutableDataY()
{
    return const_cast<uint16_t *>(DataY());
}
uint16_t *I010Buffer::MutableDataU()
{
    return const_cast<uint16_t *>(DataU());
}
uint16_t *I010Buffer::MutableDataV()
{
    return const_cast<uint16_t *>(DataV());
}

void I010Buffer::cropAndScaleFrom(const I010BufferInterface &src,
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

    const uint16_t *y_plane = src.DataY() + src.StrideY() * offsetY + offsetX;
    const uint16_t *u_plane = src.DataU() + src.StrideU() * uv_offset_y + uv_offset_x;
    const uint16_t *v_plane = src.DataV() + src.StrideV() * uv_offset_y + uv_offset_x;
    int res = libyuv::I420Scale_16(y_plane, src.StrideY(),
                                   u_plane, src.StrideU(),
                                   v_plane, src.StrideV(),
                                   cropWidth, cropHeight,
                                   MutableDataY(), StrideY(),
                                   MutableDataU(), StrideU(),
                                   MutableDataV(), StrideV(),
                                   width(), height(),
                                   libyuv::kFilterBox);

    OCTK_DCHECK_EQ(res, 0);
}

void I010Buffer::scaleFrom(const I010BufferInterface &src)
{
    cropAndScaleFrom(src, 0, 0, src.width(), src.height());
}
OCTK_END_NAMESPACE
