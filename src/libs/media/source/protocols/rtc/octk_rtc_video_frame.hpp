/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2026~Present ChengXueWen.
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

#pragma once

#include <octk_frame_generator.hpp>
#include <octk_camera_capture.hpp>
#include <octk_shared_pointer.hpp>
#include <octk_source_sink.hpp>

OCTK_BEGIN_NAMESPACE

class RtcVideoFrame
{
public:
    using SharedPtr = SharedPointer<RtcVideoFrame>;

    enum class Format
    {
        kI420,
        kNV12
    };

    enum class Rotation
    {
        kAngle0 = 0,
        kAngle90 = 90,
        kAngle180 = 180,
        kAngle270 = 270
    };

    static SharedPtr create(const VideoFrame &frame);
    static SharedPtr createI420(const uint8_t *data, int width, int height);

    virtual SharedPtr copy() = 0;

    // The resolution of the frame in pixels. For formats where some planes are
    // subsampled, this is the highest-resolution plane.
    virtual int width() const = 0;
    virtual int height() const = 0;
    virtual Format format() const = 0;

    virtual uint16_t id() const = 0;
    virtual int64_t timestamp() const = 0;
    virtual Rotation rotation() const = 0;

    // Returns pointer to the pixel data for a given plane. The memory is owned by
    // the VideoFrameBuffer object and must not be freed by the caller.
    virtual const uint8_t *dataY() const = 0;
    virtual const uint8_t *dataU() const = 0;
    virtual const uint8_t *dataV() const = 0;

    // Returns the number of bytes between successive rows for a given plane.
    virtual int strideY() const = 0;
    virtual int strideU() const = 0;
    virtual int strideV() const = 0;

    //virtual int convertToARGB(BufferType type, uint8_t *dstArgb, int dstStrideArgb, int dstWidth, int dstHeight) = 0;

protected:
    virtual ~RtcVideoFrame() { }
};

using RtcVideoSink = Sink<SharedPointer<RtcVideoFrame>>;
using RtcVideoSinkCallback = SinkCallback<SharedPointer<RtcVideoFrame>>;

class RtcVideoSinkAdapter : public RtcVideoSink
{
public:
    RtcVideoSinkAdapter(const SharedPointer<VideoSinkInterface<VideoFrame>> &videoFrameSink)
        : mVideoFrameSink(videoFrameSink) { };
    ~RtcVideoSinkAdapter() override = default;

    void onData(const DataType &data) override;

protected:
    SharedPointer<I420Buffer> mI420Buffer;
    SharedPointer<VideoSinkInterface<VideoFrame>> mVideoFrameSink;
};


using RtcVideoSource = Source<SharedPointer<RtcVideoFrame>>;
using RtcVideoProvider = SourceProvider<SharedPointer<RtcVideoFrame>>;
using RtcVideoBroadcaster = SourceBroadcaster<SharedPointer<RtcVideoFrame>>;

class RtcVideoGeneratorPrivate;
class OCTK_MEDIA_API RtcVideoGenerator : public RtcVideoProvider
{
public:
    OCTK_DEFINE_SHARED_PTR(RtcVideoGenerator);

    ~RtcVideoGenerator() override;

    static SharedPtr create(FrameGeneratorInterface::UniquePtr generator, int fps, StringView name = "");
    static SharedPtr createSquareGenerator(int width, int height, int numSquares, int fps, StringView name = "");

    int fps() const;
    int width() const;
    int height() const;

    VideoSourceInterface<VideoFrame> *source() const;

protected:
    RtcVideoGenerator(StringView name);

private:
    OCTK_DEFINE_DPTR(RtcVideoGenerator)
    OCTK_DECLARE_PRIVATE(RtcVideoGenerator)
    OCTK_DISABLE_COPY_MOVE(RtcVideoGenerator)
};

class RtcVideoCapturePrivate;
class OCTK_MEDIA_API RtcVideoCapture : public RtcVideoProvider
{
public:
    OCTK_DEFINE_SHARED_PTR(RtcVideoCapture);

    ~RtcVideoCapture() override;

    static SharedPtr create(const CameraCapture::SharedPtr &capture, StringView name = "");

    VideoSourceInterface<VideoFrame> *source();

protected:
    RtcVideoCapture(StringView name);

private:
    OCTK_DEFINE_DPTR(RtcVideoCapture)
    OCTK_DECLARE_PRIVATE(RtcVideoCapture)
    OCTK_DISABLE_COPY_MOVE(RtcVideoCapture)
};

OCTK_END_NAMESPACE
