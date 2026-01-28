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

class RtcDtmfSender  {
public:
    class Observer {
    public:
        virtual void OnToneChange(const String tone, const String tone_buffer) = 0;

        virtual void OnToneChange(const String tone) = 0;

    protected:
        virtual ~Observer() = default;
    };

    static const int kDtmfDefaultCommaDelayMs = 2000;

    virtual void RegisterObserver(RTCDtmfSenderObserver* observer) = 0;

    virtual void UnregisterObserver() = 0;

    virtual bool InsertDtmf(const String tones, int duration,
                            int inter_tone_gap) = 0;

    virtual bool InsertDtmf(const String tones, int duration, int inter_tone_gap,
                            int comma_delay) = 0;

    virtual bool CanInsertDtmf() = 0;

    virtual const String tones() const = 0;

    virtual int duration() const = 0;

    virtual int inter_tone_gap() const = 0;

    virtual int comma_delay() const = 0;
};

OCTK_END_NAMESPACE
