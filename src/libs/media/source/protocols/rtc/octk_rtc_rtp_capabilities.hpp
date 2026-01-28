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

#include <octk_rtc_rtp_parameters.hpp>

OCTK_BEGIN_NAMESPACE

class RtcRtpCodecCapability
{
public:
    static const SharedPointer<RtcRtpCodecCapability> Create();

    virtual void set_mime_type(const String &mime_type) = 0;
    virtual void set_clock_rate(int clock_rate) = 0;
    virtual void set_channels(int channels) = 0;
    virtual void set_sdp_fmtp_line(const String &sdp_fmtp_line) = 0;

    virtual String mime_type() const = 0;
    virtual int clock_rate() const = 0;
    virtual int channels() const = 0;
    virtual String sdp_fmtp_line() const = 0;

protected:
    virtual ~RtcRtpCodecCapability() { }
};

class RtcRtpHeaderExtensionCapability
{
public:
    virtual const String uri() = 0;
    virtual void set_uri(const String uri) = 0;

    virtual int preferred_id() = 0;
    virtual void set_preferred_id(int value) = 0;

    virtual bool preferred_encrypt() = 0;
    virtual void set_preferred_encrypt(bool value) = 0;
};

class RtcRtpCapabilities
{
public:
    virtual const Vector<const SharedPointer<RtcRtpCodecCapability>> codecs() = 0;
    virtual void set_codecs(const Vector<const SharedPointer<RtcRtpCodecCapability>> codecs) = 0;

    virtual const Vector<const SharedPointer<RtcRtpHeaderExtensionCapability>> header_extensions() = 0;

    virtual void set_header_extensions(
        const Vector<const SharedPointer<RtcRtpHeaderExtensionCapability>> header_extensions) = 0;

    // virtual const Vector<const SharedPointer<RtcFecMechanism>> fec() = 0;
    // virtual void set_fec(const Vector<const SharedPointer<RtcFecMechanism>> fec) = 0;
};

OCTK_END_NAMESPACE
