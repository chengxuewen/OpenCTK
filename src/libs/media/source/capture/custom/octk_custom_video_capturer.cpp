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

#include <octk_custom_video_capturer.hpp>
#include <octk_video_frame_buffer.hpp>
#include <octk_video_rotation.hpp>
#include <octk_i420_buffer.hpp>

#include <algorithm>

OCTK_BEGIN_NAMESPACE

CustomVideoCapturer::~CustomVideoCapturer() = default;

void CustomVideoCapturer::onOutputFormatRequest(int width,
                                              int height,
                                              const Optional<int> &max_fps)
{
    Optional<std::pair<int, int>> target_aspect_ratio = std::make_pair(width, height);
    Optional<int> max_pixel_count = width * height;
    video_adapter_.onOutputFormatRequest(target_aspect_ratio, max_pixel_count, max_fps);
}

void CustomVideoCapturer::onFrame(const VideoFrame &original_frame)
{
    int cropped_width = 0;
    int cropped_height = 0;
    int out_width = 0;
    int out_height = 0;

    VideoFrame frame = this->maybePreprocess(original_frame);

    bool enable_adaptation;
    {
        Mutex::Locker locker(&lock_);
        enable_adaptation = enable_adaptation_;
    }
    if (!enable_adaptation)
    {
        broadcaster_.onFrame(frame);
        return;
    }

    if (!video_adapter_.adaptFrameResolution(frame.width(), frame.height(), frame.timestampUSecs() * 1000,
                                             &cropped_width, &cropped_height, &out_width, &out_height))
    {
        // Drop frame in order to respect frame rate constraint.
        return;
    }

    if (out_height != frame.height() || out_width != frame.width())
    {
        // Video adapter has requested a down-scale. Allocate a new buffer and
        // return scaled version.
        // For simplicity, only scale here without cropping.
        std::shared_ptr<I420Buffer> scaled_buffer = I420Buffer::Create(out_width, out_height);
        scaled_buffer->scaleFrom(*frame.videoFrameBuffer()->ToI420());
        VideoFrame::Builder new_frame_builder =
            VideoFrame::Builder()
                .setVideoFrameBuffer(scaled_buffer)
                .setRotation(VideoRotation::Angle0)
                .setTimestampUSecs(frame.timestampUSecs())
                .setId(frame.id());
        if (frame.hasUpdateRect())
        {
            VideoFrame::UpdateRect new_rect = frame.updateRect().scaleWithFrame(
                frame.width(), frame.height(), 0, 0, frame.width(), frame.height(),
                out_width, out_height);
            new_frame_builder.setUpdateRect(new_rect);
        }
        broadcaster_.onFrame(new_frame_builder.build());
    }
    else
    {
        // No adaptations needed, just return the frame as is.
        broadcaster_.onFrame(frame);
    }
}

VideoSinkWants CustomVideoCapturer::getSinkWants()
{
    return broadcaster_.wants();
}

void CustomVideoCapturer::addOrUpdateSink(VideoSinkInterface<VideoFrame> *sink, const VideoSinkWants &wants)
{
    broadcaster_.addOrUpdateSink(sink, wants);
    this->updateVideoAdapter();
}

void CustomVideoCapturer::removeSink(VideoSinkInterface<VideoFrame> *sink)
{
    broadcaster_.removeSink(sink);
    this->updateVideoAdapter();
}

void CustomVideoCapturer::updateVideoAdapter()
{
    video_adapter_.OnSinkWants(broadcaster_.wants());
}

VideoFrame CustomVideoCapturer::maybePreprocess(const VideoFrame &frame)
{
    Mutex::Locker locker(&lock_);
    if (preprocessor_ != nullptr)
    {
        return preprocessor_->Preprocess(frame);
    }
    else
    {
        return frame;
    }
}
OCTK_END_NAMESPACE