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

int I420DataSize(int height, int stride_y, int stride_u, int stride_v)
{
    return stride_y * height + (stride_u + stride_v) * ((height + 1) / 2);
}
} // namespace

I420Buffer::I420Buffer(int width, int height)
    : I420Buffer(width, height, width, (width + 1) / 2, (width + 1) / 2)
{
}

I420Buffer::I420Buffer(int width, int height, int stride_y, int stride_u, int stride_v)
    : width_(width)
    , height_(height)
    , stride_y_(stride_y)
    , stride_u_(stride_u)
    , stride_v_(stride_v)
    , data_(static_cast<uint8_t *>(
          utils::alignedMalloc(I420DataSize(height, stride_y, stride_u, stride_v), kBufferAlignment)))
{
    OCTK_DCHECK_GT(width, 0);
    OCTK_DCHECK_GT(height, 0);
    OCTK_DCHECK_GE(stride_y, width);
    OCTK_DCHECK_GE(stride_u, (width + 1) / 2);
    OCTK_DCHECK_GE(stride_v, (width + 1) / 2);
}

I420Buffer::~I420Buffer() { }

// static
std::shared_ptr<I420Buffer> I420Buffer::Create(int width, int height)
{
    return std::make_shared<I420Buffer>(width, height);
}

// static
std::shared_ptr<I420Buffer> I420Buffer::Create(int width, int height, int stride_y, int stride_u, int stride_v)
{
    return std::make_shared<I420Buffer>(width, height, stride_y, stride_u, stride_v);
}

// static
std::shared_ptr<I420Buffer> I420Buffer::Copy(const I420BufferInterface &source)
{
    return Copy(source.width(),
                source.height(),
                source.DataY(),
                source.StrideY(),
                source.DataU(),
                source.StrideU(),
                source.DataV(),
                source.StrideV());
}

// static
std::shared_ptr<I420Buffer> I420Buffer::Copy(int width,
                                             int height,
                                             const uint8_t *data_y,
                                             int stride_y,
                                             const uint8_t *data_u,
                                             int stride_u,
                                             const uint8_t *data_v,
                                             int stride_v)
{
    // Note: May use different strides than the input data.
    std::shared_ptr<I420Buffer> buffer = Create(width, height);
    OCTK_CHECK_EQ(0,
                  libyuv::I420Copy(data_y,
                                   stride_y,
                                   data_u,
                                   stride_u,
                                   data_v,
                                   stride_v,
                                   buffer->MutableDataY(),
                                   buffer->StrideY(),
                                   buffer->MutableDataU(),
                                   buffer->StrideU(),
                                   buffer->MutableDataV(),
                                   buffer->StrideV(),
                                   width,
                                   height));
    return buffer;
}

// static
std::shared_ptr<I420Buffer> I420Buffer::Rotate(const I420BufferInterface &src, VideoRotation rotation)
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

    std::shared_ptr<I420Buffer> buffer = I420Buffer::Create(rotated_width, rotated_height);

    OCTK_CHECK_EQ(0,
                  libyuv::I420Rotate(src.DataY(),
                                     src.StrideY(),
                                     src.DataU(),
                                     src.StrideU(),
                                     src.DataV(),
                                     src.StrideV(),
                                     buffer->MutableDataY(),
                                     buffer->StrideY(),
                                     buffer->MutableDataU(),
                                     buffer->StrideU(),
                                     buffer->MutableDataV(),
                                     buffer->StrideV(),
                                     src.width(),
                                     src.height(),
                                     static_cast<libyuv::RotationMode>(rotation)));

    return buffer;
}

void I420Buffer::InitializeData() { memset(data_.get(), 0, I420DataSize(height_, stride_y_, stride_u_, stride_v_)); }

std::shared_ptr<I420BufferInterface> I420Buffer::ToI420() { return I420Buffer::Copy(*this); }

int I420Buffer::width() const { return width_; }

int I420Buffer::height() const { return height_; }

const uint8_t *I420Buffer::DataY() const { return data_.get(); }
const uint8_t *I420Buffer::DataU() const { return data_.get() + stride_y_ * height_; }
const uint8_t *I420Buffer::DataV() const { return data_.get() + stride_y_ * height_ + stride_u_ * ((height_ + 1) / 2); }

int I420Buffer::StrideY() const { return stride_y_; }
int I420Buffer::StrideU() const { return stride_u_; }
int I420Buffer::StrideV() const { return stride_v_; }

uint8_t *I420Buffer::MutableDataY() { return const_cast<uint8_t *>(DataY()); }
uint8_t *I420Buffer::MutableDataU() { return const_cast<uint8_t *>(DataU()); }
uint8_t *I420Buffer::MutableDataV() { return const_cast<uint8_t *>(DataV()); }

// static
void I420Buffer::SetBlack(I420Buffer *buffer)
{
    OCTK_CHECK(libyuv::I420Rect(buffer->MutableDataY(),
                                buffer->StrideY(),
                                buffer->MutableDataU(),
                                buffer->StrideU(),
                                buffer->MutableDataV(),
                                buffer->StrideV(),
                                0,
                                0,
                                buffer->width(),
                                buffer->height(),
                                0,
                                128,
                                128) == 0);
}

void I420Buffer::cropAndScaleFrom(const I420BufferInterface &src,
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

    const uint8_t *y_plane = src.DataY() + src.StrideY() * offsetY + offsetX;
    const uint8_t *u_plane = src.DataU() + src.StrideU() * uv_offset_y + uv_offset_x;
    const uint8_t *v_plane = src.DataV() + src.StrideV() * uv_offset_y + uv_offset_x;
    int res = libyuv::I420Scale(y_plane,
                                src.StrideY(),
                                u_plane,
                                src.StrideU(),
                                v_plane,
                                src.StrideV(),
                                cropWidth,
                                cropHeight,
                                MutableDataY(),
                                StrideY(),
                                MutableDataU(),
                                StrideU(),
                                MutableDataV(),
                                StrideV(),
                                width(),
                                height(),
                                libyuv::kFilterBox);

    OCTK_DCHECK_EQ(res, 0);
}

void I420Buffer::cropAndScaleFrom(const I420BufferInterface &src)
{
    const auto width = this->width();
    const auto height = this->height();
    const int cropWidth = height > 0 ? std::min(src.width(), width * src.height() / height) : src.width();
    const int cropHeight = width > 0 ? std::min(src.height(), height * src.width() / width) : src.height();
    this->cropAndScaleFrom(src, (src.width() - cropWidth) / 2, (src.height() - cropHeight) / 2, cropWidth, cropHeight);
}

void I420Buffer::scaleFrom(const I420BufferInterface &src) { cropAndScaleFrom(src, 0, 0, src.width(), src.height()); }

OCTK_END_NAMESPACE
