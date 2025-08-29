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

#ifndef _OCTK_FRAME_GENERATOR_CAPTURER_HPP
#define _OCTK_FRAME_GENERATOR_CAPTURER_HPP

#include <octk_video_source_interface.hpp>
#include <octk_custom_video_capturer.hpp>
#include <octk_task_queue_factory.hpp>
#include <octk_video_track_source.hpp>
#include <octk_video_broadcaster.hpp>
#include <octk_frame_generator.hpp>
#include <octk_repeating_task.hpp>
#include <octk_video_adapter.hpp>
#include <octk_video_frame.hpp>
#include <octk_optional.hpp>
#include <octk_memory.hpp>
#include <octk_result.hpp>
#include <octk_mutex.hpp>
#include <octk_clock.hpp>

#include <cstddef>
#include <cstdint>

OCTK_BEGIN_NAMESPACE

class FrameGeneratorCapturer : public CustomVideoCapturer
{
public:
    class SinkWantsObserver
    {
    public:
        // OnSinkWantsChanged is called when FrameGeneratorCapturer::AddOrUpdateSink is called.
        virtual void OnSinkWantsChanged(VideoSinkInterface<VideoFrame> *sink, const VideoSinkWants &wants) = 0;

    protected:
        virtual ~SinkWantsObserver() { }
    };

    FrameGeneratorCapturer(Clock *clock,
                           std::unique_ptr<FrameGeneratorInterface> frame_generator,
                           int target_fps,
                           TaskQueueFactory &task_queue_factory);
    virtual ~FrameGeneratorCapturer();

    void start() override;
    void stop() override;
    void changeResolution(size_t width, size_t height);
    void changeFramerate(int target_framerate);

    int getFrameWidth() const override;
    int getFrameHeight() const override;

    struct Resolution
    {
        int width;
        int height;
    };
    Optional<Resolution> getResolution() const;

    void onOutputFormatRequest(int width, int height, const Optional<int> &max_fps);

    void setSinkWantsObserver(SinkWantsObserver *observer);

    void addOrUpdateSink(VideoSinkInterface<VideoFrame> *sink, const VideoSinkWants &wants) override;
    void removeSink(VideoSinkInterface<VideoFrame> *sink) override;

    void forceFrame();
    void setFakeRotation(VideoRotation rotation);
    void setFakeColorSpace(Optional<ColorSpace> color_space);

    bool init();

private:
    void insertFrame();
    static bool run(void *obj);
    int getCurrentConfiguredFramerate();

    bool mSending;
    Clock *const mClock;
    RepeatingTaskHandle mFrameTask;
    SinkWantsObserver *mSinkWantsObserver OCTK_ATTRIBUTE_GUARDED_BY(&mMutex);

    Mutex mMutex;
    std::unique_ptr<FrameGeneratorInterface> mFrameGenerator;

    int mSourceFps OCTK_ATTRIBUTE_GUARDED_BY(&mMutex);
    int mTargetCaptureFps OCTK_ATTRIBUTE_GUARDED_BY(&mMutex);
    VideoRotation mFakeRotation = VideoRotation::Angle0;
    Optional<ColorSpace> mFakeColorSpace OCTK_ATTRIBUTE_GUARDED_BY(&mMutex);

    std::unique_ptr<TaskQueue, TaskQueueDeleter> task_queue_;
};

/**
 * @brief Implements a VideoTrackSourceInterface to be used for creating VideoTracks.
 * @details The video source is generated using a FrameGeneratorCapturer, specifically a SquareGenerator
 * that generates frames with randomly sized and colored squares.
 */
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

    FrameGeneratorCapturerVideoTrackSource(Config config, Clock *clock, bool isScreenCast);
    FrameGeneratorCapturerVideoTrackSource(std::unique_ptr<FrameGeneratorInterface> frameGenerator,
                                           int targetFps,
                                           Clock *clock,
                                           bool isScreenCast);
    FrameGeneratorCapturerVideoTrackSource(std::unique_ptr<FrameGeneratorCapturer> video_capturer, bool isScreenCast);
    ~FrameGeneratorCapturerVideoTrackSource() override;

    ResultS start();
    void stop();

    bool isScreencast() const override;

protected:
    VideoSourceInterface<VideoFrame> *source() override;

private:
    OCTK_DEFINE_DPTR(FrameGeneratorCapturerVideoTrackSource)
    OCTK_DECLARE_PRIVATE(FrameGeneratorCapturerVideoTrackSource)
    OCTK_DISABLE_COPY_MOVE(FrameGeneratorCapturerVideoTrackSource)
};

OCTK_END_NAMESPACE

#endif // _OCTK_FRAME_GENERATOR_CAPTURER_HPP
