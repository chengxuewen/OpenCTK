//
// Created by cxw on 25-8-15.
//

#ifndef _OCTK_VIDEO_STREAM_ENCODER_HPP
#define _OCTK_VIDEO_STREAM_ENCODER_HPP

#include <octk_media_global.hpp>
#include <octk_inlined_vector.hpp>
#include <octk_resource.hpp>
#include <octk_rtc_context.hpp>
#include <octk_rtp_sender_interface.hpp>
#include <octk_sequence_checker.hpp>
#include <octk_pending_task_safety_flag.hpp>
#include <octk_data_rate.hpp>
#include <octk_encoded_image.hpp>
#include <octk_video_bitrate_allocation.hpp>
#include <octk_video_rotation.hpp>
#include <octk_video_sink_interface.hpp>
#include <octk_video_stream_encoder_settings.hpp>
#include <octk_video_stream_encoder_interface.hpp>
#include <octk_video_codec.hpp>
#include <octk_video_encoder.hpp>

OCTK_BEGIN_NAMESPACE

// VideoStreamEncoder represent a video encoder that accepts raw video frames as
// input and produces an encoded bit stream.
// Usage:
//  Instantiate.
//  Call SetSink.
//  Call SetSource.
//  Call ConfigureEncoder with the codec settings.
//  Call Stop() when done.
class VideoStreamEncoder : public VideoStreamEncoderInterface,
                           private EncodedImageCallback,
                           public VideoSourceRestrictionsListener {
public:
    // TODO(bugs.webrtc.org/12000): Reporting of VideoBitrateAllocation is being
    // deprecated. Instead VideoLayersAllocation should be reported.
    enum class BitrateAllocationCallbackType {
        kVideoBitrateAllocation,
        kVideoBitrateAllocationWhenScreenSharing,
        kVideoLayersAllocation
    };
    VideoStreamEncoder(
        const RtcContext& env,
        uint32_t number_of_cores,
        VideoStreamEncoderObserver* encoder_stats_observer,
        const VideoStreamEncoderSettings& settings,
        std::unique_ptr<OveruseFrameDetector> overuse_detector,
        std::unique_ptr<FrameCadenceAdapterInterface> frame_cadence_adapter,
        std::unique_ptr<webrtc::TaskQueueBase, webrtc::TaskQueueDeleter>
            encoder_queue,
        BitrateAllocationCallbackType allocation_cb_type,
        webrtc::VideoEncoderFactory::EncoderSelectorInterface* encoder_selector =
            nullptr);
    ~VideoStreamEncoder() override;

    VideoStreamEncoder(const VideoStreamEncoder&) = delete;
    VideoStreamEncoder& operator=(const VideoStreamEncoder&) = delete;

    void AddAdaptationResource(rtc::scoped_refptr<Resource> resource) override;
    std::vector<rtc::scoped_refptr<Resource>> GetAdaptationResources() override;

    void SetSource(rtc::VideoSourceInterface<VideoFrame>* source,
                   const DegradationPreference& degradation_preference) override;

    void SetSink(EncoderSink* sink, bool rotation_applied) override;

    // TODO(perkj): Can we remove VideoCodec.startBitrate ?
    void SetStartBitrate(int start_bitrate_bps) override;

    void SetFecControllerOverride(
        FecControllerOverride* fec_controller_override) override;

    void ConfigureEncoder(VideoEncoderConfig config,
                          size_t max_data_payload_length) override;
    void ConfigureEncoder(VideoEncoderConfig config,
                          size_t max_data_payload_length,
                          SetParametersCallback callback) override;

    // Permanently stop encoding. After this method has returned, it is
    // guaranteed that no encoded frames will be delivered to the sink.
    void Stop() override;

    void SendKeyFrame(const std::vector<VideoFrameType>& layers = {}) override;

    void OnLossNotification(
        const VideoEncoder::LossNotification& loss_notification) override;

    void OnBitrateUpdated(DataRate target_bitrate,
                          DataRate stable_target_bitrate,
                          DataRate target_headroom,
                          uint8_t fraction_lost,
                          int64_t round_trip_time_ms,
                          double cwnd_reduce_ratio) override;

    DataRate UpdateTargetBitrate(DataRate target_bitrate,
                                 double cwnd_reduce_ratio);

protected:
    friend class VideoStreamEncoderFrameCadenceRestrictionTest;

    // Used for testing. For example the `ScalingObserverInterface` methods must
    // be called on `encoder_queue_`.
    TaskQueueBase* encoder_queue() { return encoder_queue_.get(); }

    void OnVideoSourceRestrictionsUpdated(
        VideoSourceRestrictions restrictions,
        const VideoAdaptationCounters& adaptation_counters,
        rtc::scoped_refptr<Resource> reason,
        const VideoSourceRestrictions& unfiltered_restrictions) override;

    // Used for injected test resources.
    // TODO(eshr): Move all adaptation tests out of VideoStreamEncoder tests.
    void InjectAdaptationResource(rtc::scoped_refptr<Resource> resource,
                                  VideoAdaptationReason reason);
    void InjectAdaptationConstraint(AdaptationConstraint* adaptation_constraint);

    void AddRestrictionsListenerForTesting(
        VideoSourceRestrictionsListener* restrictions_listener);
    void RemoveRestrictionsListenerForTesting(
        VideoSourceRestrictionsListener* restrictions_listener);

private:
    class CadenceCallback : public FrameCadenceAdapterInterface::Callback {
    public:
        explicit CadenceCallback(VideoStreamEncoder& video_stream_encoder)
            : video_stream_encoder_(video_stream_encoder) {}
        // FrameCadenceAdapterInterface::Callback overrides.
        void OnFrame(Timestamp post_time,
                     bool queue_overload,
                     const VideoFrame& frame) override {
            video_stream_encoder_.OnFrame(post_time, queue_overload, frame);
        }
        void OnDiscardedFrame() override {
            video_stream_encoder_.OnDiscardedFrame();
        }
        void RequestRefreshFrame() override {
            video_stream_encoder_.RequestRefreshFrame();
        }

    private:
        VideoStreamEncoder& video_stream_encoder_;
    };

    class VideoFrameInfo {
    public:
        VideoFrameInfo(int width, int height, bool is_texture)
            : width(width), height(height), is_texture(is_texture) {}
        int width;
        int height;
        bool is_texture;
        int pixel_count() const { return width * height; }
    };

    struct EncoderRateSettings {
        EncoderRateSettings();
        EncoderRateSettings(const VideoBitrateAllocation& bitrate,
                            double framerate_fps,
                            DataRate bandwidth_allocation,
                            DataRate encoder_target,
                            DataRate stable_encoder_target);
        bool operator==(const EncoderRateSettings& rhs) const;
        bool operator!=(const EncoderRateSettings& rhs) const;

        VideoEncoder::RateControlParameters rate_control;
        // This is the scalar target bitrate before the VideoBitrateAllocator, i.e.
        // the `target_bitrate` argument of the OnBitrateUpdated() method. This is
        // needed because the bitrate allocator may truncate the total bitrate and a
        // later call to the same allocator instance, e.g.
        // |using last_encoder_rate_setings_->bitrate.get_sum_bps()|, may trick it
        // into thinking the available bitrate has decreased since the last call.
        DataRate encoder_target;
        DataRate stable_encoder_target;
    };

    class DegradationPreferenceManager;

    void ReconfigureEncoder() RTC_RUN_ON(encoder_queue_);
    void OnEncoderSettingsChanged() RTC_RUN_ON(encoder_queue_);
    void OnFrame(Timestamp post_time,
                 bool queue_overload,
                 const VideoFrame& video_frame);
    void OnDiscardedFrame();
    void RequestRefreshFrame();

    void MaybeEncodeVideoFrame(const VideoFrame& frame,
                               int64_t time_when_posted_in_ms);

    void EncodeVideoFrame(const VideoFrame& frame,
                          int64_t time_when_posted_in_ms);
    // Indicates whether frame should be dropped because the pixel count is too
    // large for the current bitrate configuration.
    bool DropDueToSize(uint32_t pixel_count) const RTC_RUN_ON(encoder_queue_);

    // Implements EncodedImageCallback.
    EncodedImageCallback::Result OnEncodedImage(
        const EncodedImage& encoded_image,
        const CodecSpecificInfo* codec_specific_info) override;

    void OnDroppedFrame(EncodedImageCallback::DropReason reason) override;

    bool EncoderPaused() const;
    void TraceFrameDropStart();
    void TraceFrameDropEnd();

    // Returns a copy of `rate_settings` with the `bitrate` field updated using
    // the current VideoBitrateAllocator.
    EncoderRateSettings UpdateBitrateAllocation(
        const EncoderRateSettings& rate_settings) RTC_RUN_ON(encoder_queue_);

    uint32_t GetInputFramerateFps() RTC_RUN_ON(encoder_queue_);
    void SetEncoderRates(const EncoderRateSettings& rate_settings)
        RTC_RUN_ON(encoder_queue_);

    void RunPostEncode(const EncodedImage& encoded_image,
                       int64_t time_sent_us,
                       int temporal_index,
                       DataSize frame_size);
    void ReleaseEncoder() RTC_RUN_ON(encoder_queue_);
    // After calling this function `resource_adaptation_processor_` will be null.
    void ShutdownResourceAdaptationQueue();

    void RequestEncoderSwitch() RTC_RUN_ON(encoder_queue_);

    // Augments an EncodedImage received from an encoder with parsable
    // information.
    EncodedImage AugmentEncodedImage(
        const EncodedImage& encoded_image,
        const CodecSpecificInfo* codec_specific_info);

    void ProcessDroppedFrame(const VideoFrame& frame,
                             VideoStreamEncoderObserver::DropReason reason)
        RTC_RUN_ON(encoder_queue_);

    const RtcContext env_;
    TaskQueueBase* const worker_queue_;

    const int number_of_cores_;

    EncoderSink* sink_ = nullptr;
    const VideoStreamEncoderSettings settings_;
    const BitrateAllocationCallbackType allocation_cb_type_;
    const RateControlSettings rate_control_settings_;

    webrtc::VideoEncoderFactory::EncoderSelectorInterface* const
        encoder_selector_from_constructor_;
    std::unique_ptr<VideoEncoderFactory::EncoderSelectorInterface> const
        encoder_selector_from_factory_;
    // Pointing to either encoder_selector_from_constructor_ or
    // encoder_selector_from_factory_ but can be nullptr.
    VideoEncoderFactory::EncoderSelectorInterface* const encoder_selector_;

    VideoStreamEncoderObserver* const encoder_stats_observer_;
    // Adapter that avoids public inheritance of the cadence adapter's callback
    // interface.
    CadenceCallback cadence_callback_{*this};
    // Frame cadence encoder adapter. Frames enter this adapter first, and it then
    // forwards them to our OnFrame method.
    std::unique_ptr<FrameCadenceAdapterInterface> frame_cadence_adapter_
        OCTK_ATTRIBUTE_GUARDED_BY(encoder_queue_) RTC_PT_GUARDED_BY(encoder_queue_);

    VideoEncoderConfig encoder_config_ OCTK_ATTRIBUTE_GUARDED_BY(encoder_queue_);
    std::unique_ptr<VideoEncoder> encoder_ OCTK_ATTRIBUTE_GUARDED_BY(encoder_queue_)
        RTC_PT_GUARDED_BY(encoder_queue_);
    bool encoder_initialized_ = false;
    std::unique_ptr<VideoBitrateAllocator> rate_allocator_
        OCTK_ATTRIBUTE_GUARDED_BY(encoder_queue_) RTC_PT_GUARDED_BY(encoder_queue_);
    int max_framerate_ OCTK_ATTRIBUTE_GUARDED_BY(encoder_queue_) = -1;

    // Set when ConfigureEncoder has been called in order to lazy reconfigure the
    // encoder on the next frame.
    bool pending_encoder_reconfiguration_ OCTK_ATTRIBUTE_GUARDED_BY(encoder_queue_) = false;
    // Set when configuration must create a new encoder object, e.g.,
    // because of a codec change.
    bool pending_encoder_creation_ OCTK_ATTRIBUTE_GUARDED_BY(encoder_queue_) = false;
    absl::InlinedVector<SetParametersCallback, 2> encoder_configuration_callbacks_
        OCTK_ATTRIBUTE_GUARDED_BY(encoder_queue_);

    std::optional<VideoFrameInfo> last_frame_info_ OCTK_ATTRIBUTE_GUARDED_BY(encoder_queue_);
    int crop_width_ OCTK_ATTRIBUTE_GUARDED_BY(encoder_queue_) = 0;
    int crop_height_ OCTK_ATTRIBUTE_GUARDED_BY(encoder_queue_) = 0;
    std::optional<uint32_t> encoder_target_bitrate_bps_
        OCTK_ATTRIBUTE_GUARDED_BY(encoder_queue_);
    size_t max_data_payload_length_ OCTK_ATTRIBUTE_GUARDED_BY(encoder_queue_) = 0;
    std::optional<EncoderRateSettings> last_encoder_rate_settings_
        OCTK_ATTRIBUTE_GUARDED_BY(encoder_queue_);
    bool encoder_paused_and_dropped_frame_ OCTK_ATTRIBUTE_GUARDED_BY(encoder_queue_) = false;

    // Set to true if at least one frame was sent to encoder since last encoder
    // initialization.
    bool was_encode_called_since_last_initialization_
        OCTK_ATTRIBUTE_GUARDED_BY(encoder_queue_) = false;

    bool encoder_failed_ OCTK_ATTRIBUTE_GUARDED_BY(encoder_queue_) = false;

    // Used to make sure incoming time stamp is increasing for every frame.
    int64_t last_captured_timestamp_ OCTK_ATTRIBUTE_GUARDED_BY(encoder_queue_) = 0;
    // Delta used for translating between NTP and internal timestamps.
    const int64_t delta_ntp_internal_ms_ OCTK_ATTRIBUTE_GUARDED_BY(encoder_queue_);

    int64_t last_frame_log_ms_ OCTK_ATTRIBUTE_GUARDED_BY(encoder_queue_);
    int captured_frame_count_ OCTK_ATTRIBUTE_GUARDED_BY(encoder_queue_) = 0;
    int dropped_frame_cwnd_pushback_count_ OCTK_ATTRIBUTE_GUARDED_BY(encoder_queue_) = 0;
    int dropped_frame_encoder_block_count_ OCTK_ATTRIBUTE_GUARDED_BY(encoder_queue_) = 0;
    std::optional<VideoFrame> pending_frame_ OCTK_ATTRIBUTE_GUARDED_BY(encoder_queue_);
    int64_t pending_frame_post_time_us_ OCTK_ATTRIBUTE_GUARDED_BY(encoder_queue_) = 0;

    VideoFrame::UpdateRect accumulated_update_rect_
        OCTK_ATTRIBUTE_GUARDED_BY(encoder_queue_);
    bool accumulated_update_rect_is_valid_ OCTK_ATTRIBUTE_GUARDED_BY(encoder_queue_) = true;

    FecControllerOverride* fec_controller_override_
        OCTK_ATTRIBUTE_GUARDED_BY(encoder_queue_) = nullptr;
    std::optional<int64_t> last_parameters_update_ms_
        OCTK_ATTRIBUTE_GUARDED_BY(encoder_queue_);
    std::optional<int64_t> last_encode_info_ms_ OCTK_ATTRIBUTE_GUARDED_BY(encoder_queue_);

    VideoEncoder::EncoderInfo encoder_info_ OCTK_ATTRIBUTE_GUARDED_BY(encoder_queue_);
    VideoCodec send_codec_ OCTK_ATTRIBUTE_GUARDED_BY(encoder_queue_);

    FrameDropper frame_dropper_ OCTK_ATTRIBUTE_GUARDED_BY(encoder_queue_);
    // If frame dropper is not force disabled, frame dropping might still be
    // disabled if VideoEncoder::GetEncoderInfo() indicates that the encoder has a
    // trusted rate controller. This is determined on a per-frame basis, as the
    // encoder behavior might dynamically change.
    bool force_disable_frame_dropper_ OCTK_ATTRIBUTE_GUARDED_BY(encoder_queue_) = false;
    // Incremented on worker thread whenever `frame_dropper_` determines that a
    // frame should be dropped. Decremented on whichever thread runs
    // OnEncodedImage(), which is only called by one thread but not necessarily
    // the worker thread.
    std::atomic<int> pending_frame_drops_{0};

    // Congestion window frame drop ratio (drop 1 in every
    // cwnd_frame_drop_interval_ frames).
    std::optional<int> cwnd_frame_drop_interval_ OCTK_ATTRIBUTE_GUARDED_BY(encoder_queue_);
    // Frame counter for congestion window frame drop.
    int cwnd_frame_counter_ OCTK_ATTRIBUTE_GUARDED_BY(encoder_queue_) = 0;

    std::unique_ptr<EncoderBitrateAdjuster> bitrate_adjuster_
        OCTK_ATTRIBUTE_GUARDED_BY(encoder_queue_);

    // TODO(sprang): Change actually support keyframe per simulcast stream, or
    // turn this into a simple bool `pending_keyframe_request_`.
    std::vector<VideoFrameType> next_frame_types_ OCTK_ATTRIBUTE_GUARDED_BY(encoder_queue_);

    FrameEncodeMetadataWriter frame_encode_metadata_writer_{this};

    // Provides video stream input states: current resolution and frame rate.
    VideoStreamInputStateProvider input_state_provider_;

    const std::unique_ptr<VideoStreamAdapter> video_stream_adapter_
        OCTK_ATTRIBUTE_GUARDED_BY(encoder_queue_);
    // Responsible for adapting input resolution or frame rate to ensure resources
    // (e.g. CPU or bandwidth) are not overused. Adding resources can occur on any
    // thread.
    std::unique_ptr<ResourceAdaptationProcessorInterface>
        resource_adaptation_processor_ OCTK_ATTRIBUTE_GUARDED_BY(encoder_queue_);
    std::unique_ptr<DegradationPreferenceManager> degradation_preference_manager_
        OCTK_ATTRIBUTE_GUARDED_BY(encoder_queue_);
    std::vector<AdaptationConstraint*> adaptation_constraints_
        OCTK_ATTRIBUTE_GUARDED_BY(encoder_queue_);
    // Handles input, output and stats reporting related to VideoStreamEncoder
    // specific resources, such as "encode usage percent" measurements and "QP
    // scaling". Also involved with various mitigations such as initial frame
    // dropping.
    // The manager primarily operates on the `encoder_queue_` but its lifetime is
    // tied to the VideoStreamEncoder (which is destroyed off the encoder queue)
    // and its resource list is accessible from any thread.
    VideoStreamEncoderResourceManager stream_resource_manager_
        OCTK_ATTRIBUTE_GUARDED_BY(encoder_queue_);
    std::vector<rtc::scoped_refptr<Resource>> additional_resources_
        OCTK_ATTRIBUTE_GUARDED_BY(encoder_queue_);
    // Carries out the VideoSourceRestrictions provided by the
    // ResourceAdaptationProcessor, i.e. reconfigures the source of video frames
    // to provide us with different resolution or frame rate.
    // This class is thread-safe.
    VideoSourceSinkController video_source_sink_controller_
        OCTK_ATTRIBUTE_GUARDED_BY(worker_queue_);

    // Default bitrate limits in EncoderInfoSettings allowed.
    const bool default_limits_allowed_;

    // QP parser is used to extract QP value from encoded frame when that is not
    // provided by encoder.
    QpParser qp_parser_;
    const bool qp_parsing_allowed_;

    // The quality convergence controller is used to determine if a codec has
    // reached its target quality. This is used for screenshare to determine when
    // there's no need to continue encoding the same repeated frame.
    QualityConvergenceController quality_convergence_controller_
        OCTK_ATTRIBUTE_GUARDED_BY(encoder_queue_);

    // Enables encoder switching on initialization failures.
    bool switch_encoder_on_init_failures_;

    const std::optional<int> vp9_low_tier_core_threshold_;
    const std::optional<int> experimental_encoder_thread_limit_;

    // This is a copy of restrictions (glorified max_pixel_count) set by
    // OnVideoSourceRestrictionsUpdated. It is used to scale down encoding
    // resolution if needed when using requested_resolution.
    //
    // TODO(webrtc:14451) Split video_source_sink_controller_
    // so that ownership on restrictions/wants is kept on &encoder_queue_, that
    // these extra copies would not be needed.
    std::optional<VideoSourceRestrictions> latest_restrictions_
        OCTK_ATTRIBUTE_GUARDED_BY(encoder_queue_);

    // Used to cancel any potentially pending tasks to the worker thread.
    // Refrenced by tasks running on `encoder_queue_` so need to be destroyed
    // after stopping that queue. Must be created and destroyed on
    // `worker_queue_`.
    ScopedTaskSafety task_safety_;

    std::unique_ptr<TaskQueueBase, TaskQueueDeleter> encoder_queue_;

    //  Required for automatic corruption detection.
    std::unique_ptr<FrameInstrumentationGenerator>
        frame_instrumentation_generator_;
};

OCTK_END_NAMESPACE

#endif // _OCTK_VIDEO_STREAM_ENCODER_HPP
