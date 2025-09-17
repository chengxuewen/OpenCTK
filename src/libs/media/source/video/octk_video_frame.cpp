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

#include <octk_video_frame.hpp>
#include <octk_date_time.hpp>

#include <algorithm>
#include <cstdint>
#include <optional>
#include <utility>

OCTK_BEGIN_NAMESPACE

class VideoFramePrivate
{
public:
    VideoFramePrivate(VideoFrame *p);
    virtual ~VideoFramePrivate();

protected:
    OCTK_DEFINE_PPTR(VideoFrame)
    OCTK_DECLARE_PUBLIC(VideoFrame)

    OCTK_DISABLE_COPY_MOVE(VideoFramePrivate)
};

VideoFramePrivate::VideoFramePrivate(VideoFrame *p)
    : mPPtr(p)
{
}

VideoFramePrivate::~VideoFramePrivate() { }

// VideoFrame::VideoFrame(VideoFramePrivate *d)
//     : mDPtr(d)
// {
// }

VideoFrame::~VideoFrame() { }

void VideoFrame::UpdateRect::unionRect(const UpdateRect &other)
{
    if (other.isEmpty())
    {
        return;
    }
    if (isEmpty())
    {
        *this = other;
        return;
    }
    int right = std::max(offsetX + width, other.offsetX + other.width);
    int bottom = std::max(offsetY + height, other.offsetY + other.height);
    offsetX = std::min(offsetX, other.offsetX);
    offsetY = std::min(offsetY, other.offsetY);
    width = right - offsetX;
    height = bottom - offsetY;
    OCTK_DCHECK_GT(width, 0);
    OCTK_DCHECK_GT(height, 0);
}

void VideoFrame::UpdateRect::intersectRect(const UpdateRect &other)
{
    if (other.isEmpty() || isEmpty())
    {
        makeEmptyUpdate();
        return;
    }

    int right = std::min(offsetX + width, other.offsetX + other.width);
    int bottom = std::min(offsetY + height, other.offsetY + other.height);
    offsetX = std::max(offsetX, other.offsetX);
    offsetY = std::max(offsetY, other.offsetY);
    width = right - offsetX;
    height = bottom - offsetY;
    if (width <= 0 || height <= 0)
    {
        makeEmptyUpdate();
    }
}

void VideoFrame::UpdateRect::makeEmptyUpdate() { width = height = offsetX = offsetY = 0; }

bool VideoFrame::UpdateRect::isEmpty() const { return width == 0 && height == 0; }

VideoFrame::UpdateRect VideoFrame::UpdateRect::scaleWithFrame(int frame_width,
                                                              int frame_height,
                                                              int crop_x,
                                                              int crop_y,
                                                              int cropWidth,
                                                              int cropHeight,
                                                              int scaledWidth,
                                                              int scaledHeight) const
{
    OCTK_DCHECK_GT(frame_width, 0);
    OCTK_DCHECK_GT(frame_height, 0);

    OCTK_DCHECK_GT(cropWidth, 0);
    OCTK_DCHECK_GT(cropHeight, 0);

    OCTK_DCHECK_LE(cropWidth + crop_x, frame_width);
    OCTK_DCHECK_LE(cropHeight + crop_y, frame_height);

    OCTK_DCHECK_GT(scaledWidth, 0);
    OCTK_DCHECK_GT(scaledHeight, 0);

    // Check if update rect is out of the cropped area.
    if (offsetX + width < crop_x || offsetX > crop_x + cropWidth || offsetY + height < crop_y ||
        offsetY > crop_y + cropWidth)
    {
        return {0, 0, 0, 0};
    }

    int x = offsetX - crop_x;
    int w = width;
    if (x < 0)
    {
        w += x;
        x = 0;
    }
    int y = offsetY - crop_y;
    int h = height;
    if (y < 0)
    {
        h += y;
        y = 0;
    }

    // Lower corner is rounded down.
    x = x * scaledWidth / cropWidth;
    y = y * scaledHeight / cropHeight;
    // Upper corner is rounded up.
    w = (w * scaledWidth + cropWidth - 1) / cropWidth;
    h = (h * scaledHeight + cropHeight - 1) / cropHeight;

    // Round to full 2x2 blocks due to possible subsampling in the pixel data.
    if (x % 2)
    {
        --x;
        ++w;
    }
    if (y % 2)
    {
        --y;
        ++h;
    }
    if (w % 2)
    {
        ++w;
    }
    if (h % 2)
    {
        ++h;
    }

    // Expand the update rect by 2 pixels in each direction to include any
    // possible scaling artifacts.
    if (scaledWidth != cropWidth || scaledHeight != cropHeight)
    {
        if (x > 0)
        {
            x -= 2;
            w += 2;
        }
        if (y > 0)
        {
            y -= 2;
            h += 2;
        }
        w += 2;
        h += 2;
    }

    // Ensure update rect is inside frame dimensions.
    if (x + w > scaledWidth)
    {
        w = scaledWidth - x;
    }
    if (y + h > scaledHeight)
    {
        h = scaledHeight - y;
    }
    OCTK_DCHECK_GE(w, 0);
    OCTK_DCHECK_GE(h, 0);
    if (w == 0 || h == 0)
    {
        w = 0;
        h = 0;
        x = 0;
        y = 0;
    }

    return {x, y, w, h};
}

VideoFrame::Builder::Builder() = default;

VideoFrame::Builder::~Builder() = default;

VideoFrame VideoFrame::Builder::build()
{
    OCTK_CHECK(mVideoFrameBuffer != nullptr);
    return VideoFrame(mId,
                      mVideoFrameBuffer,
                      mTimestampUSecs,
                      mPresentationTimestamp,
                      mReferenceTime,
                      mRtpTimestamp,
                      mNtpTimeMSecs,
                      mRotation,
                      mColorSpace,
                      mRenderParameters,
                      mUpdateRect,
                      mPacketInfos);
}

VideoFrame::Builder &VideoFrame::Builder::setVideoFrameBuffer(const std::shared_ptr<VideoFrameBuffer> &buffer)
{
    mVideoFrameBuffer = buffer;
    return *this;
}

VideoFrame::Builder &VideoFrame::Builder::setTimestampMSecs(int64_t timestamp_ms)
{
    mTimestampUSecs = timestamp_ms * DateTime::kUSecsPerMSec;
    return *this;
}

VideoFrame::Builder &VideoFrame::Builder::setTimestampUSecs(int64_t timestampUSecs)
{
    mTimestampUSecs = timestampUSecs;
    return *this;
}

VideoFrame::Builder &VideoFrame::Builder::setRtpTimestamp(uint32_t rtp_timestamp)
{
    mRtpTimestamp = rtp_timestamp;
    return *this;
}

VideoFrame::Builder &VideoFrame::Builder::setNtpTimeMSecs(int64_t ntpTimeMSecs)
{
    mNtpTimeMSecs = ntpTimeMSecs;
    return *this;
}

VideoFrame::Builder &VideoFrame::Builder::setRotation(VideoRotation rotation)
{
    mRotation = rotation;
    return *this;
}

VideoFrame::Builder &VideoFrame::Builder::setColorSpace(const ColorSpace *colorSpace)
{
    mColorSpace = colorSpace ? utils::make_optional(*colorSpace) : utils::nullopt;
    return *this;
}

VideoFrame::Builder &VideoFrame::Builder::setId(uint16_t id)
{
    mId = id;
    return *this;
}

VideoFrame::Builder &VideoFrame::Builder::setPacketInfos(RtpPacketInfos packetInfos)
{
    mPacketInfos = std::move(packetInfos);
    return *this;
}

VideoFrame::VideoFrame(const std::shared_ptr<VideoFrameBuffer> &buffer, VideoRotation rotation, int64_t timestampUSecs)
    : mVideoFrameBuffer(buffer)
    , mRtpTimestamp(0)
    , mNtpTimeMSecs(0)
    , mTimestampUSecs(timestampUSecs)
    , mRotation(rotation)
{
}

VideoFrame::VideoFrame(const std::shared_ptr<VideoFrameBuffer> &buffer,
                       uint32_t rtpTimestamp,
                       int64_t renderTimeMSecs,
                       VideoRotation rotation)
    : mVideoFrameBuffer(buffer)
    , mRtpTimestamp(rtpTimestamp)
    , mNtpTimeMSecs(0)
    , mTimestampUSecs(renderTimeMSecs * DateTime::kUSecsPerMSec)
    , mRotation(rotation)
{
    OCTK_DCHECK(buffer);
}

VideoFrame VideoFrame::copy(const VideoFrame &other)
{
    return VideoFrame(other.id(),
                      other.videoFrameBuffer(),
                      other.timestampUSecs(),
                      other.presentationTimestamp(),
                      other.referenceTime(),
                      other.rtpTimestamp(),
                      other.ntpTimeMSecs(),
                      other.rotation(),
                      other.colorSpace(),
                      other.renderParameters(),
                      other.updateRect(),
                      other.packetInfos());
}

VideoFrame::VideoFrame(uint16_t id,
                       const std::shared_ptr<VideoFrameBuffer> &buffer,
                       int64_t timestampUSecs,
                       const Optional<Timestamp> &presentationTimestamp,
                       const Optional<Timestamp> &referenceTime,
                       uint32_t rtpTimestamp,
                       int64_t ntpTimeMSecs,
                       VideoRotation rotation,
                       const Optional<ColorSpace> &colorSpace,
                       const RenderParameters &renderParameters,
                       const Optional<UpdateRect> &updateRect,
                       RtpPacketInfos packetInfos)
    : mId(id)
    , mVideoFrameBuffer(buffer)
    , mRtpTimestamp(rtpTimestamp)
    , mNtpTimeMSecs(ntpTimeMSecs)
    , mTimestampUSecs(timestampUSecs)
    , mPresentationTimestamp(presentationTimestamp)
    , mReferenceTime(referenceTime)
    , mRotation(rotation)
    , mColorSpace(colorSpace)
    , mRenderParameters(renderParameters)
    , mUpdateRect(updateRect)
    , mPacketInfos(std::move(packetInfos))
{
    if (mUpdateRect)
    {
        OCTK_DCHECK_GE(mUpdateRect->offsetX, 0);
        OCTK_DCHECK_GE(mUpdateRect->offsetY, 0);
        OCTK_DCHECK_LE(mUpdateRect->offsetX + mUpdateRect->width, width());
        OCTK_DCHECK_LE(mUpdateRect->offsetY + mUpdateRect->height, height());
    }
}

int VideoFrame::width() const { return mVideoFrameBuffer ? mVideoFrameBuffer->width() : 0; }

int VideoFrame::height() const { return mVideoFrameBuffer ? mVideoFrameBuffer->height() : 0; }

uint32_t VideoFrame::size() const { return width() * height(); }

std::shared_ptr<VideoFrameBuffer> VideoFrame::videoFrameBuffer() const { return mVideoFrameBuffer; }

void VideoFrame::setVideoFrameBuffer(const std::shared_ptr<VideoFrameBuffer> &buffer)
{
    OCTK_CHECK(buffer);
    mVideoFrameBuffer = buffer;
}

int64_t VideoFrame::renderTimeMSecs() const { return timestampUSecs() / DateTime::kUSecsPerMSec; }

OCTK_END_NAMESPACE
