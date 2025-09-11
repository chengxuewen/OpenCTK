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

#include <octk_video_source_base.hpp>
#include <octk_checks.hpp>

#include <algorithm>

OCTK_BEGIN_NAMESPACE

VideoSourceBase::VideoSourceBase() = default;
VideoSourceBase::~VideoSourceBase() = default;

void VideoSourceBase::addOrUpdateSink(VideoSinkInterface<VideoFrame>* sink, const VideoSinkWants& wants)
{
    OCTK_DCHECK(sink != nullptr);

    SinkPair* sinkPair = FindSinkPair(sink);
    if (!sinkPair)
    {
        mSinks.push_back(SinkPair(sink, wants));
    }
    else
    {
        sinkPair->wants = wants;
    }
}

void VideoSourceBase::removeSink(VideoSinkInterface<VideoFrame>* sink)
{
    OCTK_DCHECK(sink != nullptr);
    OCTK_DCHECK(FindSinkPair(sink));
    mSinks.erase(std::remove_if(mSinks.begin(), mSinks.end(),
                                [sink](const SinkPair& sink_pair) { return sink_pair.sink == sink; }),
                 mSinks.end());
}

VideoSourceBase::SinkPair* VideoSourceBase::FindSinkPair(const VideoSinkInterface<VideoFrame>* sink)
{
    auto iter = std::find_if(mSinks.begin(), mSinks.end(),
                                        [sink](const SinkPair &sinkPair) { return sinkPair.sink == sink; });
    if (iter != mSinks.end())
    {
        return &*iter;
    }
    return nullptr;
}

VideoSourceBaseGuarded::VideoSourceBaseGuarded() = default;
VideoSourceBaseGuarded::~VideoSourceBaseGuarded() = default;

void VideoSourceBaseGuarded::addOrUpdateSink(VideoSinkInterface<VideoFrame>* sink, const VideoSinkWants& wants)
{
    OCTK_DCHECK_RUN_ON(&mSourceSequence);
    OCTK_DCHECK(sink != nullptr);

    SinkPair* sink_pair = FindSinkPair(sink);
    if (!sink_pair)
    {
        mSinks.push_back(SinkPair(sink, wants));
    }
    else
    {
        sink_pair->wants = wants;
    }
}

void VideoSourceBaseGuarded::removeSink(VideoSinkInterface<VideoFrame>* sink)
{
    OCTK_DCHECK_RUN_ON(&mSourceSequence);
    OCTK_DCHECK(sink != nullptr);
    OCTK_DCHECK(FindSinkPair(sink));
    mSinks.erase(std::remove_if(mSinks.begin(), mSinks.end(),
                                [sink](const SinkPair& sink_pair) { return sink_pair.sink == sink; }),
                 mSinks.end());
}

VideoSourceBaseGuarded::SinkPair* VideoSourceBaseGuarded::FindSinkPair(const VideoSinkInterface<VideoFrame>* sink)
{
    OCTK_DCHECK_RUN_ON(&mSourceSequence);
    auto iter = std::find_if(mSinks.begin(), mSinks.end(),
                                        [sink](const SinkPair &sinkPair) { return sinkPair.sink == sink; });
    if (iter != mSinks.end())
    {
        return &*iter;
    }
    return nullptr;
}

const std::vector<VideoSourceBaseGuarded::SinkPair>& VideoSourceBaseGuarded::sinkPairs() const
{
    OCTK_DCHECK_RUN_ON(&mSourceSequence);
    return mSinks;
}

OCTK_END_NAMESPACE
