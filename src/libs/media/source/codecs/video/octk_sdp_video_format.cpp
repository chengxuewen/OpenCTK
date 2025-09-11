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

#include <octk_video_codec_types.hpp>
#include <octk_sdp_video_format.hpp>
#include <octk_media_constants.hpp>
#include <octk_h264_profile.hpp>
#include <octk_vp9_profile.hpp>
#include <octk_av1_profile.hpp>
#include <octk_string_utils.hpp>
#include <octk_video_codec.hpp>

OCTK_BEGIN_NAMESPACE

namespace
{

// TODO(bugs.webrtc.org/15847): remove code duplication of IsSameCodecSpecific
// in media/base/codec.cc
std::string
GetFmtpParameterOrDefault(const CodecParameterMap &params, const std::string &name, const std::string &default_value)
{
    const auto it = params.find(name);
    if (it != params.end())
    {
        return it->second;
    }
    return default_value;
}

std::string H264GetPacketizationModeOrDefault(const CodecParameterMap &params)
{
    // If packetization-mode is not present, default to "0".
    // https://tools.ietf.org/html/rfc6184#section-6.2
    return GetFmtpParameterOrDefault(params, media::kH264FmtpPacketizationMode, "0");
}

bool H264IsSamePacketizationMode(const CodecParameterMap &left, const CodecParameterMap &right)
{
    return H264GetPacketizationModeOrDefault(left) == H264GetPacketizationModeOrDefault(right);
}

std::string AV1GetTierOrDefault(const CodecParameterMap &params)
{
    // If the parameter is not present, the tier MUST be inferred to be 0.
    // https://aomediacodec.github.io/av1-rtp-spec/#72-sdp-parameters
    return GetFmtpParameterOrDefault(params, media::kAv1FmtpTier, "0");
}

bool AV1IsSameTier(const CodecParameterMap &left, const CodecParameterMap &right)
{
    return AV1GetTierOrDefault(left) == AV1GetTierOrDefault(right);
}

std::string AV1GetLevelIdxOrDefault(const CodecParameterMap &params)
{
    // If the parameter is not present, it MUST be inferred to be 5 (level 3.1).
    // https://aomediacodec.github.io/av1-rtp-spec/#72-sdp-parameters
    return GetFmtpParameterOrDefault(params, media::kAv1FmtpLevelIdx, "5");
}

bool AV1IsSameLevelIdx(const CodecParameterMap &left, const CodecParameterMap &right)
{
    return AV1GetLevelIdxOrDefault(left) == AV1GetLevelIdxOrDefault(right);
}

#ifdef OCTK_ENABLE_H265
std::string GetH265TxModeOrDefault(const CodecParameterMap &params)
{
    // If TxMode is not present, a value of "SRST" must be inferred.
    // https://tools.ietf.org/html/rfc7798@section-7.1
    return GetFmtpParameterOrDefault(params, cricket::kH265FmtpTxMode, "SRST");
}

bool IsSameH265TxMode(const CodecParameterMap &left, const CodecParameterMap &right)
{
    return utils::stringEqualsIgnoreCase(GetH265TxModeOrDefault(left), GetH265TxModeOrDefault(right));
}
#endif

// Some (video) codecs are actually families of codecs and rely on parameters
// to distinguish different incompatible family members.
bool IsSameCodecSpecific(const std::string &name1,
                         const CodecParameterMap &params1,
                         const std::string &name2,
                         const CodecParameterMap &params2)
{
    // The assumption when calling this function is that the two formats have the
    // same name.
    OCTK_DCHECK(utils::stringEqualsIgnoreCase(name1, name2));

    VideoCodecType codec_type = PayloadStringToCodecType(name1);
    switch (codec_type)
    {
        case kVideoCodecH264:
            return H264IsSameProfile(params1, params2) && H264IsSamePacketizationMode(params1, params2);
        case kVideoCodecVP9: return VP9IsSameProfile(params1, params2);
        case kVideoCodecAV1:
            return AV1IsSameProfile(params1, params2) && AV1IsSameTier(params1, params2) &&
                   AV1IsSameLevelIdx(params1, params2);
#ifdef OCTK_ENABLE_H265
        case kVideoCodecH265:
            return H265IsSameProfile(params1, params2) && H265IsSameTier(params1, params2) &&
                   IsSameH265TxMode(params1, params2);
#endif
        default: return true;
    }
}

} // namespace

SdpVideoFormat::SdpVideoFormat(const std::string &name)
    : name(name)
{
}

SdpVideoFormat::SdpVideoFormat(const std::string &name, const CodecParameterMap &parameters)
    : name(name)
    , parameters(parameters)
{
}

SdpVideoFormat::SdpVideoFormat(const std::string &name,
                               const CodecParameterMap &parameters,
                               const InlinedVector<ScalabilityMode, kScalabilityModeCount> &scalability_modes)
    : name(name)
    , parameters(parameters)
    , scalability_modes(scalability_modes)
{
}

SdpVideoFormat::SdpVideoFormat(const SdpVideoFormat &format,
                               const InlinedVector<ScalabilityMode, kScalabilityModeCount> &modes)
    : SdpVideoFormat(format)
{
    scalability_modes = modes;
}

SdpVideoFormat::SdpVideoFormat(const SdpVideoFormat &) = default;
SdpVideoFormat::SdpVideoFormat(SdpVideoFormat &&) = default;
SdpVideoFormat &SdpVideoFormat::operator=(const SdpVideoFormat &) = default;
SdpVideoFormat &SdpVideoFormat::operator=(SdpVideoFormat &&) = default;

SdpVideoFormat::~SdpVideoFormat() = default;

std::string SdpVideoFormat::toString() const
{
    // rtc::StringBuilder builder;
    std::stringstream ss;
    ss << "Codec name: " << name << ", parameters: {";
    for (const auto &kv : parameters)
    {
        ss << " " << kv.first << "=" << kv.second;
    }

    ss << " }";
    if (!scalability_modes.empty())
    {
        ss << ", scalability_modes: [";
        bool first = true;
        for (const auto scalability_mode : scalability_modes)
        {
            if (first)
            {
                first = false;
            }
            else
            {
                ss << ", ";
            }
            ss << ScalabilityModeToString(scalability_mode);
        }
        ss << "]";
    }

    return ss.str();
}

bool SdpVideoFormat::IsSameCodec(const SdpVideoFormat &other) const
{
    // Two codecs are considered the same if the name matches (case insensitive)
    // and certain codec-specific parameters match.
    return utils::stringEqualsIgnoreCase(name, other.name) &&
           IsSameCodecSpecific(name, parameters, other.name, other.parameters);
}

bool SdpVideoFormat::IsCodecInList(ArrayView<const SdpVideoFormat> formats) const
{
    for (const auto &format : formats)
    {
        if (IsSameCodec(format))
        {
            return true;
        }
    }
    return false;
}

bool operator==(const SdpVideoFormat &a, const SdpVideoFormat &b)
{
    return a.name == b.name && a.parameters == b.parameters && a.scalability_modes == b.scalability_modes;
}

const SdpVideoFormat SdpVideoFormat::VP8() { return SdpVideoFormat(media::kVp8CodecName, {}); }

const SdpVideoFormat SdpVideoFormat::H264()
{
    // H264 will typically require more tweaking like setting
    // * packetization-mode (which defaults to 0 but 1 is more common)
    // * level-asymmetry-allowed (which defaults to 0 but 1 is more common)
    // * profile-level-id of which there are many.
    return SdpVideoFormat(media::kH264CodecName, {});
}

const SdpVideoFormat SdpVideoFormat::VP9Profile0()
{
    return SdpVideoFormat(media::kVp9CodecName, {{kVP9FmtpProfileId, VP9ProfileToString(VP9Profile::kProfile0)}});
}

const SdpVideoFormat SdpVideoFormat::VP9Profile1()
{
    return SdpVideoFormat(media::kVp9CodecName, {{kVP9FmtpProfileId, VP9ProfileToString(VP9Profile::kProfile1)}});
}

const SdpVideoFormat SdpVideoFormat::VP9Profile2()
{
    return SdpVideoFormat(media::kVp9CodecName, {{kVP9FmtpProfileId, VP9ProfileToString(VP9Profile::kProfile2)}});
}

const SdpVideoFormat SdpVideoFormat::VP9Profile3()
{
    return SdpVideoFormat(media::kVp9CodecName, {{kVP9FmtpProfileId, VP9ProfileToString(VP9Profile::kProfile3)}});
}

const SdpVideoFormat SdpVideoFormat::AV1Profile0()
{
    // https://aomediacodec.github.io/av1-rtp-spec/#72-sdp-parameters
    return SdpVideoFormat(media::kAv1CodecName,
                          {{media::kAv1FmtpProfile, AV1ProfileToString(AV1Profile::kProfile0).data()},
                           {media::kAv1FmtpLevelIdx, "5"},
                           {media::kAv1FmtpTier, "0"}});
}

const SdpVideoFormat SdpVideoFormat::AV1Profile1()
{
    // https://aomediacodec.github.io/av1-rtp-spec/#72-sdp-parameters
    return SdpVideoFormat(media::kAv1CodecName,
                          {{media::kAv1FmtpProfile, AV1ProfileToString(AV1Profile::kProfile1).data()},
                           {media::kAv1FmtpLevelIdx, "5"},
                           {media::kAv1FmtpTier, "0"}});
}

Optional<SdpVideoFormat> FuzzyMatchSdpVideoFormat(ArrayView<const SdpVideoFormat> supported_formats,
                                                  const SdpVideoFormat &format)
{
    Optional<SdpVideoFormat> res;
    int best_parameter_match = 0;
    for (const auto &supported_format : supported_formats)
    {
        if (utils::stringEqualsIgnoreCase(supported_format.name, format.name))
        {
            int matching_parameters = 0;
            for (const auto &kv : supported_format.parameters)
            {
                auto it = format.parameters.find(kv.first);
                if (it != format.parameters.end() && it->second == kv.second)
                {
                    matching_parameters += 1;
                }
            }

            if (!res || matching_parameters > best_parameter_match)
            {
                res = supported_format;
                best_parameter_match = matching_parameters;
            }
        }
    }

    if (!res)
    {
        OCTK_INFO() << "Failed to match SdpVideoFormat " << format.toString();
    }
    else if (*res != format)
    {
        OCTK_INFO() << "Matched SdpVideoFormat " << format.toString() << " with " << res->toString();
    }

    return res;
}

OCTK_END_NAMESPACE
