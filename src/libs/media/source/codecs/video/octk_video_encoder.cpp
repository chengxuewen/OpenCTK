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
// VideoCodecVP8 VideoEncoder::GetDefaultVp8Settings()
// {
//     VideoCodecVP8 vp8_settings;
//     memset(&vp8_settings, 0, sizeof(vp8_settings));
//
//     vp8_settings.numberOfTemporalLayers = 1;
//     vp8_settings.denoisingOn = true;
//     vp8_settings.automaticResizeOn = false;
//     vp8_settings.keyFrameInterval = 3000;
//
//     return vp8_settings;
// }

// VideoCodecVP9 VideoEncoder::GetDefaultVp9Settings()
// {
//     VideoCodecVP9 vp9_settings;
//     memset(&vp9_settings, 0, sizeof(vp9_settings));
//
//     vp9_settings.numberOfTemporalLayers = 1;
//     vp9_settings.denoisingOn = true;
//     vp9_settings.keyFrameInterval = 3000;
//     vp9_settings.adaptiveQpMode = true;
//     vp9_settings.automaticResizeOn = true;
//     vp9_settings.numberOfSpatialLayers = 1;
//     vp9_settings.flexibleMode = false;
//     vp9_settings.interLayerPred = InterLayerPredMode::kOn;
//
//     return vp9_settings;
// }
//
VideoCodecH264 VideoEncoder::getDefaultH264Settings()
{
    VideoCodecH264 h264_settings;
    memset(&h264_settings, 0, sizeof(h264_settings));

    h264_settings.keyFrameInterval = 3000;
    h264_settings.numberOfTemporalLayers = 1;
    return h264_settings;
}

VideoEncoder::ScalingSettings::ScalingSettings() = default;

VideoEncoder::ScalingSettings::ScalingSettings(KOff) : ScalingSettings() {}

VideoEncoder::ScalingSettings::ScalingSettings(int low, int high) : thresholds(QpThresholds(low, high)) {}

VideoEncoder::ScalingSettings::ScalingSettings(int low,
                                               int high,
                                               int min_pixels)
    : thresholds(QpThresholds(low, high)), min_pixels_per_frame(min_pixels) {}

VideoEncoder::ScalingSettings::ScalingSettings(const ScalingSettings &) = default;

VideoEncoder::ScalingSettings::~ScalingSettings() {}

// static
constexpr VideoEncoder::ScalingSettings::KOff VideoEncoder::ScalingSettings::kOff;
// static
constexpr uint8_t VideoEncoder::EncoderInfo::kMaxFramerateFraction;

bool VideoEncoder::ResolutionBitrateLimits::operator==(const ResolutionBitrateLimits &rhs) const
{
    return frame_size_pixels == rhs.frame_size_pixels &&
           min_start_bitrate_bps == rhs.min_start_bitrate_bps &&
           min_bitrate_bps == rhs.min_bitrate_bps &&
           max_bitrate_bps == rhs.max_bitrate_bps;
}

VideoEncoder::EncoderInfo::EncoderInfo()
    : scaling_settings(VideoEncoder::ScalingSettings::kOff)
    , requested_resolution_alignment(1)
    , apply_alignment_to_all_simulcast_layers(false)
    , supports_native_handle(false)
    , implementation_name("unknown")
    , has_trusted_rate_controller(false)
    , is_hardware_accelerated(true)
    , fps_allocation{InlinedVector<uint8_t, kMaxTemporalStreams>(1, kMaxFramerateFraction)}
    , supports_simulcast(false)
    , preferred_pixel_formats{VideoFrameBuffer::Type::kI420}
{
}

VideoEncoder::EncoderInfo::EncoderInfo(const EncoderInfo &) = default;

VideoEncoder::EncoderInfo::~EncoderInfo() = default;

std::string VideoEncoder::EncoderInfo::toString() const
{
    std::stringstream ss;

    ss << "EncoderInfo { "
          "ScalingSettings { ";
    if (scaling_settings.thresholds)
    {
        ss << "Thresholds { "
              "low = "
           << scaling_settings.thresholds->low
           << ", high = " << scaling_settings.thresholds->high << "}, ";
    }
    ss << "min_pixels_per_frame = " << scaling_settings.min_pixels_per_frame
       << " }";
    ss << ", requested_resolution_alignment = " << requested_resolution_alignment
       << ", apply_alignment_to_all_simulcast_layers = "
       << apply_alignment_to_all_simulcast_layers
       << ", supports_native_handle = " << supports_native_handle
       << ", implementation_name = '" << implementation_name
       << "'"
          ", has_trusted_rate_controller = "
       << has_trusted_rate_controller
       << ", is_hardware_accelerated = " << is_hardware_accelerated
       << ", fps_allocation = [";
    size_t num_spatial_layer_with_fps_allocation = 0;
    for (size_t i = 0; i < kMaxSpatialLayers; ++i)
    {
        if (!fps_allocation[i].empty())
        {
            num_spatial_layer_with_fps_allocation = i + 1;
        }
    }
    bool first = true;
    for (size_t i = 0; i < num_spatial_layer_with_fps_allocation; ++i)
    {
        if (fps_allocation[i].empty())
        {
            break;
        }
        if (!first)
        {
            ss << ", ";
        }
        const InlinedVector<uint8_t, kMaxTemporalStreams> &fractions = fps_allocation[i];
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
    for (size_t i = 0; i < resolution_bitrate_limits.size(); ++i)
    {
        if (i > 0)
        {
            ss << ", ";
        }
        ResolutionBitrateLimits l = resolution_bitrate_limits[i];
        ss << "Limits { "
              "frame_size_pixels = "
           << l.frame_size_pixels
           << ", min_start_bitrate_bps = " << l.min_start_bitrate_bps
           << ", min_bitrate_bps = " << l.min_bitrate_bps
           << ", max_bitrate_bps = " << l.max_bitrate_bps << "} ";
    }
    ss << "] "
          ", supports_simulcast = "
       << supports_simulcast;
    ss << ", preferred_pixel_formats = [";
    for (size_t i = 0; i < preferred_pixel_formats.size(); ++i)
    {
        if (i > 0)
        {
            ss << ", ";
        }
        ss << videoFrameBufferTypeToString(preferred_pixel_formats.at(i));
    }
    ss << "]";
    if (is_qp_trusted.has_value())
    {
        ss << ", is_qp_trusted = " << is_qp_trusted.value();
    }
    ss << "}";
    return ss.str();
}

bool VideoEncoder::EncoderInfo::operator==(const EncoderInfo &rhs) const
{
    if (scaling_settings.thresholds.has_value() != rhs.scaling_settings.thresholds.has_value())
    {
        return false;
    }
    if (scaling_settings.thresholds.has_value())
    {
        QpThresholds l = *scaling_settings.thresholds;
        QpThresholds r = *rhs.scaling_settings.thresholds;
        if (l.low != r.low || l.high != r.high)
        {
            return false;
        }
    }
    if (scaling_settings.min_pixels_per_frame !=
        rhs.scaling_settings.min_pixels_per_frame)
    {
        return false;
    }

    if (supports_native_handle != rhs.supports_native_handle ||
        implementation_name != rhs.implementation_name ||
        has_trusted_rate_controller != rhs.has_trusted_rate_controller ||
        is_hardware_accelerated != rhs.is_hardware_accelerated)
    {
        return false;
    }

    for (size_t i = 0; i < kMaxSpatialLayers; ++i)
    {
        if (fps_allocation[i] != rhs.fps_allocation[i])
        {
            return false;
        }
    }

    if (resolution_bitrate_limits != rhs.resolution_bitrate_limits ||
        supports_simulcast != rhs.supports_simulcast)
    {
        return false;
    }

    return true;
}

Optional<VideoEncoder::ResolutionBitrateLimits>
VideoEncoder::EncoderInfo::GetEncoderBitrateLimitsForResolution(int frame_size_pixels) const
{
    std::vector<ResolutionBitrateLimits> bitrate_limits = resolution_bitrate_limits;

    // Sort the list of bitrate limits by resolution.
    sort(bitrate_limits.begin(), bitrate_limits.end(),
         [](const ResolutionBitrateLimits &lhs,
            const ResolutionBitrateLimits &rhs) {
             return lhs.frame_size_pixels < rhs.frame_size_pixels;
         });

    for (size_t i = 0; i < bitrate_limits.size(); ++i)
    {
        OCTK_DCHECK_GE(bitrate_limits[i].min_bitrate_bps, 0);
        OCTK_DCHECK_GE(bitrate_limits[i].min_start_bitrate_bps, 0);
        OCTK_DCHECK_GE(bitrate_limits[i].max_bitrate_bps,
                       bitrate_limits[i].min_bitrate_bps);
        if (i > 0)
        {
            // The bitrate limits aren't expected to decrease with resolution.
            OCTK_DCHECK_GE(bitrate_limits[i].min_bitrate_bps,
                           bitrate_limits[i - 1].min_bitrate_bps);
            OCTK_DCHECK_GE(bitrate_limits[i].min_start_bitrate_bps,
                           bitrate_limits[i - 1].min_start_bitrate_bps);
            OCTK_DCHECK_GE(bitrate_limits[i].max_bitrate_bps,
                           bitrate_limits[i - 1].max_bitrate_bps);
        }

        if (bitrate_limits[i].frame_size_pixels >= frame_size_pixels)
        {
            return utils::make_optional<ResolutionBitrateLimits>(bitrate_limits[i]);
        }
    }

    return utils::nullopt;
}

VideoEncoder::RateControlParameters::RateControlParameters()
    : bitrate(VideoBitrateAllocation()), framerate_fps(0.0), bandwidth_allocation(DataRate::Zero()) {}

VideoEncoder::RateControlParameters::RateControlParameters(const VideoBitrateAllocation &bitrate,
                                                           double framerate_fps)
    : bitrate(bitrate)
    , framerate_fps(framerate_fps)
    , bandwidth_allocation(DataRate::BitsPerSec(bitrate.get_sum_bps())) {}

VideoEncoder::RateControlParameters::RateControlParameters(const VideoBitrateAllocation &bitrate,
                                                           double framerate_fps,
                                                           DataRate bandwidth_allocation)
    : bitrate(bitrate), framerate_fps(framerate_fps), bandwidth_allocation(bandwidth_allocation) {}

bool VideoEncoder::RateControlParameters::operator==(const VideoEncoder::RateControlParameters &rhs) const
{
    return std::tie(bitrate, framerate_fps, bandwidth_allocation) ==
           std::tie(rhs.bitrate, rhs.framerate_fps, rhs.bandwidth_allocation);
}

bool VideoEncoder::RateControlParameters::operator!=(const VideoEncoder::RateControlParameters &rhs) const
{
    return !(rhs == *this);
}

VideoEncoder::RateControlParameters::~RateControlParameters() = default;

// void VideoEncoder::SetFecControllerOverride(FecControllerOverride * /* fec_controller_override */) {}

int32_t VideoEncoder::initEncode(const VideoCodec *codec_settings,
                                 int32_t number_of_cores,
                                 size_t max_payload_size)
{
    const VideoEncoder::Capabilities capabilities(/* loss_notification= */ false);
    const VideoEncoder::Settings settings(capabilities, number_of_cores,
                                          max_payload_size);
    // In theory, this and the other version of InitEncode() could end up calling
    // each other in a loop until we get a stack overflow.
    // In practice, any subclass of VideoEncoder would overload at least one
    // of these, and we have a TODO in the header file to make this pure virtual.
    return this->initEncode(codec_settings, settings);
}

int VideoEncoder::initEncode(const VideoCodec *codec_settings,
                             const VideoEncoder::Settings &settings)
{
    // In theory, this and the other version of InitEncode() could end up calling
    // each other in a loop until we get a stack overflow.
    // In practice, any subclass of VideoEncoder would overload at least one
    // of these, and we have a TODO in the header file to make this pure virtual.
    return this->initEncode(codec_settings, settings.number_of_cores,
                            settings.max_payload_size);
}

void VideoEncoder::onPacketLossRateUpdate(float /* packet_loss_rate */) {}

void VideoEncoder::onRttUpdate(int64_t /* rtt_ms */) {}

void VideoEncoder::onLossNotification(const LossNotification & /* loss_notification */) {}
OCTK_END_NAMESPACE
