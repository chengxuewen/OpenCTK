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


class RtcDtlsTransportInformation  {
public:
    enum class TransportState {
        kNew,         // Has not started negotiating yet.
        kConnecting,  // In the process of negotiating a secure connection.
        kConnected,   // Completed negotiation and verified fingerprints.
        kClosed,      // Intentionally closed.
        kFailed,      // Failure due to an error or failing to verify a remote
        // fingerprint.
        kNumValues
      };
    virtual RtcDtlsTransportInformation& operator=(
        const SharedPointer<RtcDtlsTransportInformation> c) = 0;

    virtual State state() const = 0;
    virtual int ssl_cipher_suite() const = 0;
    virtual int srtp_cipher_suite() const = 0;
};


class RtcDtlsTransport
{
    LIB_WEBRTC_API static const SharedPointer<RtcDtlsTransport> Create();

public:
    class Observer {
    public:
        virtual void OnStateChange(RtcDtlsTransportInformation info) = 0;

        virtual void OnError(const int type, const char* message) = 0;

    protected:
        virtual ~Observer() = default;
    };

    virtual const SharedPointer<RtcDtlsTransportInformation> GetInformation() = 0;

    virtual void RegisterObserver(RTCDtlsTransportObserver* observer) = 0;

    virtual void UnregisterObserver() = 0;
};


OCTK_END_NAMESPACE
