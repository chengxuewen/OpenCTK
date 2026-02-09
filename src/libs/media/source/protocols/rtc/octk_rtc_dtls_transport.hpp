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

enum class RtcDtlsTransportState
{
    kNew,        // Has not started negotiating yet.
    kConnecting, // In the process of negotiating a secure connection.
    kConnected,  // Completed negotiation and verified fingerprints.
    kClosed,     // Intentionally closed.
    kFailed,     // Failure due to an error or failing to verify a remote
    // fingerprint.
    kNumValues
};

class RtcDtlsTransportInformation
{
public:
    OCTK_DEFINE_SHARED_PTR(RtcDtlsTransportInformation);

    virtual void copy(const RtcDtlsTransportInformation::SharedPtr &other) = 0;
    virtual RtcDtlsTransportState state() const = 0;
    virtual int srtpCipherSuite() const = 0;
    virtual int sslCipherSuite() const = 0;

protected:
    virtual ~RtcDtlsTransportInformation() = default;
};

class RtcDtlsTransport
{
public:
    OCTK_DEFINE_SHARED_PTR(RtcDtlsTransport);

    class Observer
    {
    public:
        virtual void onStateChange(const RtcDtlsTransportInformation::SharedPtr &info) = 0;

        virtual void onError(int type, const char *message) = 0;

    protected:
        virtual ~Observer() = default;
    };

    virtual RtcDtlsTransportInformation::SharedPtr getInformation() = 0;

    virtual void registerObserver(Observer *observer) = 0;

    virtual void unregisterObserver() = 0;

protected:
    virtual ~RtcDtlsTransport() = default;
};


OCTK_END_NAMESPACE
