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

#ifndef _OCTK_VIDEO_FRAME_BUFFER_HPP
#define _OCTK_VIDEO_FRAME_BUFFER_HPP

#include <octk_media_global.hpp>
#include <octk_array_view.hpp>

#include <cstdint>
#include <string>

OCTK_BEGIN_NAMESPACE

class I420BufferInterface;

class I420ABufferInterface;

class I422BufferInterface;

class I444BufferInterface;

class I010BufferInterface;

class I210BufferInterface;

class I410BufferInterface;

class NV12BufferInterface;

// Base class for frame buffers of different types of pixel format and storage.
// The tag in type() indicates how the data is represented, and each type is
// implemented as a subclass. To access the pixel data, call the appropriate
// GetXXX() function, where XXX represents the type. There is also a function
// ToI420() that returns a frame buffer in I420 format, converting from the
// underlying representation if necessary. I420 is the most widely accepted
// format and serves as a fallback for video sinks that can only handle I420,
// e.g. the internal WebRTC software encoders. A special enum value 'kNative' is
// provided for external clients to implement their own frame buffer
// representations, e.g. as textures. The external client can produce such
// native frame buffers from custom video sources, and then cast it back to the
// correct subclass in custom video sinks. The purpose of this is to improve
// performance by providing an optimized path without intermediate conversions.
// Frame metadata such as rotation and timestamp are stored in
// webrtc::VideoFrame, and not here.
class OCTK_MEDIA_API VideoFrameBuffer
{
public:
    // New frame buffer types will be added conservatively when there is an
    // opportunity to optimize the path between some pair of video source and
    // video sink.
    // GENERATED_JAVA_ENUM_PACKAGE: org.webrtc
    // GENERATED_JAVA_CLASS_NAME_OVERRIDE: VideoFrameBufferType
    enum class Type
    {
        kNative,
        kI420,
        kI420A,
        kI422,
        kI444,
        kI010,
        kI210,
        kI410,
        kNV12,
    };

    // This function specifies in what pixel format the data is stored in.
    virtual Type type() const = 0;

    // The resolution of the frame in pixels. For formats where some planes are
    // subsampled, this is the highest-resolution plane.
    virtual int width() const = 0;
    virtual int height() const = 0;

    // Returns a memory-backed frame buffer in I420 format. If the pixel data is
    // in another format, a conversion will take place. All implementations must
    // provide a fallback to I420 for compatibility with e.g. the internal WebRTC
    // software encoders.
    // Conversion may fail, for example if reading the pixel data from a texture
    // fails. If the conversion fails, nullptr is returned.
    virtual std::shared_ptr<I420BufferInterface> ToI420() = 0;

    // GetI420() methods should return I420 buffer if conversion is trivial, i.e
    // no change for binary data is needed. Otherwise these methods should return
    // nullptr. One example of buffer with that property is
    // WebrtcVideoFrameAdapter in Chrome - it's I420 buffer backed by a shared
    // memory buffer. Therefore it must have type kNative. Yet, ToI420()
    // doesn't affect binary data at all. Another example is any I420A buffer.
    // TODO(https://crbug.com/webrtc/12021): Make this method non-virtual and
    // behave as the other GetXXX methods below.
    virtual const I420BufferInterface *GetI420() const;

    // A format specific scale function. Default implementation works by
    // converting to I420. But more efficient implementations may override it,
    // especially for kNative.
    // First, the image is cropped to `cropWidth` and `cropHeight` and then
    // scaled to `scaled_width` and `scaled_height`.
    virtual std::shared_ptr<VideoFrameBuffer> CropAndScale(int offsetX, int offsetY, int cropWidth, int cropHeight,
                                                           int scaled_width, int scaled_height);

    // Alias for common use case.
    std::shared_ptr<VideoFrameBuffer> Scale(int scaled_width, int scaled_height)
    {
        return CropAndScale(0, 0, width(), height(), scaled_width, scaled_height);
    }

    // These functions should only be called if type() is of the correct type.
    // Calling with a different type will result in a crash.
    const I420ABufferInterface *GetI420A() const;
    const I422BufferInterface *GetI422() const;
    const I444BufferInterface *GetI444() const;
    const I010BufferInterface *GetI010() const;
    const I210BufferInterface *GetI210() const;
    const I410BufferInterface *GetI410() const;
    const NV12BufferInterface *GetNV12() const;

    // From a kNative frame, returns a VideoFrameBuffer with a pixel format in
    // the list of types that is in the main memory with a pixel perfect
    // conversion for encoding with a software encoder. Returns nullptr if the
    // frame type is not supported, mapping is not possible, or if the kNative
    // frame has not implemented this method. Only callable if type() is kNative.
    virtual std::shared_ptr<VideoFrameBuffer> GetMappedFrameBuffer(ArrayView<Type> types);

    // For logging: returns a textual representation of the storage.
    virtual std::string storage_representation() const;

protected:
    virtual ~VideoFrameBuffer() {}
};

// Update when VideoFrameBuffer::Type is updated.
const char *videoFrameBufferTypeToString(VideoFrameBuffer::Type type);

// This interface represents planar formats.
class PlanarYuvBuffer : public VideoFrameBuffer
{
public:
    virtual int ChromaWidth() const = 0;
    virtual int ChromaHeight() const = 0;

    // Returns the number of steps(in terms of Data*() return type) between
    // successive rows for a given plane.
    virtual int StrideY() const = 0;
    virtual int StrideU() const = 0;
    virtual int StrideV() const = 0;

protected:
    ~PlanarYuvBuffer() override {}
};

// This interface represents 8-bit color depth formats: Type::kI420,
// Type::kI420A, Type::kI422 and Type::kI444.
class PlanarYuv8Buffer : public PlanarYuvBuffer
{
public:
    // Returns pointer to the pixel data for a given plane. The memory is owned by
    // the VideoFrameBuffer object and must not be freed by the caller.
    virtual const uint8_t *DataY() const = 0;
    virtual const uint8_t *DataU() const = 0;
    virtual const uint8_t *DataV() const = 0;

protected:
    ~PlanarYuv8Buffer() override {}
};

class OCTK_MEDIA_API I420BufferInterface : public PlanarYuv8Buffer
{
public:
    Type type() const override;

    int ChromaWidth() const final;
    int ChromaHeight() const final;

    const I420BufferInterface *GetI420() const final;

protected:
    ~I420BufferInterface() override {}
};

class OCTK_MEDIA_API I420ABufferInterface : public I420BufferInterface
{
public:
    Type type() const final;
    virtual const uint8_t *DataA() const = 0;
    virtual int StrideA() const = 0;

protected:
    ~I420ABufferInterface() override {}
};

// Represents Type::kI422, 4:2:2 planar with 8 bits per pixel.
class I422BufferInterface : public PlanarYuv8Buffer
{
public:
    Type type() const final;

    int ChromaWidth() const final;
    int ChromaHeight() const final;

    std::shared_ptr<VideoFrameBuffer> CropAndScale(int offsetX, int offsetY, int cropWidth, int cropHeight,
                                                   int scaled_width, int scaled_height) override;

protected:
    ~I422BufferInterface() override {}
};

// Represents Type::kI444, 4:4:4 planar with 8 bits per pixel.
class I444BufferInterface : public PlanarYuv8Buffer
{
public:
    Type type() const final;

    int ChromaWidth() const final;
    int ChromaHeight() const final;

    std::shared_ptr<VideoFrameBuffer> CropAndScale(int offsetX, int offsetY, int cropWidth, int cropHeight,
                                                   int scaled_width, int scaled_height) override;

protected:
    ~I444BufferInterface() override {}
};

// This interface represents 8-bit to 16-bit color depth formats: Type::kI010,
// Type::kI210, or Type::kI410.
class PlanarYuv16BBuffer : public PlanarYuvBuffer
{
public:
    // Returns pointer to the pixel data for a given plane. The memory is owned by
    // the VideoFrameBuffer object and must not be freed by the caller.
    virtual const uint16_t *DataY() const = 0;
    virtual const uint16_t *DataU() const = 0;
    virtual const uint16_t *DataV() const = 0;

protected:
    ~PlanarYuv16BBuffer() override {}
};

// Represents Type::kI010, allocates 16 bits per pixel and fills 10 least
// significant bits with color information.
class I010BufferInterface : public PlanarYuv16BBuffer
{
public:
    Type type() const override;

    int ChromaWidth() const final;
    int ChromaHeight() const final;

protected:
    ~I010BufferInterface() override {}
};

// Represents Type::kI210, allocates 16 bits per pixel and fills 10 least
// significant bits with color information.
class I210BufferInterface : public PlanarYuv16BBuffer
{
public:
    Type type() const override;

    int ChromaWidth() const final;
    int ChromaHeight() const final;

protected:
    ~I210BufferInterface() override {}
};

// Represents Type::kI410, allocates 16 bits per pixel and fills 10 least
// significant bits with color information.
class I410BufferInterface : public PlanarYuv16BBuffer
{
public:
    Type type() const override;

    int ChromaWidth() const final;
    int ChromaHeight() const final;

protected:
    ~I410BufferInterface() override {}
};

class BiplanarYuvBuffer : public VideoFrameBuffer
{
public:
    virtual int ChromaWidth() const = 0;
    virtual int ChromaHeight() const = 0;

    // Returns the number of steps(in terms of Data*() return type) between
    // successive rows for a given plane.
    virtual int StrideY() const = 0;
    virtual int StrideUV() const = 0;

protected:
    ~BiplanarYuvBuffer() override {}
};

class BiplanarYuv8Buffer : public BiplanarYuvBuffer
{
public:
    virtual const uint8_t *DataY() const = 0;
    virtual const uint8_t *DataUV() const = 0;

protected:
    ~BiplanarYuv8Buffer() override {}
};

// Represents Type::kNV12. NV12 is full resolution Y and half-resolution
// interleved UV.
class OCTK_MEDIA_API NV12BufferInterface : public BiplanarYuv8Buffer
{
public:
    Type type() const override;

    int ChromaWidth() const final;
    int ChromaHeight() const final;

    std::shared_ptr<VideoFrameBuffer> CropAndScale(int offsetX, int offsetY, int cropWidth, int cropHeight,
                                                   int scaled_width, int scaled_height) override;

protected:
    ~NV12BufferInterface() override {}
};

namespace utils
{
std::shared_ptr<I420BufferInterface> wrapI420Buffer(int width,
                                                    int height,
                                                    const uint8_t *y_plane,
                                                    int y_stride,
                                                    const uint8_t *u_plane,
                                                    int u_stride,
                                                    const uint8_t *v_plane,
                                                    int v_stride,
                                                    std::function<void()> no_longer_used);

std::shared_ptr<I422BufferInterface> WrapI422Buffer(int width,
                                                    int height,
                                                    const uint8_t *y_plane,
                                                    int y_stride,
                                                    const uint8_t *u_plane,
                                                    int u_stride,
                                                    const uint8_t *v_plane,
                                                    int v_stride,
                                                    std::function<void()> no_longer_used);

std::shared_ptr<I444BufferInterface> WrapI444Buffer(int width,
                                                    int height,
                                                    const uint8_t *y_plane,
                                                    int y_stride,
                                                    const uint8_t *u_plane,
                                                    int u_stride,
                                                    const uint8_t *v_plane,
                                                    int v_stride,
                                                    std::function<void()> no_longer_used);

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
                                                      std::function<void()> no_longer_used);

std::shared_ptr<PlanarYuvBuffer> WrapYuvBuffer(VideoFrameBuffer::Type type,
                                               int width,
                                               int height,
                                               const uint8_t *y_plane,
                                               int y_stride,
                                               const uint8_t *u_plane,
                                               int u_stride,
                                               const uint8_t *v_plane,
                                               int v_stride,
                                               std::function<void()> no_longer_used);

std::shared_ptr<I010BufferInterface> WrapI010Buffer(int width,
                                                    int height,
                                                    const uint16_t *y_plane,
                                                    int y_stride,
                                                    const uint16_t *u_plane,
                                                    int u_stride,
                                                    const uint16_t *v_plane,
                                                    int v_stride,
                                                    std::function<void()> no_longer_used);

std::shared_ptr<I210BufferInterface> WrapI210Buffer(int width,
                                                    int height,
                                                    const uint16_t *y_plane,
                                                    int y_stride,
                                                    const uint16_t *u_plane,
                                                    int u_stride,
                                                    const uint16_t *v_plane,
                                                    int v_stride,
                                                    std::function<void()> no_longer_used);

std::shared_ptr<I410BufferInterface> WrapI410Buffer(int width,
                                                    int height,
                                                    const uint16_t *y_plane,
                                                    int y_stride,
                                                    const uint16_t *u_plane,
                                                    int u_stride,
                                                    const uint16_t *v_plane,
                                                    int v_stride,
                                                    std::function<void()> no_longer_used);
}
OCTK_END_NAMESPACE

#endif // _OCTK_VIDEO_FRAME_BUFFER_HPP
