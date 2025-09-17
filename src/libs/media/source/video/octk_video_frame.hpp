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

#ifndef _OCTK_VIDEO_FRAME_HPP
#define _OCTK_VIDEO_FRAME_HPP

#include <octk_video_frame_buffer.hpp>
#include <octk_rtp_packet_infos.hpp>
#include <octk_video_rotation.hpp>
#include <octk_media_global.hpp>
#include <octk_video_codec.hpp>
#include <octk_color_space.hpp>
#include <octk_timestamp.hpp>
#include <octk_optional.hpp>

OCTK_BEGIN_NAMESPACE

class VideoFramePrivate;

class OCTK_MEDIA_API VideoFrame : public std::enable_shared_from_this<VideoFrame>
{
public:
    static constexpr uint16_t kNotSetId = 0;

    struct OCTK_MEDIA_API UpdateRect
    {
        int width = 0;
        int height = 0;
        int offsetX = 0;
        int offsetY = 0;

        UpdateRect() = default;
        UpdateRect(int w, int h, int x, int y)
            : width(w)
            , height(h)
            , offsetX(x)
            , offsetY(y)
        {
        }
        /**
         * @brief Makes this UpdateRect a bounding box of this and other rect.
         */
        void unionRect(const UpdateRect &other);

        // Makes this UpdateRect an intersection of this and other rect.
        void intersectRect(const UpdateRect &other);

        // Sets everything to 0, making this UpdateRect a zero-size (empty) update.
        void makeEmptyUpdate();

        bool isEmpty() const;

        // Per-member equality check. Empty rectangles with different offsets would
        // be considered different.
        bool operator==(const UpdateRect &other) const
        {
            return other.width == width && other.height == height && other.offsetX == offsetX &&
                   other.offsetY == offsetY;
        }

        bool operator!=(const UpdateRect &other) const { return !(*this == other); }

        // Scales updateRect given original frame dimensions.
        // Cropping is applied first, then rect is scaled down.
        // Update rect is snapped to 2x2 grid due to possible UV subsampling and
        // then expanded by additional 2 pixels in each direction to accommodate any
        // possible scaling artifacts.
        // Note, close but not equal update_rects on original frame may result in
        // the same scaled update rects.
        UpdateRect scaleWithFrame(int frameWidth,
                                  int frameHeight,
                                  int cropX,
                                  int cropY,
                                  int cropWidth,
                                  int cropHeight,
                                  int scaledWidth,
                                  int scaledHeight) const;
    };

    struct OCTK_MEDIA_API ProcessingTime
    {
        TimeDelta elapsed() const { return finish - start; }
        Timestamp start;
        Timestamp finish;
    };

    struct OCTK_MEDIA_API RenderParameters
    {
        bool useLowLatencyRendering = false;
        Optional<int32_t> maxCompositionDelayInFrames;

        bool operator==(const RenderParameters &other) const
        {
            return other.useLowLatencyRendering == useLowLatencyRendering &&
                   other.maxCompositionDelayInFrames == maxCompositionDelayInFrames;
        }

        bool operator!=(const RenderParameters &other) const { return !(*this == other); }
    };

    // Preferred way of building VideoFrame objects.
    class OCTK_MEDIA_API Builder
    {
    public:
        Builder();
        ~Builder();

        VideoFrame build();
        Builder &setVideoFrameBuffer(const std::shared_ptr<VideoFrameBuffer> &buffer);
        Builder &setTimestampMSecs(int64_t timestamp_ms);
        Builder &setTimestampUSecs(int64_t timestampUSecs);
        Builder &setCaptureTimeIdentifier(const Optional<Timestamp> &presentationTimestamp)
        {
            mPresentationTimestamp = presentationTimestamp;
            return *this;
        }
        Builder &setPresentationTimestamp(const Optional<Timestamp> &presentationTimestamp)
        {
            mPresentationTimestamp = presentationTimestamp;
            return *this;
        }
        Builder &setReferenceTime(const Optional<Timestamp> &referenceTime)
        {
            mReferenceTime = referenceTime;
            return *this;
        }
        Builder &setRtpTimestamp(uint32_t rtp_timestamp);
        Builder &setNtpTimeMSecs(int64_t ntp_time_ms);
        Builder &setRotation(VideoRotation rotation);
        Builder &setColorSpace(const Optional<ColorSpace> &colorSpace)
        {
            mColorSpace = colorSpace;
            return *this;
        }
        Builder &setColorSpace(const ColorSpace *colorSpace);
        Builder &setId(uint16_t id);
        Builder &setUpdateRect(const Optional<UpdateRect> &updateRect)
        {
            mUpdateRect = updateRect;
            return *this;
        }
        Builder &setPacketInfos(RtpPacketInfos packet_infos);

    private:
        uint16_t mId = kNotSetId;
        std::shared_ptr<VideoFrameBuffer> mVideoFrameBuffer;
        int64_t mTimestampUSecs = 0;
        Optional<Timestamp> mPresentationTimestamp;
        Optional<Timestamp> mReferenceTime;
        uint32_t mRtpTimestamp = 0;
        int64_t mNtpTimeMSecs = 0;
        VideoRotation mRotation = VideoRotation::Angle0;
        Optional<ColorSpace> mColorSpace;
        RenderParameters mRenderParameters;
        Optional<UpdateRect> mUpdateRect;
        RtpPacketInfos mPacketInfos;
    };

    // To be deprecated. Migrate all use to Builder.
    //     explicit VideoFrame(VideoFramePrivate *d);
    VideoFrame(const std::shared_ptr<VideoFrameBuffer> &buffer, VideoRotation rotation, int64_t timestampUSecs);
    VideoFrame(const std::shared_ptr<VideoFrameBuffer> &buffer,
               uint32_t rtpTimestamp,
               int64_t renderTimeMsecs,
               VideoRotation rotation);
    VideoFrame(uint16_t id,
               const std::shared_ptr<VideoFrameBuffer> &buffer,
               int64_t timestampUSecs,
               const Optional<Timestamp> &presentationTimestamp,
               const Optional<Timestamp> &referenceTime,
               uint32_t rtpTimestamp,
               int64_t ntpTimeMsecs,
               VideoRotation rotation,
               const Optional<ColorSpace> &colorSpace,
               const RenderParameters &renderParameters,
               const Optional<UpdateRect> &updateRect,
               RtpPacketInfos packetInfos);
    virtual ~VideoFrame();

    static VideoFrame copy(const VideoFrame &other);

    int width() const;
    int height() const;
    uint32_t size() const; // Get frame size in pixels.

    // Get frame ID. Returns `kNotSetId` if ID is not set. Not guaranteed to be
    // transferred from the sender to the receiver, but preserved on the sender
    // side. The id should be propagated between all frame modifications during
    // its lifetime from capturing to sending as encoded image. It is intended to
    // be unique over a time window of a few minutes for the peer connection to
    // which the corresponding video stream belongs to.
    uint16_t id() const { return mId; }
    void setId(uint16_t id) { mId = id; }

    // System monotonic clock, same timebase as rtc::TimeMicros().
    int64_t timestampUSecs() const { return mTimestampUSecs; }
    void setTimestampUSecs(int64_t timestampUSecs) { mTimestampUSecs = timestampUSecs; }

    // TODO(https://bugs.webrtc.org/373365537): Remove this once its usage is
    // removed from blink.
    const Optional<Timestamp> &captureTimeIdentifier() const { return mPresentationTimestamp; }

    const Optional<Timestamp> &presentationTimestamp() const { return mPresentationTimestamp; }

    void setPresentationTimestamp(const Optional<Timestamp> &presentationTimestamp)
    {
        mPresentationTimestamp = presentationTimestamp;
    }

    const Optional<Timestamp> &referenceTime() const { return mReferenceTime; }
    void setReferenceTime(const Optional<Timestamp> &referenceTime) { mReferenceTime = referenceTime; }

    // Set frame timestamp (90kHz).
    void setRtpTimestamp(uint32_t rtpTimestamp) { mRtpTimestamp = rtpTimestamp; }

    // Get frame timestamp (90kHz).
    uint32_t rtpTimestamp() const { return mRtpTimestamp; }

    // Set capture ntp time in milliseconds.
    void setNtpTimeMSecs(int64_t ntpTimeMSecs) { mNtpTimeMSecs = ntpTimeMSecs; }

    // Get capture ntp time in milliseconds.
    int64_t ntpTimeMSecs() const { return mNtpTimeMSecs; }

    // Naming convention for Coordination of Video Orientation. Please see
    // http://www.etsi.org/deliver/etsi_ts/126100_126199/126114/12.07.00_60/ts_126114v120700p.pdf
    //
    // "pending rotation" or "pending" = a frame that has a VideoRotation > 0.
    //
    // "not pending" = a frame that has a VideoRotation == 0.
    //
    // "apply rotation" = modify a frame from being "pending" to being "not
    //                    pending" rotation (a no-op for "unrotated").
    //
    VideoRotation rotation() const { return mRotation; }
    void setRotation(VideoRotation rotation) { mRotation = rotation; }

    // Get color space when available.
    const Optional<ColorSpace> &colorSpace() const { return mColorSpace; }
    void setColorSpace(const Optional<ColorSpace> &colorSpace) { mColorSpace = colorSpace; }

    RenderParameters renderParameters() const { return mRenderParameters; }
    void setRenderParameters(const RenderParameters &renderParameters) { mRenderParameters = renderParameters; }

    // Get render time in milliseconds.
    int64_t renderTimeMSecs() const;

    // Return the underlying buffer. Never nullptr for a properly initialized VideoFrame.
    std::shared_ptr<VideoFrameBuffer> videoFrameBuffer() const;
    void setVideoFrameBuffer(const std::shared_ptr<VideoFrameBuffer> &buffer);

    // Return true if the frame is stored in a texture.
    bool isTexture() const { return videoFrameBuffer()->type() == VideoFrameBuffer::Type::kNative; }

    bool hasUpdateRect() const { return mUpdateRect.has_value(); }

    // Returns updateRect set by the builder or setUpdateRect() or whole frame rect if no update rect is available.
    UpdateRect updateRect() const { return mUpdateRect.value_or(UpdateRect{0, 0, this->width(), this->height()}); }
    // Rectangle must be within the frame dimensions.
    void setUpdateRect(const VideoFrame::UpdateRect &updateRect)
    {
        OCTK_DCHECK_GE(updateRect.offsetX, 0);
        OCTK_DCHECK_GE(updateRect.offsetY, 0);
        OCTK_DCHECK_LE(updateRect.offsetX + updateRect.width, this->width());
        OCTK_DCHECK_LE(updateRect.offsetY + updateRect.height, this->height());
        mUpdateRect = updateRect;
    }

    void clearUpdateRect() { mUpdateRect = utils::nullopt; }

    // Get information about packets used to assemble this video frame.
    // Might be empty if the information isn't available.
    const RtpPacketInfos &packetInfos() const { return mPacketInfos; }
    void setPacketInfos(RtpPacketInfos value) { mPacketInfos = std::move(value); }

    const Optional<ProcessingTime> processingTime() const { return mProcessingTime; }
    void setProcessingTime(const ProcessingTime &processingTime) { mProcessingTime = processingTime; }

private:
    uint16_t mId;
    // An opaque reference counted handle that stores the pixel data.
    std::shared_ptr<VideoFrameBuffer> mVideoFrameBuffer;
    uint32_t mRtpTimestamp;
    int64_t mNtpTimeMSecs;
    int64_t mTimestampUSecs;
    Optional<Timestamp> mPresentationTimestamp;
    // Contains a monotonically increasing clock time and represents the time
    // when the frame was captured. Not all platforms provide the "true" sample
    // capture time in |reference_time| but might instead use a somewhat delayed
    // (by the time it took to capture the frame) version of it.
    Optional<Timestamp> mReferenceTime;
    VideoRotation mRotation;
    Optional<ColorSpace> mColorSpace;
    // Contains parameters that affect have the frame should be rendered.
    RenderParameters mRenderParameters;
    // Updated since the last frame area. If present it means that the bounding
    // box of all the changes is within the rectangular area and is close to it.
    // If absent, it means that there's no information about the change at all and
    // updateRect() will return a rectangle corresponding to the entire frame.
    Optional<UpdateRect> mUpdateRect;
    // Information about packets used to assemble this video frame. This is needed
    // by `SourceTracker` when the frame is delivered to the RTCRtpReceiver's
    // MediaStreamTrack, in order to implement getContributingSources(). See:
    // https://w3c.github.io/webrtc-pc/#dom-rtcrtpreceiver-getcontributingsources
    RtpPacketInfos mPacketInfos;
    // Processing timestamps of the frame. For received video frames these are the
    // timestamps when the frame is sent to the decoder and the decoded image
    // returned from the decoder.
    // Currently, not set for locally captured video frames.
    Optional<ProcessingTime> mProcessingTime;

protected:
    // OCTK_DEFINE_DPTR(VideoFrame)
    // OCTK_DECLARE_PRIVATE(VideoFrame)
    // OCTK_DISABLE_COPY_MOVE(VideoFrame)
};
OCTK_END_NAMESPACE

#endif // _OCTK_VIDEO_FRAME_HPP
