//
// Created by cxw on 25-8-15.
//

#ifndef _OCTK_ENCODER_SETTINGS_HPP
#define _OCTK_ENCODER_SETTINGS_HPP

#include <octk_video_encoder_config.hpp>
#include <octk_video_encoder.hpp>
#include <octk_video_codec.hpp>

OCTK_BEGIN_NAMESPACE

// Information about an encoder available when reconfiguring the encoder.
class EncoderSettings
{
public:
    EncoderSettings(VideoEncoder::EncoderInfo encoder_info, VideoEncoderConfig encoder_config, VideoCodec video_codec);
    EncoderSettings(const EncoderSettings &other);
    EncoderSettings &operator=(const EncoderSettings &other);

    // Encoder capabilities, implementation info, etc.
    const VideoEncoder::EncoderInfo &encoder_info() const;
    // Configuration parameters, ultimately coming from the API and negotiation.
    const VideoEncoderConfig &encoder_config() const;
    // Lower level config, heavily based on the VideoEncoderConfig.
    const VideoCodec &video_codec() const;

private:
    VideoEncoder::EncoderInfo encoder_info_;
    VideoEncoderConfig encoder_config_;
    VideoCodec video_codec_;
};

VideoCodecType GetVideoCodecTypeOrGeneric(const Optional<EncoderSettings> &settings);

OCTK_END_NAMESPACE

#endif // _OCTK_ENCODER_SETTINGS_HPP
