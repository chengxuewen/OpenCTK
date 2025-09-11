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
#include <octk_i422_buffer.hpp>
#include <octk_i444_buffer.hpp>
#include <octk_nv12_buffer.hpp>
#include <octk_video_frame_buffer.hpp>

#include <libyuv.h>

#include <string>
#include <functional>

OCTK_BEGIN_NAMESPACE

std::shared_ptr<VideoFrameBuffer> VideoFrameBuffer::CropAndScale(int offsetX, int offsetY, int cropWidth,
                                                                 int cropHeight, int scaled_width, int scaled_height)
{
    std::shared_ptr<I420Buffer> result = I420Buffer::Create(scaled_width, scaled_height);
    result->cropAndScaleFrom(*this->ToI420(), offsetX, offsetY, cropWidth, cropHeight);
    return result;
}

const I420BufferInterface *VideoFrameBuffer::GetI420() const
{
    // Overridden by subclasses that can return an I420 buffer without any
    // conversion, in particular, I420BufferInterface.
    return nullptr;
}

const I420ABufferInterface *VideoFrameBuffer::GetI420A() const
{
    OCTK_CHECK(type() == Type::kI420A);
    return static_cast<const I420ABufferInterface *>(this);
}

const I444BufferInterface *VideoFrameBuffer::GetI444() const
{
    OCTK_CHECK(type() == Type::kI444);
    return static_cast<const I444BufferInterface *>(this);
}

const I422BufferInterface *VideoFrameBuffer::GetI422() const
{
    OCTK_CHECK(type() == Type::kI422);
    return static_cast<const I422BufferInterface *>(this);
}

const I010BufferInterface *VideoFrameBuffer::GetI010() const
{
    OCTK_CHECK(type() == Type::kI010);
    return static_cast<const I010BufferInterface *>(this);
}

const I210BufferInterface *VideoFrameBuffer::GetI210() const
{
    OCTK_CHECK(type() == Type::kI210);
    return static_cast<const I210BufferInterface *>(this);
}

const I410BufferInterface *VideoFrameBuffer::GetI410() const
{
    OCTK_CHECK(type() == Type::kI410);
    return static_cast<const I410BufferInterface *>(this);
}

const NV12BufferInterface *VideoFrameBuffer::GetNV12() const
{
    OCTK_CHECK(type() == Type::kNV12);
    return static_cast<const NV12BufferInterface *>(this);
}

std::shared_ptr<VideoFrameBuffer> VideoFrameBuffer::GetMappedFrameBuffer(ArrayView<Type> /* types */)
{
    OCTK_CHECK(type() == Type::kNative);
    return nullptr;
}

std::string VideoFrameBuffer::storage_representation() const { return "?"; }

VideoFrameBuffer::Type I420BufferInterface::type() const { return Type::kI420; }

const char *videoFrameBufferTypeToString(VideoFrameBuffer::Type type)
{
    switch (type)
    {
        case VideoFrameBuffer::Type::kNative:
            return "kNative";
        case VideoFrameBuffer::Type::kI420:
            return "kI420";
        case VideoFrameBuffer::Type::kI420A:
            return "kI420A";
        case VideoFrameBuffer::Type::kI444:
            return "kI444";
        case VideoFrameBuffer::Type::kI422:
            return "kI422";
        case VideoFrameBuffer::Type::kI010:
            return "kI010";
        case VideoFrameBuffer::Type::kI210:
            return "kI210";
        case VideoFrameBuffer::Type::kI410:
            return "kI410";
        case VideoFrameBuffer::Type::kNV12:
            return "kNV12";
        default:
            OCTK_DCHECK_NOTREACHED();
    }
    return "";
}

int I420BufferInterface::ChromaWidth() const { return (width() + 1) / 2; }

int I420BufferInterface::ChromaHeight() const { return (height() + 1) / 2; }

const I420BufferInterface *I420BufferInterface::GetI420() const { return this; }

VideoFrameBuffer::Type I420ABufferInterface::type() const { return Type::kI420A; }

VideoFrameBuffer::Type I444BufferInterface::type() const { return Type::kI444; }

int I444BufferInterface::ChromaWidth() const { return width(); }

int I444BufferInterface::ChromaHeight() const { return height(); }

std::shared_ptr<VideoFrameBuffer> I444BufferInterface::CropAndScale(int offsetX, int offsetY, int cropWidth,
                                                                    int cropHeight, int scaled_width,
                                                                    int scaled_height)
{
    std::shared_ptr<I444Buffer> result = I444Buffer::Create(scaled_width, scaled_height);
    result->cropAndScaleFrom(*this, offsetX, offsetY, cropWidth, cropHeight);
    return result;
}

VideoFrameBuffer::Type I422BufferInterface::type() const { return Type::kI422; }

int I422BufferInterface::ChromaWidth() const { return (width() + 1) / 2; }

int I422BufferInterface::ChromaHeight() const { return height(); }

std::shared_ptr<VideoFrameBuffer> I422BufferInterface::CropAndScale(int offsetX, int offsetY, int cropWidth,
                                                                    int cropHeight, int scaled_width,
                                                                    int scaled_height)
{
    std::shared_ptr<I422Buffer> result = I422Buffer::Create(scaled_width, scaled_height);
    result->cropAndScaleFrom(*this, offsetX, offsetY, cropWidth, cropHeight);
    return result;
}

VideoFrameBuffer::Type I010BufferInterface::type() const { return Type::kI010; }

int I010BufferInterface::ChromaWidth() const { return (width() + 1) / 2; }

int I010BufferInterface::ChromaHeight() const { return (height() + 1) / 2; }

VideoFrameBuffer::Type I210BufferInterface::type() const { return Type::kI210; }

int I210BufferInterface::ChromaWidth() const { return (width() + 1) / 2; }

int I210BufferInterface::ChromaHeight() const { return height(); }

VideoFrameBuffer::Type I410BufferInterface::type() const { return Type::kI410; }

int I410BufferInterface::ChromaWidth() const { return width(); }

int I410BufferInterface::ChromaHeight() const { return height(); }

VideoFrameBuffer::Type NV12BufferInterface::type() const { return Type::kNV12; }

int NV12BufferInterface::ChromaWidth() const { return (width() + 1) / 2; }

int NV12BufferInterface::ChromaHeight() const { return (height() + 1) / 2; }

std::shared_ptr<VideoFrameBuffer> NV12BufferInterface::CropAndScale(int offsetX, int offsetY, int cropWidth,
                                                                    int cropHeight, int scaled_width,
                                                                    int scaled_height)
{
    std::shared_ptr<NV12Buffer> result = NV12Buffer::Create(scaled_width, scaled_height);
    result->cropAndScaleFrom(*this, offsetX, offsetY, cropWidth, cropHeight);
    return result;
}

namespace utils
{

// Template to implement a wrapped buffer for a I4??BufferInterface.
template <typename Base>
class WrappedYuvBuffer : public Base
{
public:
    WrappedYuvBuffer(int width,
                     int height,
                     const uint8_t *y_plane,
                     int y_stride,
                     const uint8_t *u_plane,
                     int u_stride,
                     const uint8_t *v_plane,
                     int v_stride,
                     std::function<void()> no_longer_used)
        : width_(width), height_(height), y_plane_(y_plane), u_plane_(u_plane), v_plane_(v_plane), y_stride_(y_stride)
        , u_stride_(u_stride), v_stride_(v_stride), no_longer_used_cb_(no_longer_used) {}

    ~WrappedYuvBuffer() override { no_longer_used_cb_(); }

    int width() const override { return width_; }

    int height() const override { return height_; }

    const uint8_t *DataY() const override { return y_plane_; }

    const uint8_t *DataU() const override { return u_plane_; }

    const uint8_t *DataV() const override { return v_plane_; }

    int StrideY() const override { return y_stride_; }

    int StrideU() const override { return u_stride_; }

    int StrideV() const override { return v_stride_; }

private:
    friend class std::shared_ptr<WrappedYuvBuffer>;

    const int width_;
    const int height_;
    const uint8_t *const y_plane_;
    const uint8_t *const u_plane_;
    const uint8_t *const v_plane_;
    const int y_stride_;
    const int u_stride_;
    const int v_stride_;
    std::function<void()> no_longer_used_cb_;
};

// Template to implement a wrapped buffer for a I4??BufferInterface.
template <typename BaseWithA>
class WrappedYuvaBuffer : public WrappedYuvBuffer<BaseWithA>
{
public:
    WrappedYuvaBuffer(int width,
                      int height,
                      const uint8_t *y_plane,
                      int y_stride,
                      const uint8_t *u_plane,
                      int u_stride,
                      const uint8_t *v_plane,
                      int v_stride,
                      const uint8_t *a_plane,
                      int a_stride,
                      std::function<void()> no_longer_used)
        : WrappedYuvBuffer<BaseWithA>(width,
                                      height,
                                      y_plane,
                                      y_stride,
                                      u_plane,
                                      u_stride,
                                      v_plane,
                                      v_stride,
                                      no_longer_used), a_plane_(a_plane), a_stride_(a_stride) {}

    const uint8_t *DataA() const override { return a_plane_; }
    int StrideA() const override { return a_stride_; }

private:
    const uint8_t *const a_plane_;
    const int a_stride_;
};

class I444BufferBase : public I444BufferInterface
{
public:
    std::shared_ptr<I420BufferInterface> ToI420() final
    {
        std::shared_ptr<I420Buffer> i420_buffer = I420Buffer::Create(width(), height());
        libyuv::I444ToI420(DataY(), StrideY(), DataU(), StrideU(), DataV(), StrideV(),
                           i420_buffer->MutableDataY(), i420_buffer->StrideY(),
                           i420_buffer->MutableDataU(), i420_buffer->StrideU(),
                           i420_buffer->MutableDataV(), i420_buffer->StrideV(),
                           width(), height());
        return i420_buffer;
    }
};

class I422BufferBase : public I422BufferInterface
{
public:
    std::shared_ptr<I420BufferInterface> ToI420() final
    {
        std::shared_ptr<I420Buffer> i420_buffer = I420Buffer::Create(width(), height());
        libyuv::I422ToI420(DataY(), StrideY(), DataU(), StrideU(), DataV(), StrideV(),
                           i420_buffer->MutableDataY(), i420_buffer->StrideY(),
                           i420_buffer->MutableDataU(), i420_buffer->StrideU(),
                           i420_buffer->MutableDataV(), i420_buffer->StrideV(),
                           width(), height());
        return i420_buffer;
    }
};

// Template to implement a wrapped buffer for a PlanarYuv16BBuffer.
template <typename Base>
class WrappedYuv16BBuffer : public Base
{
public:
    WrappedYuv16BBuffer(int width,
                        int height,
                        const uint16_t *y_plane,
                        int y_stride,
                        const uint16_t *u_plane,
                        int u_stride,
                        const uint16_t *v_plane,
                        int v_stride,
                        std::function<void()> no_longer_used)
        : width_(width), height_(height), y_plane_(y_plane), u_plane_(u_plane), v_plane_(v_plane), y_stride_(y_stride)
        , u_stride_(u_stride), v_stride_(v_stride), no_longer_used_cb_(no_longer_used) {}

    ~WrappedYuv16BBuffer() override { no_longer_used_cb_(); }

    int width() const override { return width_; }

    int height() const override { return height_; }

    const uint16_t *DataY() const override { return y_plane_; }

    const uint16_t *DataU() const override { return u_plane_; }

    const uint16_t *DataV() const override { return v_plane_; }

    int StrideY() const override { return y_stride_; }

    int StrideU() const override { return u_stride_; }

    int StrideV() const override { return v_stride_; }

private:
    // friend class RefCountedObject<WrappedYuv16BBuffer>;

    const int width_;
    const int height_;
    const uint16_t *const y_plane_;
    const uint16_t *const u_plane_;
    const uint16_t *const v_plane_;
    const int y_stride_;
    const int u_stride_;
    const int v_stride_;
    std::function<void()> no_longer_used_cb_;
};

class I010BufferBase : public I010BufferInterface
{
public:
    std::shared_ptr<I420BufferInterface> ToI420() final
    {
        std::shared_ptr<I420Buffer> i420_buffer = I420Buffer::Create(width(), height());
        libyuv::I010ToI420(DataY(), StrideY(), DataU(), StrideU(), DataV(), StrideV(),
                           i420_buffer->MutableDataY(), i420_buffer->StrideY(),
                           i420_buffer->MutableDataU(), i420_buffer->StrideU(),
                           i420_buffer->MutableDataV(), i420_buffer->StrideV(),
                           width(), height());
        return i420_buffer;
    }
};

class I210BufferBase : public I210BufferInterface
{
public:
    std::shared_ptr<I420BufferInterface> ToI420() final
    {
        std::shared_ptr<I420Buffer> i420_buffer = I420Buffer::Create(width(), height());
        libyuv::I210ToI420(DataY(), StrideY(), DataU(), StrideU(), DataV(), StrideV(),
                           i420_buffer->MutableDataY(), i420_buffer->StrideY(),
                           i420_buffer->MutableDataU(), i420_buffer->StrideU(),
                           i420_buffer->MutableDataV(), i420_buffer->StrideV(),
                           width(), height());
        return i420_buffer;
    }
};

class I410BufferBase : public I410BufferInterface
{
public:
    std::shared_ptr<I420BufferInterface> ToI420() final
    {
        std::shared_ptr<I420Buffer> i420_buffer = I420Buffer::Create(width(), height());
        libyuv::I410ToI420(DataY(), StrideY(), DataU(), StrideU(), DataV(), StrideV(),
                           i420_buffer->MutableDataY(), i420_buffer->StrideY(),
                           i420_buffer->MutableDataU(), i420_buffer->StrideU(),
                           i420_buffer->MutableDataV(), i420_buffer->StrideV(),
                           width(), height());
        return i420_buffer;
    }
};

class I420BufferBase : public I420BufferInterface
{
public:
    std::shared_ptr<I420BufferInterface> ToI420() final
    {
        std::shared_ptr<I420Buffer> i420_buffer = I420Buffer::Create(width(), height());
        libyuv::I420Copy(DataY(), StrideY(), DataU(), StrideU(), DataV(), StrideV(),
                         i420_buffer->MutableDataY(), i420_buffer->StrideY(),
                         i420_buffer->MutableDataU(), i420_buffer->StrideU(),
                         i420_buffer->MutableDataV(), i420_buffer->StrideV(),
                         width(), height());
        return i420_buffer;
    }
};

std::shared_ptr<I420BufferInterface> wrapI420Buffer(int width,
                                                    int height,
                                                    const uint8_t *y_plane,
                                                    int y_stride,
                                                    const uint8_t *u_plane,
                                                    int u_stride,
                                                    const uint8_t *v_plane,
                                                    int v_stride,
                                                    std::function<void()> no_longer_used)
{
    return std::shared_ptr<I420BufferInterface>(new WrappedYuvBuffer<I420BufferBase>(width, height,
                                                                                     y_plane, y_stride,
                                                                                     u_plane, u_stride,
                                                                                     v_plane, v_stride,
                                                                                     no_longer_used));
}

class I420ABufferBase : public I420ABufferInterface
{
public:
    std::shared_ptr<I420BufferInterface> ToI420() final
    {
        std::shared_ptr<I420Buffer> i420A_buffer = I420Buffer::Create(width(), height());
        libyuv::I420Copy(DataY(), StrideY(), DataU(), StrideU(), DataV(), StrideV(),
                         i420A_buffer->MutableDataY(), i420A_buffer->StrideY(),
                         i420A_buffer->MutableDataU(), i420A_buffer->StrideU(),
                         i420A_buffer->MutableDataV(), i420A_buffer->StrideV(),
                         width(), height());
        return i420A_buffer;
    }
};

std::shared_ptr<I420ABufferInterface> WrapI420ABuffer(int width,
                                                      int height,
                                                      const uint8_t *y_plane,
                                                      int y_stride,
                                                      const uint8_t *u_plane,
                                                      int u_stride,
                                                      const uint8_t *v_plane,
                                                      int v_stride,
                                                      const uint8_t *a_plane,
                                                      int a_stride,
                                                      std::function<void()> no_longer_used)
{
    return std::shared_ptr<I420ABufferInterface>(new WrappedYuvaBuffer<I420ABufferBase>(width, height,
                                                                                        y_plane, y_stride,
                                                                                        u_plane, u_stride,
                                                                                        v_plane, v_stride,
                                                                                        a_plane, a_stride,
                                                                                        no_longer_used));
}

std::shared_ptr<I422BufferInterface> WrapI422Buffer(int width,
                                                    int height,
                                                    const uint8_t *y_plane,
                                                    int y_stride,
                                                    const uint8_t *u_plane,
                                                    int u_stride,
                                                    const uint8_t *v_plane,
                                                    int v_stride,
                                                    std::function<void()> no_longer_used)
{
    return std::shared_ptr<I422BufferBase>(new WrappedYuvBuffer<I422BufferBase>(width, height,
                                                                                y_plane, y_stride,
                                                                                u_plane, u_stride,
                                                                                v_plane, v_stride,
                                                                                no_longer_used));
}

std::shared_ptr<I444BufferInterface> WrapI444Buffer(int width,
                                                    int height,
                                                    const uint8_t *y_plane,
                                                    int y_stride,
                                                    const uint8_t *u_plane,
                                                    int u_stride,
                                                    const uint8_t *v_plane,
                                                    int v_stride,
                                                    std::function<void()> no_longer_used)
{
    return std::shared_ptr<I444BufferInterface>(new WrappedYuvBuffer<I444BufferBase>(width, height,
                                                                                     y_plane, y_stride,
                                                                                     u_plane, u_stride,
                                                                                     v_plane, v_stride,
                                                                                     no_longer_used));
}

std::shared_ptr<PlanarYuvBuffer> WrapYuvBuffer(VideoFrameBuffer::Type type,
                                               int width,
                                               int height,
                                               const uint8_t *y_plane,
                                               int y_stride,
                                               const uint8_t *u_plane,
                                               int u_stride,
                                               const uint8_t *v_plane,
                                               int v_stride,
                                               std::function<void()> no_longer_used)
{
    switch (type)
    {
        case VideoFrameBuffer::Type::kI420:
            return wrapI420Buffer(width, height, y_plane, y_stride, u_plane, u_stride,
                                  v_plane, v_stride, no_longer_used);
        case VideoFrameBuffer::Type::kI422:
            return WrapI422Buffer(width, height, y_plane, y_stride, u_plane, u_stride,
                                  v_plane, v_stride, no_longer_used);
        case VideoFrameBuffer::Type::kI444:
            return WrapI444Buffer(width, height, y_plane, y_stride, u_plane, u_stride,
                                  v_plane, v_stride, no_longer_used);
        default:
            OCTK_CHECK_NOTREACHED();
    }
    return nullptr;
}

std::shared_ptr<I010BufferInterface> WrapI010Buffer(int width,
                                                    int height,
                                                    const uint16_t *y_plane,
                                                    int y_stride,
                                                    const uint16_t *u_plane,
                                                    int u_stride,
                                                    const uint16_t *v_plane,
                                                    int v_stride,
                                                    std::function<void()> no_longer_used)
{
    return std::shared_ptr<I010BufferInterface>(new WrappedYuv16BBuffer<I010BufferBase>(width, height,
                                                                                        y_plane, y_stride,
                                                                                        u_plane, u_stride,
                                                                                        v_plane, v_stride,
                                                                                        no_longer_used));
}

std::shared_ptr<I210BufferInterface> WrapI210Buffer(int width,
                                                    int height,
                                                    const uint16_t *y_plane,
                                                    int y_stride,
                                                    const uint16_t *u_plane,
                                                    int u_stride,
                                                    const uint16_t *v_plane,
                                                    int v_stride,
                                                    std::function<void()> no_longer_used)
{
    return std::shared_ptr<I210BufferInterface>(new WrappedYuv16BBuffer<I210BufferBase>(width, height,
                                                                                        y_plane, y_stride,
                                                                                        u_plane, u_stride,
                                                                                        v_plane, v_stride,
                                                                                        no_longer_used));
}

std::shared_ptr<I410BufferInterface> WrapI410Buffer(int width,
                                                    int height,
                                                    const uint16_t *y_plane,
                                                    int y_stride,
                                                    const uint16_t *u_plane,
                                                    int u_stride,
                                                    const uint16_t *v_plane,
                                                    int v_stride,
                                                    std::function<void()> no_longer_used)
{
    return std::shared_ptr<I410BufferInterface>(new WrappedYuv16BBuffer<I410BufferBase>(width, height,
                                                                                        y_plane, y_stride,
                                                                                        u_plane, u_stride,
                                                                                        v_plane, v_stride,
                                                                                        no_longer_used));
}
} // namespace utils
OCTK_END_NAMESPACE
