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

#ifndef _OCTK_VIDEO_ENCODED_IMAGE_HPP
#define _OCTK_VIDEO_ENCODED_IMAGE_HPP

#include <octk_video_filter_settings.hpp>
#include <octk_video_codec_constants.hpp>
#include <octk_video_content_type.hpp>
#include <octk_video_codec_types.hpp>
#include <octk_video_frame_type.hpp>
#include <octk_rtp_packet_infos.hpp>
#include <octk_video_rotation.hpp>
#include <octk_video_timing.hpp>
#include <octk_color_space.hpp>
#include <octk_timestamp.hpp>
#include <octk_optional.hpp>
#include <octk_buffer.hpp>

#include <cstdint>
#include <cstddef>
#include <utility>
#include <map>

OCTK_BEGIN_NAMESPACE

// Abstract interface for buffer storage. Intended to support buffers owned by
// external encoders with special release requirements, e.g, java encoders with
// releaseOutputBuffer.
class EncodedImageBufferInterface
{
public:
    using value_type = uint8_t;

    virtual const uint8_t* data() const = 0;
    // TODO(bugs.webrtc.org/9378): Make interface essentially read-only, delete
    // this non-const data method.
    virtual uint8_t* data() = 0;
    virtual size_t size() const = 0;

    const uint8_t* begin() const { return data(); }
    const uint8_t* end() const { return data() + size(); }
};

// Basic implementation of EncodedImageBufferInterface.
class OCTK_MEDIA_API EncodedImageBuffer : public EncodedImageBufferInterface
{
public:
    explicit EncodedImageBuffer(size_t size);
    EncodedImageBuffer(const uint8_t* data, size_t size);
    explicit EncodedImageBuffer(Buffer buffer);

    static std::shared_ptr<EncodedImageBuffer> Create() { return Create(0); }
    static std::shared_ptr<EncodedImageBuffer> Create(size_t size);
    static std::shared_ptr<EncodedImageBuffer> Create(const uint8_t* data, size_t size);
    static std::shared_ptr<EncodedImageBuffer> Create(Buffer buffer);

    const uint8_t* data() const override;
    uint8_t* data() override;
    size_t size() const override;
    void Realloc(size_t t);

protected:
    Buffer buffer_;
};

// TODO(bug.webrtc.org/9378): This is a legacy api class, which is slowly being
// cleaned up. Direct use of its members is strongly discouraged.
class OCTK_MEDIA_API EncodedImage
{
public:
    EncodedImage();
    EncodedImage(EncodedImage&&);
    EncodedImage(const EncodedImage&);

    ~EncodedImage();

    EncodedImage& operator=(EncodedImage&&);
    EncodedImage& operator=(const EncodedImage&);

    // Frame capture time in RTP timestamp representation (90kHz).
    void setRtpTimestamp(uint32_t timestamp) { mRtpTimestamp = timestamp; }
    uint32_t rtpTimestamp() const { return mRtpTimestamp; }

    void setEncodeTime(int64_t encode_start_ms, int64_t encode_finish_ms);

    // Frame capture time in local time.
    Timestamp captureTime() const;

    // Frame capture time in ntp epoch time, i.e. time since 1st Jan 1900
    int64_t ntpTimeMs() const { return mNtpTimeMSecs; }

    // Every simulcast layer (= encoding) has its own encoder and RTP stream.
    // There can be no dependencies between different simulcast layers.
    Optional<int> simulcastIndex() const { return simulcast_index_; }
    void setSimulcastIndex(Optional<int> simulcast_index)
    {
        OCTK_DCHECK_GE(simulcast_index.value_or(0), 0);
        OCTK_DCHECK_LT(simulcast_index.value_or(0), kMaxSimulcastStreams);
        simulcast_index_ = simulcast_index;
    }

    const Optional<Timestamp>& presentationTimestamp() const { return mPresentationTimestamp; }
    void setPresentationTimestamp(const Optional<Timestamp>& presentation_timestamp)
    {
        mPresentationTimestamp = presentation_timestamp;
    }

    // Encoded images can have dependencies between spatial and/or temporal
    // layers, depending on the scalability mode used by the encoder. See diagrams
    // at https://w3c.github.io/webrtc-svc/#dependencydiagrams*.
    Optional<int> spatialIndex() const { return spatial_index_; }
    void setSpatialIndex(Optional<int> spatial_index)
    {
        OCTK_DCHECK_GE(spatial_index.value_or(0), 0);
        OCTK_DCHECK_LT(spatial_index.value_or(0), kMaxSpatialLayers);
        spatial_index_ = spatial_index;
    }

    Optional<int> temporalIndex() const { return temporal_index_; }
    void setTemporalIndex(Optional<int> temporal_index)
    {
        OCTK_DCHECK_GE(temporal_index_.value_or(0), 0);
        OCTK_DCHECK_LT(temporal_index_.value_or(0), kMaxTemporalStreams);
        temporal_index_ = temporal_index;
    }

    // These methods can be used to set/get size of subframe with spatial index
    // `spatial_index` on encoded frames that consist of multiple spatial layers.
    Optional<size_t> spatialLayerFrameSize(int spatial_index) const;
    void setSpatialLayerFrameSize(int spatial_index, size_t size_bytes);

    const ColorSpace* colorSpace() const { return mColorSpace ? &*mColorSpace : nullptr; }
    void setColorSpace(const Optional<octk::ColorSpace>& color_space) { mColorSpace = color_space; }

    Optional<VideoPlayoutDelay> playoutDelay() const { return playout_delay_; }

    void setPlayoutDelay(Optional<VideoPlayoutDelay> playout_delay) { playout_delay_ = playout_delay; }

    // These methods along with the private member video_frame_tracking_id_ are
    // meant for media quality testing purpose only.
    Optional<uint16_t> videoFrameTrackingId() const { return video_frame_tracking_id_; }
    void setVideoFrameTrackingId(Optional<uint16_t> tracking_id) { video_frame_tracking_id_ = tracking_id; }

    const RtpPacketInfos& packetInfos() const { return mPacketInfos; }
    void setPacketInfos(RtpPacketInfos packet_infos) { mPacketInfos = std::move(packet_infos); }

    bool retransmissionAllowed() const { return retransmission_allowed_; }
    void setRetransmissionAllowed(bool retransmission_allowed) { retransmission_allowed_ = retransmission_allowed; }

    size_t size() const { return size_; }
    void setSize(size_t new_size)
    {
        // Allow set_size(0) even if we have no buffer.
        OCTK_DCHECK_LE(new_size, new_size == 0 ? 0 : capacity());
        size_ = new_size;
    }

    void setEncodedData(std::shared_ptr<EncodedImageBufferInterface> encoded_data)
    {
        encoded_data_ = encoded_data;
        size_ = encoded_data->size();
    }

    void clearEncodedData()
    {
        encoded_data_ = nullptr;
        size_ = 0;
    }

    std::shared_ptr<EncodedImageBufferInterface> getEncodedData() const { return encoded_data_; }

    const uint8_t* data() const { return encoded_data_ ? encoded_data_->data() : nullptr; }

    const uint8_t* begin() const { return data(); }
    const uint8_t* end() const { return data() + size(); }

    // Returns whether the encoded image can be considered to be of target
    // quality.
    [[deprecated]] bool isAtTargetQuality() const { return at_target_quality_; }

    // Sets that the encoded image can be considered to be of target quality to
    // true or false.
    [[deprecated]] void setAtTargetQuality(bool at_target_quality) { at_target_quality_ = at_target_quality; }

    // Returns whether the frame that was encoded is a steady-state refresh frame
    // intended to improve the visual quality.
    bool isSteadyStateRefreshFrame() const { return is_steady_state_refresh_frame_; }

    void setIsSteadyStateRefreshFrame(bool refresh_frame) { is_steady_state_refresh_frame_ = refresh_frame; }

    VideoFrameType frameType() const { return _frameType; }

    void setFrameType(VideoFrameType frame_type) { _frameType = frame_type; }
    VideoContentType contentType() const { return content_type_; }
    VideoRotation rotation() const { return mRotation; }

    Optional<CorruptionDetectionFilterSettings> corruption_detection_filter_settings() const
    {
        return corruption_detection_filter_settings_;
    }
    void set_corruption_detection_filter_settings(const CorruptionDetectionFilterSettings& settings)
    {
        corruption_detection_filter_settings_ = settings;
    }

    uint32_t _encodedWidth = 0;
    uint32_t _encodedHeight = 0;
    // NTP time of the capture time in local timebase in milliseconds.
    // TODO(minyue): make this member private.
    int64_t mNtpTimeMSecs = 0;
    int64_t capture_time_ms_ = 0;
    VideoFrameType _frameType = VideoFrameType::Delta;
    VideoRotation mRotation = VideoRotation::Angle0;
    VideoContentType content_type_ = VideoContentType::Unspecified;
    int qp_ = -1; // Quantizer value.

    struct Timing
    {
        uint8_t flags = VideoSendTiming::kInvalid;
        int64_t encode_start_ms = 0;
        int64_t encode_finish_ms = 0;
        int64_t packetization_finish_ms = 0;
        int64_t pacer_exit_ms = 0;
        int64_t network_timestamp_ms = 0;
        int64_t network2_timestamp_ms = 0;
        int64_t receive_start_ms = 0;
        int64_t receive_finish_ms = 0;
    } timing_;
    EncodedImage::Timing video_timing() const { return timing_; }
    EncodedImage::Timing* video_timing_mutable() { return &timing_; }

private:
    size_t capacity() const { return encoded_data_ ? encoded_data_->size() : 0; }

    // When set, indicates that all future frames will be constrained with those
    // limits until the application indicates a change again.
    Optional<VideoPlayoutDelay> playout_delay_;

    std::shared_ptr<EncodedImageBufferInterface> encoded_data_;
    size_t size_ = 0; // Size of encoded frame data.
    uint32_t mRtpTimestamp = 0;
    Optional<int> simulcast_index_;
    Optional<Timestamp> mPresentationTimestamp;
    Optional<int> spatial_index_;
    Optional<int> temporal_index_;
    std::map<int, size_t> spatial_layer_frame_size_bytes_;
    Optional<octk::ColorSpace> mColorSpace;
    // This field is meant for media quality testing purpose only. When enabled it
    // carries the webrtc::VideoFrame id field from the sender to the receiver.
    Optional<uint16_t> video_frame_tracking_id_;
    // Information about packets used to assemble this video frame. This is needed
    // by `SourceTracker` when the frame is delivered to the RTCRtpReceiver's
    // MediaStreamTrack, in order to implement getContributingSources(). See:
    // https://w3c.github.io/webrtc-pc/#dom-rtcrtpreceiver-getcontributingsources
    RtpPacketInfos mPacketInfos;
    bool retransmission_allowed_ = true;
    // True if the encoded image can be considered to be of target quality.
    bool at_target_quality_ = false;
    // True if the frame that was encoded is a steady-state refresh frame intended
    // to improve the visual quality.
    bool is_steady_state_refresh_frame_ = false;

    // Filter settings for corruption detection suggested by the encoder
    // implementation, if any. Otherwise generic per-codec-type settings will be
    // used.
    Optional<CorruptionDetectionFilterSettings> corruption_detection_filter_settings_;
};

OCTK_END_NAMESPACE

#endif // _OCTK_VIDEO_ENCODED_IMAGE_HPP
