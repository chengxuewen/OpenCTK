/*
*  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source
*  tree. An additional intellectual property rights grant can be found
*  in the file PATENTS.  All contributing project authors may
*  be found in the AUTHORS file in the root of the source tree.
*/

#ifndef _OCTK_VIDEO_BITRATE_ALLOCATOR_FACTORY_HPP
#define _OCTK_VIDEO_BITRATE_ALLOCATOR_FACTORY_HPP

#include <octk_video_bitrate_allocator.hpp>
#include <octk_video_codec.hpp>
#include <octk_rtc_context.hpp>

OCTK_BEGIN_NAMESPACE

// A factory that creates VideoBitrateAllocator.
// NOTE: This class is still under development and may change without notice.
class VideoBitrateAllocatorFactory
{
public:
    virtual ~VideoBitrateAllocatorFactory() = default;

    // Creates a VideoBitrateAllocator for a specific video codec.
    virtual std::unique_ptr<VideoBitrateAllocator> Create(const RtcContext &env, const VideoCodec &codec) = 0;
};

OCTK_END_NAMESPACE

#endif // _OCTK_VIDEO_BITRATE_ALLOCATOR_FACTORY_HPP
