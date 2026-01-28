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
#include <octk_rtc_types.hpp>
#include <octk_vector.hpp>

OCTK_BEGIN_NAMESPACE

class RtcMediaTrack;
class RtcDtlsTransport;
class RtcDtmfSender;

class RtcRtpSender
{
public:
    virtual bool set_track(const SharedPointer<RtcMediaTrack> track) = 0;

    virtual const SharedPointer<RtcMediaTrack> track() const = 0;

    virtual const SharedPointer<RtcDtlsTransport> dtls_transport() const = 0;

    virtual uint32_t ssrc() const = 0;

    virtual RtcMediaType media_type() const = 0;

    virtual const String id() const = 0;

    virtual const Vector<String> stream_ids() const = 0;

    virtual void set_stream_ids(const Vector<String> stream_ids) const = 0;

    virtual const Vector<const SharedPointer<RtcRtpEncodingParameters>> init_send_encodings() const = 0;

    virtual const SharedPointer<RtcRtpParameters> parameters() const = 0;

    virtual bool set_parameters(const SharedPointer<RtcRtpParameters> parameters) = 0;

    virtual const SharedPointer<RtcDtmfSender> dtmf_sender() const = 0;
};

OCTK_END_NAMESPACE
