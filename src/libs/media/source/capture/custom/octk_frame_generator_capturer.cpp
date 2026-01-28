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
#include <octk_create_frame_generator.hpp>
#include <octk_repeating_task.hpp>
#include <octk_once_flag.hpp>

OCTK_BEGIN_NAMESPACE

FrameGeneratorCapturer::FrameGeneratorCapturer(Clock *clock,
                                               std::unique_ptr<FrameGeneratorInterface> frameGenerator,
                                               int targetFps,
                                               const TaskQueueBase::SharedPtr &taskQueue)
    : mClock(clock)
    , mSending(true)
    , mSinkWantsObserver(nullptr)
    , mFrameGenerator(std::move(frameGenerator))
    , mSourceFps(targetFps)
    , mTargetCaptureFps(targetFps)
    , mTaskQueue(taskQueue ? taskQueue : TaskQueueThread::makeShared())
{
    OCTK_DCHECK(mFrameGenerator);
    OCTK_DCHECK_GT(targetFps, 0);
}

FrameGeneratorCapturer::~FrameGeneratorCapturer()
{
    this->stop();
    // Deconstruct first as tasks in th access other fields of the instance of this class.
    mTaskQueue = nullptr;
}

void FrameGeneratorCapturer::setFakeRotation(VideoRotation rotation)
{
    Mutex::Lock locker(mMutex);
    mFakeRotation = rotation;
}

void FrameGeneratorCapturer::setFakeColorSpace(Optional<ColorSpace> colorSpace)
{
    Mutex::Lock locker(mMutex);
    mFakeColorSpace = colorSpace;
}

bool FrameGeneratorCapturer::init()
{
    // This check is added because frame_generator_ might be file based and should not crash because a file moved.
    if (mFrameGenerator.get() == nullptr)
    {
        return false;
    }

    mFrameTask = RepeatingTaskHandle::delayedStart(mTaskQueue.get(),
                                                   TimeDelta::Seconds(1) / this->getCurrentConfiguredFramerate(),
                                                   [this]
                                                   {
                                                       this->insertFrame();
                                                       return TimeDelta::Seconds(1) /
                                                              this->getCurrentConfiguredFramerate();
                                                   });
    return true;
}

void FrameGeneratorCapturer::insertFrame()
{
    Mutex::Lock locker(mMutex);
    if (mSending)
    {
        // TODO(srte): Use more advanced frame rate control to allow arbitrary fractions.
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
                               .setUpdateRect(frameData.updateRect)
                               .setColorSpace(mFakeColorSpace)
                               .build();
        CustomVideoCapturer::onFrame(frame);
    }
}

Optional<FrameGeneratorCapturer::Resolution> FrameGeneratorCapturer::getResolution() const
{
    FrameGeneratorInterface::Resolution resolution = mFrameGenerator->getResolution();
    return Resolution{static_cast<int>(resolution.width), static_cast<int>(resolution.height)};
}

void FrameGeneratorCapturer::start()
{
    {
        Mutex::Lock locker(mMutex);
        mSending = true;
    }
    if (!mFrameTask.isRunning())
    {
        mFrameTask = RepeatingTaskHandle::start(mTaskQueue.get(),
                                                [this]
                                                {
                                                    this->insertFrame();
                                                    return TimeDelta::Seconds(1) /
                                                           this->getCurrentConfiguredFramerate();
                                                });
    }
}

void FrameGeneratorCapturer::stop()
{
    Mutex::Lock locker(mMutex);
    mSending = false;
}

void FrameGeneratorCapturer::changeResolution(size_t width, size_t height)
{
    Mutex::Lock locker(mMutex);
    mFrameGenerator->changeResolution(width, height);
}

void FrameGeneratorCapturer::changeFramerate(int targetFramerate)
{
    Mutex::Lock locker(mMutex);
    OCTK_CHECK(mTargetCaptureFps > 0);
    if (targetFramerate > mSourceFps)
    {
        OCTK_WARNING() << "Target framerate clamped from " << targetFramerate << " to " << mSourceFps;
    }
    if (mSourceFps % mTargetCaptureFps != 0)
    {
        int decimation = std::round(static_cast<double>(mSourceFps) / mTargetCaptureFps);
        int effective_rate = mTargetCaptureFps / decimation;
        OCTK_WARNING() << "Target framerate, " << targetFramerate << ", is an uneven fraction of the source rate, "
                       << mSourceFps << ". The framerate will be :" << effective_rate;
    }
    mTargetCaptureFps = std::min(mSourceFps, targetFramerate);
}

int FrameGeneratorCapturer::getFrameWidth() const
{
    return static_cast<int>(mFrameGenerator->getResolution().width);
}

int FrameGeneratorCapturer::getFrameHeight() const
{
    return static_cast<int>(mFrameGenerator->getResolution().height);
}

void FrameGeneratorCapturer::onOutputFormatRequest(int width, int height, const Optional<int> &max_fps)
{
    CustomVideoCapturer::onOutputFormatRequest(width, height, max_fps);
}

void FrameGeneratorCapturer::setSinkWantsObserver(SinkWantsObserver *observer)
{
    Mutex::Lock locker(mMutex);
    OCTK_DCHECK(!mSinkWantsObserver);
    mSinkWantsObserver = observer;
}

void FrameGeneratorCapturer::addOrUpdateSink(VideoSinkInterface<VideoFrame> *sink, const VideoSinkWants &wants)
{
    CustomVideoCapturer::addOrUpdateSink(sink, wants);
    {
        Mutex::Lock locker(mMutex);
        if (mSinkWantsObserver)
        {
            // Tests need to observe unmodified sink wants.
            mSinkWantsObserver->OnSinkWantsChanged(sink, wants);
        }
    }
    this->changeFramerate(this->getSinkWants().maxFramerateFps);
}

void FrameGeneratorCapturer::removeSink(VideoSinkInterface<VideoFrame> *sink)
{
    CustomVideoCapturer::removeSink(sink);
    this->changeFramerate(this->getSinkWants().maxFramerateFps);
}

void FrameGeneratorCapturer::forceFrame()
{
    // One-time non-repeating task,
    mTaskQueue->postTask([this] { this->insertFrame(); });
}

int FrameGeneratorCapturer::getCurrentConfiguredFramerate()
{
    Mutex::Lock locker(mMutex);
    return mTargetCaptureFps;
}

class FrameGeneratorCapturerVideoTrackSourcePrivate
{
    OCTK_DEFINE_PPTR(FrameGeneratorCapturerVideoTrackSource)
    OCTK_DECLARE_PUBLIC(FrameGeneratorCapturerVideoTrackSource)
    OCTK_DISABLE_COPY_MOVE(FrameGeneratorCapturerVideoTrackSourcePrivate)

public:
    FrameGeneratorCapturerVideoTrackSourcePrivate(FrameGeneratorCapturerVideoTrackSource *p, bool isScreenCast);
    virtual ~FrameGeneratorCapturerVideoTrackSourcePrivate() { }

    TaskQueueBase::SharedPtr mTaskQueue{TaskQueueThread::makeShared()};
    UniquePointer<FrameGeneratorCapturer> mFrameGeneratorCapturer;
    std::atomic_bool mStarted{false};
    const bool mIsScreenCast;
    OnceFlag mInitOnceFlag;
};

FrameGeneratorCapturerVideoTrackSourcePrivate::FrameGeneratorCapturerVideoTrackSourcePrivate(
    FrameGeneratorCapturerVideoTrackSource *p,
    bool isScreenCast)
    : mPPtr(p)
    , mIsScreenCast(isScreenCast)
{
}

FrameGeneratorCapturerVideoTrackSource::FrameGeneratorCapturerVideoTrackSource(Config config,
                                                                               Clock *clock,
                                                                               bool isScreenCast)
    : VideoTrackSource(false /* remote */)
    , mDPtr(new FrameGeneratorCapturerVideoTrackSourcePrivate(this, isScreenCast))
{
    OCTK_D(FrameGeneratorCapturerVideoTrackSource);
    d->mFrameGeneratorCapturer = utils::make_unique<FrameGeneratorCapturer>(
        clock,
        utils::CreateSquareFrameGenerator(config.width, config.height, utils::nullopt, config.num_squares_generated),
        config.frames_per_second,
        d->mTaskQueue);
}

FrameGeneratorCapturerVideoTrackSource::FrameGeneratorCapturerVideoTrackSource(
    std::unique_ptr<FrameGeneratorInterface> frameGenerator,
    int targetFps,
    Clock *clock,
    bool isScreenCast)
    : VideoTrackSource(false /* remote */)
    , mDPtr(new FrameGeneratorCapturerVideoTrackSourcePrivate(this, isScreenCast))
{
    OCTK_D(FrameGeneratorCapturerVideoTrackSource);
    d->mFrameGeneratorCapturer = utils::make_unique<FrameGeneratorCapturer>(clock,
                                                                            std::move(frameGenerator),
                                                                            targetFps,
                                                                            d->mTaskQueue);
}

FrameGeneratorCapturerVideoTrackSource::FrameGeneratorCapturerVideoTrackSource(
    std::unique_ptr<FrameGeneratorCapturer> frameGeneratorCapturer,
    bool isScreenCast)
    : VideoTrackSource(false /* remote */)
    , mDPtr(new FrameGeneratorCapturerVideoTrackSourcePrivate(this, isScreenCast))
{
    OCTK_D(FrameGeneratorCapturerVideoTrackSource);
    d->mFrameGeneratorCapturer = std::move(frameGeneratorCapturer);
}

FrameGeneratorCapturerVideoTrackSource::~FrameGeneratorCapturerVideoTrackSource()
{
    this->stop();
}

Status FrameGeneratorCapturerVideoTrackSource::start()
{
    OCTK_D(FrameGeneratorCapturerVideoTrackSource);
    if (d->mInitOnceFlag.enter())
    {
        d->mFrameGeneratorCapturer->init(); //TODO::return result?
        d->mInitOnceFlag.leave();
    }
    if (!d->mStarted.exchange(true))
    {
        d->mFrameGeneratorCapturer->start();
        this->setState(kLive);
    }
    return okStatus;
}

void FrameGeneratorCapturerVideoTrackSource::stop()
{
    OCTK_D(FrameGeneratorCapturerVideoTrackSource);
    if (d->mStarted.exchange(false))
    {
        d->mFrameGeneratorCapturer->stop();
    }
}

bool FrameGeneratorCapturerVideoTrackSource::isScreencast() const
{
    OCTK_D(const FrameGeneratorCapturerVideoTrackSource);
    return d->mIsScreenCast;
}

VideoSourceInterface<VideoFrame> *FrameGeneratorCapturerVideoTrackSource::source()
{
    OCTK_D(FrameGeneratorCapturerVideoTrackSource);
    return d->mFrameGeneratorCapturer.get();
}

OCTK_END_NAMESPACE
