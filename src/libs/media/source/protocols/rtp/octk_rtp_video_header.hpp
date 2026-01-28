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

#ifndef _OCTK_RTP_SOURCE_RTP_VIDEO_HEADER_HPP
#define _OCTK_RTP_SOURCE_RTP_VIDEO_HEADER_HPP

#include <private/octk_dependency_descriptor_p.hpp>
#include <octk_frame_instrumentation_data.hpp>
#include <octk_video_frame_metadata.hpp>
#include <octk_video_content_type.hpp>
#include <octk_video_codec_types.hpp>
#include <octk_video_frame_type.hpp>
#include <octk_vp8_types.hpp>
#include <octk_vp9_types.hpp>
#include <octk_video_rotation.hpp>
#include <octk_video_timing.hpp>
#include <octk_rtp_headers.hpp>
#include <octk_color_space.hpp>
#include <octk_h264_types.hpp>
#include <octk_optional.hpp>
#include <octk_variant.hpp>

#include <cstdint>
#include <bitset>

OCTK_BEGIN_NAMESPACE
// Details passed in the rtp payload for legacy generic rtp packetizer.
// TODO(bugs.webrtc.org/9772): Deprecate in favor of passing generic video
// details in an rtp header extension.
struct RTPVideoHeaderLegacyGeneric
{
    uint16_t picture_id;
};

using RTPVideoTypeHeader = Variant<VariantMonostate,
                                   RTPVideoHeaderVP8,
                                   RTPVideoHeaderVP9,
                                   RTPVideoHeaderH264,
                                   RTPVideoHeaderLegacyGeneric>;

struct RTPVideoHeader
{
    struct GenericDescriptorInfo
    {
        GenericDescriptorInfo();
        GenericDescriptorInfo(const GenericDescriptorInfo &other);
        ~GenericDescriptorInfo();

        int64_t frame_id = 0;
        int spatial_index = 0;
        int temporal_index = 0;
        //        absl::InlinedVector<DecodeTargetIndication, 10> decode_target_indications;
        //        absl::InlinedVector<int64_t, 5> dependencies;
        //        absl::InlinedVector<int, 4> chain_diffs;
        std::vector<DecodeTargetIndication> decode_target_indications;
        std::vector<int64_t> dependencies;
        std::vector<int> chain_diffs;
        std::bitset<32> active_decode_targets = ~uint32_t{0};
    };

    static RTPVideoHeader FromMetadata(const VideoFrameMetadata &metadata);

    RTPVideoHeader();
    RTPVideoHeader(const RTPVideoHeader &other);

    ~RTPVideoHeader();

    // The subset of RTPVideoHeader that is exposed in the Insertable Streams API.
    VideoFrameMetadata GetAsMetadata() const;
    void SetFromMetadata(const VideoFrameMetadata &metadata);

    Optional<GenericDescriptorInfo> generic;

    VideoFrameType frame_type = VideoFrameType::kEmpty;
    uint16_t width = 0;
    uint16_t height = 0;
    VideoRotation rotation = VideoRotation::kAngle0;
    VideoContentType content_type = VideoContentType::Unspecified;
    bool is_first_packet_in_frame = false;
    bool is_last_packet_in_frame = false;
    bool is_last_frame_in_picture = true;
    uint8_t simulcastIdx = 0;
    VideoCodecType codec = VideoCodecType::kVideoCodecGeneric;

    Optional<VideoPlayoutDelay> playout_delay;
    VideoSendTiming video_timing;
    Optional<ColorSpace> colorSpace;
    // This field is meant for media quality testing purpose only. When enabled it
    // carries the webrtc::VideoFrame id field from the sender to the receiver.
    Optional<uint16_t> video_frame_tracking_id;
    RTPVideoTypeHeader video_type_header;

    // When provided, is sent as is as an RTP header extension according to
    // http://www.webrtc.org/experiments/rtp-hdrext/abs-capture-time.
    // Otherwise, it is derived from other relevant information.
    Optional<AbsoluteCaptureTime> absolute_capture_time;

    // Required for automatic corruption detection.
    Optional<Variant<FrameInstrumentationSyncData, FrameInstrumentationData>> frame_instrumentation_data;
};

OCTK_END_NAMESPACE

#endif  // _OCTK_RTP_SOURCE_RTP_VIDEO_HEADER_HPP
