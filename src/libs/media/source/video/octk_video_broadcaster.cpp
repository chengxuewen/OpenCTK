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

#include <octk_video_source_interface.hpp>
#include <octk_video_sink_interface.hpp>
#include <octk_video_frame_buffer.hpp>
#include <octk_video_broadcaster.hpp>
#include <octk_video_rotation.hpp>
#include <octk_i420_buffer.hpp>
#include <octk_video_frame.hpp>
#include <octk_numeric.hpp>
#include <octk_logging.hpp>

#include <algorithm>
#include <vector>

OCTK_BEGIN_NAMESPACE

VideoBroadcaster::VideoBroadcaster() = default;
VideoBroadcaster::~VideoBroadcaster() = default;

void VideoBroadcaster::addOrUpdateSink(VideoSinkInterface<VideoFrame> *sink, const VideoSinkWants &wants)
{
    OCTK_DCHECK(sink != nullptr);
    std::lock_guard<std::mutex> lock(mSinksAndWantsMutex);
    if (!FindSinkPair(sink))
    {
        // `Sink` is a new sink, which didn't receive previous frame.
        mPreviousFrameSentToAllSinks = false;

        if (mLastConstraints.has_value())
        {
            OCTK_INFO() << __func__ << " forwarding stored constraints min_fps "
                        << mLastConstraints->minFps.value_or(-1) << " maxFps "
                        << mLastConstraints->maxFps.value_or(-1);
            sink->onConstraintsChanged(*mLastConstraints);
        }
    }
    VideoSourceBase::addOrUpdateSink(sink, wants);
    UpdateWants();
}

void VideoBroadcaster::removeSink(VideoSinkInterface<VideoFrame> *sink)
{
    OCTK_DCHECK(sink != nullptr);
    std::lock_guard<std::mutex> lock(mSinksAndWantsMutex);
    VideoSourceBase::removeSink(sink);
    UpdateWants();
}

bool VideoBroadcaster::frameWanted() const
{
    std::lock_guard<std::mutex> lock(mSinksAndWantsMutex);
    return !this->sinkPairs().empty();
}

VideoSinkWants VideoBroadcaster::wants() const
{
    std::lock_guard<std::mutex> lock(mSinksAndWantsMutex);
    return mCurrentWants;
}

void VideoBroadcaster::onFrame(const VideoFrame &frame)
{
    std::lock_guard<std::mutex> lock(mSinksAndWantsMutex);
    bool currentFrameWasDiscarded = false;
    for (auto &sinkPair : this->sinkPairs())
    {
        if (sinkPair.wants.rotationApplied && frame.rotation() != VideoRotation::Angle0)
        {
            // Calls to OnFrame are not synchronized with changes to the sink wants.
            // When rotationApplied is set to true, one or a few frames may get here
            // with rotation still pending. Protect sinks that don't expect any
            // pending rotation.
            OCTK_INFO() << "Discarding frame with unexpected rotation.";
            sinkPair.sink->onDiscardedFrame();
            currentFrameWasDiscarded = true;
            continue;
        }
        if (sinkPair.wants.blackFrames)
        {
            VideoFrame blackFrame = VideoFrame::Builder()
                                        .setVideoFrameBuffer(GetBlackFrameBuffer(frame.width(), frame.height()))
                                        .setRotation(frame.rotation())
                                        .setTimestampUSecs(frame.timestampUSecs())
                                        .setId(frame.id())
                                        .build();
            sinkPair.sink->onFrame(blackFrame);
        }
        else if (!mPreviousFrameSentToAllSinks && frame.hasUpdateRect())
        {
            // Since last frame was not sent to some sinks, no reliable update
            // information is available, so we need to clear the update rect.
            VideoFrame copy = VideoFrame::copy(frame);
            copy.clearUpdateRect();
            sinkPair.sink->onFrame(copy);
        }
        else
        {
            sinkPair.sink->onFrame(frame);
        }
    }
    mPreviousFrameSentToAllSinks = !currentFrameWasDiscarded;
}

void VideoBroadcaster::onDiscardedFrame()
{
    std::lock_guard<std::mutex> lock(mSinksAndWantsMutex);
    for (auto &sinkPair : sinkPairs())
    {
        sinkPair.sink->onDiscardedFrame();
    }
}

void VideoBroadcaster::processConstraints(const VideoTrackSourceConstraints &constraints)
{
    std::lock_guard<std::mutex> lock(mSinksAndWantsMutex);
    OCTK_INFO() << __func__ << " min_fps " << constraints.minFps.value_or(-1) << " maxFps "
                << constraints.maxFps.value_or(-1) << " broadcasting to " << sinkPairs().size() << " sinks.";
    mLastConstraints = constraints;
    for (auto &sinkPair : sinkPairs())
    {
        sinkPair.sink->onConstraintsChanged(constraints);
    }
}

void VideoBroadcaster::UpdateWants()
{
    VideoSinkWants wants;
    wants.rotationApplied = false;
    wants.resolutionAlignment = 1;
    wants.aggregates.emplace(VideoSinkWants::Aggregates());
    wants.isActive = false;

    // TODO(webrtc:14451) : I think it makes sense to always
    // "ignore" encoders that are not active. But that would
    // probably require a controlled roll out with a field trials?
    // To play it safe, only ignore inactive encoders is there is an
    // active encoder using the new api (scale_resolution_down_to),
    // this means that there is only a behavioural change when using new
    // api.
    bool ignore_inactive_encoders_old_api = false;
    for (auto &sink : sinkPairs())
    {
        if (sink.wants.isActive && sink.wants.requestedResolution.has_value())
        {
            ignore_inactive_encoders_old_api = true;
            break;
        }
    }

    for (auto &sink : sinkPairs())
    {
        if (!sink.wants.isActive && (sink.wants.requestedResolution || ignore_inactive_encoders_old_api))
        {
            continue;
        }
        // wants.rotationApplied == ANY(sink.wants.rotationApplied)
        if (sink.wants.rotationApplied)
        {
            wants.rotationApplied = true;
        }
        // wants.maxPixelCount == MIN(sink.wants.maxPixelCount)
        if (sink.wants.maxPixelCount < wants.maxPixelCount)
        {
            wants.maxPixelCount = sink.wants.maxPixelCount;
        }
        // Select the minimum requested target_pixel_count, if any, of all sinks so
        // that we don't over utilize the resources for any one.
        // TODO(sprang): Consider using the median instead, since the limit can be
        // expressed by maxPixelCount.
        if (sink.wants.targetPixelCount &&
            (!wants.targetPixelCount || (*sink.wants.targetPixelCount < *wants.targetPixelCount)))
        {
            wants.targetPixelCount = sink.wants.targetPixelCount;
        }
        // Select the minimum for the requested max framerates.
        if (sink.wants.maxFramerateFps < wants.maxFramerateFps)
        {
            wants.maxFramerateFps = sink.wants.maxFramerateFps;
        }
        wants.resolutionAlignment = utils::lcm(wants.resolutionAlignment, sink.wants.resolutionAlignment);

        // Pick MAX(requestedResolution) since the actual can be downscaled in
        // encoder instead.
        if (sink.wants.requestedResolution)
        {
            if (!wants.requestedResolution)
            {
                wants.requestedResolution = sink.wants.requestedResolution;
            }
            else
            {
                wants.requestedResolution->width =
                    utils::mathMax(wants.requestedResolution->width, sink.wants.requestedResolution->width);
                wants.requestedResolution->height =
                    utils::mathMax(wants.requestedResolution->height, sink.wants.requestedResolution->height);
            }
        }
        else if (sink.wants.isActive)
        {
            wants.aggregates->anyActiveWithoutRequestedResolution = true;
        }

        wants.isActive |= sink.wants.isActive;
    }

    if (wants.targetPixelCount && *wants.targetPixelCount >= wants.maxPixelCount)
    {
        wants.targetPixelCount.emplace(wants.maxPixelCount);
    }
    mCurrentWants = wants;
}

const std::shared_ptr<VideoFrameBuffer> &VideoBroadcaster::GetBlackFrameBuffer(int width, int height)
{
    if (!mBlackFrameBuffer || mBlackFrameBuffer->width() != width || mBlackFrameBuffer->height() != height)
    {
        std::shared_ptr<I420Buffer> buffer = I420Buffer::create(width, height);
        I420Buffer::SetBlack(buffer.get());
        mBlackFrameBuffer = buffer;
    }

    return mBlackFrameBuffer;
}

OCTK_END_NAMESPACE
