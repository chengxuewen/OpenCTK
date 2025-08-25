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

#ifndef _OCTK_VIDEO_SOURCE_BASE_HPP
#define _OCTK_VIDEO_SOURCE_BASE_HPP

#include <octk_video_source_interface.hpp>
#include <octk_video_sink_interface.hpp>
#include <octk_sequence_checker.hpp>
#include <octk_video_frame.hpp>

#include <vector>

OCTK_BEGIN_NAMESPACE

// VideoSourceBase is not thread safe. Before using this class, consider using
// VideoSourceBaseGuarded below instead, which is an identical implementation
// but applies a sequence checker to help protect internal state.
// TODO(bugs.webrtc.org/12780): Delete this class.
class VideoSourceBase : public VideoSourceInterface<VideoFrame>
{
public:
    VideoSourceBase();
    ~VideoSourceBase() override;

    void addOrUpdateSink(VideoSinkInterface<VideoFrame> *sink, const VideoSinkWants &wants) override;
    void removeSink(VideoSinkInterface<VideoFrame> *sink) override;

protected:
    struct SinkPair
    {
        SinkPair(VideoSinkInterface<VideoFrame> *sink, VideoSinkWants wants)
            : sink(sink)
            , wants(wants)
        {
        }
        VideoSinkInterface<VideoFrame> *sink;
        VideoSinkWants wants;
    };
    SinkPair *FindSinkPair(const VideoSinkInterface<VideoFrame> *sink);

    const std::vector<SinkPair> &sinkPairs() const { return sinks_; }

private:
    std::vector<SinkPair> sinks_;
};

// VideoSourceBaseGuarded assumes that operations related to sinks, occur on the
// same TQ/thread that the object was constructed on.
class VideoSourceBaseGuarded : public VideoSourceInterface<VideoFrame>
{
public:
    VideoSourceBaseGuarded();
    ~VideoSourceBaseGuarded() override;

    void addOrUpdateSink(VideoSinkInterface<VideoFrame> *sink, const VideoSinkWants &wants) override;
    void removeSink(VideoSinkInterface<VideoFrame> *sink) override;

protected:
    struct SinkPair
    {
        SinkPair(VideoSinkInterface<VideoFrame> *sink, VideoSinkWants wants)
            : sink(sink)
            , wants(wants)
        {
        }
        VideoSinkInterface<VideoFrame> *sink;
        VideoSinkWants wants;
    };

    SinkPair *FindSinkPair(const VideoSinkInterface<VideoFrame> *sink);
    const std::vector<SinkPair> &sinkPairs() const;

    // Keep the `source_sequence_` checker protected to allow sub classes the
    // ability to call Detach() if/when appropriate.
    OCTK_ATTRIBUTE_NO_UNIQUE_ADDRESS SequenceChecker source_sequence_;

private:
    std::vector<SinkPair> sinks_ OCTK_ATTRIBUTE_GUARDED_BY(&source_sequence_);
};

OCTK_END_NAMESPACE

#endif // _OCTK_VIDEO_SOURCE_BASE_HPP
