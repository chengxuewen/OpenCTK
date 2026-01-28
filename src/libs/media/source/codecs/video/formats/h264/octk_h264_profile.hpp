/*
*  Copyright (c) 2021 The WebRTC project authors. All Rights Reserved.
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source
*  tree. An additional intellectual property rights grant can be found
*  in the file PATENTS.  All contributing project authors may
*  be found in the AUTHORS file in the root of the source tree.
*/

#ifndef _OCTK_H264_PROFILE_HPP
#define _OCTK_H264_PROFILE_HPP

//#include <octk_rtp_parameters.hpp>
#include <octk_codec_specific_info.hpp>
#include <octk_optional.hpp>

#include <map>

OCTK_BEGIN_NAMESPACE

enum class H264Profile
{
    kProfileConstrainedBaseline,
    kProfileBaseline,
    kProfileMain,
    kProfileConstrainedHigh,
    kProfileHigh,
    kProfilePredictiveHigh444,
};

// All values are equal to ten times the level number, except level 1b which is
// special.
enum class H264Level
{
    kLevel1_b = 0,
    kLevel1 = 10,
    kLevel1_1 = 11,
    kLevel1_2 = 12,
    kLevel1_3 = 13,
    kLevel2 = 20,
    kLevel2_1 = 21,
    kLevel2_2 = 22,
    kLevel3 = 30,
    kLevel3_1 = 31,
    kLevel3_2 = 32,
    kLevel4 = 40,
    kLevel4_1 = 41,
    kLevel4_2 = 42,
    kLevel5 = 50,
    kLevel5_1 = 51,
    kLevel5_2 = 52
};

struct H264ProfileLevelId
{
    constexpr H264ProfileLevelId(H264Profile profile, H264Level level)
        : profile(profile)
        , level(level)
    {
    }
    H264Profile profile;
    H264Level level;
};

// Parse profile level id that is represented as a string of 3 hex bytes.
// Nothing will be returned if the string is not a recognized H264
// profile level id.
Optional<H264ProfileLevelId> ParseH264ProfileLevelId(const char *str);

// Parse profile level id that is represented as a string of 3 hex bytes
// contained in an SDP key-value map. A default profile level id will be
// returned if the profile-level-id key is missing. Nothing will be returned if
// the key is present but the string is invalid.
OCTK_MEDIA_API Optional<H264ProfileLevelId> ParseSdpForH264ProfileLevelId(const CodecParameterMap &params);

// Given that a decoder supports up to a given frame size (in pixels) at up to a
// given number of frames per second, return the highest H.264 level where it
// can guarantee that it will be able to support all valid encoded streams that
// are within that level.
OCTK_MEDIA_API Optional<H264Level> H264SupportedLevel(int max_frame_pixel_count, float max_fps);

// Returns canonical string representation as three hex bytes of the profile
// level id, or returns nothing for invalid profile level ids.
OCTK_MEDIA_API Optional<std::string> H264ProfileLevelIdToString(const H264ProfileLevelId &profile_level_id);

// Returns true if the parameters have the same H264 profile (Baseline, High,
// etc).
OCTK_MEDIA_API bool H264IsSameProfile(const CodecParameterMap &params1, const CodecParameterMap &params2);

OCTK_END_NAMESPACE

#endif // _OCTK_H264_PROFILE_HPP
