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
    virtual RtcRtcpFeedbackType type() = 0;
    virtual void set_type(RtcRtcpFeedbackType value) = 0;

    virtual RtcRtcpFeedbackMessageType message_type() = 0;
    virtual void set_message_type(RtcRtcpFeedbackMessageType value) = 0;

    virtual bool operator==(const SharedPointer<RtcRtcpFeedback> o) = 0;
    virtual bool operator!=(const SharedPointer<RtcRtcpFeedback> o) = 0;
};

class RtcRtpExtension
{
public:
    enum RTCFilter
    {
        kDiscardEncryptedExtension,
        kPreferEncryptedExtension,
        kRequireEncryptedExtension,
    };

    virtual const String ToString() const = 0;
    virtual bool operator==(const SharedPointer<RtcRtpExtension> o) const = 0;

    virtual const String uri() = 0;
    virtual void set_uri(const String uri) = 0;

    virtual int id() = 0;
    virtual void set_id(int value) = 0;

    virtual bool encrypt() = 0;
    virtual void set_encrypt(bool value) = 0;
};

class RtpFecParameters
{
    virtual uint32_t ssrc() = 0;
    virtual void set_ssrc(uint32_t value) = 0;

    virtual RtcFecMechanism mechanism() = 0;
    virtual void set_mechanism(RtcFecMechanism value) = 0;

    virtual bool operator==(const RtpFecParameters &o) const = 0;
    virtual bool operator!=(const RtpFecParameters &o) const = 0;
};

class RtcRtpRtxParameters
{
    virtual uint32_t ssrc() = 0;
    virtual void set_ssrc(uint32_t value) = 0;

    virtual bool operator==(const SharedPointer<RtcRtpRtxParameters> o) const = 0;

    virtual bool operator!=(const SharedPointer<RtcRtpRtxParameters> o) const = 0;
};

class RtcRtpCodecParameters
{
public:
    virtual const String mime_type() const = 0;

    virtual const String name() = 0;
    virtual void set_name(const String name) = 0;

    virtual RtcMediaType kind() = 0;
    virtual void set_kind(RtcMediaType value) = 0;

    virtual int payload_type() = 0;
    virtual void set_payload_type(int value) = 0;

    virtual int clock_rate() = 0;
    virtual void set_clock_rate(int value) = 0;

    virtual int num_channels() = 0;
    virtual void set_num_channels(int value) = 0;

    virtual int max_ptime() = 0;
    virtual void set_max_ptime(int value) = 0;

    virtual int ptime() = 0;
    virtual void set_ptime(int value) = 0;

    virtual const Vector<const SharedPointer<RtcRtcpFeedback>> rtcp_feedback() = 0;
    virtual void set_rtcp_feedback(const Vector<const SharedPointer<RtcRtcpFeedback>> feecbacks) = 0;

    virtual const Vector<std::pair<String, String>> parameters() = 0;
    virtual void set_parameters(const VectorMap<String, String> parameters) = 0;

    virtual bool operator==(const SharedPointer<RtcRtpCodecParameters> o) = 0;
    virtual bool operator!=(const SharedPointer<RtcRtpCodecParameters> o) = 0;

protected:
    virtual ~RtcRtpCodecParameters() { }
};

class RtcRtcpParameters
{
public:
    virtual uint32_t ssrc() = 0;
    virtual void set_ssrc(uint32_t value) = 0;

    virtual const String cname() = 0;
    virtual void set_cname(const String) = 0;

    virtual bool reduced_size() = 0;
    virtual void set_reduced_size(bool value) = 0;

    virtual bool mux() = 0;
    virtual void set_mux(bool value) = 0;

    virtual bool operator==(const SharedPointer<RtcRtcpParameters> o) const = 0;
    virtual bool operator!=(const SharedPointer<RtcRtcpParameters> o) const = 0;
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
    static SharedPointer<RtcRtpEncodingParameters> Create();

    virtual uint32_t ssrc() = 0;
    virtual void set_ssrc(uint32_t value) = 0;

    virtual double bitrate_priority() = 0;
    virtual void set_bitrate_priority(double value) = 0;

    virtual RtcPriority network_priority() = 0;
    virtual void set_network_priority(RtcPriority value) = 0;

    virtual int max_bitrate_bps() = 0;
    virtual void set_max_bitrate_bps(int value) = 0;

    virtual int min_bitrate_bps() = 0;
    virtual void set_min_bitrate_bps(int value) = 0;

    virtual double max_framerate() = 0;
    virtual void set_max_framerate(double value) = 0;

    virtual int num_temporal_layers() = 0;
    virtual void set_num_temporal_layers(int value) = 0;

    virtual double scale_resolution_down_by() = 0;
    virtual void set_scale_resolution_down_by(double value) = 0;

    virtual const String scalability_mode() = 0;
    virtual void set_scalability_mode(const String mode) = 0;

    virtual bool active() = 0;
    virtual void set_active(bool value) = 0;

    virtual const String rid() = 0;
    virtual void set_rid(const String rid) = 0;

    virtual bool adaptive_ptime() = 0;
    virtual void set_adaptive_ptime(bool value) = 0;

    virtual bool operator==(const SharedPointer<RtcRtpEncodingParameters> o) const = 0;
    virtual bool operator!=(const SharedPointer<RtcRtpEncodingParameters> o) const = 0;
};

struct RtcRtpParameters
{
public:
    // static const SharedPointer<RtcRtpParameters> Create();
    virtual const String transaction_id() = 0;
    virtual void set_transaction_id(const String id) = 0;

    virtual const String mid() = 0;
    virtual void set_mid(const String mid) = 0;

    virtual const Vector<const SharedPointer<RtcRtpCodecParameters>> codecs() = 0;
    virtual void set_codecs(const Vector<const SharedPointer<RtcRtpCodecParameters>> codecs) = 0;

    virtual const Vector<const SharedPointer<RtcRtpExtension>> header_extensions() = 0;
    virtual void set_header_extensions(const Vector<const SharedPointer<RtcRtpExtension>> header_extensions) = 0;

    virtual const Vector<const SharedPointer<RtcRtpEncodingParameters>> encodings() = 0;
    virtual void set_encodings(const Vector<const SharedPointer<RtcRtpEncodingParameters>> encodings) = 0;

    virtual const SharedPointer<RtcRtcpParameters> rtcp_parameters() = 0;
    virtual void set_rtcp_parameters(const SharedPointer<RtcRtcpParameters> rtcp_parameters) = 0;

    virtual RtcDegradationPreference GetDegradationPreference() = 0;
    virtual void SetDegradationPreference(RtcDegradationPreference value) = 0;

    virtual bool operator==(const SharedPointer<RtcRtpParameters> o) const = 0;
    virtual bool operator!=(const SharedPointer<RtcRtpParameters> o) const = 0;
};

OCTK_END_NAMESPACE
