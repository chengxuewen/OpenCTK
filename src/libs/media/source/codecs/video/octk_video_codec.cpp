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

#include <octk_video_codec.hpp>

#include <sstream>

OCTK_BEGIN_NAMESPACE

namespace
{
constexpr char kPayloadNameVp8[] = "VP8";
constexpr char kPayloadNameVp9[] = "VP9";
constexpr char kPayloadNameAv1[] = "AV1";
// TODO(bugs.webrtc.org/13166): Remove AV1X when backwards compatibility is not
// needed.
constexpr char kPayloadNameAv1x[] = "AV1X";
constexpr char kPayloadNameH264[] = "H264";
constexpr char kPayloadNameGeneric[] = "Generic";
constexpr char kPayloadNameH265[] = "H265";
} // namespace

bool VideoCodecVP8::operator==(const VideoCodecVP8 &other) const
{
    return (numberOfTemporalLayers == other.numberOfTemporalLayers && denoisingOn == other.denoisingOn &&
            automaticResizeOn == other.automaticResizeOn && keyFrameInterval == other.keyFrameInterval);
}

bool VideoCodecVP9::operator==(const VideoCodecVP9 &other) const
{
    return (numberOfTemporalLayers == other.numberOfTemporalLayers && denoisingOn == other.denoisingOn &&
            keyFrameInterval == other.keyFrameInterval && adaptiveQpMode == other.adaptiveQpMode &&
            automaticResizeOn == other.automaticResizeOn && numberOfSpatialLayers == other.numberOfSpatialLayers &&
            flexibleMode == other.flexibleMode);
}

bool VideoCodecH264::operator==(const VideoCodecH264 &other) const
{
    return (keyFrameInterval == other.keyFrameInterval && numberOfTemporalLayers == other.numberOfTemporalLayers);
}

VideoCodec::VideoCodec()
    : codecType(kVideoCodecGeneric)
    , width(0)
    , height(0)
    , startBitrate(0)
    , maxBitrate(0)
    , minBitrate(0)
    , maxFramerate(0)
    , active(true)
    , qpMax(0)
    , numberOfSimulcastStreams(0)
    , simulcastStream()
    , spatialLayers()
    , mode(VideoCodecMode::kRealtimeVideo)
    , expect_encode_from_texture(false)
    , timing_frame_thresholds({0, 0})
    , legacy_conference_mode(false)
    , codec_specific_()
    , complexity_(VideoCodecComplexity::kComplexityNormal)
{
}

std::string VideoCodec::toString() const
{
    // char string_buf[2048];
    // rtc::SimpleStringBuilder ss(string_buf);

    std::stringstream ss;
    ss << "VideoCodec {" << "type: " << CodecTypeToPayloadString(codecType)
       << ", mode: " << (mode == VideoCodecMode::kRealtimeVideo ? "RealtimeVideo" : "Screensharing");
    if (IsSinglecast())
    {
        Optional<ScalabilityMode> scalability_mode = GetScalabilityMode();
        if (scalability_mode.has_value())
        {
            ss << ", Singlecast: {" << width << "x" << height << " " << ScalabilityModeToString(*scalability_mode)
               << (active ? ", active" : ", inactive") << "}";
        }
    }
    else
    {
        ss << ", Simulcast: {";
        for (size_t i = 0; i < numberOfSimulcastStreams; ++i)
        {
            const SimulcastStream stream = simulcastStream[i];
            Optional<ScalabilityMode> scalability_mode = stream.GetScalabilityMode();
            if (scalability_mode.has_value())
            {
                ss << "[" << stream.width << "x" << stream.height << " " << ScalabilityModeToString(*scalability_mode)
                   << (stream.active ? ", active" : ", inactive") << "]";
            }
        }
        ss << "}";
    }
    ss << "}";
    return ss.str();
}

VideoCodecVP8 *VideoCodec::vp8()
{
    OCTK_DCHECK_EQ(codecType, kVideoCodecVP8);
    return &codec_specific_.VP8;
}

const VideoCodecVP8 &VideoCodec::vp8() const
{
    OCTK_DCHECK_EQ(codecType, kVideoCodecVP8);
    return codec_specific_.VP8;
}

VideoCodecVP9 *VideoCodec::vp9()
{
    OCTK_DCHECK_EQ(codecType, kVideoCodecVP9);
    return &codec_specific_.VP9;
}

const VideoCodecVP9 &VideoCodec::vp9() const
{
    OCTK_DCHECK_EQ(codecType, kVideoCodecVP9);
    return codec_specific_.VP9;
}

VideoCodecH264 *VideoCodec::h264()
{
    OCTK_DCHECK_EQ(codecType, kVideoCodecH264);
    return &codec_specific_.H264;
}

const VideoCodecH264 &VideoCodec::h264() const
{
    OCTK_DCHECK_EQ(codecType, kVideoCodecH264);
    return codec_specific_.H264;
}

VideoCodecAV1 *VideoCodec::aV1()
{
    OCTK_DCHECK_EQ(codecType, kVideoCodecAV1);
    return &codec_specific_.AV1;
}

const VideoCodecAV1 &VideoCodec::aV1() const
{
    OCTK_DCHECK_EQ(codecType, kVideoCodecAV1);
    return codec_specific_.AV1;
}

const char *CodecTypeToPayloadString(VideoCodecType type)
{
    switch (type)
    {
        case kVideoCodecVP8: return kPayloadNameVp8;
        case kVideoCodecVP9: return kPayloadNameVp9;
        case kVideoCodecAV1: return kPayloadNameAv1;
        case kVideoCodecH264: return kPayloadNameH264;
        case kVideoCodecGeneric: return kPayloadNameGeneric;
        case kVideoCodecH265: return kPayloadNameH265;
    }
    OCTK_CHECK_NOTREACHED();
}

VideoCodecType PayloadStringToCodecType(const std::string &name)
{
    if (utils::stringEqualsIgnoreCase(name, kPayloadNameVp8))
        return kVideoCodecVP8;
    if (utils::stringEqualsIgnoreCase(name, kPayloadNameVp9))
        return kVideoCodecVP9;
    if (utils::stringEqualsIgnoreCase(name, kPayloadNameAv1) || utils::stringEqualsIgnoreCase(name, kPayloadNameAv1x))
        return kVideoCodecAV1;
    if (utils::stringEqualsIgnoreCase(name, kPayloadNameH264))
        return kVideoCodecH264;
    if (utils::stringEqualsIgnoreCase(name, kPayloadNameH265))
        return kVideoCodecH265;
    return kVideoCodecGeneric;
}

VideoCodecComplexity VideoCodec::GetVideoEncoderComplexity() const { return complexity_; }

void VideoCodec::SetVideoEncoderComplexity(VideoCodecComplexity complexity_setting)
{
    complexity_ = complexity_setting;
}

bool VideoCodec::GetFrameDropEnabled() const { return frame_drop_enabled_; }

void VideoCodec::SetFrameDropEnabled(bool enabled) { frame_drop_enabled_ = enabled; }

OCTK_END_NAMESPACE
