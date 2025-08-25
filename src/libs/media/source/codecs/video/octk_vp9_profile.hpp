/*
*  Copyright (c) 2021 The WebRTC project authors. All Rights Reserved.
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source
*  tree. An additional intellectual property rights grant can be found
*  in the file PATENTS.  All contributing project authors may
*  be found in the AUTHORS file in the root of the source tree.
*/

#ifndef _OCTK_VP9_PROFILE_HPP
#define _OCTK_VP9_PROFILE_HPP

#include <octk_rtp_parameters.hpp>
#include <octk_optional.hpp>

OCTK_BEGIN_NAMESPACE


// Profile information for VP9 video.
extern OCTK_MEDIA_API const char kVP9FmtpProfileId[];

enum class VP9Profile
{
    kProfile0,
    kProfile1,
    kProfile2,
    kProfile3,
};

// Helper functions to convert VP9Profile to std::string. Returns "0" by
// default.
OCTK_MEDIA_API std::string VP9ProfileToString(VP9Profile profile);

// Helper functions to convert std::string to VP9Profile. Returns null if given
// an invalid profile string.
Optional<VP9Profile> StringToVP9Profile(const std::string &str);

// Parse profile that is represented as a string of single digit contained in an
// SDP key-value map. A default profile(kProfile0) will be returned if the
// profile key is missing. Nothing will be returned if the key is present but
// the string is invalid.
OCTK_MEDIA_API Optional<VP9Profile> ParseSdpForVP9Profile(const CodecParameterMap &params);

// Returns true if the parameters have the same VP9 profile, or neither contains
// VP9 profile.
bool VP9IsSameProfile(const CodecParameterMap &params1, const CodecParameterMap &params2);

OCTK_END_NAMESPACE

#endif // _OCTK_VP9_PROFILE_HPP
