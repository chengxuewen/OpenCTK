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

#include <octk_rtc_types.hpp>

OCTK_BEGIN_NAMESPACE

class RtcRtpCodecCapability
{
public:
    OCTK_DEFINE_SHARED_PTR(RtcRtpCodecCapability)

    virtual int channels() const = 0;
    virtual void setChannels(int channels) = 0;

    virtual int clockRate() const = 0;
    virtual void setClockRate(int clockRate) = 0;

    virtual String mimeType() const = 0;
    virtual void setMimeType(StringView mimeType) = 0;

    virtual String sdpFmtpLine() const = 0;
    virtual void setSdpFmtpLine(StringView sdpFmtpLine) = 0;

protected:
    virtual ~RtcRtpCodecCapability() = default;
};

class RtcRtpHeaderExtensionCapability
{
public:
    OCTK_DEFINE_SHARED_PTR(RtcRtpHeaderExtensionCapability)

    virtual String uri() = 0;
    virtual void setUri(StringView uri) = 0;

    virtual int preferredId() = 0;
    virtual void setPreferredId(int value) = 0;

    virtual bool preferredEncrypt() = 0;
    virtual void setPreferredEncrypt(bool value) = 0;

protected:
    virtual ~RtcRtpHeaderExtensionCapability() = default;
};

class RtcRtpCapabilities
{
public:
    OCTK_DEFINE_SHARED_PTR(RtcRtpCapabilities)

    virtual Vector<SharedPointer<RtcRtpCodecCapability>> codecs() = 0;
    virtual void setCodecs(const Vector<SharedPointer<RtcRtpCodecCapability>> &codecs) = 0;

    virtual Vector<SharedPointer<RtcRtpHeaderExtensionCapability>> headerExtensions() = 0;
    virtual void setHeaderExtensions(
        const Vector<SharedPointer<RtcRtpHeaderExtensionCapability>> &headerExtensions) = 0;

    // virtual const Vector<const SharedPointer<RtcFecMechanism>> fec() = 0;
    // virtual void set_fec(const Vector<const SharedPointer<RtcFecMechanism>> fec) = 0;

protected:
    virtual ~RtcRtpCapabilities() = default;
};

OCTK_END_NAMESPACE
