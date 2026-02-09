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

#include <octk_rtc_rtp_capabilities.hpp>
#include <octk_rtc_rtp_parameters.hpp>
#include <octk_rtc_rtp_receiver.hpp>
#include <octk_rtc_rtp_sender.hpp>
#include <octk_rtc_types.hpp>
#include <octk_vector.hpp>
#include <octk_status.hpp>

OCTK_BEGIN_NAMESPACE

class RtcRtpTransceiverInit
{
public:
    OCTK_DEFINE_SHARED_PTR(RtcRtpTransceiverInit)

    virtual Vector<String> streamIds() const = 0;
    virtual void setStreamIds(const Vector<String> &ids) = 0;

    virtual RtcRtpTransceiverDirection direction() const = 0;
    virtual void setDirection(RtcRtpTransceiverDirection value) = 0;

    virtual Vector<RtcRtpEncodingParameters::SharedPtr> sendEncodings() const = 0;
    virtual void setSendEncodings(const Vector<RtcRtpEncodingParameters::SharedPtr> &sendEncodings) = 0;

protected:
    virtual ~RtcRtpTransceiverInit() = default;
};

class RtcRtpTransceiver
{
public:
    OCTK_DEFINE_SHARED_PTR(RtcRtpTransceiver)

    virtual void stopInternal() = 0;
    virtual String stopStandard() = 0;

    virtual String mid() const = 0;

    virtual bool isStopped() const = 0;
    virtual bool isStopping() const = 0;

    virtual String transceiverId() const = 0;
    virtual RtcMediaType mediaType() const = 0;

    virtual RtcRtpSender::SharedPtr sender() const = 0;
    virtual RtcRtpReceiver::SharedPtr receiver() const = 0;

    virtual RtcRtpTransceiverDirection firedDirection() const = 0;
    virtual RtcRtpTransceiverDirection currentCirection() const = 0;

    virtual RtcRtpTransceiverDirection direction() const = 0;
    virtual Status setDirection(RtcRtpTransceiverDirection newDirection) = 0;

    virtual void setCodecPreferences(const Vector<RtcRtpCodecCapability::SharedPtr> &codecs) = 0;

protected:
    virtual ~RtcRtpTransceiver() = default;
};

OCTK_END_NAMESPACE
