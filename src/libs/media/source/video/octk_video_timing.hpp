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

#ifndef _OCTK_VIDEO_TIMING_HPP
#define _OCTK_VIDEO_TIMING_HPP

#include <octk_media_global.hpp>
#include <octk_time_delta.hpp>

#include <limits>
#include <string>

OCTK_BEGIN_NAMESPACE

// Video timing timestamps in ms counted from capture_time_ms of a frame.
// This structure represents data sent in video-timing RTP header extension.
struct OCTK_MEDIA_API VideoSendTiming
{
    enum TimingFrameFlags : uint8_t
    {
        kNotTriggered = 0,  // Timing info valid, but not to be transmitted.
        // Used on send-side only.
        kTriggeredByTimer = 1 << 0,  // Frame marked for tracing by periodic timer.
        kTriggeredBySize = 1 << 1,   // Frame marked for tracing due to size.
        kInvalid = utils::numericMax<uint8_t>()  // Invalid, ignore!
    };

    // Returns |time_ms - base_ms| capped at max 16-bit value.
    // Used to fill this data structure as per
    // https://webrtc.org/experiments/rtp-hdrext/video-timing/ extension stores
    // 16-bit deltas of timestamps from packet capture time.
    static uint16_t GetDeltaCappedMs(int64_t base_ms, int64_t time_ms);
    static uint16_t GetDeltaCappedMs(TimeDelta delta);

    uint16_t encode_start_delta_ms;
    uint16_t encode_finish_delta_ms;
    uint16_t packetization_finish_delta_ms;
    uint16_t pacer_exit_delta_ms;
    uint16_t network_timestamp_delta_ms;
    uint16_t network2_timestamp_delta_ms;
    uint8_t flags = TimingFrameFlags::kInvalid;
};

// Used to report precise timings of a 'timing frames'. Contains all important
// timestamps for a lifetime of that specific frame. Reported as a string via
// GetStats(). Only frame which took the longest between two GetStats calls is
// reported.
struct OCTK_MEDIA_API TimingFrameInfo
{
    TimingFrameInfo();

    // Returns end-to-end delay of a frame, if sender and receiver timestamps are
    // synchronized, -1 otherwise.
    int64_t EndToEndDelay() const;

    // Returns true if current frame took longer to process than `other` frame.
    // If other frame's clocks are not synchronized, current frame is always
    // preferred.
    bool IsLongerThan(const TimingFrameInfo &other) const;

    // Returns true if flags are set to indicate this frame was marked for tracing
    // due to the size being outside some limit.
    bool IsOutlier() const;

    // Returns true if flags are set to indicate this frame was marked fro tracing
    // due to cyclic timer.
    bool IsTimerTriggered() const;

    // Returns true if the timing data is marked as invalid, in which case it
    // should be ignored.
    bool IsInvalid() const;

    std::string toString() const;

    bool operator<(const TimingFrameInfo &other) const;

    bool operator<=(const TimingFrameInfo &other) const;

    uint32_t rtp_timestamp;  // Identifier of a frame.
    // All timestamps below are in local monotonous clock of a receiver.
    // If sender clock is not yet estimated, sender timestamps
    // (capture_time_ms ... pacer_exit_ms) are negative values, still
    // relatively correct.
    int64_t capture_time_ms;          // Captrue time of a frame.
    int64_t encode_start_ms;          // Encode start time.
    int64_t encode_finish_ms;         // Encode completion time.
    int64_t packetization_finish_ms;  // Time when frame was passed to pacer.
    int64_t pacer_exit_ms;  // Time when last packet was pushed out of pacer.
    // Two in-network RTP processor timestamps: meaning is application specific.
    int64_t network_timestamp_ms;
    int64_t network2_timestamp_ms;
    int64_t receive_start_ms;   // First received packet time.
    int64_t receive_finish_ms;  // Last received packet time.
    int64_t decode_start_ms;    // Decode start time.
    int64_t decode_finish_ms;   // Decode completion time.
    int64_t render_time_ms;     // Proposed render time to insure smooth playback.

    uint8_t flags;  // Flags indicating validity and/or why tracing was triggered.
};

// Minimum and maximum playout delay values from capture to render.
// These are best effort values.
//
// min = max = 0 indicates that the receiver should try and render
// frame as soon as possible.
//
// min = x, max = y indicates that the receiver is free to adapt
// in the range (x, y) based on network jitter.
// This class ensures invariant 0 <= min <= max <= kMax.
class OCTK_MEDIA_API VideoPlayoutDelay
{
public:
    // Maximum supported value for the delay limit.
    static const TimeDelta &maxValue()
    {
        static TimeDelta maxTimeDelta = TimeDelta::Millis(10) * 0xFFF;
        return maxTimeDelta;
    }

    // Creates delay limits that indicates receiver should try to render frame
    // as soon as possible.
    static VideoPlayoutDelay Minimal()
    {
        return VideoPlayoutDelay(TimeDelta::Zero(), TimeDelta::Zero());
    }

    // Creates valid, but unspecified limits.
    VideoPlayoutDelay() = default;
    VideoPlayoutDelay(const VideoPlayoutDelay &) = default;
    VideoPlayoutDelay &operator=(const VideoPlayoutDelay &) = default;
    VideoPlayoutDelay(TimeDelta min, TimeDelta max);

    bool Set(TimeDelta min, TimeDelta max);

    TimeDelta Min() const { return min_; }
    TimeDelta Max() const { return max_; }

    friend bool operator==(const VideoPlayoutDelay &lhs, const VideoPlayoutDelay &rhs)
    {
        return lhs.min_ == rhs.min_ && lhs.max_ == rhs.max_;
    }

private:
    TimeDelta min_ = TimeDelta::Zero();
    TimeDelta max_ = maxValue();
};
OCTK_END_NAMESPACE

#endif // _OCTK_VIDEO_TIMING_HPP
