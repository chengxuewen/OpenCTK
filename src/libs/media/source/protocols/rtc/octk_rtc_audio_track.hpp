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

#include <octk_rtc_media_track.hpp>

OCTK_BEGIN_NAMESPACE

/**
 * The RtcAudioTrack class represents an audio track in WebRTC.
 * Audio tracks are used to transmit audio data over a WebRTC peer connection.
 * This class is a subclass of the RtcMediaTrack class, which provides a base
 * interface for all media tracks in WebRTC.
 */
class RtcAudioTrack : public RtcMediaTrack
{
public:
    // volume in [0-10]
    virtual void SetVolume(double volume) = 0;

    virtual void AddSink(RtcAudioTrackSink *sink) = 0;

    virtual void RemoveSink(RtcAudioTrackSink *sink) = 0;

protected:
    virtual ~RtcAudioTrack() { }
};

OCTK_END_NAMESPACE
