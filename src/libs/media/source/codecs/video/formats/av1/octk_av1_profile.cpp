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

#include <private/octk_media_constants_p.hpp>
#include <octk_string_to_number.hpp>
#include <octk_av1_profile.hpp>

OCTK_BEGIN_NAMESPACE

StringView AV1ProfileToString(AV1Profile profile)
{
    switch (profile)
    {
        case AV1Profile::kProfile0: return "0";
        case AV1Profile::kProfile1: return "1";
        case AV1Profile::kProfile2: return "2";
    }
    return "0";
}

Optional<AV1Profile> StringToAV1Profile(StringView str)
{
    const Optional<int> i = utils::stringToNumber<int>(str);
    if (!i.has_value())
        return utils::nullopt;

    switch (i.value())
    {
        case 0: return AV1Profile::kProfile0;
        case 1: return AV1Profile::kProfile1;
        case 2: return AV1Profile::kProfile2;
        default: return utils::nullopt;
    }
}

Optional<AV1Profile> ParseSdpForAV1Profile(const CodecParameterMap &params)
{
    const auto profile_it = params.find(media::kAv1FmtpProfile);
    if (profile_it == params.end())
    {
        return AV1Profile::kProfile0;
    }
    const std::string &profile_str = profile_it->second;
    return StringToAV1Profile(profile_str);
}

bool AV1IsSameProfile(const CodecParameterMap &params1, const CodecParameterMap &params2)
{
    const Optional<AV1Profile> profile = ParseSdpForAV1Profile(params1);
    const Optional<AV1Profile> other_profile = ParseSdpForAV1Profile(params2);
    return profile && other_profile && profile == other_profile;
}

OCTK_END_NAMESPACE
