//
// Created by cxw on 25-8-15.
//

#ifndef _OCTK_ADAPTATION_CONSTRAINT_HPP
#define _OCTK_ADAPTATION_CONSTRAINT_HPP

#include <octk_video_source_restrictions.hpp>
#include <octk_video_stream_input_state.hpp>
#include <octk_resource.hpp>

OCTK_BEGIN_NAMESPACE

// Adaptation constraints have the ability to prevent applying a proposed
// adaptation (expressed as restrictions before/after adaptation).
class AdaptationConstraint
{
public:
    virtual ~AdaptationConstraint();

    virtual std::string Name() const = 0;

    // TODO(https://crbug.com/webrtc/11172): When we have multi-stream adaptation
    // support, this interface needs to indicate which stream the adaptation
    // applies to.
    virtual bool IsAdaptationUpAllowed(const VideoStreamInputState &input_state,
                                       const VideoSourceRestrictions &restrictions_before,
                                       const VideoSourceRestrictions &restrictions_after) const = 0;
};

OCTK_END_NAMESPACE

#endif // _OCTK_ADAPTATION_CONSTRAINT_HPP
