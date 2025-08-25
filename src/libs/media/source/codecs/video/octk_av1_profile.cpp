//
// Created by cxw on 25-8-15.
//

#include <octk_av1_profile.hpp>
#include <octk_string_to_number.hpp>
#include <octk_media_constants.hpp>

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
        return AV1Profile::kProfile0;
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
