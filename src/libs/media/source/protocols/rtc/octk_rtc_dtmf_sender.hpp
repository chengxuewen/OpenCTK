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

#include <octk_rtc_audio_track.hpp>

OCTK_BEGIN_NAMESPACE

class RtcDtmfSender
{
public:
    OCTK_DEFINE_SHARED_PTR(RtcDtmfSender);

    class Observer
    {
    public:
        virtual void onToneChange(const StringView tone, const StringView toneBuffer) = 0;

        virtual void onToneChange(const StringView tone) = 0;

    protected:
        virtual ~Observer() = default;
    };

    OCTK_STATIC_CONSTANT_NUMBER(kDtmfDefaultCommaDelayMs, 2000)

    virtual void registerObserver(Observer *observer) = 0;
    virtual void unregisterObserver() = 0;

    virtual bool insertDtmf(const StringView tones, int duration, int interToneGap, int commaDelay) = 0;
    virtual bool insertDtmf(const StringView tones, int duration, int interToneGap) = 0;

    virtual int interToneGap() const = 0;

    virtual int commaDelay() const = 0;

    virtual bool canInsertDtmf() = 0;

    virtual String tones() const = 0;

    virtual int duration() const = 0;
};

OCTK_END_NAMESPACE
