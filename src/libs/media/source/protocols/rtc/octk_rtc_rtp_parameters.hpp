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
#include <octk_rtc_types.hpp>
#include <octk_vector_map.hpp>
#include <octk_vector.hpp>

OCTK_BEGIN_NAMESPACE

enum class RtcRtpTransceiverDirection
{
    kSendRecv,
    kSendOnly,
    kRecvOnly,
    kInactive,
    kStopped,
};

enum class RtcFecMechanism
{
    RED,
    RED_AND_ULPFEC,
    FLEXFEC,
};

enum class RtcRtcpFeedbackType
{
    CCM,
    LNTF,
    NACK,
    REMB,
    TRANSPORT_CC,
};

enum class RtcRtcpFeedbackMessageType
{
    GENERIC_NACK,
    PLI,
    FIR,
};

enum class RtcDtxStatus
{
    DISABLED,
    ENABLED,
};

enum class RtcDegradationPreference
{
    DISABLED,
    MAINTAIN_FRAMERATE,
    MAINTAIN_RESOLUTION,
    BALANCED,
};

class RtcRtcpFeedback
{
public:
    OCTK_DEFINE_SHARED_PTR(RtcRtcpFeedback)

    virtual RtcRtcpFeedbackType type() = 0;
    virtual void setType(RtcRtcpFeedbackType value) = 0;

    virtual RtcRtcpFeedbackMessageType messageType() = 0;
    virtual void setMessageType(RtcRtcpFeedbackMessageType value) = 0;

    virtual bool isEqual(const SharedPointer<RtcRtcpFeedback> &other) const = 0;

protected:
    virtual ~RtcRtcpFeedback() = default;
};

class RtcRtpExtension
{
public:
    OCTK_DEFINE_SHARED_PTR(RtcRtpExtension)

    enum class Filter
    {
        kPreferEncrypted,
        kDiscardEncrypted,
        kRequireEncrypted,
    };

    virtual String toString() const = 0;

    virtual String uri() const = 0;
    virtual void setUri(StringView uri) = 0;

    virtual int id() const = 0;
    virtual void setId(int value) = 0;

    virtual bool encrypt() const = 0;
    virtual void setEncrypt(bool value) = 0;

    virtual bool isEqual(const SharedPointer<RtcRtpExtension> &other) const = 0;

protected:
    virtual ~RtcRtpExtension() = default;
};

class RtpFecParameters
{
public:
    OCTK_DEFINE_SHARED_PTR(RtpFecParameters)

    virtual uint32_t ssrc() = 0;
    virtual void set_ssrc(uint32_t value) = 0;

    virtual RtcFecMechanism mechanism() = 0;
    virtual void set_mechanism(RtcFecMechanism value) = 0;

    virtual bool operator==(const RtpFecParameters &o) const = 0;
    virtual bool operator!=(const RtpFecParameters &o) const = 0;
};

class RtcRtpRtxParameters
{
public:
    OCTK_DEFINE_SHARED_PTR(RtcRtpRtxParameters)

    virtual uint32_t ssrc() = 0;
    virtual void set_ssrc(uint32_t value) = 0;

    virtual bool operator==(const SharedPointer<RtcRtpRtxParameters> o) const = 0;

    virtual bool operator!=(const SharedPointer<RtcRtpRtxParameters> o) const = 0;
};

class RtcRtpCodecParameters
{
public:
    OCTK_DEFINE_SHARED_PTR(RtcRtpCodecParameters)

    virtual String mimeType() const = 0;

    virtual int pTime() const = 0;
    virtual void setPTime(int value) = 0;

    virtual int maxPTime() const = 0;
    virtual void setMaxPTime(int value) = 0;

    virtual int clockRate() const = 0;
    virtual void setClockRate(int value) = 0;

    virtual String name() const = 0;
    virtual void setName(StringView name) = 0;

    virtual int payloadType() const = 0;
    virtual void setPayloadType(int value) = 0;

    virtual int numChannels() const = 0;
    virtual void setNumChannels(int value) = 0;

    virtual RtcMediaType kind() const = 0;
    virtual void setKind(RtcMediaType value) = 0;

    virtual Vector<std::pair<String, String>> parameters() const = 0;
    virtual void setParameters(const VectorMap<String, String> &parameters) = 0;

    virtual Vector<SharedPointer<RtcRtcpFeedback>> rtcpFeedback() const = 0;
    virtual void setRtcpFeedback(const Vector<SharedPointer<RtcRtcpFeedback>> &feecbacks) = 0;

    virtual bool isEqual(const SharedPointer<RtcRtpCodecParameters> &other) const = 0;

protected:
    virtual ~RtcRtpCodecParameters() = default;
};

class RtcRtcpParameters
{
public:
    OCTK_DEFINE_SHARED_PTR(RtcRtcpParameters)

    virtual uint32_t ssrc() const = 0;
    virtual void setSsrc(uint32_t value) = 0;

    virtual String cname() const = 0;
    virtual void setCName(StringView value) = 0;

    virtual bool reducedSize() const = 0;
    virtual void setReducedSize(bool value) = 0;

    virtual bool mux() const = 0;
    virtual void setMux(bool value) = 0;

    virtual bool isEqual(const SharedPointer<RtcRtcpParameters> &other) const = 0;

protected:
    virtual ~RtcRtcpParameters() = default;
};

enum class RtcPriority
{
    kVeryLow,
    kLow,
    kMedium,
    kHigh,
};

class RtcRtpEncodingParameters
{
public:
    OCTK_DEFINE_SHARED_PTR(RtcRtpEncodingParameters)

    virtual uint32_t ssrc() const = 0;
    virtual void setSsrc(uint32_t value) = 0;

    virtual double bitratePriority() const = 0;
    virtual void setBitratePriority(double value) = 0;

    virtual RtcPriority networkPriority() const = 0;
    virtual void setNetworkPriority(RtcPriority value) = 0;

    virtual int maxBitrateBps() const = 0;
    virtual void setMaxBitrateBps(int value) = 0;

    virtual int minBitrateBps() const = 0;
    virtual void setMinBitrateBps(int value) = 0;

    virtual double maxFramerate() const = 0;
    virtual void setMaxFramerate(double value) = 0;

    virtual int numTemporalLayers() const = 0;
    virtual void setNumTemporalLayers(int value) = 0;

    virtual double scaleResolutionDownBy() const = 0;
    virtual void setScaleResolutionDownBy(double value) = 0;

    virtual String scalabilityMode() const = 0;
    virtual void setScalabilityMode(StringView mode) = 0;

    virtual bool active() const = 0;
    virtual void setActive(bool value) = 0;

    virtual String rid() const = 0;
    virtual void setRid(StringView rid) = 0;

    virtual bool adaptivePtime() const = 0;
    virtual void setAdaptivePtime(bool value) = 0;

    virtual bool isEqual(const SharedPointer<RtcRtpEncodingParameters> &other) const = 0;

protected:
    virtual ~RtcRtpEncodingParameters() = default;
};

class RtcRtpParameters
{
public:
    OCTK_DEFINE_SHARED_PTR(RtcRtpParameters)

    virtual String transactionId() const = 0;
    virtual void setTransactionId(StringView id) = 0;

    virtual String mid() const = 0;
    virtual void setMid(StringView mid) = 0;

    virtual Vector<SharedPointer<RtcRtpCodecParameters>> codecs() const = 0;
    virtual void setCodecs(const Vector<SharedPointer<RtcRtpCodecParameters>> &codecs) = 0;

    virtual Vector<SharedPointer<RtcRtpExtension>> headerExtensions() const = 0;
    virtual void setHeaderExtensions(const Vector<SharedPointer<RtcRtpExtension>> &header_extensions) = 0;

    virtual Vector<SharedPointer<RtcRtpEncodingParameters>> encodings() const = 0;
    virtual void setEncodings(const Vector<SharedPointer<RtcRtpEncodingParameters>> &encodings) = 0;

    virtual SharedPointer<RtcRtcpParameters> rtcpParameters() const = 0;
    virtual void setRtcpParameters(const SharedPointer<RtcRtcpParameters> &rtcp_parameters) = 0;

    virtual RtcDegradationPreference degradationPreference() const = 0;
    virtual void setDegradationPreference(RtcDegradationPreference value) = 0;

    virtual bool isEqual(const SharedPointer<RtcRtpParameters> &other) const = 0;

protected:
    virtual ~RtcRtpParameters() = default;
};

OCTK_END_NAMESPACE
