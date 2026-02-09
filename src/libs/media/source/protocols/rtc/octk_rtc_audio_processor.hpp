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

class RtcAudioProcessor
{
public:
    using SharedPtr = SharedPointer<RtcAudioProcessor>;

    class CustomProcessing
    {
    public:
        virtual void process(int nBands, int nFrames, int bufferSize, float *buffer) = 0;

        virtual void initialize(int sampleRateHZ, int nChannels) = 0;

        virtual void reset(int newRate) = 0;

        virtual void release() = 0;

    protected:
        virtual ~CustomProcessing() = default;
    };

    virtual void setCapturePostProcessing(CustomProcessing *capturePostProcessing) = 0;
    virtual void setRenderPreProcessing(CustomProcessing *renderPreProcessing) = 0;

protected:
    virtual ~RtcAudioProcessor() = default;
};

OCTK_END_NAMESPACE
