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

#ifndef _OCTK_VIDEO_CODEC_HPP
#define _OCTK_VIDEO_CODEC_HPP

#include <octk_video_layers_allocation.hpp>
#include <octk_video_codec_constants.hpp>
#include <octk_video_codec_types.hpp>
#include <octk_scalability_mode.hpp>
#include <octk_simulcast_stream.hpp>
#include <octk_optional.hpp>

OCTK_BEGIN_NAMESPACE

typedef SimulcastStream SpatialLayer;
// The VideoCodec class represents an old defacto-apis, which we're migrating away from slowly.

// Video codec
enum class VideoCodecComplexity
{
    kComplexityLow = -1,
    kComplexityNormal = 0,
    kComplexityHigh = 1,
    kComplexityHigher = 2,
    kComplexityMax = 3
};

// VP8 specific
struct VideoCodecVP8
{
    bool operator==(const VideoCodecVP8 &other) const;
    bool operator!=(const VideoCodecVP8 &other) const { return !(*this == other); }
    // Temporary utility method for transition deleting numberOfTemporalLayers
    // setting (replaced by ScalabilityMode).
    void SetNumberOfTemporalLayers(unsigned char n) { numberOfTemporalLayers = n; }
    unsigned char numberOfTemporalLayers;
    bool denoisingOn;
    bool automaticResizeOn;
    int keyFrameInterval;
};

enum class InterLayerPredMode : int
{
    kOff = 0,     // Inter-layer prediction is disabled.
    kOn = 1,      // Inter-layer prediction is enabled.
    kOnKeyPic = 2 // Inter-layer prediction is enabled but limited to key frames.
};

// VP9 specific.
struct VideoCodecVP9
{
    bool operator==(const VideoCodecVP9 &other) const;
    bool operator!=(const VideoCodecVP9 &other) const { return !(*this == other); }
    // Temporary utility method for transition deleting numberOfTemporalLayers
    // setting (replaced by ScalabilityMode).
    void SetNumberOfTemporalLayers(unsigned char n) { numberOfTemporalLayers = n; }
    unsigned char numberOfTemporalLayers;
    bool denoisingOn;
    int keyFrameInterval;
    bool adaptiveQpMode;
    bool automaticResizeOn;
    unsigned char numberOfSpatialLayers;
    bool flexibleMode;
    InterLayerPredMode interLayerPred;
};

// H264 specific.
struct VideoCodecH264
{
    bool operator==(const VideoCodecH264 &other) const;
    bool operator!=(const VideoCodecH264 &other) const { return !(*this == other); }
    // Temporary utility method for transition deleting numberOfTemporalLayers
    // setting (replaced by ScalabilityMode).
    void SetNumberOfTemporalLayers(unsigned char n) { numberOfTemporalLayers = n; }
    int keyFrameInterval;
    uint8_t numberOfTemporalLayers;
};

struct VideoCodecAV1
{
    bool operator==(const VideoCodecAV1 &other) const { return automatic_resize_on == other.automatic_resize_on; }
    bool operator!=(const VideoCodecAV1 &other) const { return !(*this == other); }
    bool automatic_resize_on;
};

// Translates from name of codec to codec type and vice versa.
OCTK_MEDIA_API const char *CodecTypeToPayloadString(VideoCodecType type);
OCTK_MEDIA_API VideoCodecType PayloadStringToCodecType(const std::string &name);

union VideoCodecUnion
{
    VideoCodecVP8 VP8;
    VideoCodecVP9 VP9;
    VideoCodecH264 H264;
    VideoCodecAV1 AV1;
};

enum class VideoCodecMode
{
    kRealtimeVideo,
    kScreensharing
};

// Common video codec properties
class OCTK_MEDIA_API VideoCodec
{
public:
    VideoCodec();

    // Scalability mode as described in
    // https://www.w3.org/TR/webrtc-svc/#scalabilitymodes*
    Optional<ScalabilityMode> GetScalabilityMode() const { return scalability_mode_; }
    void SetScalabilityMode(ScalabilityMode scalability_mode) { scalability_mode_ = scalability_mode; }
    void UnsetScalabilityMode() { scalability_mode_ = utils::nullopt; }

    VideoCodecComplexity GetVideoEncoderComplexity() const;
    void SetVideoEncoderComplexity(VideoCodecComplexity complexity_setting);

    bool GetFrameDropEnabled() const;
    void SetFrameDropEnabled(bool enabled);

    bool IsSinglecast() const { return numberOfSimulcastStreams <= 1; }
    bool IsSimulcast() const { return !IsSinglecast(); }

    // Public variables. TODO(hta): Make them private with accessors.
    VideoCodecType codecType;

    // TODO(nisse): Change to int, for consistency.
    uint16_t width;
    uint16_t height;

    unsigned int startBitrate; // kilobits/sec.
    unsigned int maxBitrate;   // kilobits/sec.
    unsigned int minBitrate;   // kilobits/sec.

    uint32_t maxFramerate;

    // This enables/disables encoding and sending when there aren't multiple
    // simulcast streams,by allocating 0 bitrate if inactive.
    bool active;

    unsigned int qpMax;
    // The actual number of simulcast streams. This is <= 1 in singlecast (it can
    // be 0 in old code paths), but it is also 1 in the {active,inactive,inactive}
    // "single RTP simulcast" use case and the legacy kSVC use case. In all other
    // cases this is the same as the number of encodings (which may include
    // inactive encodings). In other words:
    // - `numberOfSimulcastStreams <= 1` in singlecast and singlecast-like setups
    //   including legacy kSVC (encodings interpreted as spatial layers) or
    //   standard kSVC (1 active encoding).
    // - `numberOfSimulcastStreams > 1` in simulcast of 2+ active encodings.
    unsigned char numberOfSimulcastStreams;
    SimulcastStream simulcastStream[kMaxSimulcastStreams];
    SpatialLayer spatialLayers[kMaxSpatialLayers];

    VideoCodecMode mode;
    bool expect_encode_from_texture;

    // Timing frames configuration. There is delay of delay_ms between two
    // consequent timing frames, excluding outliers. Frame is always made a
    // timing frame if it's at least outlier_ratio in percent of "ideal" average
    // frame given bitrate and framerate, i.e. if it's bigger than
    // |outlier_ratio / 100.0 * bitrate_bps / fps| in bits. This way, timing
    // frames will not be sent too often usually. Yet large frames will always
    // have timing information for debug purposes because they are more likely to
    // cause extra delays.
    struct TimingFrameTriggerThresholds
    {
        int64_t delay_ms;
        uint16_t outlier_ratio_percent;
    } timing_frame_thresholds;

    // Legacy Google conference mode flag for simulcast screenshare
    bool legacy_conference_mode;

    bool operator==(const VideoCodec &other) const = delete;
    bool operator!=(const VideoCodec &other) const = delete;
    std::string ToString() const;

    // Accessors for codec specific information.
    // There is a const version of each that returns a reference,
    // and a non-const version that returns a pointer, in order
    // to allow modification of the parameters.
    VideoCodecVP8 *VP8();
    const VideoCodecVP8 &VP8() const;
    VideoCodecVP9 *VP9();
    const VideoCodecVP9 &VP9() const;
    VideoCodecH264 *H264();
    const VideoCodecH264 &H264() const;
    VideoCodecAV1 *AV1();
    const VideoCodecAV1 &AV1() const;

private:
    // TODO(hta): Consider replacing the union with a pointer type.
    // This will allow removing the VideoCodec* types from this file.
    VideoCodecUnion codec_specific_;
    Optional<ScalabilityMode> scalability_mode_;
    // 'complexity_' indicates the CPU capability of the client. It's used to
    // determine encoder CPU complexity (e.g., cpu_used for VP8, VP9. and AV1).
    VideoCodecComplexity complexity_;
    bool frame_drop_enabled_ = false;
};

OCTK_END_NAMESPACE

#endif // _OCTK_VIDEO_CODEC_HPP
