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

#pragma once

#include <octk_rtc_media_source.hpp>
#include <octk_rtc_video_frame.hpp>
#include <octk_rtc_audio_track.hpp>
#include <octk_shared_pointer.hpp>

OCTK_BEGIN_NAMESPACE

class RtcVideoTrackSource : public RtcMediaSource, public RtcVideoSource
{
public:
    OCTK_DEFINE_SHARED_PTR(RtcVideoTrackSource)

    struct Stats
    {
        // Original size of captured frame, before video adaptation.
        int inputWidth;
        int inputHeight;
    };

    virtual bool getStats(Stats *stats) = 0;
};

class RtcVideoTrack : public RtcMediaTrack, public RtcVideoSource
{
public:
    OCTK_DEFINE_SHARED_PTR(RtcVideoTrack)

    enum class ContentHint
    {
        kNone,
        kFluid,
        kDetailed,
        kText
    };

    virtual ContentHint contentHint() const = 0;
    virtual void setContentHint(ContentHint hint) = 0;

    virtual SharedPointer<RtcVideoTrackSource> getSource() const = 0;
};

OCTK_END_NAMESPACE
