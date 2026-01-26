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

#include <octk_video_timing.hpp>
#include <octk_safe_conversions.hpp>
#include <octk_array_view.hpp>
#include <octk_algorithm.hpp>
#include <octk_logging.hpp>

#include <cstdint>
#include <sstream>
#include <string>

OCTK_BEGIN_NAMESPACE

uint16_t VideoSendTiming::GetDeltaCappedMs(int64_t base_ms, int64_t time_ms)
{
    if (time_ms < base_ms)
    {
        OCTK_ERROR() << "Delta " << (time_ms - base_ms)
                     << "ms expected to be positive";
    }
    return utils::saturated_cast<uint16_t>(time_ms - base_ms);
}

uint16_t VideoSendTiming::GetDeltaCappedMs(TimeDelta delta)
{
    if (delta < TimeDelta::Zero())
    {
        OCTK_ERROR() << "Delta " << delta.ms()
                     << "ms expected to be positive";
    }
    return utils::saturated_cast<uint16_t>(delta.ms());
}

TimingFrameInfo::TimingFrameInfo()
    : rtp_timestamp(0), capture_time_ms(-1), encode_start_ms(-1), encode_finish_ms(-1), packetization_finish_ms(-1)
    , pacer_exit_ms(-1), network_timestamp_ms(-1), network2_timestamp_ms(-1), receive_start_ms(-1), receive_finish_ms(
        -1), decode_start_ms(-1), decode_finish_ms(-1), render_time_ms(-1), flags(VideoSendTiming::kNotTriggered) {}

int64_t TimingFrameInfo::EndToEndDelay() const
{
    return capture_time_ms >= 0 ? decode_finish_ms - capture_time_ms : -1;
}

bool TimingFrameInfo::IsLongerThan(const TimingFrameInfo &other) const
{
    int64_t other_delay = other.EndToEndDelay();
    return other_delay == -1 || EndToEndDelay() > other_delay;
}

bool TimingFrameInfo::operator<(const TimingFrameInfo &other) const
{
    return other.IsLongerThan(*this);
}

bool TimingFrameInfo::operator<=(const TimingFrameInfo &other) const
{
    return !IsLongerThan(other);
}

bool TimingFrameInfo::IsOutlier() const
{
    return !IsInvalid() && (flags & VideoSendTiming::kTriggeredBySize);
}

bool TimingFrameInfo::IsTimerTriggered() const
{
    return !IsInvalid() && (flags & VideoSendTiming::kTriggeredByTimer);
}

bool TimingFrameInfo::IsInvalid() const
{
    return flags == VideoSendTiming::kInvalid;
}

std::string TimingFrameInfo::toString() const
{
    if (IsInvalid())
    {
        return "";
    }

    std::stringstream ss;

    ss << rtp_timestamp << ',' << capture_time_ms << ',' << encode_start_ms << ','
       << encode_finish_ms << ',' << packetization_finish_ms << ','
       << pacer_exit_ms << ',' << network_timestamp_ms << ','
       << network2_timestamp_ms << ',' << receive_start_ms << ','
       << receive_finish_ms << ',' << decode_start_ms << ',' << decode_finish_ms
       << ',' << render_time_ms << ',' << IsOutlier() << ','
       << IsTimerTriggered();

    return ss.str();
}

VideoPlayoutDelay::VideoPlayoutDelay(TimeDelta min, TimeDelta max)
    : min_(utils::clamp(min, TimeDelta::Zero(), maxValue())), max_(utils::clamp(max, min_, maxValue()))
{
    if (!(TimeDelta::Zero() <= min && min <= max && max <= maxValue()))
    {
        OCTK_ERROR() << "Invalid video playout delay: [" << utils::toString(min) << "," << utils::toString(max)
                     << "]. Clamped to [" << utils::toString(this->Min()) << "," << utils::toString(this->Max())
                     << "]";
    }
}

bool VideoPlayoutDelay::Set(TimeDelta min, TimeDelta max)
{
    if (TimeDelta::Zero() <= min && min <= max && max <= maxValue())
    {
        min_ = min;
        max_ = max;
        return true;
    }
    return false;
}
OCTK_END_NAMESPACE