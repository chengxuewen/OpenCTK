/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2026~Present ChengXueWen.
** Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
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

#include <octk_media_config.hpp>
#include <octk_h264_codecs.hpp>
#include <octk_inlined_vector.hpp>
#include <octk_media_constants.hpp>
#include <octk_optional.hpp>
#include <octk_logging.hpp>
#include <octk_memory.hpp>
#include <octk_checks.hpp>

#include <memory>
#include <string>

#if OCTK_FEATURE_MEDIA_USE_H264
#    include <private/octk_video_decoder_openh264_p.hpp>
#    include <private/octk_video_encoder_openh264_p.hpp>
#endif

OCTK_BEGIN_NAMESPACE

namespace
{

#if OCTK_FEATURE_MEDIA_USE_H264
bool g_rtc_use_h264 = true;
#endif

// If H.264 OpenH264/FFmpeg codec is supported.
bool IsH264CodecSupported()
{
#if OCTK_FEATURE_MEDIA_USE_H264
    return g_rtc_use_h264;
#else
    return false;
#endif
}

constexpr ScalabilityMode kSupportedScalabilityModes[] = {ScalabilityMode::kL1T1,
                                                          ScalabilityMode::kL1T2,
                                                          ScalabilityMode::kL1T3};

} // namespace

SdpVideoFormat CreateH264Format(H264Profile profile,
                                H264Level level,
                                const std::string &packetization_mode,
                                bool add_scalability_modes)
{
    const Optional<std::string> profile_string = H264ProfileLevelIdToString(H264ProfileLevelId(profile, level));
    OCTK_CHECK(profile_string);
    InlinedVector<ScalabilityMode, kScalabilityModeCount> scalability_modes;
    if (add_scalability_modes)
    {
        for (const auto scalability_mode : kSupportedScalabilityModes)
        {
            scalability_modes.push_back(scalability_mode);
        }
    }
    return SdpVideoFormat(media::kH264CodecName,
                          {{media::kH264FmtpProfileLevelId, *profile_string},
                           {media::kH264FmtpLevelAsymmetryAllowed, "1"},
                           {media::kH264FmtpPacketizationMode, packetization_mode}},
                          scalability_modes);
}

void DisableRtcUseH264()
{
#if OCTK_FEATURE_MEDIA_USE_H264
    g_rtc_use_h264 = false;
#endif
}

std::vector<SdpVideoFormat> SupportedH264Codecs(bool add_scalability_modes)
{
    // TRACE_EVENT0("webrtc", __func__);
    if (!IsH264CodecSupported())
    {
        return std::vector<SdpVideoFormat>();
    }
    // We only support encoding Constrained Baseline Profile (CBP), but the
    // decoder supports more profiles. We can list all profiles here that are
    // supported by the decoder and that are also supersets of CBP, i.e. the
    // decoder for that profile is required to be able to decode CBP. This means
    // we can encode and send CBP even though we negotiated a potentially
    // higher profile. See the H264 spec for more information.
    //
    // We support both packetization modes 0 (mandatory) and 1 (optional,
    // preferred).
    return {
        CreateH264Format(H264Profile::kProfileBaseline, H264Level::kLevel3_1, "1", add_scalability_modes),
        CreateH264Format(H264Profile::kProfileBaseline, H264Level::kLevel3_1, "0", add_scalability_modes),
        CreateH264Format(H264Profile::kProfileConstrainedBaseline, H264Level::kLevel3_1, "1", add_scalability_modes),
        CreateH264Format(H264Profile::kProfileConstrainedBaseline, H264Level::kLevel3_1, "0", add_scalability_modes),
        CreateH264Format(H264Profile::kProfileMain, H264Level::kLevel3_1, "1", add_scalability_modes),
        CreateH264Format(H264Profile::kProfileMain, H264Level::kLevel3_1, "0", add_scalability_modes)};
}

std::vector<SdpVideoFormat> SupportedH264DecoderCodecs()
{
    // TRACE_EVENT0("webrtc", __func__);
    if (!IsH264CodecSupported())
    {
        return std::vector<SdpVideoFormat>();
    }

    std::vector<SdpVideoFormat> supportedCodecs = SupportedH264Codecs();

    // OpenH264 doesn't yet support High Predictive 4:4:4 encoding but it does
    // support decoding.
    supportedCodecs.push_back(CreateH264Format(H264Profile::kProfilePredictiveHigh444, H264Level::kLevel3_1, "1"));
    supportedCodecs.push_back(CreateH264Format(H264Profile::kProfilePredictiveHigh444, H264Level::kLevel3_1, "0"));

    return supportedCodecs;
}

H264EncoderSettings H264EncoderSettings::Parse(const SdpVideoFormat &format)
{
    auto it = format.parameters.find(media::kH264FmtpPacketizationMode);
    if (it != format.parameters.end())
    {
        if (it->second == "0")
        {
            return {.packetization_mode = H264PacketizationMode::SingleNalUnit};
        }
        else if (it->second == "1")
        {
            return {.packetization_mode = H264PacketizationMode::NonInterleaved};
        }
    }
    return {};
}

Nonnull<std::unique_ptr<VideoEncoder>> CreateH264Encoder([[maybe_unused]] const MediaContext &env,
                                                         [[maybe_unused]] H264EncoderSettings settings)
{
#if OCTK_FEATURE_MEDIA_USE_H264
    OCTK_CHECK(g_rtc_use_h264);
    OCTK_INFO() << "Creating H264EncoderImpl.";
    return utils::make_unique<VideoEncoderOpenh264>(env, settings.packetization_mode);
#else
    OCTK_CHECK_NOTREACHED();
#endif
}

bool H264Encoder::IsSupported()
{
    return IsH264CodecSupported();
}

bool H264Encoder::SupportsScalabilityMode(ScalabilityMode scalability_mode)
{
    for (const auto &entry : kSupportedScalabilityModes)
    {
        if (entry == scalability_mode)
        {
            return true;
        }
    }
    return false;
}

std::unique_ptr<H264Decoder> H264Decoder::Create()
{
    OCTK_DCHECK(H264Decoder::IsSupported());
#if OCTK_FEATURE_MEDIA_USE_H264
    OCTK_CHECK(g_rtc_use_h264);
    OCTK_INFO() << "Creating H264DecoderImpl.";
    return std::make_unique<H264DecoderImpl>();
#else
    OCTK_DCHECK_NOTREACHED();
    return nullptr;
#endif
}

bool H264Decoder::IsSupported()
{
    return IsH264CodecSupported();
}

OCTK_END_NAMESPACE