//
// Created by cxw on 25-9-5.
//

#ifndef _OCTK_RTP_SOURCE_HPP
#define _OCTK_RTP_SOURCE_HPP

#include <octk_rtp_headers.hpp>
#include <octk_time_delta.hpp>
#include <octk_timestamp.hpp>
#include <octk_optional.hpp>

#include <stdint.h>

OCTK_BEGIN_NAMESPACE

enum class RtpSourceType
{
    SSRC,
    CSRC,
};

class RtpSource
{
public:
    struct Extensions
    {
        Optional<uint8_t> audio_level;

        // Fields from the Absolute Capture Time header extension:
        // http://www.webrtc.org/experiments/rtp-hdrext/abs-capture-time
        Optional<AbsoluteCaptureTime> absolute_capture_time;

        // Clock offset between the local clock and the capturer's clock.
        // Do not confuse with `AbsoluteCaptureTime::estimated_capture_clock_offset`
        // which instead represents the clock offset between a remote sender and the
        // capturer. The following holds:
        //   Capture's NTP Clock = Local NTP Clock + Local-Capture Clock Offset
        Optional<TimeDelta> local_capture_clock_offset;
    };

    RtpSource() = delete;

    RtpSource(Timestamp timestamp,
              uint32_t source_id,
              RtpSourceType source_type,
              uint32_t rtp_timestamp,
              const RtpSource::Extensions &extensions)
        : timestamp_(timestamp)
        , source_id_(source_id)
        , source_type_(source_type)
        , extensions_(extensions)
        , rtp_timestamp_(rtp_timestamp)
    {
    }

    RtpSource(const RtpSource &) = default;
    RtpSource &operator=(const RtpSource &) = default;
    ~RtpSource() = default;

    Timestamp timestamp() const { return timestamp_; }

    // The identifier of the source can be the CSRC or the SSRC.
    uint32_t source_id() const { return source_id_; }

    // The source can be either a contributing source or a synchronization source.
    RtpSourceType source_type() const { return source_type_; }

    Optional<uint8_t> audio_level() const { return extensions_.audio_level; }

    void set_audio_level(const Optional<uint8_t> &level) { extensions_.audio_level = level; }

    uint32_t rtp_timestamp() const { return rtp_timestamp_; }

    Optional<AbsoluteCaptureTime> absolute_capture_time() const { return extensions_.absolute_capture_time; }

    Optional<TimeDelta> local_capture_clock_offset() const { return extensions_.local_capture_clock_offset; }

    bool operator==(const RtpSource &o) const
    {
        return timestamp_ == o.timestamp() && source_id_ == o.source_id() && source_type_ == o.source_type() &&
               extensions_.audio_level == o.extensions_.audio_level &&
               extensions_.absolute_capture_time == o.extensions_.absolute_capture_time &&
               rtp_timestamp_ == o.rtp_timestamp();
    }

private:
    Timestamp timestamp_;
    uint32_t source_id_;
    RtpSourceType source_type_;
    RtpSource::Extensions extensions_;
    uint32_t rtp_timestamp_;
};

OCTK_END_NAMESPACE

#endif // _OCTK_RTP_SOURCE_HPP
