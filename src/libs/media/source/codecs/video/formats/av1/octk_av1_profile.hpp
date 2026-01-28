/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
** Copyright (c) 2022 The WebRTC project authors. All Rights Reserved.
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

#pragma once

#include <octk_codec_specific_info.hpp>
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