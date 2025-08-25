/*
*  Copyright (c) 2022 The WebRTC project authors. All Rights Reserved.
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source
*  tree. An additional intellectual property rights grant can be found
*  in the file PATENTS.  All contributing project authors may
*  be found in the AUTHORS file in the root of the source tree.
*/

#ifndef _OCTK_AV1_PROFILE_HPP
#define _OCTK_AV1_PROFILE_HPP

#include <octk_rtp_parameters.hpp>
#include <octk_string_view.hpp>
#include <octk_optional.hpp>

OCTK_BEGIN_NAMESPACE

// Profiles can be found at:
// https://aomedia.org/av1/specification/annex-a/#profiles
// The enum values match the number specified in the SDP.
enum class AV1Profile
{
    kProfile0 = 0,
    kProfile1 = 1,
    kProfile2 = 2,
};

// Helper function which converts an AV1Profile to std::string. Returns "0" if
// an unknown value is passed in.
OCTK_MEDIA_API StringView AV1ProfileToString(AV1Profile profile);

// Helper function which converts a std::string to AV1Profile. Returns null if
// |profile| is not a valid profile string.
Optional<AV1Profile> StringToAV1Profile(StringView profile);

// Parses an SDP key-value map of format parameters to retrive an AV1 profile.
// Returns an AV1Profile if one has been specified, `kProfile0` if no profile is
// specified and an empty value if the profile key is present but contains an
// invalid value.
OCTK_MEDIA_API Optional<AV1Profile> ParseSdpForAV1Profile(const CodecParameterMap &params);

// Returns true if the parameters have the same AV1 profile or neither contains
// an AV1 profile, otherwise false.
bool AV1IsSameProfile(const CodecParameterMap &params1, const CodecParameterMap &params2);

OCTK_END_NAMESPACE

#endif // _OCTK_AV1_PROFILE_HPP
