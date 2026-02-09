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

#include <octk_frame_generator_capturer.hpp>
#include <octk_rtc_video_frame.hpp>
#include <octk_memory.hpp>
#include <octk_yuv.hpp>

OCTK_BEGIN_NAMESPACE

class RtcVideoFrameDefault : public RtcVideoFrame
{
public:
    RtcVideoFrameDefault(const VideoFrame &frame)
        : mI420Buffer(frame.videoFrameBuffer()->toI420())
        , mTimestampUSecs(frame.timestampUSecs())
        , mVideoRotation(frame.rotation())
        , mId(frame.id())
    {
    }
    RtcVideoFrameDefault(std::shared_ptr<I420BufferInterface> buffer,
                         VideoRotation rotation,
                         int64_t timestamp_us,
                         uint16_t id)
        : mI420Buffer(buffer)
        , mTimestampUSecs(timestamp_us)
        , mVideoRotation(rotation)
        , mId(id)
    {
    }
    ~RtcVideoFrameDefault() override = default;

    SharedPtr copy() override
    {
        return SharedPtr(new RtcVideoFrameDefault(mI420Buffer, mVideoRotation, mTimestampUSecs, mId),
                         [](RtcVideoFrameDefault *p) { delete p; });
    }

    int width() const override { return mI420Buffer->width(); }
    int height() const override { return mI420Buffer->height(); }
    Format format() const override { return Format::kI420; }

    uint16_t id() const override { return mId; }
    int64_t timestamp() const override { return mTimestampUSecs; }
    Rotation rotation() const override
    {
        switch (mVideoRotation)
        {
            case VideoRotation::kAngle0: return Rotation::kAngle0;
            case VideoRotation::kAngle90: return Rotation::kAngle90;
            case VideoRotation::kAngle180: return Rotation::kAngle180;
            case VideoRotation::kAngle270: return Rotation::kAngle270;
        }
        return Rotation::kAngle0;
    }

    const uint8_t *dataY() const override { return mI420Buffer->dataY(); }
    const uint8_t *dataU() const override { return mI420Buffer->dataU(); }
    const uint8_t *dataV() const override { return mI420Buffer->dataV(); }

    int strideY() const override { return mI420Buffer->strideY(); }
    int strideU() const override { return mI420Buffer->strideU(); }
    int strideV() const override { return mI420Buffer->strideV(); }

private:
    uint16_t mId{0};
    int64_t mTimestampUSecs{0};
    std::shared_ptr<I420BufferInterface> mI420Buffer;
    VideoRotation mVideoRotation{VideoRotation::kAngle0};
};

RtcVideoFrame::SharedPtr RtcVideoFrame::create(const VideoFrame &frame)
{
    return utils::make_shared<RtcVideoFrameDefault>(frame);
}

RtcVideoFrame::SharedPtr RtcVideoFrame::createI420(const uint8_t *data, int width, int height)
{
    auto buffer = I420Buffer::create(width, height);
    utils::yuv::copyI420(OCTK_I420_Y_PTR(data, width, height),
                         OCTK_I420_Y_STRIDE(width),
                         OCTK_I420_U_PTR(data, width, height),
                         OCTK_I420_U_STRIDE(width),
                         OCTK_I420_V_PTR(data, width, height),
                         OCTK_I420_V_STRIDE(width),
                         buffer->MutableDataY(),
                         buffer->strideY(),
                         buffer->MutableDataU(),
                         buffer->strideU(),
                         buffer->MutableDataV(),
                         buffer->strideV(),
                         width,
                         height);
    return utils::make_shared<RtcVideoFrameDefault>(buffer, octk::VideoRotation::kAngle0, 0, 0);
}

void RtcVideoSinkAdapter::onData(const DataType &data)
{
    if (!mI420Buffer.get() || mI420Buffer->width() != data->width() || mI420Buffer->height() != data->height())
    {
        mI420Buffer = I420Buffer::create(data->width(), data->height());
    }
    utils::yuv::copyI420(data->dataY(),
                         data->strideY(),
                         data->dataU(),
                         data->strideU(),
                         data->dataV(),
                         data->strideV(),
                         mI420Buffer->MutableDataY(),
                         mI420Buffer->strideY(),
                         mI420Buffer->MutableDataU(),
                         mI420Buffer->strideU(),
                         mI420Buffer->MutableDataV(),
                         mI420Buffer->strideV(),
                         data->width(),
                         data->height());
    mVideoFrameSink->onFrame(VideoFrame(mI420Buffer, octk::VideoRotation::kAngle0, data->timestamp()));
}

class RtcVideoGeneratorPrivate : public VideoSinkInterface<VideoFrame>
{
public:
    RtcVideoGeneratorPrivate(RtcVideoGenerator *p, StringView name)
        : mPPtr(p)
        , mName(name)
    {
    }
    ~RtcVideoGeneratorPrivate() override { }

    void onFrame(const VideoFrame &frame) override
    {
        const auto sinks = mPPtr->sinks();
        const auto rtcFrame = RtcVideoFrame::create(frame);
        for (const auto &sink : sinks)
        {
            sink->onData(rtcFrame);
        }
    }

    std::unique_ptr<FrameGeneratorCapturerVideoTrackSource> mGeneratorSource;
    const std::string mName;
    int mHeight;
    int mWidth;
    int mFps;

private:
    OCTK_DEFINE_PPTR(RtcVideoGenerator)
    OCTK_DECLARE_PUBLIC(RtcVideoGenerator)
    OCTK_DISABLE_COPY_MOVE(RtcVideoGeneratorPrivate)
};

RtcVideoGenerator::SharedPtr RtcVideoGenerator::create(FrameGeneratorInterface::UniquePtr generator,
                                                       int fps,
                                                       StringView name)
{
    if (generator && fps > 0)
    {
        const auto generatorName = name.empty() ? generator->name() : name.data();
        auto videoGenerator = SharedPtr(new RtcVideoGenerator(generatorName));
        videoGenerator->dFunc()->mHeight = generator->getResolution().height;
        videoGenerator->dFunc()->mWidth = generator->getResolution().width;
        videoGenerator->dFunc()->mFps = fps;
        auto trackSource = std::make_unique<FrameGeneratorCapturerVideoTrackSource>(std::move(generator),
                                                                                    fps,
                                                                                    Clock::GetRealTimeClock(),
                                                                                    false);
        trackSource->addOrUpdateSink(videoGenerator->dFunc(), VideoSinkWants());
        videoGenerator->dFunc()->mGeneratorSource = std::move(trackSource);
        videoGenerator->dFunc()->mGeneratorSource->start();
        return videoGenerator;
    }
    OCTK_ERROR("createSquareGenerator: invalid parameter");
    return nullptr;
}

RtcVideoGenerator::SharedPtr RtcVideoGenerator::createSquareGenerator(int width,
                                                                      int height,
                                                                      int numSquares,
                                                                      int fps,
                                                                      StringView name)
{
    if (width > 0 && height > 0 && numSquares > 0 && fps > 0)
    {
        auto generator = utils::make_unique<SquareGenerator>(width,
                                                             height,
                                                             SquareGenerator::OutputType::kI420,
                                                             numSquares);
        return create(std::move(generator), fps, name);
    }
    OCTK_ERROR("createSquareGenerator: invalid parameter");
    return nullptr;
}

RtcVideoGenerator::RtcVideoGenerator(StringView name)
    : mDPtr(new RtcVideoGeneratorPrivate(this, name))
{
}

RtcVideoGenerator::~RtcVideoGenerator()
{
}

int RtcVideoGenerator::fps() const
{
    OCTK_D(const RtcVideoGenerator);
    return d->mFps;
}

int RtcVideoGenerator::width() const
{
    OCTK_D(const RtcVideoGenerator);
    return d->mWidth;
}

int RtcVideoGenerator::height() const
{
    OCTK_D(const RtcVideoGenerator);
    return d->mHeight;
}

VideoSourceInterface<VideoFrame> *RtcVideoGenerator::source() const
{
    OCTK_D(const RtcVideoGenerator);
    return d->mGeneratorSource.get();
}

OCTK_END_NAMESPACE
