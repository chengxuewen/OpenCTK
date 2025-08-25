/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
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

#ifndef _OCTK_RTP_PACKET_INFO_HPP
#define _OCTK_RTP_PACKET_INFO_HPP

#include <octk_rtp_headers.hpp>
#include <octk_time_delta.hpp>

#include <cstdint>
#include <optional>
#include <utility>
#include <vector>

OCTK_BEGIN_NAMESPACE

//
// Structure to hold information about a received `RtpPacket`. It is primarily
// used to carry per-packet information from when a packet is received until
// the information is passed to `SourceTracker`.
//
class OCTK_MEDIA_API RtpPacketInfo
{
public:
    RtpPacketInfo();

    RtpPacketInfo(uint32_t ssrc,
                  std::vector<uint32_t> csrcs,
                  uint32_t rtp_timestamp,
                  Timestamp receive_time);

    RtpPacketInfo(const RTPHeader &rtp_header, Timestamp receive_time);

    RtpPacketInfo(const RtpPacketInfo &other) = default;
    RtpPacketInfo(RtpPacketInfo &&other) = default;
    RtpPacketInfo &operator=(const RtpPacketInfo &other) = default;
    RtpPacketInfo &operator=(RtpPacketInfo &&other) = default;

    uint32_t ssrc() const { return mSsrc; }
    void set_ssrc(uint32_t value) { mSsrc = value; }

    const std::vector<uint32_t> &csrcs() const { return csrcs_; }
    void set_csrcs(std::vector<uint32_t> value) { csrcs_ = std::move(value); }

    uint32_t rtp_timestamp() const { return rtp_timestamp_; }
    void set_rtp_timestamp(uint32_t value) { rtp_timestamp_ = value; }

    Timestamp receive_time() const { return receive_time_; }
    void set_receive_time(Timestamp value) { receive_time_ = value; }

    Optional<uint8_t> audio_level() const { return audio_level_; }
    RtpPacketInfo &set_audio_level(Optional<uint8_t> value)
    {
        audio_level_ = value;
        return *this;
    }

    const Optional<AbsoluteCaptureTime> &absolute_capture_time() const
    {
        return absolute_capture_time_;
    }
    RtpPacketInfo &set_absolute_capture_time(
        const Optional<AbsoluteCaptureTime> &value)
    {
        absolute_capture_time_ = value;
        return *this;
    }

    const Optional<TimeDelta> &local_capture_clock_offset() const
    {
        return local_capture_clock_offset_;
    }
    RtpPacketInfo &set_local_capture_clock_offset(
        Optional<TimeDelta> value)
    {
        local_capture_clock_offset_ = value;
        return *this;
    }

private:
    // Fields from the RTP header:
    // https://tools.ietf.org/html/rfc3550#section-5.1
    uint32_t mSsrc;
    std::vector<uint32_t> csrcs_;
    uint32_t rtp_timestamp_;

    // Local `webrtc::Clock`-based timestamp of when the packet was received.
    Timestamp receive_time_;

    // Fields from the Audio Level header extension:
    // https://tools.ietf.org/html/rfc6464#section-3
    Optional<uint8_t> audio_level_;

    // Fields from the Absolute Capture Time header extension:
    // http://www.webrtc.org/experiments/rtp-hdrext/abs-capture-time
    Optional<AbsoluteCaptureTime> absolute_capture_time_;

    // Clock offset between the local clock and the capturer's clock.
    // Do not confuse with `AbsoluteCaptureTime::estimated_capture_clock_offset`
    // which instead represents the clock offset between a remote sender and the
    // capturer. The following holds:
    //   Capture's NTP Clock = Local NTP Clock + Local-Capture Clock Offset
    Optional<TimeDelta> local_capture_clock_offset_;
};

OCTK_MEDIA_API bool operator==(const RtpPacketInfo &lhs, const RtpPacketInfo &rhs);

inline bool operator!=(const RtpPacketInfo &lhs, const RtpPacketInfo &rhs)
{
    return !(lhs == rhs);
}
OCTK_END_NAMESPACE

#endif // _OCTK_RTP_PACKET_INFO_HPP
