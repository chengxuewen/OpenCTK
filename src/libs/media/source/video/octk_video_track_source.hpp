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

#ifndef _OCTK_VIDEO_TRACK_SOURCE_HPP
#define _OCTK_VIDEO_TRACK_SOURCE_HPP

#include <octk_recordable_encoded_frame.hpp>
#include <octk_video_source_interface.hpp>
#include <octk_media_stream_interface.hpp>
#include <octk_video_sink_interface.hpp>
#include <octk_video_frame.hpp>
#include <octk_optional.hpp>

OCTK_BEGIN_NAMESPACE

// VideoTrackSource is a convenience base class for implementations of VideoTrackSourceInterface.
class OCTK_MEDIA_API VideoTrackSource : public Notifier<VideoTrackSourceInterface>
{
public:
    explicit VideoTrackSource(bool remote);

    SourceState state() const override
    {
        OCTK_DCHECK_RUN_ON(&mSignalingThreadChecker);
        return mState;
    }

    void setState(SourceState newState);

    bool remote() const override { return mIsRemote; }

    bool isScreencast() const override { return false; }
    Optional<bool> needsDenoising() const override { return utils::nullopt; }

    bool getStats(Stats *stats) override { return false; }

    void addOrUpdateSink(VideoSinkInterface<VideoFrame> *sink, const VideoSinkWants &wants) override;
    void removeSink(VideoSinkInterface<VideoFrame> *sink) override;

    bool isSupportsEncodedOutput() const override { return false; }
    void generateKeyFrame() override { }
    void addEncodedSink(VideoSinkInterface<RecordableEncodedFrame> *sink) override { }
    void removeEncodedSink(VideoSinkInterface<RecordableEncodedFrame> *sink) override { }

protected:
    virtual VideoSourceInterface<VideoFrame> *source() = 0;

private:
    OCTK_ATTRIBUTE_NO_UNIQUE_ADDRESS ContextChecker mWorkerThreadChecker{ContextChecker::InitialState::kDetached};
    OCTK_ATTRIBUTE_NO_UNIQUE_ADDRESS ContextChecker mSignalingThreadChecker;
    SourceState mState OCTK_ATTRIBUTE_GUARDED_BY(&mSignalingThreadChecker);
    const bool mIsRemote;
};

OCTK_END_NAMESPACE

#endif //  _OCTK_VIDEO_TRACK_SOURCE_HPP
