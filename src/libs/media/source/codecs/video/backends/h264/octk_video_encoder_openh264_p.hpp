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

#pragma once

#include <octk_media_context.hpp>
#include <octk_field_trials_view.hpp>
#include <octk_sdp_video_format.hpp>
#include <octk_video_encoder.hpp>
#include <octk_i420_buffer.hpp>
#include <octk_h264_types.hpp>

#if OCTK_FEATURE_MEDIA_USE_H264

OCTK_BEGIN_NAMESPACE

class VideoEncoderOpenh264Private;
class OCTK_MEDIA_API VideoEncoderOpenh264 : public VideoEncoder
{
public:
    struct LayerConfig
    {
        int simulcast_idx = 0;
        int width = -1;
        int height = -1;
        bool sending = true;
        bool key_frame_request = false;
        float max_frame_rate = 0;
        uint32_t target_bps = 0;
        uint32_t max_bps = 0;
        bool frame_dropping_on = false;
        int key_frame_interval = 0;
        int num_temporal_layers = 1;

        void SetStreamState(bool send_stream);
    };

    static H264PacketizationMode parseSdpVideoFormat(const SdpVideoFormat &format);

    VideoEncoderOpenh264(const MediaContext &mediaContext, H264PacketizationMode mode);

    ~VideoEncoderOpenh264() override;

    // `settings.max_payload_size` is ignored.
    // The following members of `codec_settings` are used. The rest are ignored.
    // - codecType (must be kVideoCodecH264)
    // - targetBitrate
    // - maxFramerate
    // - width
    // - height
    int32_t initEncode(const VideoCodec *codec_settings, const VideoEncoder::Settings &settings) override;
    int32_t release() override;

    int32_t registerEncodeCompleteCallback(EncodedImageCallback *callback) override;
    void setRates(const RateControlParameters &parameters) override;

    // The result of encoding - an EncodedImage and CodecSpecificInfo - are
    // passed to the encode complete callback.
    int32_t encode(const VideoFrame &frame, const std::vector<VideoFrameType> *frame_types) override;

    EncoderInfo getEncoderInfo() const override;

    // Exposed for testing.
    H264PacketizationMode packetizationMode() const;

protected:
    OCTK_DEFINE_DPTR(VideoEncoderOpenh264)
    OCTK_DECLARE_PRIVATE(VideoEncoderOpenh264)
    OCTK_DISABLE_COPY_MOVE(VideoEncoderOpenh264)
};

OCTK_END_NAMESPACE

#endif // #if OCTK_FEATURE_MEDIA_USE_H264