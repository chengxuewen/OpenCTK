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

#include <octk_frame_generator_capturer.hpp>
#include <octk_repeating_task.hpp>

OCTK_BEGIN_NAMESPACE

FrameGeneratorCapturer::FrameGeneratorCapturer(Clock *clock,
                                               std::unique_ptr<FrameGeneratorInterface> frame_generator,
                                               int target_fps,
                                               TaskQueueFactory &task_queue_factory)
    : mClock(clock), mSending(true), mSinkWantsObserver(nullptr), mFrameGenerator(std::move(frame_generator))
    , mSourceFps(target_fps), mTargetCaptureFps(target_fps), task_queue_(task_queue_factory.CreateTaskQueue(
        "FrameGenCapQ",
        TaskQueueFactory::Priority::HIGH))
{
    OCTK_DCHECK(mFrameGenerator);
    OCTK_DCHECK_GT(target_fps, 0);
}

FrameGeneratorCapturer::~FrameGeneratorCapturer()
{
    stop();
    // Deconstruct first as tasks in the TaskQueue access other fields of the
    // instance of this class.
    task_queue_ = nullptr;
}

void FrameGeneratorCapturer::setFakeRotation(VideoRotation rotation)
{
    Mutex::Locker locker(&mMutex);
    mFakeRotation = rotation;
}

void FrameGeneratorCapturer::setFakeColorSpace(Optional<ColorSpace> color_space)
{
    Mutex::Locker locker(&mMutex);
    mFakeColorSpace = color_space;
}

bool FrameGeneratorCapturer::init()
{
    // This check is added because frame_generator_ might be file based and should
    // not crash because a file moved.
    if (mFrameGenerator.get() == nullptr)
    {
        return false;
    }

    mFrameTask = RepeatingTaskHandle::DelayedStart(
        task_queue_.get(),
        TimeDelta::Seconds(1) / getCurrentConfiguredFramerate(),
        [this] {
            insertFrame();
            return TimeDelta::Seconds(1) / getCurrentConfiguredFramerate();
        },
        TaskQueue::DelayPrecision::kHigh);
    return true;
}

void FrameGeneratorCapturer::insertFrame()
{
    Mutex::Locker locker(&mMutex);
    if (mSending)
    {
        // TODO(srte): Use more advanced frame rate control to allow arbitrary
        // fractions.
        int decimation = std::round(static_cast<double>(mSourceFps) / mTargetCaptureFps);
        for (int i = 1; i < decimation; ++i)
        {
            mFrameGenerator->skipnextFrame();
        }

        FrameGeneratorInterface::VideoFrameData frameData = mFrameGenerator->nextFrame();
        VideoFrame frame = VideoFrame::Builder()
            .setVideoFrameBuffer(frameData.buffer)
            .setRotation(mFakeRotation)
            .setTimestampUSecs(mClock->TimeInMicroseconds())
            .setUpdateRect(frameData.update_rect)
            .setColorSpace(mFakeColorSpace)
            .build();
        CustomVideoCapturer::onFrame(frame);
    }
}

Optional<FrameGeneratorCapturer::Resolution>
FrameGeneratorCapturer::getResolution() const
{
    FrameGeneratorInterface::Resolution resolution = mFrameGenerator->getResolution();
    return Resolution{.width = static_cast<int>(resolution.width), .height = static_cast<int>(resolution.height)};
}

void FrameGeneratorCapturer::start()
{
    {
        Mutex::Locker locker(&mMutex);
        mSending = true;
    }
    if (!mFrameTask.Running())
    {
        mFrameTask = RepeatingTaskHandle::Start(task_queue_.get(),
                                                [this] {
                                                     insertFrame();
                                                     return TimeDelta::Seconds(1) / getCurrentConfiguredFramerate();
                                                 },
                                                TaskQueue::DelayPrecision::kHigh);
    }
}

void FrameGeneratorCapturer::stop()
{
    Mutex::Locker locker(&mMutex);
    mSending = false;
}

void FrameGeneratorCapturer::changeResolution(size_t width, size_t height)
{
    Mutex::Locker locker(&mMutex);
    mFrameGenerator->changeResolution(width, height);
}

void FrameGeneratorCapturer::changeFramerate(int target_framerate)
{
    Mutex::Locker locker(&mMutex);
    OCTK_CHECK(mTargetCaptureFps > 0);
    if (target_framerate > mSourceFps)
        OCTK_WARNING() << "Target framerate clamped from " << target_framerate
                       << " to " << mSourceFps;
    if (mSourceFps % mTargetCaptureFps != 0)
    {
        int decimation = std::round(static_cast<double>(mSourceFps) / mTargetCaptureFps);
        int effective_rate = mTargetCaptureFps / decimation;
        OCTK_WARNING() << "Target framerate, " << target_framerate
                       << ", is an uneven fraction of the source rate, "
                       << mSourceFps
                       << ". The framerate will be :" << effective_rate;
    }
    mTargetCaptureFps = std::min(mSourceFps, target_framerate);
}

int FrameGeneratorCapturer::getFrameWidth() const
{
    return static_cast<int>(mFrameGenerator->getResolution().width);
}

int FrameGeneratorCapturer::getFrameHeight() const
{
    return static_cast<int>(mFrameGenerator->getResolution().height);
}

void FrameGeneratorCapturer::onOutputFormatRequest(int width,
                                                   int height,
                                                   const Optional<int> &max_fps)
{
    CustomVideoCapturer::onOutputFormatRequest(width, height, max_fps);
}

void FrameGeneratorCapturer::setSinkWantsObserver(SinkWantsObserver *observer)
{
    Mutex::Locker locker(&mMutex);
    OCTK_DCHECK(!mSinkWantsObserver);
    mSinkWantsObserver = observer;
}

void FrameGeneratorCapturer::addOrUpdateSink(VideoSinkInterface<VideoFrame> *sink,
                                             const VideoSinkWants &wants)
{
    CustomVideoCapturer::addOrUpdateSink(sink, wants);
    {
        Mutex::Locker locker(&mMutex);
        if (mSinkWantsObserver)
        {
            // Tests need to observe unmodified sink wants.
            mSinkWantsObserver->OnSinkWantsChanged(sink, wants);
        }
    }
    changeFramerate(this->getSinkWants().max_framerate_fps);
}

void FrameGeneratorCapturer::removeSink(VideoSinkInterface<VideoFrame> *sink)
{
    CustomVideoCapturer::removeSink(sink);
    changeFramerate(this->getSinkWants().max_framerate_fps);
}

void FrameGeneratorCapturer::forceFrame()
{
    // One-time non-repeating task,
    task_queue_->PostTask([this] { insertFrame(); });
}

int FrameGeneratorCapturer::getCurrentConfiguredFramerate()
{
    Mutex::Locker locker(&mMutex);
    return mTargetCaptureFps;
}
OCTK_END_NAMESPACE
