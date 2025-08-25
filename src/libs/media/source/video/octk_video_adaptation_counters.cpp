//
// Created by cxw on 25-8-15.
//

#include <octk_video_adaptation_counters.hpp>

OCTK_BEGIN_NAMESPACE


bool VideoAdaptationCounters::operator==(const VideoAdaptationCounters &rhs) const
{
    return fps_adaptations == rhs.fps_adaptations && resolution_adaptations == rhs.resolution_adaptations;
}

bool VideoAdaptationCounters::operator!=(const VideoAdaptationCounters &rhs) const { return !(rhs == *this); }

VideoAdaptationCounters VideoAdaptationCounters::operator+(const VideoAdaptationCounters &other) const
{
    return VideoAdaptationCounters(resolution_adaptations + other.resolution_adaptations,
                                   fps_adaptations + other.fps_adaptations);
}

std::string VideoAdaptationCounters::ToString() const
{
    // rtc::StringBuilder ss;
    std::stringstream ss;
    ss << "{ res=" << resolution_adaptations << " fps=" << fps_adaptations << " }";
    return ss.str();
}
OCTK_END_NAMESPACE
