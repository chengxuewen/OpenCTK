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

#include <octk_checks.hpp>
#include <octk_video_source_base.hpp>

// #include <absl/algorithm/container.h>

#include <algorithm>

// #include "absl/algorithm/container.h"
// #include "rtc_base/checks.h"

OCTK_BEGIN_NAMESPACE

VideoSourceBase::VideoSourceBase() = default;
VideoSourceBase::~VideoSourceBase() = default;

void VideoSourceBase::addOrUpdateSink(VideoSinkInterface<VideoFrame>* sink, const VideoSinkWants& wants)
{
    OCTK_DCHECK(sink != nullptr);

    SinkPair* sink_pair = FindSinkPair(sink);
    if (!sink_pair)
    {
        sinks_.push_back(SinkPair(sink, wants));
    }
    else
    {
        sink_pair->wants = wants;
    }
}

void VideoSourceBase::removeSink(VideoSinkInterface<VideoFrame>* sink)
{
    OCTK_DCHECK(sink != nullptr);
    OCTK_DCHECK(FindSinkPair(sink));
    sinks_.erase(std::remove_if(sinks_.begin(), sinks_.end(),
                                [sink](const SinkPair& sink_pair) { return sink_pair.sink == sink; }),
                 sinks_.end());
}

VideoSourceBase::SinkPair* VideoSourceBase::FindSinkPair(const VideoSinkInterface<VideoFrame>* sink)
{
    //    auto sink_pair_it = absl::c_find_if(sinks_,
    //                                        [sink](const SinkPair &sink_pair) { return sink_pair.sink == sink; });
    //    if (sink_pair_it != sinks_.end())
    //    {
    //        return &*sink_pair_it;
    //    }
    return nullptr;
}

VideoSourceBaseGuarded::VideoSourceBaseGuarded() = default;
VideoSourceBaseGuarded::~VideoSourceBaseGuarded() = default;

void VideoSourceBaseGuarded::addOrUpdateSink(VideoSinkInterface<VideoFrame>* sink, const VideoSinkWants& wants)
{
    //    OCTK_DCHECK_RUN_ON(&source_sequence_);
    OCTK_DCHECK(sink != nullptr);

    SinkPair* sink_pair = FindSinkPair(sink);
    if (!sink_pair)
    {
        sinks_.push_back(SinkPair(sink, wants));
    }
    else
    {
        sink_pair->wants = wants;
    }
}

void VideoSourceBaseGuarded::removeSink(VideoSinkInterface<VideoFrame>* sink)
{
    //    OCTK_DCHECK_RUN_ON(&source_sequence_);
    OCTK_DCHECK(sink != nullptr);
    OCTK_DCHECK(FindSinkPair(sink));
    sinks_.erase(std::remove_if(sinks_.begin(), sinks_.end(),
                                [sink](const SinkPair& sink_pair) { return sink_pair.sink == sink; }),
                 sinks_.end());
}

VideoSourceBaseGuarded::SinkPair* VideoSourceBaseGuarded::FindSinkPair(const VideoSinkInterface<VideoFrame>* sink)
{
    //    OCTK_DCHECK_RUN_ON(&source_sequence_);
    //    auto sink_pair_it = absl::c_find_if(sinks_,
    //                                        [sink](const SinkPair &sink_pair) { return sink_pair.sink == sink; });
    //    if (sink_pair_it != sinks_.end())
    //    {
    //        return &*sink_pair_it;
    //    }
    return nullptr;
}

const std::vector<VideoSourceBaseGuarded::SinkPair>& VideoSourceBaseGuarded::sinkPairs() const
{
    //  OCTK_DCHECK_RUN_ON(&source_sequence_);
    return sinks_;
}

OCTK_END_NAMESPACE
