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

#include <octk_rgba_buffer.hpp>
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
} // namespace

RGBABuffer::RGBABuffer(int width, int height)
    : mWidth(width)
      , mHeight(height)
      , mData(static_cast<uint8_t *>(utils::alignedMalloc(mWidth * mHeight * 4, kBufferAlignment)))
{
    OCTK_DCHECK_GT(width, 0);
    OCTK_DCHECK_GT(height, 0);
}

RGBABuffer::~RGBABuffer() = default;

std::shared_ptr<RGBABuffer> RGBABuffer::create(int width, int height)
{
    return std::make_shared<RGBABuffer>(width, height);
}

std::shared_ptr<RGBABuffer> RGBABuffer::copy(const I420BufferInterface &i420Buffer)
{
    std::shared_ptr<RGBABuffer> buffer = RGBABuffer::create(i420Buffer.width(), i420Buffer.height());
    libyuv::I420ToRGBA(i420Buffer.dataY(),
                       i420Buffer.strideY(),
                       i420Buffer.dataU(),
                       i420Buffer.strideU(),
                       i420Buffer.dataV(),
                       i420Buffer.strideV(),
                       buffer->mutableData(),
                       buffer->stride(),
                       buffer->width(),
                       buffer->height());
    return buffer;
}

std::shared_ptr<RGBABuffer> RGBABuffer::copy(const RGBABufferInterface &rgbaBuffer)
{
    std::shared_ptr<RGBABuffer> buffer = RGBABuffer::create(rgbaBuffer.width(), rgbaBuffer.height());
    libyuv::ARGBCopy(rgbaBuffer.data(),
                     rgbaBuffer.stride(),
                     buffer->mutableData(),
                     buffer->stride(),
                     buffer->width(),
                     buffer->height());
    return buffer;
}

std::shared_ptr<I420BufferInterface> RGBABuffer::toI420()
{
    std::shared_ptr<I420Buffer> i420Buffer = I420Buffer::create(this->width(), this->height());
    libyuv::RGBAToI420(this->data(),
                       this->stride(),
                       i420Buffer->MutableDataY(),
                       i420Buffer->strideY(),
                       i420Buffer->MutableDataU(),
                       i420Buffer->strideU(),
                       i420Buffer->MutableDataV(),
                       i420Buffer->strideV(),
                       this->width(),
                       this->height());
    return i420Buffer;
}

std::shared_ptr<RGBABufferInterface> RGBABuffer::toRGBA() { return RGBABuffer::copy(*this); }

int RGBABuffer::width() const { return mWidth; }
int RGBABuffer::height() const { return mHeight; }

int RGBABuffer::stride() const { return this->width() * 4; }

uint8_t *RGBABuffer::mutableData() { return mData.get(); }

const uint8_t *RGBABuffer::data() const { return mData.get(); }

void RGBABuffer::InitializeData() { memset(mData.get(), 0, this->stride() * this->height() * sizeof(uint8_t)); }

void RGBABuffer::cropAndScaleFrom(const RGBABufferInterface &src,
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

    const uint8_t *srcData = src.data() + src.stride() * offsetY + offsetX * 4;
    const int res = libyuv::ARGBScale(srcData,
                                      src.stride(),
                                      cropWidth,
                                      cropHeight,
                                      this->mutableData(),
                                      this->stride(),
                                      this->width(),
                                      this->height(),
                                      libyuv::kFilterBox);
    OCTK_DCHECK_EQ(res, 0);
}

OCTK_END_NAMESPACE