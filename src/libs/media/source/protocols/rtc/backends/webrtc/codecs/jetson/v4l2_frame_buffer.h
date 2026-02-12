#pragma once

#include "v4l2_utils.h"

#include <common_video/include/video_frame_buffer.h>
#include <rtc_base/memory/aligned_malloc.h>
#include <api/video/i420_buffer.h>
#include <api/video/video_frame.h>

// #include <linux/videodev2.h>
// #include <vector>

class V4L2FrameBuffer : public webrtc::VideoFrameBuffer
{
public:
    static rtc::scoped_refptr<V4L2FrameBuffer> Create(int width, int height, int size, uint32_t format);
    static rtc::scoped_refptr<V4L2FrameBuffer> Create(int width, int height, V4L2Buffer buffer);

    Type type() const override;
    int width() const override;
    int height() const override;
    rtc::scoped_refptr<webrtc::I420BufferInterface> ToI420() override;
    rtc::scoped_refptr<webrtc::I420BufferInterface> ToI420TST() { return ToI420(); }

    uint32_t format() const;
    uint32_t size() const;
    uint32_t flags() const;
    timeval timestamp() const;

    const void *Data() const;
    uint8_t *MutableData();
    V4L2Buffer GetRawBuffer();
    int GetDmaFd() const;
    bool SetDmaFd(int fd);
    void SetTimestamp(timeval timestamp);
    rtc::scoped_refptr<V4L2FrameBuffer> Clone() const;

protected:
    V4L2FrameBuffer(int width, int height, int size, uint32_t format);
    V4L2FrameBuffer(int width, int height, V4L2Buffer buffer);
    ~V4L2FrameBuffer() override;

private:
    const int width_;
    const int height_;
    const uint32_t format_;
    uint32_t size_;
    uint32_t flags_;
    bool has_mutable_data_;
    timeval timestamp_;
    V4L2Buffer buffer_;
    const std::unique_ptr<uint8_t, webrtc::AlignedFreeDeleter> data_;
};

using V4L2FrameBufferRef = rtc::scoped_refptr<V4L2FrameBuffer>;

class IFrameProcessor
{
public:
    virtual ~IFrameProcessor() = default;

    /**
     * Submits a frame buffer to a processing unit (e.g., ISP, encoder).
     *
     * This method enqueues the given frame buffer into the hardware pipeline. The callback will be
     * triggered with the resulting buffer when processing is completed.
     *
     * @param frame_buffer Frame buffer to be processed by the device.
     * @param on_capture Callback invoked with the resulting frame buffer.
     */
    virtual void EmplaceBuffer(V4L2FrameBufferRef frame_buffer, std::function<void(V4L2FrameBufferRef)> on_capture) = 0;
};
