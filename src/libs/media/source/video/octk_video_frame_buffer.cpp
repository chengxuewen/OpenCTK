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
#include <octk_rgba_buffer.hpp>
#include <octk_i420_buffer.hpp>
#include <octk_i422_buffer.hpp>
#include <octk_i444_buffer.hpp>
#include <octk_nv12_buffer.hpp>
#include <octk_video_frame_buffer.hpp>

#include <libyuv.h>

#include <string>
#include <functional>

OCTK_BEGIN_NAMESPACE
std::shared_ptr<VideoFrameBuffer> VideoFrameBuffer::cropAndScale(int offsetX,
                                                                 int offsetY,
                                                                 int cropWidth,
                                                                 int cropHeight,
                                                                 int scaledWidth,
                                                                 int scaledHeight)
{
    std::shared_ptr<I420Buffer> result = I420Buffer::create(scaledWidth, scaledHeight);
    result->cropAndScaleFrom(*this->toI420(), offsetX, offsetY, cropWidth, cropHeight);
    return result;
}

std::shared_ptr<RGBABufferInterface> VideoFrameBuffer::toRGBA()
{
    if (I420Buffer::Type::kI420 == this->type())
    {
        return RGBABuffer::copy(*this->getI420());
    }
    const auto i420Buffer = this->toI420();
    return RGBABuffer::copy(*i420Buffer.get());
}

const I420BufferInterface *VideoFrameBuffer::getI420() const
{
    // Overridden by subclasses that can return an I420 buffer without any
    // conversion, in particular, I420BufferInterface.
    return nullptr;
}

const I420ABufferInterface *VideoFrameBuffer::getI420A() const
{
    OCTK_CHECK(this->type() == Type::kI420A);
    return static_cast<const I420ABufferInterface *>(this);
}

const I444BufferInterface *VideoFrameBuffer::getI444() const
{
    OCTK_CHECK(this->type() == Type::kI444);
    return static_cast<const I444BufferInterface *>(this);
}

const I422BufferInterface *VideoFrameBuffer::getI422() const
{
    OCTK_CHECK(this->type() == Type::kI422);
    return static_cast<const I422BufferInterface *>(this);
}

const I010BufferInterface *VideoFrameBuffer::getI010() const
{
    OCTK_CHECK(this->type() == Type::kI010);
    return static_cast<const I010BufferInterface *>(this);
}

const I210BufferInterface *VideoFrameBuffer::getI210() const
{
    OCTK_CHECK(this->type() == Type::kI210);
    return static_cast<const I210BufferInterface *>(this);
}

const I410BufferInterface *VideoFrameBuffer::getI410() const
{
    OCTK_CHECK(this->type() == Type::kI410);
    return static_cast<const I410BufferInterface *>(this);
}

const NV12BufferInterface *VideoFrameBuffer::getNV12() const
{
    OCTK_CHECK(this->type() == Type::kNV12);
    return static_cast<const NV12BufferInterface *>(this);
}

std::shared_ptr<VideoFrameBuffer> VideoFrameBuffer::getMappedFrameBuffer(ArrayView<Type> /* types */)
{
    OCTK_CHECK(this->type() == Type::kNative);
    return nullptr;
}

std::string VideoFrameBuffer::storageRepresentation() const { return "?"; }

VideoFrameBuffer::Type RGBABufferInterface::type() const { return Type::kRGBA; }

VideoFrameBuffer::Type I420BufferInterface::type() const { return Type::kI420; }

const char *videoFrameBufferTypeToString(VideoFrameBuffer::Type type)
{
    switch (type)
    {
        case VideoFrameBuffer::Type::kNative: return "kNative";
        case VideoFrameBuffer::Type::kRGBA: return "kRGBA";
        case VideoFrameBuffer::Type::kI420: return "kI420";
        case VideoFrameBuffer::Type::kI420A: return "kI420A";
        case VideoFrameBuffer::Type::kI444: return "kI444";
        case VideoFrameBuffer::Type::kI422: return "kI422";
        case VideoFrameBuffer::Type::kI010: return "kI010";
        case VideoFrameBuffer::Type::kI210: return "kI210";
        case VideoFrameBuffer::Type::kI410: return "kI410";
        case VideoFrameBuffer::Type::kNV12: return "kNV12";
        default: OCTK_DCHECK_NOTREACHED();
    }
    return "";
}

int I420BufferInterface::chromaWidth() const { return (this->width() + 1) / 2; }

int I420BufferInterface::chromaHeight() const { return (this->height() + 1) / 2; }

const I420BufferInterface *I420BufferInterface::getI420() const { return this; }

VideoFrameBuffer::Type I420ABufferInterface::type() const { return Type::kI420A; }

VideoFrameBuffer::Type I444BufferInterface::type() const { return Type::kI444; }

int I444BufferInterface::chromaWidth() const { return this->width(); }

int I444BufferInterface::chromaHeight() const { return this->height(); }

std::shared_ptr<VideoFrameBuffer> I444BufferInterface::cropAndScale(int offsetX,
                                                                    int offsetY,
                                                                    int cropWidth,
                                                                    int cropHeight,
                                                                    int scaledWidth,
                                                                    int scaledHeight)
{
    std::shared_ptr<I444Buffer> result = I444Buffer::Create(scaledWidth, scaledHeight);
    result->cropAndScaleFrom(*this, offsetX, offsetY, cropWidth, cropHeight);
    return result;
}

VideoFrameBuffer::Type I422BufferInterface::type() const { return Type::kI422; }

int I422BufferInterface::chromaWidth() const { return (this->width() + 1) / 2; }

int I422BufferInterface::chromaHeight() const { return this->height(); }

std::shared_ptr<VideoFrameBuffer> I422BufferInterface::cropAndScale(int offsetX,
                                                                    int offsetY,
                                                                    int cropWidth,
                                                                    int cropHeight,
                                                                    int scaledWidth,
                                                                    int scaledHeight)
{
    std::shared_ptr<I422Buffer> result = I422Buffer::create(scaledWidth, scaledHeight);
    result->cropAndScaleFrom(*this, offsetX, offsetY, cropWidth, cropHeight);
    return result;
}

VideoFrameBuffer::Type I010BufferInterface::type() const { return Type::kI010; }

int I010BufferInterface::chromaWidth() const { return (this->width() + 1) / 2; }

int I010BufferInterface::chromaHeight() const { return (this->height() + 1) / 2; }

VideoFrameBuffer::Type I210BufferInterface::type() const { return Type::kI210; }

int I210BufferInterface::chromaWidth() const { return (this->width() + 1) / 2; }

int I210BufferInterface::chromaHeight() const { return this->height(); }

VideoFrameBuffer::Type I410BufferInterface::type() const { return Type::kI410; }

int I410BufferInterface::chromaWidth() const { return this->width(); }

int I410BufferInterface::chromaHeight() const { return this->height(); }

VideoFrameBuffer::Type NV12BufferInterface::type() const { return Type::kNV12; }

int NV12BufferInterface::chromaWidth() const { return (this->width() + 1) / 2; }

int NV12BufferInterface::chromaHeight() const { return (this->height() + 1) / 2; }

std::shared_ptr<VideoFrameBuffer> NV12BufferInterface::cropAndScale(int offsetX,
                                                                    int offsetY,
                                                                    int cropWidth,
                                                                    int cropHeight,
                                                                    int scaledWidth,
                                                                    int scaledHeight)
{
    std::shared_ptr<NV12Buffer> result = NV12Buffer::create(scaledWidth, scaledHeight);
    result->cropAndScaleFrom(*this, offsetX, offsetY, cropWidth, cropHeight);
    return result;
}

namespace utils
{
template <typename Base> class WrappedRgbBuffer : public Base
{
public:
    WrappedRgbBuffer(int width, int height, const uint8_t *data, int stride, std::function<void()> noLongerUsed)
        : mWidth(width)
          , mHeight(height)
          , mData(data)
          , mStride(stride)
          , mNoLongerUsedCallback(noLongerUsed)
    {
    }

    ~WrappedRgbBuffer() override { mNoLongerUsedCallback(); }

    int width() const override { return mWidth; }

    int height() const override { return mHeight; }

    const uint8_t *data() const override { return mData; }

    int stride() const override { return mStride; }

private:
    friend class std::shared_ptr<WrappedRgbBuffer>;

    const int mWidth;
    const int mHeight;
    const int mStride;
    const uint8_t *const mData;
    std::function<void()> mNoLongerUsedCallback;
};

class RGBABufferBase : public RGBABufferInterface
{
public:
    std::shared_ptr<I420BufferInterface> toI420() final
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
    std::shared_ptr<RGBABufferInterface> toRGBA() final { return RGBABuffer::copy(*this); }
};

// Template to implement a wrapped buffer for a I4??BufferInterface.
template <typename Base> class WrappedYuvBuffer : public Base
{
public:
    WrappedYuvBuffer(int width,
                     int height,
                     const uint8_t *yPlane,
                     int yStride,
                     const uint8_t *uPlane,
                     int uStride,
                     const uint8_t *vPlane,
                     int vStride,
                     std::function<void()> noLongerUsed)
        : mWidth(width)
          , mHeight(height)
          , mYPlane(yPlane)
          , mUPlane(uPlane)
          , mVPlane(vPlane)
          , mYStride(yStride)
          , mUStride(uStride)
          , mVStride(vStride)
          , mNoLongerUsedCallback(noLongerUsed)
    {
    }

    ~WrappedYuvBuffer() override { mNoLongerUsedCallback(); }

    int width() const override { return mWidth; }

    int height() const override { return mHeight; }

    const uint8_t *dataY() const override { return mYPlane; }

    const uint8_t *dataU() const override { return mUPlane; }

    const uint8_t *dataV() const override { return mVPlane; }

    int strideY() const override { return mYStride; }

    int strideU() const override { return mUStride; }

    int strideV() const override { return mVStride; }

private:
    friend class std::shared_ptr<WrappedYuvBuffer>;

    const int mWidth;
    const int mHeight;
    const uint8_t *const mYPlane;
    const uint8_t *const mUPlane;
    const uint8_t *const mVPlane;
    const int mYStride;
    const int mUStride;
    const int mVStride;
    std::function<void()> mNoLongerUsedCallback;
};

// Template to implement a wrapped buffer for a I4??BufferInterface.
template <typename BaseWithA> class WrappedYuvaBuffer : public WrappedYuvBuffer<BaseWithA>
{
public:
    WrappedYuvaBuffer(int width,
                      int height,
                      const uint8_t *yPlane,
                      int yStride,
                      const uint8_t *uPlane,
                      int uStride,
                      const uint8_t *vPlane,
                      int vStride,
                      const uint8_t *aPlane,
                      int aStride,
                      std::function<void()> noLongerUsed)
        : WrappedYuvBuffer<BaseWithA>(width, height, yPlane, yStride, uPlane, uStride, vPlane, vStride, noLongerUsed)
          , mAPlane(aPlane)
          , mAStride(aStride)
    {
    }

    const uint8_t *dataA() const override { return mAPlane; }
    int strideA() const override { return mAStride; }

private:
    const uint8_t *const mAPlane;
    const int mAStride;
};

class I444BufferBase : public I444BufferInterface
{
public:
    std::shared_ptr<I420BufferInterface> toI420() final
    {
        std::shared_ptr<I420Buffer> i420Buffer = I420Buffer::create(this->width(), this->height());
        libyuv::I444ToI420(this->dataY(),
                           this->strideY(),
                           this->dataU(),
                           this->strideU(),
                           this->dataV(),
                           this->strideV(),
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
};

class I422BufferBase : public I422BufferInterface
{
public:
    std::shared_ptr<I420BufferInterface> toI420() final
    {
        std::shared_ptr<I420Buffer> i420Buffer = I420Buffer::create(this->width(), this->height());
        libyuv::I422ToI420(this->dataY(),
                           this->strideY(),
                           this->dataU(),
                           this->strideU(),
                           this->dataV(),
                           this->strideV(),
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
};

// Template to implement a wrapped buffer for a PlanarYuv16BBuffer.
template <typename Base> class WrappedYuv16BBuffer : public Base
{
public:
    WrappedYuv16BBuffer(int width,
                        int height,
                        const uint16_t *yPlane,
                        int yStride,
                        const uint16_t *uPlane,
                        int uStride,
                        const uint16_t *vPlane,
                        int vStride,
                        std::function<void()> noLongerUsed)
        : mWidth(width)
          , mHeight(height)
          , mYPlane(yPlane)
          , mUPlane(uPlane)
          , mVPlane(vPlane)
          , mYStride(yStride)
          , mUStride(uStride)
          , mVStride(vStride)
          , mNoLongerUsedCallback(noLongerUsed)
    {
    }

    ~WrappedYuv16BBuffer() override { mNoLongerUsedCallback(); }

    int width() const override { return mWidth; }

    int height() const override { return mHeight; }

    const uint16_t *dataY() const override { return mYPlane; }

    const uint16_t *dataU() const override { return mUPlane; }

    const uint16_t *dataV() const override { return mVPlane; }

    int strideY() const override { return mYStride; }

    int strideU() const override { return mUStride; }

    int strideV() const override { return mVStride; }

private:
    // friend class RefCountedObject<WrappedYuv16BBuffer>;

    const int mWidth;
    const int mHeight;
    const uint16_t *const mYPlane;
    const uint16_t *const mUPlane;
    const uint16_t *const mVPlane;
    const int mYStride;
    const int mUStride;
    const int mVStride;
    std::function<void()> mNoLongerUsedCallback;
};

class I010BufferBase : public I010BufferInterface
{
public:
    std::shared_ptr<I420BufferInterface> toI420() final
    {
        std::shared_ptr<I420Buffer> i420Buffer = I420Buffer::create(this->width(), this->height());
        libyuv::I010ToI420(this->dataY(),
                           this->strideY(),
                           this->dataU(),
                           this->strideU(),
                           this->dataV(),
                           this->strideV(),
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
};

class I210BufferBase : public I210BufferInterface
{
public:
    std::shared_ptr<I420BufferInterface> toI420() final
    {
        std::shared_ptr<I420Buffer> i420Buffer = I420Buffer::create(this->width(), this->height());
        libyuv::I210ToI420(this->dataY(),
                           this->strideY(),
                           this->dataU(),
                           this->strideU(),
                           this->dataV(),
                           this->strideV(),
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
};

class I410BufferBase : public I410BufferInterface
{
public:
    std::shared_ptr<I420BufferInterface> toI420() final
    {
        std::shared_ptr<I420Buffer> i420Buffer = I420Buffer::create(this->width(), this->height());
        libyuv::I410ToI420(this->dataY(),
                           this->strideY(),
                           this->dataU(),
                           this->strideU(),
                           this->dataV(),
                           this->strideV(),
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
};

class I420BufferBase : public I420BufferInterface
{
public:
    std::shared_ptr<I420BufferInterface> toI420() final
    {
        std::shared_ptr<I420Buffer> i420Buffer = I420Buffer::create(this->width(), this->height());
        libyuv::I420Copy(this->dataY(),
                         this->strideY(),
                         this->dataU(),
                         this->strideU(),
                         this->dataV(),
                         this->strideV(),
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
};

std::shared_ptr<RGBABufferInterface>
wrapRGBABuffer(int width, int height, const uint8_t *data, int stride, std::function<void()> noLongerUsed)
{
    return std::shared_ptr<RGBABufferInterface>(
        new WrappedRgbBuffer<RGBABufferBase>(width, height, data, stride, noLongerUsed));
}

std::shared_ptr<I420BufferInterface> wrapI420Buffer(int width,
                                                    int height,
                                                    const uint8_t *yPlane,
                                                    int yStride,
                                                    const uint8_t *uPlane,
                                                    int uStride,
                                                    const uint8_t *vPlane,
                                                    int vStride,
                                                    std::function<void()> noLongerUsed)
{
    return std::shared_ptr<I420BufferInterface>(
        new WrappedYuvBuffer<
            I420BufferBase>(width, height, yPlane, yStride, uPlane, uStride, vPlane, vStride, noLongerUsed));
}

class I420ABufferBase : public I420ABufferInterface
{
public:
    std::shared_ptr<I420BufferInterface> toI420() final
    {
        std::shared_ptr<I420Buffer> i420ABuffer = I420Buffer::create(this->width(), this->height());
        libyuv::I420Copy(this->dataY(),
                         this->strideY(),
                         this->dataU(),
                         this->strideU(),
                         this->dataV(),
                         this->strideV(),
                         i420ABuffer->MutableDataY(),
                         i420ABuffer->strideY(),
                         i420ABuffer->MutableDataU(),
                         i420ABuffer->strideU(),
                         i420ABuffer->MutableDataV(),
                         i420ABuffer->strideV(),
                         this->width(),
                         this->height());
        return i420ABuffer;
    }
};

std::shared_ptr<I420ABufferInterface> wrapI420ABuffer(int width,
                                                      int height,
                                                      const uint8_t *yPlane,
                                                      int yStride,
                                                      const uint8_t *uPlane,
                                                      int uStride,
                                                      const uint8_t *vPlane,
                                                      int vStride,
                                                      const uint8_t *aPlane,
                                                      int aStride,
                                                      std::function<void()> noLongerUsed)
{
    return std::shared_ptr<I420ABufferInterface>(new WrappedYuvaBuffer<I420ABufferBase>(width,
        height,
        yPlane,
        yStride,
        uPlane,
        uStride,
        vPlane,
        vStride,
        aPlane,
        aStride,
        noLongerUsed));
}

std::shared_ptr<I422BufferInterface> wrapI422Buffer(int width,
                                                    int height,
                                                    const uint8_t *yPlane,
                                                    int yStride,
                                                    const uint8_t *uPlane,
                                                    int uStride,
                                                    const uint8_t *vPlane,
                                                    int vStride,
                                                    std::function<void()> noLongerUsed)
{
    return std::shared_ptr<I422BufferBase>(
        new WrappedYuvBuffer<
            I422BufferBase>(width, height, yPlane, yStride, uPlane, uStride, vPlane, vStride, noLongerUsed));
}

std::shared_ptr<I444BufferInterface> wrapI444Buffer(int width,
                                                    int height,
                                                    const uint8_t *yPlane,
                                                    int yStride,
                                                    const uint8_t *uPlane,
                                                    int uStride,
                                                    const uint8_t *vPlane,
                                                    int vStride,
                                                    std::function<void()> noLongerUsed)
{
    return std::shared_ptr<I444BufferInterface>(
        new WrappedYuvBuffer<
            I444BufferBase>(width, height, yPlane, yStride, uPlane, uStride, vPlane, vStride, noLongerUsed));
}

std::shared_ptr<PlanarYuvBuffer> wrapYuvBuffer(VideoFrameBuffer::Type type,
                                               int width,
                                               int height,
                                               const uint8_t *yPlane,
                                               int yStride,
                                               const uint8_t *uPlane,
                                               int uStride,
                                               const uint8_t *vPlane,
                                               int vStride,
                                               std::function<void()> noLongerUsed)
{
    switch (type)
    {
        case VideoFrameBuffer::Type::kI420:
        {
            return wrapI420Buffer(width, height, yPlane, yStride, uPlane, uStride, vPlane, vStride, noLongerUsed);
        }
        case VideoFrameBuffer::Type::kI422:
        {
            return wrapI422Buffer(width, height, yPlane, yStride, uPlane, uStride, vPlane, vStride, noLongerUsed);
        }
        case VideoFrameBuffer::Type::kI444:
        {
            return wrapI444Buffer(width, height, yPlane, yStride, uPlane, uStride, vPlane, vStride, noLongerUsed);
        }
        default: OCTK_CHECK_NOTREACHED();
    }
    return nullptr;
}

std::shared_ptr<I010BufferInterface> wrapI010Buffer(int width,
                                                    int height,
                                                    const uint16_t *yPlane,
                                                    int yStride,
                                                    const uint16_t *uPlane,
                                                    int uStride,
                                                    const uint16_t *vPlane,
                                                    int vStride,
                                                    std::function<void()> noLongerUsed)
{
    return std::shared_ptr<I010BufferInterface>(
        new WrappedYuv16BBuffer<
            I010BufferBase>(width, height, yPlane, yStride, uPlane, uStride, vPlane, vStride, noLongerUsed));
}

std::shared_ptr<I210BufferInterface> wrapI210Buffer(int width,
                                                    int height,
                                                    const uint16_t *yPlane,
                                                    int yStride,
                                                    const uint16_t *uPlane,
                                                    int uStride,
                                                    const uint16_t *vPlane,
                                                    int vStride,
                                                    std::function<void()> noLongerUsed)
{
    return std::shared_ptr<I210BufferInterface>(
        new WrappedYuv16BBuffer<
            I210BufferBase>(width, height, yPlane, yStride, uPlane, uStride, vPlane, vStride, noLongerUsed));
}

std::shared_ptr<I410BufferInterface> wrapI410Buffer(int width,
                                                    int height,
                                                    const uint16_t *yPlane,
                                                    int yStride,
                                                    const uint16_t *uPlane,
                                                    int uStride,
                                                    const uint16_t *vPlane,
                                                    int vStride,
                                                    std::function<void()> noLongerUsed)
{
    return std::shared_ptr<I410BufferInterface>(
        new WrappedYuv16BBuffer<
            I410BufferBase>(width, height, yPlane, yStride, uPlane, uStride, vPlane, vStride, noLongerUsed));
}
} // namespace utils

OCTK_END_NAMESPACE