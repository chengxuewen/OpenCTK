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

#pragma once

#include <octk_video_bitrate_allocation.hpp>
// #include <octk_video_codec_interface.hpp>
// #include <octk_video_frame_type.hpp>
// #include <octk_inlined_vector.hpp>
#include <octk_codec_specific_info.hpp>
#include <octk_encoded_image.hpp>
// #include <octk_video_codec.hpp>
#include <octk_video_frame.hpp>
#include <octk_data_rate.hpp>
#include <octk_optional.hpp>

OCTK_BEGIN_NAMESPACE

constexpr int kDefaultMinPixelsPerFrame = 320 * 180;

class OCTK_MEDIA_API EncodedImageCallback
{
public:
    virtual ~EncodedImageCallback() { }

    struct Result
    {
        enum Error
        {
            OK,
            ERROR_SEND_FAILED, // Failed to send the packet.
        };

        explicit Result(Error error)
            : error(error)
        {
        }
        Result(Error error, uint32_t frame_id)
            : error(error)
            , frame_id(frame_id)
        {
        }

        Error error;

        // Frame ID assigned to the frame. The frame ID should be the same as the ID
        // seen by the receiver for this frame. RTP timestamp of the frame is used
        // as frame ID when RTP is used to send video. Must be used only when
        // error=OK.
        uint32_t frame_id = 0;

        // Tells the encoder that the next frame is should be dropped.
        bool drop_next_frame = false;
    };

    // Used to signal the encoder about reason a frame is dropped.
    // kDroppedByMediaOptimizations - dropped by MediaOptimizations (for rate
    // limiting purposes).
    // kDroppedByEncoder - dropped by encoder's internal rate limiter.
    // TODO(bugs.webrtc.org/10164): Delete this enum? It duplicates the more
    // general VideoStreamEncoderObserver::DropReason. Also,
    // kDroppedByMediaOptimizations is not produced by any encoder, but by
    // VideoStreamEncoder.
    enum class DropReason : uint8_t
    {
        kDroppedByMediaOptimizations,
        kDroppedByEncoder
    };

    // Callback function which is called when an image has been encoded.
    virtual Result OnEncodedImage(const EncodedImage &encoded_image, const CodecSpecificInfo *codec_specific_info) = 0;

    virtual void OnDroppedFrame(DropReason /* reason */) { }
};


class OCTK_MEDIA_API VideoEncoder
{
public:
    struct QpThresholds final
    {
        QpThresholds(int l, int h)
            : low(l)
            , high(h)
        {
        }
        QpThresholds() { }
        int low{-1};
        int high{-1};
    };

    // Quality scaling is enabled if thresholds are provided.
    struct OCTK_MEDIA_API ScalingSettings
    {
    private:
        // Private magic type for kOff, implicitly convertible to ScalingSettings.
        struct KOff
        {
        };

    public:
        // TODO(bugs.webrtc.org/9078): Since Optional should be trivially copy
        // constructible, this magic value can likely be replaced by a constexpr ScalingSettings value.
        static constexpr KOff kOff = {};

        ScalingSettings(int low, int high);
        ScalingSettings(int low, int high, int min_pixels);
        ScalingSettings(const ScalingSettings &);
        ScalingSettings(KOff); // NOLINT(runtime/explicit)
        ~ScalingSettings();

        Optional<QpThresholds> thresholds;

        // We will never ask for a resolution lower than this.
        // TODO(kthelgason): Lower this limit when better testing
        // on MediaCodec and fallback implementations are in place.
        // See https://bugs.chromium.org/p/webrtc/issues/detail?id=7206
        int minPixelsPerFrame = kDefaultMinPixelsPerFrame;

    private:
        // Private constructor; to get an object without thresholds, use
        // the magic constant ScalingSettings::kOff.
        ScalingSettings();
    };

    // Bitrate limits for resolution.
    struct ResolutionBitrateLimits
    {
        ResolutionBitrateLimits(int frame_size_pixels,
                                int min_start_bitrate_bps,
                                int min_bitrate_bps,
                                int max_bitrate_bps)
            : frameSizePixels(frame_size_pixels)
            , minStartBitrateBps(min_start_bitrate_bps)
            , minBitrateBps(min_bitrate_bps)
            , maxBitrateBps(max_bitrate_bps)
        {
        }
        // Size of video frame, in pixels, the bitrate thresholds are intended for.
        int frameSizePixels = 0;
        // Recommended minimum bitrate to start encoding.
        int minStartBitrateBps = 0;
        // Recommended minimum bitrate.
        int minBitrateBps = 0;
        // Recommended maximum bitrate.
        int maxBitrateBps = 0;

        bool operator==(const ResolutionBitrateLimits &rhs) const;
        bool operator!=(const ResolutionBitrateLimits &rhs) const { return !(*this == rhs); }
    };

    // Struct containing metadata about the encoder implementing this interface.
    struct OCTK_MEDIA_API EncoderInfo
    {
        static constexpr uint8_t kMaxFramerateFraction = std::numeric_limits<uint8_t>::max();

        EncoderInfo();
        EncoderInfo(const EncoderInfo &);
        ~EncoderInfo();

        std::string toString() const;
        bool operator==(const EncoderInfo &rhs) const;
        bool operator!=(const EncoderInfo &rhs) const { return !(*this == rhs); }

        // Any encoder implementation wishing to use the WebRTC provided
        // quality scaler must populate this field.
        ScalingSettings scalingSettings;

        // The width and height of the incoming video frames should be divisible
        // by `requested_resolution_alignment`. If they are not, the encoder may
        // drop the incoming frame.
        // For example: With I420, this value would be a multiple of 2.
        // Note that this field is unrelated to any horizontal or vertical stride
        // requirements the encoder has on the incoming video frame buffers.
        uint32_t requestedResolutionAlignment;

        // Same as above but if true, each simulcast layer should also be divisible
        // by `requested_resolution_alignment`.
        // Note that scale factors `scale_resolution_down_by` may be adjusted so a
        // common multiple is not too large to avoid largely cropped frames and
        // possibly with an aspect ratio far from the original.
        // Warning: large values of scale_resolution_down_by could be changed
        // considerably, especially if `requested_resolution_alignment` is large.
        bool applyAlignmentToAllSimulcastLayers;

        // If true, encoder supports working with a native handle (e.g. texture
        // handle for hw codecs) rather than requiring a raw I420 buffer.
        bool supportsNativeHandle;

        // The name of this particular encoder implementation, e.g. "libvpx".
        std::string implementationName;

        // If this field is true, the encoder rate controller must perform
        // well even in difficult situations, and produce close to the specified
        // target bitrate seen over a reasonable time window, drop frames if
        // necessary in order to keep the rate correct, and react quickly to
        // changing bitrate targets. If this method returns true, we disable the
        // frame dropper in the media optimization module and rely entirely on the
        // encoder to produce media at a bitrate that closely matches the target.
        // Any overshooting may result in delay buildup. If this method returns
        // false (default behavior), the media opt frame dropper will drop input
        // frames if it suspect encoder misbehavior. Misbehavior is common,
        // especially in hardware codecs. Disable media opt at your own risk.
        bool hasTrustedRateController;

        // If this field is true, the encoder uses hardware support and different
        // thresholds will be used in CPU adaptation.
        bool isHardwareAccelerated;

        // For each spatial layer (simulcast stream or SVC layer), represented as an
        // element in `fps_allocation` a vector indicates how many temporal layers
        // the encoder is using for that spatial layer.
        // For each spatial/temporal layer pair, the frame rate fraction is given as
        // an 8bit unsigned integer where 0 = 0% and 255 = 100%.
        //
        // If the vector is empty for a given spatial layer, it indicates that frame
        // rates are not defined and we can't count on any specific frame rate to be
        // generated. Likely this indicates Vp8TemporalLayersType::kBitrateDynamic.
        //
        // The encoder may update this on a per-frame basis in response to both
        // internal and external signals.
        //
        // Spatial layers are treated independently, but temporal layers are
        // cumulative. For instance, if:
        //   fps_allocation[0][0] = kMaxFramerateFraction / 2;
        //   fps_allocation[0][1] = kMaxFramerateFraction;
        // Then half of the frames are in the base layer and half is in TL1, but
        // since TL1 is assumed to depend on the base layer, the frame rate is
        // indicated as the full 100% for the top layer.
        //
        // Defaults to a single spatial layer containing a single temporal layer
        // with a 100% frame rate fraction.
        InlinedVector<uint8_t, kMaxTemporalStreams> fpsAllocation[kMaxSpatialLayers];

        // Recommended bitrate limits for different resolutions.
        std::vector<ResolutionBitrateLimits> resolutionBitrateLimits;

        // Obtains the limits from `resolution_bitrate_limits` that best matches the
        // `frame_size_pixels`.
        Optional<ResolutionBitrateLimits> getEncoderBitrateLimitsForResolution(int frameSizePixels) const;

        // If true, this encoder has internal support for generating simulcast
        // streams. Otherwise, an adapter class will be needed.
        // Even if true, the config provided to InitEncode() might not be supported,
        // in such case the encoder should return
        // WEBRTC_VIDEO_CODEC_ERR_SIMULCAST_PARAMETERS_NOT_SUPPORTED.
        bool supportsSimulcast;

        // The list of pixel formats preferred by the encoder. It is assumed that if
        // the list is empty and supports_native_handle is false, then {I420} is the
        // preferred pixel format. The order of the formats does not matter.
        InlinedVector<VideoFrameBuffer::Type, kMaxPreferredPixelFormats> preferredPixelFormats;

        // Indicates whether or not QP value encoder writes into frame/slice/tile
        // header can be interpreted as average frame/slice/tile QP.
        Optional<bool> isQPTrusted;

        // The minimum QP that the encoder is expected to use with the current
        // configuration. This may be used to determine if the encoder has reached
        // its target video quality for static screenshare content.
        Optional<int> minQP;
    };

    struct OCTK_MEDIA_API RateControlParameters
    {
        RateControlParameters();
        RateControlParameters(const VideoBitrateAllocation &bitrate, double framerate_fps);
        RateControlParameters(const VideoBitrateAllocation &bitrate,
                              double framerate_fps,
                              DataRate bandwidth_allocation);
        virtual ~RateControlParameters();

        // Target bitrate, per spatial/temporal layer.
        // A target bitrate of 0bps indicates a layer should not be encoded at all.
        VideoBitrateAllocation targetBitrate;
        // Adjusted target bitrate, per spatial/temporal layer. May be lower or
        // higher than the target depending on encoder behaviour.
        VideoBitrateAllocation bitrate;
        // Target framerate, in fps. A value <= 0.0 is invalid and should be
        // interpreted as framerate target not available. In this case the encoder
        // should fall back to the max framerate specified in `codec_settings` of
        // the last InitEncode() call.
        double framerateFps;
        // The network bandwidth available for video. This is at least
        // `bitrate.get_sum_bps()`, but may be higher if the application is not
        // network constrained.
        DataRate bandwidthAllocation;

        bool operator==(const RateControlParameters &rhs) const;
        bool operator!=(const RateControlParameters &rhs) const;
    };

    struct LossNotification
    {
        // The timestamp of the last decodable frame *prior* to the last received.
        // (The last received - described below - might itself be decodable or not.)
        uint32_t timestampOfLastDecodable;
        // The timestamp of the last received frame.
        uint32_t timestampOfLastReceived;
        // Describes whether the dependencies of the last received frame were
        // all decodable.
        // `false` if some dependencies were undecodable, `true` if all dependencies
        // were decodable, and `utils::nullopt` if the dependencies are unknown.
        Optional<bool> dependenciesOfLastReceivedDecodable;
        // Describes whether the received frame was decodable.
        // `false` if some dependency was undecodable or if some packet belonging
        // to the last received frame was missed.
        // `true` if all dependencies were decodable and all packets belonging
        // to the last received frame were received.
        // `utils::nullopt` if no packet belonging to the last frame was missed, but the
        // last packet in the frame was not yet received.
        Optional<bool> lastReceivedDecodable;
    };

    // Negotiated capabilities which the VideoEncoder may expect the other
    // side to use.
    struct Capabilities
    {
        explicit Capabilities(bool loss_notification)
            : lossNotification(loss_notification)
        {
        }
        bool lossNotification;
    };

    struct Settings
    {
        Settings(const Capabilities &capabilities, int number_of_cores, size_t max_payload_size)
            : capabilities(capabilities)
            , numberOfCores(number_of_cores)
            , maxPayloadSize(max_payload_size)
        {
        }

        Capabilities capabilities;
        int numberOfCores;
        size_t maxPayloadSize;
        // Experimental API - currently only supported by LibvpxVp8Encoder and the OpenH264 encoder.
        // If set, limits the number of encoder threads.
        Optional<int> encoderThreadLimit;
    };

    static VideoCodecVP8 GetDefaultVp8Settings();
    static VideoCodecVP9 GetDefaultVp9Settings();
    static VideoCodecH264 getDefaultH264Settings();

    virtual ~VideoEncoder() { }

    // Initialize the encoder with the information from the codecSettings
    //
    // Input:
    //          - codec_settings    : Codec settings
    //          - settings          : Settings affecting the encoding itself.
    // Input for deprecated version:
    //          - number_of_cores   : Number of cores available for the encoder
    //          - max_payload_size  : The maximum size each payload is allowed
    //                                to have. Usually MTU - overhead.
    //
    // Return value                  : Set bit rate if OK
    //                                 <0 - Errors:
    //                                  WEBRTC_VIDEO_CODEC_ERR_PARAMETER
    //                                  WEBRTC_VIDEO_CODEC_ERR_SIZE
    //                                  WEBRTC_VIDEO_CODEC_MEMORY
    //                                  WEBRTC_VIDEO_CODEC_ERROR
    // TODO(bugs.webrtc.org/10720): After updating downstream projects and posting
    // an announcement to discuss-webrtc, remove the three-parameters variant
    // and make the two-parameters variant pure-virtual.
    virtual int32_t initEncode(const VideoCodec *codec, int32_t numberOfCores, size_t maxPayloadSize);
    virtual int32_t initEncode(const VideoCodec *codec, const VideoEncoder::Settings &settings);

    // Register an encode complete callback object.
    //
    // Input:
    //          - callback         : Callback object which handles encoded images.
    //
    // Return value                : WEBRTC_VIDEO_CODEC_OK if OK, < 0 otherwise.
    virtual int32_t registerEncodeCompleteCallback(EncodedImageCallback *callback) = 0;

    // Free encoder memory.
    // Return value                : WEBRTC_VIDEO_CODEC_OK if OK, < 0 otherwise.
    virtual int32_t release() = 0;

    // Encode an image (as a part of a video stream). The encoded image
    // will be returned to the user through the encode complete callback.
    //
    // Input:
    //          - frame             : Image to be encoded
    //          - frame_types       : Frame type to be generated by the encoder.
    //
    // Return value                 : WEBRTC_VIDEO_CODEC_OK if OK
    //                                <0 - Errors:
    //                                  WEBRTC_VIDEO_CODEC_ERR_PARAMETER
    //                                  WEBRTC_VIDEO_CODEC_MEMORY
    //                                  WEBRTC_VIDEO_CODEC_ERROR
    virtual int32_t encode(const VideoFrame &frame, const std::vector<VideoFrameType> *frame_types) = 0;

    // Sets rate control parameters: bitrate, framerate, etc. These settings are
    // instantaneous (i.e. not moving averages) and should apply from now until
    // the next call to SetRates().
    virtual void setRates(const RateControlParameters &parameters) = 0;

    // Inform the encoder when the packet loss rate changes.
    //
    // Input:   - packet_loss_rate  : The packet loss rate (0.0 to 1.0).
    virtual void onPacketLossRateUpdate(float packet_loss_rate);

    // Inform the encoder when the round trip time changes.
    //
    // Input:   - rtt_ms            : The new RTT, in milliseconds.
    virtual void onRttUpdate(int64_t rtt_ms);

    // Called when a loss notification is received.
    virtual void onLossNotification(const LossNotification &loss_notification);

    // Returns meta-data about the encoder, such as implementation name.
    // The output of this method may change during runtime. For instance if a
    // hardware encoder fails, it may fall back to doing software encoding using
    // an implementation with different characteristics.
    virtual EncoderInfo getEncoderInfo() const = 0;
};

OCTK_END_NAMESPACE