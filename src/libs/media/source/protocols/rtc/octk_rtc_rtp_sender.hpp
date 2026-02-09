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

#include "octk_rtc_dtls_transport.hpp"
#include "octk_rtc_dtmf_sender.hpp"


#include <octk_rtc_rtp_parameters.hpp>
#include <octk_rtc_types.hpp>
#include <octk_vector.hpp>

OCTK_BEGIN_NAMESPACE

class RtcMediaTrack;
class RtcDtmfSender;
class RtcDtlsTransport;

class RtcRtpSender
{
public:
    OCTK_DEFINE_SHARED_PTR(RtcRtpSender);

    virtual Vector<RtcRtpEncodingParameters::SharedPtr> initSendEncodings() const = 0;

    virtual RtcRtpParameters::SharedPtr parameters() const = 0;
    virtual bool setParameters(const RtcRtpParameters::SharedPtr &parameters) = 0;

    virtual RtcMediaTrack::SharedPtr track() const = 0;
    virtual bool setTrack(const RtcMediaTrack::SharedPtr &track) = 0;

    virtual Vector<String> streamIds() const = 0;
    virtual void setStreamIds(const Vector<String> &streamIds) const = 0;

    virtual RtcDtmfSender::SharedPtr dtmfSender() const = 0;
    virtual RtcDtlsTransport::SharedPtr dtlsTransport() const = 0;

    virtual RtcMediaType mediaType() const = 0;
    virtual uint32_t ssrc() const = 0;
    virtual String id() const = 0;

protected:
    virtual ~RtcRtpSender() = default;
};

OCTK_END_NAMESPACE
