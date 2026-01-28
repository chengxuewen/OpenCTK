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

#include <octk_video_encoder.hpp>

OCTK_BEGIN_NAMESPACE

// TODO(mflodman): Add default complexity for VP9 and VP9.
VideoCodecVP8 VideoEncoder::GetDefaultVp8Settings()
{
    VideoCodecVP8 vp8_settings;
    memset(&vp8_settings, 0, sizeof(vp8_settings));

    vp8_settings.numberOfTemporalLayers = 1;
    vp8_settings.denoisingOn = true;
    vp8_settings.automaticResizeOn = false;
    vp8_settings.keyFrameInterval = 3000;

    return vp8_settings;
}

VideoCodecVP9 VideoEncoder::GetDefaultVp9Settings()
{
    VideoCodecVP9 vp9_settings;
    memset(&vp9_settings, 0, sizeof(vp9_settings));

    vp9_settings.numberOfTemporalLayers = 1;
    vp9_settings.denoisingOn = true;
    vp9_settings.keyFrameInterval = 3000;
    vp9_settings.adaptiveQpMode = true;
    vp9_settings.automaticResizeOn = true;
    vp9_settings.numberOfSpatialLayers = 1;
    vp9_settings.flexibleMode = false;
    vp9_settings.interLayerPred = InterLayerPredMode::kOn;

    return vp9_settings;
}

VideoCodecH264 VideoEncoder::getDefaultH264Settings()
{
    VideoCodecH264 h264_settings;
    memset(&h264_settings, 0, sizeof(h264_settings));

    h264_settings.keyFrameInterval = 3000;
    h264_settings.numberOfTemporalLayers = 1;
    return h264_settings;
}

VideoEncoder::ScalingSettings::ScalingSettings() = default;

VideoEncoder::ScalingSettings::ScalingSettings(KOff)
    : ScalingSettings()
{
}

VideoEncoder::ScalingSettings::ScalingSettings(int low, int high)
    : thresholds(QpThresholds(low, high))
{
}

VideoEncoder::ScalingSettings::ScalingSettings(int low, int high, int min_pixels)
    : thresholds(QpThresholds(low, high))
    , minPixelsPerFrame(min_pixels)
{
}

VideoEncoder::ScalingSettings::ScalingSettings(const ScalingSettings &) = default;

VideoEncoder::ScalingSettings::~ScalingSettings()
{
}

// static
constexpr VideoEncoder::ScalingSettings::KOff VideoEncoder::ScalingSettings::kOff;
// static
constexpr uint8_t VideoEncoder::EncoderInfo::kMaxFramerateFraction;

bool VideoEncoder::ResolutionBitrateLimits::operator==(const ResolutionBitrateLimits &rhs) const
{
    return frameSizePixels == rhs.frameSizePixels && minStartBitrateBps == rhs.minStartBitrateBps &&
           minBitrateBps == rhs.minBitrateBps && maxBitrateBps == rhs.maxBitrateBps;
}

VideoEncoder::EncoderInfo::EncoderInfo()
    : scalingSettings(VideoEncoder::ScalingSettings::kOff)
    , requestedResolutionAlignment(1)
    , applyAlignmentToAllSimulcastLayers(false)
    , supportsNativeHandle(false)
    , implementationName("unknown")
    , hasTrustedRateController(false)
    , isHardwareAccelerated(true)
    , fpsAllocation{InlinedVector<uint8_t, kMaxTemporalStreams>(1, kMaxFramerateFraction)}
    , supportsSimulcast(false)
    , preferredPixelFormats{VideoFrameBuffer::Type::kI420}
{
}

VideoEncoder::EncoderInfo::EncoderInfo(const EncoderInfo &) = default;

VideoEncoder::EncoderInfo::~EncoderInfo() = default;

std::string VideoEncoder::EncoderInfo::toString() const
{
    std::stringstream ss;

    ss << "EncoderInfo { "
          "ScalingSettings { ";
    if (scalingSettings.thresholds)
    {
        ss << "Thresholds { "
              "low = "
           << scalingSettings.thresholds->low << ", high = " << scalingSettings.thresholds->high << "}, ";
    }
    ss << "min_pixels_per_frame = " << scalingSettings.minPixelsPerFrame << " }";
    ss << ", requested_resolution_alignment = " << requestedResolutionAlignment
       << ", apply_alignment_to_all_simulcast_layers = " << applyAlignmentToAllSimulcastLayers
       << ", supports_native_handle = " << supportsNativeHandle << ", implementation_name = '" << implementationName
       << "'"
          ", has_trusted_rate_controller = "
       << hasTrustedRateController << ", is_hardware_accelerated = " << isHardwareAccelerated << ", fps_allocation = [";
    size_t num_spatial_layer_with_fps_allocation = 0;
    for (size_t i = 0; i < kMaxSpatialLayers; ++i)
    {
        if (!fpsAllocation[i].empty())
        {
            num_spatial_layer_with_fps_allocation = i + 1;
        }
    }
    bool first = true;
    for (size_t i = 0; i < num_spatial_layer_with_fps_allocation; ++i)
    {
        if (fpsAllocation[i].empty())
        {
            break;
        }
        if (!first)
        {
            ss << ", ";
        }
        const InlinedVector<uint8_t, kMaxTemporalStreams> &fractions = fpsAllocation[i];
        if (!fractions.empty())
        {
            first = false;
            ss << "[ ";
            for (size_t i = 0; i < fractions.size(); ++i)
            {
                if (i > 0)
                {
                    ss << ", ";
                }
                ss << (static_cast<double>(fractions[i]) / kMaxFramerateFraction);
            }
            ss << "] ";
        }
    }
    ss << "]";
    ss << ", resolution_bitrate_limits = [";
    for (size_t i = 0; i < resolutionBitrateLimits.size(); ++i)
    {
        if (i > 0)
        {
            ss << ", ";
        }
        ResolutionBitrateLimits l = resolutionBitrateLimits[i];
        ss << "Limits { "
              "frame_size_pixels = "
           << l.frameSizePixels << ", min_start_bitrate_bps = " << l.minStartBitrateBps
           << ", min_bitrate_bps = " << l.minBitrateBps << ", max_bitrate_bps = " << l.maxBitrateBps << "} ";
    }
    ss << "] "
          ", supports_simulcast = "
       << supportsSimulcast;
    ss << ", preferred_pixel_formats = [";
    for (size_t i = 0; i < preferredPixelFormats.size(); ++i)
    {
        if (i > 0)
        {
            ss << ", ";
        }
        ss << videoFrameBufferTypeToString(preferredPixelFormats.at(i));
    }
    ss << "]";
    if (isQPTrusted.has_value())
    {
        ss << ", is_qp_trusted = " << isQPTrusted.value();
    }
    ss << "}";
    return ss.str();
}

bool VideoEncoder::EncoderInfo::operator==(const EncoderInfo &rhs) const
{
    if (scalingSettings.thresholds.has_value() != rhs.scalingSettings.thresholds.has_value())
    {
        return false;
    }
    if (scalingSettings.thresholds.has_value())
    {
        QpThresholds l = *scalingSettings.thresholds;
        QpThresholds r = *rhs.scalingSettings.thresholds;
        if (l.low != r.low || l.high != r.high)
        {
            return false;
        }
    }
    if (scalingSettings.minPixelsPerFrame != rhs.scalingSettings.minPixelsPerFrame)
    {
        return false;
    }

    if (supportsNativeHandle != rhs.supportsNativeHandle || implementationName != rhs.implementationName ||
        hasTrustedRateController != rhs.hasTrustedRateController || isHardwareAccelerated != rhs.isHardwareAccelerated)
    {
        return false;
    }

    for (size_t i = 0; i < kMaxSpatialLayers; ++i)
    {
        if (fpsAllocation[i] != rhs.fpsAllocation[i])
        {
            return false;
        }
    }

    if (resolutionBitrateLimits != rhs.resolutionBitrateLimits || supportsSimulcast != rhs.supportsSimulcast)
    {
        return false;
    }

    return true;
}

Optional<VideoEncoder::ResolutionBitrateLimits> VideoEncoder::EncoderInfo::getEncoderBitrateLimitsForResolution(
    int frameSizePixels) const
{
    std::vector<ResolutionBitrateLimits> bitrate_limits = resolutionBitrateLimits;

    // Sort the list of bitrate limits by resolution.
    sort(bitrate_limits.begin(),
         bitrate_limits.end(),
         [](const ResolutionBitrateLimits &lhs, const ResolutionBitrateLimits &rhs)
         { return lhs.frameSizePixels < rhs.frameSizePixels; });

    for (size_t i = 0; i < bitrate_limits.size(); ++i)
    {
        OCTK_DCHECK_GE(bitrate_limits[i].minBitrateBps, 0);
        OCTK_DCHECK_GE(bitrate_limits[i].minStartBitrateBps, 0);
        OCTK_DCHECK_GE(bitrate_limits[i].maxBitrateBps, bitrate_limits[i].minBitrateBps);
        if (i > 0)
        {
            // The bitrate limits aren't expected to decrease with resolution.
            OCTK_DCHECK_GE(bitrate_limits[i].minBitrateBps, bitrate_limits[i - 1].minBitrateBps);
            OCTK_DCHECK_GE(bitrate_limits[i].minStartBitrateBps, bitrate_limits[i - 1].minStartBitrateBps);
            OCTK_DCHECK_GE(bitrate_limits[i].maxBitrateBps, bitrate_limits[i - 1].maxBitrateBps);
        }

        if (bitrate_limits[i].frameSizePixels >= frameSizePixels)
        {
            return utils::make_optional<ResolutionBitrateLimits>(bitrate_limits[i]);
        }
    }

    return utils::nullopt;
}

VideoEncoder::RateControlParameters::RateControlParameters()
    : bitrate(VideoBitrateAllocation())
    , framerateFps(0.0)
    , bandwidthAllocation(DataRate::Zero())
{
}

VideoEncoder::RateControlParameters::RateControlParameters(const VideoBitrateAllocation &bitrate, double framerate_fps)
    : bitrate(bitrate)
    , framerateFps(framerate_fps)
    , bandwidthAllocation(DataRate::BitsPerSec(bitrate.get_sum_bps()))
{
}

VideoEncoder::RateControlParameters::RateControlParameters(const VideoBitrateAllocation &bitrate,
                                                           double framerate_fps,
                                                           DataRate bandwidth_allocation)
    : bitrate(bitrate)
    , framerateFps(framerate_fps)
    , bandwidthAllocation(bandwidth_allocation)
{
}

bool VideoEncoder::RateControlParameters::operator==(const VideoEncoder::RateControlParameters &rhs) const
{
    return std::tie(bitrate, framerateFps, bandwidthAllocation) ==
           std::tie(rhs.bitrate, rhs.framerateFps, rhs.bandwidthAllocation);
}

bool VideoEncoder::RateControlParameters::operator!=(const VideoEncoder::RateControlParameters &rhs) const
{
    return !(rhs == *this);
}

VideoEncoder::RateControlParameters::~RateControlParameters() = default;

// void VideoEncoder::SetFecControllerOverride(FecControllerOverride * /* fec_controller_override */) {}

int32_t VideoEncoder::initEncode(const VideoCodec *codec_settings, int32_t number_of_cores, size_t max_payload_size)
{
    const VideoEncoder::Capabilities capabilities(/* loss_notification= */ false);
    const VideoEncoder::Settings settings(capabilities, number_of_cores, max_payload_size);
    // In theory, this and the other version of InitEncode() could end up calling
    // each other in a loop until we get a stack overflow.
    // In practice, any subclass of VideoEncoder would overload at least one
    // of these, and we have a TODO in the header file to make this pure virtual.
    return this->initEncode(codec_settings, settings);
}

int VideoEncoder::initEncode(const VideoCodec *codec_settings, const VideoEncoder::Settings &settings)
{
    // In theory, this and the other version of InitEncode() could end up calling
    // each other in a loop until we get a stack overflow.
    // In practice, any subclass of VideoEncoder would overload at least one
    // of these, and we have a TODO in the header file to make this pure virtual.
    return this->initEncode(codec_settings, settings.numberOfCores, settings.maxPayloadSize);
}

void VideoEncoder::onPacketLossRateUpdate(float /* packet_loss_rate */)
{
}

void VideoEncoder::onRttUpdate(int64_t /* rtt_ms */)
{
}

void VideoEncoder::onLossNotification(const LossNotification & /* loss_notification */)
{
}

OCTK_END_NAMESPACE
