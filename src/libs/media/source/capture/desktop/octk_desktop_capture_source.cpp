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

#include <octk_desktop_capture_source.hpp>
#include <octk_desktop_capture_options.hpp>
#include <octk_video_broadcaster.hpp>
#include <octk_desktop_capturer.hpp>
#include <octk_desktop_frame.hpp>
#include <octk_video_adapter.hpp>
#include <octk_i420_buffer.hpp>
#include <octk_date_time.hpp>
#include <octk_logging.hpp>
#include <octk_memory.hpp>

#include <libyuv.h>

#include <atomic>
#include <thread>

OCTK_BEGIN_NAMESPACE

class DesktopCaptureSourcePrivate : public DesktopCapturer::Callback
{
public:
    explicit DesktopCaptureSourcePrivate(DesktopCaptureSource *p);
    ~DesktopCaptureSourcePrivate();

    void updateVideoAdapter();
    void updateLastError(const std::string &error);

    void implStart();
    void implInit(size_t targetFps, size_t deviceIndex);

    // Called before a frame capture is started.
    void OnFrameCaptureStart() override;
    // Called after a frame has been captured. `frame` is not nullptr if and only if `result` is SUCCESS.
    virtual void OnCaptureResult(DesktopCapturer::Result result,
                                 std::unique_ptr<DesktopFrame> frame) override;

    size_t mFps = 0;
    size_t mIndex = 0;
    int64_t mIntervalMSecs;
    std::string mLastError;
    std::string mWindowTitle;
    std::once_flag mInitOnceFlag;
    std::atomic<bool> mStartFlag;
    std::shared_ptr<I420Buffer> mI420Buffer;
    std::unique_ptr<std::thread> mCaptureThread;
    std::atomic<bool> mIsInited{false};
    std::atomic<int64_t> mFPSTimestamp{0};
    std::atomic<int64_t> mCaptureElapsedMSecs{0};
    std::atomic<int64_t> mCaptureConvertElapsedMSecs{0};

    std::shared_ptr<I420Buffer> mLibWebRTCI420Buffer;
    std::unique_ptr<DesktopCapturer> mLibWebRTCDesktopCapturer;

    VideoAdapter mVideoAdapter;
    VideoBroadcaster mVideoBroadcaster;

protected:
    OCTK_DEFINE_PPTR(DesktopCaptureSource)
    OCTK_DECLARE_PUBLIC(DesktopCaptureSource)

    OCTK_DISABLE_COPY_MOVE(DesktopCaptureSourcePrivate)
};

DesktopCaptureSourcePrivate::DesktopCaptureSourcePrivate(DesktopCaptureSource *p)
    : mPPtr(p)
{
}

DesktopCaptureSourcePrivate::~DesktopCaptureSourcePrivate()
{
}

void DesktopCaptureSourcePrivate::OnFrameCaptureStart()
{
    Callback::OnFrameCaptureStart();
}

void DesktopCaptureSourcePrivate::OnCaptureResult(DesktopCapturer::Result result,
                                                  std::unique_ptr<DesktopFrame> frame)
{
    OCTK_P(DesktopCaptureSource);
    const auto nowUSecs = DateTime::systemTimeUSecs();
    const auto nowMSecs = nowUSecs / DateTime::kUSecsPerMSec;
    static size_t cnt = 0;

    cnt++;
    auto desktopSize = frame->size();
    auto frameSize = desktopSize.width() * desktopSize.height() * 4;
    const auto oldUSecs = mFPSTimestamp.load();
    if (!oldUSecs)
    {
        mFPSTimestamp.store(nowUSecs);
    }
    else if (nowUSecs - oldUSecs >= DateTime::kUSecsPerSec)
    {
        OCTK_TRACE() << "FPS: " << cnt;
        mFPSTimestamp.store(nowUSecs);
        cnt = 0;
    }

    // Convert DesktopFrame to VideoFrame
    if (result != DesktopCapturer::Result::SUCCESS)
    {
        OCTK_ERROR() << "Capture frame faiiled, result: " << (int)result;
    }
    int width = frame->size().width();
    int height = frame->size().height();
    // int half_width = (width + 1) / 2;

    if (!mLibWebRTCI420Buffer.get() || mLibWebRTCI420Buffer->width() * mLibWebRTCI420Buffer->height() < width * height)
    {
        mLibWebRTCI420Buffer = I420Buffer::Create(width, height);
    }

    const auto beginConvertMSecs = DateTime::systemTimeMSecs();
    libyuv::ConvertToI420(frame->data(), 0,
                          mLibWebRTCI420Buffer->MutableDataY(), mLibWebRTCI420Buffer->StrideY(),
                          mLibWebRTCI420Buffer->MutableDataU(), mLibWebRTCI420Buffer->StrideU(),
                          mLibWebRTCI420Buffer->MutableDataV(), mLibWebRTCI420Buffer->StrideV(),
                          0, 0, width, height, width, height,
                          libyuv::kRotate0,
                          libyuv::FOURCC_ARGB);
    const auto ConvertElapsedMSecs = DateTime::systemTimeMSecs() - beginConvertMSecs;
    mI420Buffer = I420Buffer::Copy(mLibWebRTCI420Buffer->width(),
                                   mLibWebRTCI420Buffer->height(),
                                   mLibWebRTCI420Buffer->DataY(),
                                   mLibWebRTCI420Buffer->StrideY(),
                                   mLibWebRTCI420Buffer->DataU(),
                                   mLibWebRTCI420Buffer->StrideU(),
                                   mLibWebRTCI420Buffer->DataV(),
                                   mLibWebRTCI420Buffer->StrideU());

    // Notify sinks
    p->onFrame(VideoFrame(mI420Buffer, VideoRotation::Angle0, nowUSecs));

    mCaptureElapsedMSecs.store(frame->capture_time_ms());
    const auto elapsedmSecs = DateTime::systemTimeMSecs() - nowMSecs;
    mCaptureConvertElapsedMSecs.store(elapsedmSecs + mCaptureElapsedMSecs.load());
    if (1)
    {
        OCTK_TRACE() << "OnCaptureResult:new Frame"
                     << ", width:" << desktopSize.width() << ", height:" << desktopSize.height()
                     << ", capture_time_ms:" << frame->capture_time_ms()
                     << ", mCaptureElapsedMSecs:" << mCaptureElapsedMSecs.load()
                     << ", mCaptureConvertElapsedMSecs:" << mCaptureConvertElapsedMSecs.load()
                     << ", ConvertElapsedMSecs:" << ConvertElapsedMSecs
                     << ", elapsedmSecs:" << elapsedmSecs;
    }
}

void DesktopCaptureSourcePrivate::implInit(size_t targetFps, size_t deviceIndex)
{
    mLibWebRTCDesktopCapturer = DesktopCapturer::CreateScreenCapturer(DesktopCaptureOptions::CreateDefault());
    if (!mLibWebRTCDesktopCapturer)
    {
        const auto error = "LibWebRTCDesktopCapturer create failed";
        updateLastError(error);
        OCTK_WARNING() << error;
        return;
    }

    DesktopCapturer::SourceList sources;
    mLibWebRTCDesktopCapturer->GetSourceList(&sources);
    if (deviceIndex > sources.size())
    {
        const auto error = "The total sources of screen is " + std::to_string(sources.size()) +
                           ", but require source of index at " + std::to_string(deviceIndex);
        updateLastError(error);
        OCTK_WARNING() << error;
        return;
    }

    OCTK_CHECK(mLibWebRTCDesktopCapturer->SelectSource(sources[deviceIndex].id));
    mWindowTitle = sources[deviceIndex].title;
    mIntervalMSecs = DateTime::kMSecsPerSec / targetFps;
    mIndex = deviceIndex;
    mFps = targetFps;

    OCTK_DEBUG() << "init DesktopCapture finish";
    // start new thread to capture
    mIsInited.store(true);
}

void DesktopCaptureSourcePrivate::implStart()
{
    mLibWebRTCDesktopCapturer->start(this);
    while (mStartFlag.load())
    {
        mLibWebRTCDesktopCapturer->CaptureFrame();
        const auto sleepMSecs = mIntervalMSecs - mCaptureConvertElapsedMSecs.load();
        if (sleepMSecs > 0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepMSecs));
            OCTK_TRACE() << "sleep_for:sleepUSecs=" << sleepMSecs;
        }
    }
}

void DesktopCaptureSourcePrivate::updateVideoAdapter()
{
    mVideoAdapter.OnSinkWants(mVideoBroadcaster.wants());
}

void DesktopCaptureSourcePrivate::updateLastError(const std::string &error)
{
    mLastError = error;
    // mPPtr->errorOccurred(error);
}

DesktopCaptureSource::DesktopCaptureSource()
    : mDPtr(utils::make_unique<DesktopCaptureSourcePrivate>(this))
{
}

DesktopCaptureSource::DesktopCaptureSource(size_t targetFps, size_t deviceIndex)
    : mDPtr(utils::make_unique<DesktopCaptureSourcePrivate>(this))
{
    this->init(targetFps, deviceIndex);
}

DesktopCaptureSource::~DesktopCaptureSource()
{
    this->stopCapture();
}

std::string DesktopCaptureSource::windowTitle() const
{
    OCTK_D(const DesktopCaptureSource);
    return d->mWindowTitle;
}

std::string DesktopCaptureSource::lastError() const
{
    OCTK_D(const DesktopCaptureSource);
    return d->mLastError;
}

bool DesktopCaptureSource::isInited() const
{
    OCTK_D(const DesktopCaptureSource);
    return d->mIsInited.load();
}

size_t DesktopCaptureSource::index() const
{
    OCTK_D(const DesktopCaptureSource);
    return d->mIndex;
}

size_t DesktopCaptureSource::fps() const
{
    OCTK_D(const DesktopCaptureSource);
    return d->mFps;
}

bool DesktopCaptureSource::init(size_t targetFps, size_t deviceIndex)
{
    OCTK_D(DesktopCaptureSource);
    std::call_once(d->mInitOnceFlag, [=]() {
        d->implInit(targetFps, deviceIndex);
    });
    return d->mIsInited.load();
}

bool DesktopCaptureSource::startCapture()
{
    OCTK_D(DesktopCaptureSource);
    if (!d->mIsInited.load())
    {
        OCTK_WARNING() << "DesktopCaptureSource not inited";
        return false;
    }
    if (d->mStartFlag.exchange(true))
    {
        OCTK_WARNING() << "DesktopCaptureSource already been running...";
        return false;
    }

    // start new thread to capture
    d->mCaptureThread.reset(new std::thread([=]() {
        d->implStart();
    }));
    return true;
}

void DesktopCaptureSource::stopCapture()
{
    OCTK_D(DesktopCaptureSource);
    d->mStartFlag.store(false);
    if (d->mCaptureThread && d->mCaptureThread->joinable())
    {
        d->mCaptureThread->join();
    }
}

void DesktopCaptureSource::requestRefreshFrame()
{
    VideoSourceInterface<VideoFrame>::requestRefreshFrame();
}

void DesktopCaptureSource::onConstraintsChanged(const VideoTrackSourceConstraints &video_track_source_constraints)
{
    VideoSinkInterface<VideoFrame>::onConstraintsChanged(video_track_source_constraints);
}

void DesktopCaptureSource::onDiscardedFrame()
{
    VideoSinkInterface<VideoFrame>::onDiscardedFrame();
}

void DesktopCaptureSource::addOrUpdateSink(VideoSinkInterface<VideoFrame> *sink, const VideoSinkWants &wants)
{
    OCTK_D(DesktopCaptureSource);
    d->mVideoBroadcaster.addOrUpdateSink(sink, wants);
    d->updateVideoAdapter();
}

void DesktopCaptureSource::removeSink(VideoSinkInterface<VideoFrame> *sink)
{
    OCTK_D(DesktopCaptureSource);
    d->mVideoBroadcaster.removeSink(sink);
    d->updateVideoAdapter();
}

void DesktopCaptureSource::onFrame(const VideoFrame &frame)
{
    OCTK_D(DesktopCaptureSource);
    int croppedWidth = 0;
    int croppedHeight = 0;
    int outWidth = 0;
    int outHeight = 0;

    if (!d->mVideoAdapter.adaptFrameResolution(frame.width(),
                                               frame.height(),
                                               frame.timestampUSecs() * 1000,
                                               &croppedWidth,
                                               &croppedHeight,
                                               &outWidth,
                                               &outHeight))
    {
        // Drop frame in order to respect frame rate constraint.
        OCTK_TRACE("Drop frame:%d ts:%d in order to respect frame rate constraint.",
                   frame.id(), frame.timestampUSecs());
        return;
    }

    if (outHeight != frame.height() || outWidth != frame.width())
    {
        // Video adapter has requested a down-scale. Allocate a new buffer and return scaled version.
        // For simplicity, only scale here without cropping.
        auto scaledBuffer = I420Buffer::Create(outWidth, outHeight);
        scaledBuffer->scaleFrom(*frame.videoFrameBuffer()->ToI420());
        VideoFrame::Builder newFrameBuilder = VideoFrame::Builder()
            .setVideoFrameBuffer(scaledBuffer)
            .setRotation(VideoRotation::Angle0)
            .setTimestampUSecs(frame.timestampUSecs())
            .setId(frame.id());
        if (frame.hasUpdateRect())
        {
            VideoFrame::UpdateRect new_rect =
                frame.updateRect().scaleWithFrame(frame.width(), frame.height(), 0, 0,
                                                  frame.width(), frame.height(),
                                                  outWidth,
                                                  outHeight);
            newFrameBuilder.setUpdateRect(new_rect);
        }
        auto newFrame = newFrameBuilder.build();
        d->mVideoBroadcaster.onFrame(newFrame);
        // this->frameCaptured(newFrame);
    }
    else
    {
        // No adaptations needed, just return the frame as is.
        d->mVideoBroadcaster.onFrame(frame);
        // this->frameCaptured(frame);
    }
}
OCTK_END_NAMESPACE
