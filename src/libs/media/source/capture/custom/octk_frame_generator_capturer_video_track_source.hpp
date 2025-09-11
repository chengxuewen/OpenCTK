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

#ifndef _OCTK_FRAME_GENERATOR_CAPTURER_VIDEO_TRACK_SOURCE_HPP
#define _OCTK_FRAME_GENERATOR_CAPTURER_VIDEO_TRACK_SOURCE_HPP

#include <octk_frame_generator_capturer.hpp>
#include <octk_create_frame_generator.hpp>
#include <octk_task_queue_factory.hpp>
#include <octk_video_track_source.hpp>
#include <octk_memory.hpp>

#include <utility>
#include <memory>

OCTK_BEGIN_NAMESPACE
#if 0
// Implements a VideoTrackSourceInterface to be used for creating VideoTracks.
// The video source is generated using a FrameGeneratorCapturer, specifically
// a SquareGenerator that generates frames with randomly sized and colored
// squares.
class FrameGeneratorCapturerVideoTrackSourcePrivate;
class FrameGeneratorCapturerVideoTrackSource : public VideoTrackSource
{
public:
    static const int kDefaultFramesPerSecond = 30;
    static const int kDefaultWidth = 640;
    static const int kDefaultHeight = 480;
    static const int kNumSquaresGenerated = 50;

    struct Config
    {
        int frames_per_second = kDefaultFramesPerSecond;
        int width = kDefaultWidth;
        int height = kDefaultHeight;
        int num_squares_generated = 50;
    };

    FrameGeneratorCapturerVideoTrackSource(Config config, Clock *clock, bool is_screencast)
        : VideoTrackSource(false /* remote */)
        , task_queue_factory_(utils::createDefaultTaskQueueFactory())
        , is_screencast_(is_screencast)
    {
        video_capturer_ =
            utils::make_unique<FrameGeneratorCapturer>(clock,
                                                       utils::CreateSquareFrameGenerator(config.width,
                                                                                         config.height,
                                                                                         utils::nullopt,
                                                                                         config.num_squares_generated),
                                                       config.frames_per_second,
                                                       *task_queue_factory_);
        video_capturer_->init();
    }

    FrameGeneratorCapturerVideoTrackSource(std::unique_ptr<FrameGeneratorInterface> frameGenerator,
                                           int targetFps,
                                           Clock *clock,
                                           bool is_screencast)
        : VideoTrackSource(false /* remote */)
        , task_queue_factory_(utils::createDefaultTaskQueueFactory())
        , is_screencast_(is_screencast)
    {
        video_capturer_ = utils::make_unique<FrameGeneratorCapturer>(clock,
                                                                     std::move(frameGenerator),
                                                                     targetFps,
                                                                     *task_queue_factory_);
        video_capturer_->init();
    }

    FrameGeneratorCapturerVideoTrackSource(std::unique_ptr<FrameGeneratorCapturer> video_capturer, bool is_screencast)
        : VideoTrackSource(false /* remote */)
        , video_capturer_(std::move(video_capturer))
        , is_screencast_(is_screencast)
    {
    }

    ~FrameGeneratorCapturerVideoTrackSource() = default;

    void Start()
    {
        this->setState(kLive);
        video_capturer_->start();
    }

    void Stop()
    {
        this->setState(kMuted);
        video_capturer_->stop();
    }

    bool isScreencast() const override { return is_screencast_; }

protected:
    VideoSourceInterface<VideoFrame> *source() override { return video_capturer_.get(); }

private:
    OCTK_DEFINE_DPTR(FrameGeneratorCapturerVideoTrackSource)
    const std::unique_ptr<TaskQueueFactory> task_queue_factory_;
    std::unique_ptr<FrameGeneratorCapturer> video_capturer_;
    const bool is_screencast_;
};
#endif
OCTK_END_NAMESPACE

#endif // _OCTK_FRAME_GENERATOR_CAPTURER_VIDEO_TRACK_SOURCE_HPP
