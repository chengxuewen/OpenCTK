//
// Created by cxw on 25-8-15.
//

#ifndef _OCTK_VIDEO_ENCODER_CONFIG_HPP
#define _OCTK_VIDEO_ENCODER_CONFIG_HPP

#include <octk_video_codec_types.hpp>
#include <octk_field_trials_view.hpp>
#include <octk_sdp_video_format.hpp>
#include <octk_rtp_parameters.hpp>
#include <octk_scoped_refptr.hpp>
#include <octk_video_codec.hpp>
#include <octk_ref_count.hpp>
#include <octk_optional.hpp>

OCTK_BEGIN_NAMESPACE

// The `VideoStream` struct describes a simulcast layer, or "stream".
struct VideoStream
{
    VideoStream();
    ~VideoStream();
    VideoStream(const VideoStream &other);
    std::string toString() const;

    // Width/Height in pixels.
    // This is the actual width and height used to configure encoder,
    // which might be less than `scale_resolution_down_to` due to adaptation
    // or due to the source providing smaller frames than requested.
    size_t width;
    size_t height;

    // Frame rate in fps.
    int max_framerate;

    // Bitrate, in bps, for the stream.
    int min_bitrate_bps;
    int target_bitrate_bps;
    int max_bitrate_bps;

    // Scaling factor applied to the stream size.
    // `width` and `height` values are already scaled down.
    double scale_resolution_down_by;

    // Maximum Quantization Parameter to use when encoding the stream.
    int max_qp;

    // Determines the number of temporal layers that the stream should be
    // encoded with. This value should be greater than zero.
    // TODO(brandtr): This class is used both for configuring the encoder
    // (meaning that this field _must_ be set), and for signaling the app-level
    // encoder settings (meaning that the field _may_ be set). We should separate
    // this and remove this optional instead.
    Optional<size_t> num_temporal_layers;

    // The priority of this stream, to be used when allocating resources
    // between multiple streams.
    Optional<double> bitrate_priority;

    Optional<ScalabilityMode> scalability_mode;

    // If this stream is enabled by the user, or not.
    bool active;

    // An optional user supplied max_frame_resolution
    // than can be set independently of (adapted) VideoSource.
    // This value is set from RtpEncodingParameters::scale_resolution_down_to
    // (i.e. used for signaling app-level settings).
    //
    // The actual encode resolution is in `width` and `height`,
    // which can be lower than scale_resolution_down_to,
    // e.g. if source only provides lower resolution or
    // if resource adaptation is active.
    Optional<Resolution> scale_resolution_down_to;
};

class VideoEncoderConfig
{
public:
    // These are reference counted to permit copying VideoEncoderConfig and be
    // kept alive until all encoder_specific_settings go out of scope.
    // TODO(kthelgason): Consider removing the need for copying VideoEncoderConfig
    // and use Optional for encoder_specific_settings instead.
    class EncoderSpecificSettings : public RefCountInterface
    {
    public:
        // TODO(pbos): Remove FillEncoderSpecificSettings as soon as VideoCodec is
        // not in use and encoder implementations ask for codec-specific structs
        // directly.
        void FillEncoderSpecificSettings(VideoCodec *codec_struct) const;

        virtual void FillVideoCodecVp8(VideoCodecVP8 *vp8_settings) const;
        virtual void FillVideoCodecVp9(VideoCodecVP9 *vp9_settings) const;
        virtual void FillVideoCodecAv1(VideoCodecAV1 *av1_settings) const;

    private:
        ~EncoderSpecificSettings() override { }
        friend class VideoEncoderConfig;
    };

    class Vp8EncoderSpecificSettings : public EncoderSpecificSettings
    {
    public:
        explicit Vp8EncoderSpecificSettings(const VideoCodecVP8 &specifics);
        void FillVideoCodecVp8(VideoCodecVP8 *vp8_settings) const override;

    private:
        VideoCodecVP8 specifics_;
    };

    class Vp9EncoderSpecificSettings : public EncoderSpecificSettings
    {
    public:
        explicit Vp9EncoderSpecificSettings(const VideoCodecVP9 &specifics);
        void FillVideoCodecVp9(VideoCodecVP9 *vp9_settings) const override;

    private:
        VideoCodecVP9 specifics_;
    };

    class Av1EncoderSpecificSettings : public EncoderSpecificSettings
    {
    public:
        explicit Av1EncoderSpecificSettings(const VideoCodecAV1 &specifics);
        void FillVideoCodecAv1(VideoCodecAV1 *av1_settings) const override;

    private:
        VideoCodecAV1 specifics_;
    };

    enum class ContentType
    {
        kRealtimeVideo,
        kScreen,
    };

    class VideoStreamFactoryInterface : public RefCountInterface
    {
    public:
        // An implementation should return a std::vector<VideoStream> with the
        // wanted VideoStream settings for the given video resolution.
        // The size of the vector may not be larger than
        // `encoder_config.number_of_streams`.
        virtual std::vector<VideoStream> CreateEncoderStreams(const FieldTrialsView &field_trials,
                                                              int frame_width,
                                                              int frame_height,
                                                              const VideoEncoderConfig &encoder_config) = 0;

    protected:
        ~VideoStreamFactoryInterface() override { }
    };

    VideoEncoderConfig &operator=(VideoEncoderConfig &&) = default;
    VideoEncoderConfig &operator=(const VideoEncoderConfig &) = delete;

    // Mostly used by tests.  Avoid creating copies if you can.
    VideoEncoderConfig Copy() const { return VideoEncoderConfig(*this); }

    VideoEncoderConfig();
    VideoEncoderConfig(VideoEncoderConfig &&);
    ~VideoEncoderConfig();
    std::string toString() const;

    bool HasScaleResolutionDownTo() const;

    // TODO(bugs.webrtc.org/6883): Consolidate on one of these.
    VideoCodecType codec_type;
    SdpVideoFormat video_format;

    // Note: This factory can be unset, and VideoStreamEncoder will
    // then use the EncoderStreamFactory. The factory is only set by
    // tests.
    ScopedRefPtr<VideoStreamFactoryInterface> video_stream_factory;
    std::vector<SpatialLayer> spatial_layers;
    ContentType content_type;
    bool frame_drop_enabled;
    ScopedRefPtr<const EncoderSpecificSettings> encoder_specific_settings;

    // Padding will be used up to this bitrate regardless of the bitrate produced
    // by the encoder. Padding above what's actually produced by the encoder helps
    // maintaining a higher bitrate estimate. Padding will however not be sent
    // unless the estimated bandwidth indicates that the link can handle it.
    int min_transmit_bitrate_bps;
    int max_bitrate_bps;
    // The bitrate priority used for all VideoStreams.
    double bitrate_priority;

    // The simulcast layer's configurations set by the application for this video
    // sender. These are modified by the video_stream_factory before being passed
    // down to lower layers for the video encoding.
    // `simulcast_layers` is also used for configuring non-simulcast (when there
    // is a single VideoStream).
    // We have the same number of `simulcast_layers` as we have negotiated
    // encodings, for example 3 are used in both simulcast and legacy kSVC.
    std::vector<VideoStream> simulcast_layers;

    // Max number of encoded VideoStreams to produce.
    // This is the same as the number of encodings negotiated (i.e. SSRCs),
    // whether or not those encodings are `active`, except for when legacy kSVC
    // is used. In this case we have three SSRCs but `number_of_streams` is
    // changed to 1 to tell lower layers to limit the number of streams.
    size_t number_of_streams;

    // Legacy Google conference mode flag for simulcast screenshare
    bool legacy_conference_mode;

    // Indicates whether quality scaling can be used or not.
    bool is_quality_scaling_allowed;

    // Maximum Quantization Parameter.
    // This value is fed into EncoderStreamFactory that
    // apply it to all simulcast layers/spatial layers.
    int max_qp;

private:
    // Access to the copy constructor is private to force use of the Copy()
    // method for those exceptional cases where we do use it.
    VideoEncoderConfig(const VideoEncoderConfig &);
};

OCTK_END_NAMESPACE

#endif // _OCTK_VIDEO_ENCODER_CONFIG_HPP
