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

#include <octk_assert.hpp>
#include <octk_i420_buffer.hpp>
#include <octk_i444_buffer.hpp>

#include <libyuv.h>

#include <algorithm>
#include <cstdint>
#include <utility>

// Aligning pointer to 64 bytes for improved performance, e.g. use SIMD.
static const int kBufferAlignment = 64;

OCTK_BEGIN_NAMESPACE

namespace
{

int I444DataSize(int height, int stride_y, int stride_u, int stride_v)
{
    return stride_y * height + stride_u * height + stride_v * height;
}
} // namespace

I444Buffer::I444Buffer(int width, int height) : I444Buffer(width, height, width, (width), (width)) {}

I444Buffer::I444Buffer(int width, int height, int stride_y, int stride_u, int stride_v)
    : width_(width)
    , height_(height)
    , stride_y_(stride_y)
    , stride_u_(stride_u)
    , stride_v_(stride_v)
    , data_(static_cast<uint8_t *>(utils::alignedMalloc(I444DataSize(height, stride_y, stride_u, stride_v),
                                                        kBufferAlignment)))
{
    OCTK_DCHECK_GT(width, 0);
    OCTK_DCHECK_GT(height, 0);
    OCTK_DCHECK_GE(stride_y, width);
    OCTK_DCHECK_GE(stride_u, (width));
    OCTK_DCHECK_GE(stride_v, (width));
}

I444Buffer::~I444Buffer() {}

// static
std::shared_ptr<I444Buffer> I444Buffer::Create(int width, int height)
{
    return std::make_shared<I444Buffer>(width, height);
}

// static
std::shared_ptr<I444Buffer> I444Buffer::Create(int width, int height, int stride_y, int stride_u, int stride_v)
{
    return std::make_shared<I444Buffer>(width, height, stride_y, stride_u, stride_v);
}

// static
std::shared_ptr<I444Buffer> I444Buffer::Copy(const I444BufferInterface &source)
{
    return Copy(source.width(), source.height(), source.DataY(), source.StrideY(), source.DataU(), source.StrideU(),
                source.DataV(), source.StrideV());
}

// static
std::shared_ptr<I444Buffer> I444Buffer::Copy(int width, int height, const uint8_t *data_y, int stride_y,
                                             const uint8_t *data_u, int stride_u, const uint8_t *data_v, int stride_v)
{
    // Note: May use different strides than the input data.
    std::shared_ptr<I444Buffer> buffer = Create(width, height);
    OCTK_CHECK_EQ(0,
                  libyuv::I444Copy(data_y, stride_y, data_u, stride_u, data_v, stride_v, buffer->MutableDataY(),
                                   buffer->StrideY(), buffer->MutableDataU(), buffer->StrideU(),
                                   buffer->MutableDataV(), buffer->StrideV(), width, height));
    return buffer;
}

// static
std::shared_ptr<I444Buffer> I444Buffer::Rotate(const I444BufferInterface &src, VideoRotation rotation)
{
    OCTK_CHECK(src.DataY());
    OCTK_CHECK(src.DataU());
    OCTK_CHECK(src.DataV());

    int rotated_width = src.width();
    int rotated_height = src.height();
    if (rotation == VideoRotation::Angle90 || rotation == VideoRotation::Angle270)
    {
        std::swap(rotated_width, rotated_height);
    }

    std::shared_ptr<I444Buffer> buffer = I444Buffer::Create(rotated_width, rotated_height);

    OCTK_CHECK_EQ(0,
                  libyuv::I444Rotate(src.DataY(), src.StrideY(), src.DataU(), src.StrideU(), src.DataV(),
                                     src.StrideV(), buffer->MutableDataY(), buffer->StrideY(), buffer->MutableDataU(),
                                     buffer->StrideU(), buffer->MutableDataV(), buffer->StrideV(), src.width(),
                                     src.height(), static_cast<libyuv::RotationMode>(rotation)));

    return buffer;
}

std::shared_ptr<I420BufferInterface> I444Buffer::ToI420()
{
    std::shared_ptr<I420Buffer> i420_buffer = I420Buffer::Create(width(), height());
    libyuv::I444ToI420(DataY(), StrideY(), DataU(), StrideU(), DataV(), StrideV(), i420_buffer->MutableDataY(),
                       i420_buffer->StrideY(), i420_buffer->MutableDataU(), i420_buffer->StrideU(),
                       i420_buffer->MutableDataV(), i420_buffer->StrideV(), width(), height());
    return i420_buffer;
}

void I444Buffer::InitializeData() { memset(data_.get(), 0, I444DataSize(height_, stride_y_, stride_u_, stride_v_)); }

int I444Buffer::width() const { return width_; }

int I444Buffer::height() const { return height_; }

const uint8_t *I444Buffer::DataY() const { return data_.get(); }
const uint8_t *I444Buffer::DataU() const { return data_.get() + stride_y_ * height_; }
const uint8_t *I444Buffer::DataV() const { return data_.get() + stride_y_ * height_ + stride_u_ * ((height_)); }

int I444Buffer::StrideY() const { return stride_y_; }
int I444Buffer::StrideU() const { return stride_u_; }
int I444Buffer::StrideV() const { return stride_v_; }

uint8_t *I444Buffer::MutableDataY() { return const_cast<uint8_t *>(DataY()); }
uint8_t *I444Buffer::MutableDataU() { return const_cast<uint8_t *>(DataU()); }
uint8_t *I444Buffer::MutableDataV() { return const_cast<uint8_t *>(DataV()); }

void I444Buffer::cropAndScaleFrom(const I444BufferInterface &src, int offsetX, int offsetY, int cropWidth,
                                  int cropHeight)
{
    OCTK_CHECK_LE(cropWidth, src.width());
    OCTK_CHECK_LE(cropHeight, src.height());
    OCTK_CHECK_LE(cropWidth + offsetX, src.width());
    OCTK_CHECK_LE(cropHeight + offsetY, src.height());
    OCTK_CHECK_GE(offsetX, 0);
    OCTK_CHECK_GE(offsetY, 0);

    const uint8_t *y_plane = src.DataY() + src.StrideY() * offsetY + offsetX;
    const uint8_t *u_plane = src.DataU() + src.StrideU() * offsetY + offsetX;
    const uint8_t *v_plane = src.DataV() + src.StrideV() * offsetY + offsetX;
    int res = libyuv::I444Scale(y_plane, src.StrideY(), u_plane, src.StrideU(), v_plane, src.StrideV(), cropWidth,
                                cropHeight, MutableDataY(), StrideY(), MutableDataU(), StrideU(), MutableDataV(),
                                StrideV(), width(), height(), libyuv::kFilterBox);

    OCTK_DCHECK_EQ(res, 0);
}
OCTK_END_NAMESPACE
