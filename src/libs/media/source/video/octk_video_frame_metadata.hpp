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

#ifndef _OCTK_VIDEO_FRAME_METADATA_HPP
#define _OCTK_VIDEO_FRAME_METADATA_HPP

#include <private/octk_dependency_descriptor_p.hpp>
#include <octk_video_content_type.hpp>
#include <octk_video_codec_types.hpp>
#include <octk_video_frame_type.hpp>
#include <octk_vp8_types.hpp>
#include <octk_vp9_types.hpp>
#include <octk_video_rotation.hpp>
#include <octk_array_view.hpp>
#include <octk_h264_types.hpp>
#include <octk_optional.hpp>
#include <octk_variant.hpp>

#include <vector>
#include <cstdint>

OCTK_BEGIN_NAMESPACE

using RTPVideoHeaderCodecSpecifics =
    Variant<VariantMonostate, RTPVideoHeaderVP8, RTPVideoHeaderVP9, RTPVideoHeaderH264>;

// A subset of metadata from the RTP video header, exposed in insertable streams API.
class OCTK_MEDIA_API VideoFrameMetadata
{
public:
    VideoFrameMetadata();
    VideoFrameMetadata(const VideoFrameMetadata &) = default;
    VideoFrameMetadata &operator=(const VideoFrameMetadata &) = default;

    VideoFrameType GetFrameType() const;
    void SetFrameType(VideoFrameType frame_type);

    uint16_t GetWidth() const;
    void SetWidth(uint16_t width);

    uint16_t GetHeight() const;
    void SetHeight(uint16_t height);

    VideoRotation GetRotation() const;
    void SetRotation(VideoRotation rotation);

    VideoContentType GetContentType() const;
    void SetContentType(VideoContentType content_type);

    Optional<int64_t> GetFrameId() const;
    void SetFrameId(Optional<int64_t> frame_id);

    int GetSpatialIndex() const;
    void SetSpatialIndex(int spatial_index);

    int GetTemporalIndex() const;
    void SetTemporalIndex(int temporal_index);

    ArrayView<const int64_t> GetFrameDependencies() const;
    void SetFrameDependencies(ArrayView<const int64_t> frame_dependencies);

    ArrayView<const DecodeTargetIndication> GetDecodeTargetIndications() const;
    void SetDecodeTargetIndications(ArrayView<const DecodeTargetIndication> decode_target_indications);

    bool GetIsLastFrameInPicture() const;
    void SetIsLastFrameInPicture(bool is_last_frame_in_picture);

    uint8_t GetSimulcastIdx() const;
    void SetSimulcastIdx(uint8_t simulcast_idx);

    VideoCodecType GetCodec() const;
    void SetCodec(VideoCodecType codec);

    // Which varient is used depends on the VideoCodecType from GetCodecs().
    const RTPVideoHeaderCodecSpecifics &GetRTPVideoHeaderCodecSpecifics() const;
    void SetRTPVideoHeaderCodecSpecifics(RTPVideoHeaderCodecSpecifics codec_specifics);

    uint32_t GetSsrc() const;
    void SetSsrc(uint32_t ssrc);
    std::vector<uint32_t> GetCsrcs() const;
    void SetCsrcs(std::vector<uint32_t> csrcs);

    OCTK_MEDIA_API friend bool operator==(const VideoFrameMetadata &lhs, const VideoFrameMetadata &rhs);
    OCTK_MEDIA_API friend bool operator!=(const VideoFrameMetadata &lhs, const VideoFrameMetadata &rhs);

private:
    VideoFrameType frame_type_ = VideoFrameType::kEmpty;
    int16_t width_ = 0;
    int16_t height_ = 0;
    VideoRotation mRotation = VideoRotation::kAngle0;
    VideoContentType content_type_ = VideoContentType::Unspecified;

    // Corresponding to GenericDescriptorInfo.
    Optional<int64_t> frame_id_;
    int spatial_index_ = 0;
    int temporal_index_ = 0;
    //    absl::InlinedVector<int64_t, 5> frame_dependencies_;
    //    absl::InlinedVector<DecodeTargetIndication, 10> decode_target_indications_;
    std::vector<int64_t> frame_dependencies_;
    std::vector<DecodeTargetIndication> decode_target_indications_;

    bool is_last_frame_in_picture_ = true;
    uint8_t simulcast_idx_ = 0;
    VideoCodecType codec_ = VideoCodecType::kVideoCodecGeneric;
    RTPVideoHeaderCodecSpecifics codec_specifics_;

    // RTP info.
    uint32_t ssrc_ = 0u;
    std::vector<uint32_t> csrcs_;
};
OCTK_END_NAMESPACE

#endif // _OCTK_VIDEO_FRAME_METADATA_HPP
