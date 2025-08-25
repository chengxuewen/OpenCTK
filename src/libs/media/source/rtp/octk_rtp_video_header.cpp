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

#include <octk_rtp_video_header.hpp>

OCTK_BEGIN_NAMESPACE

RTPVideoHeader::GenericDescriptorInfo::GenericDescriptorInfo() = default;
RTPVideoHeader::GenericDescriptorInfo::GenericDescriptorInfo(const GenericDescriptorInfo &other) = default;
RTPVideoHeader::GenericDescriptorInfo::~GenericDescriptorInfo() = default;

// static
RTPVideoHeader RTPVideoHeader::FromMetadata(const VideoFrameMetadata &metadata)
{
    RTPVideoHeader rtp_video_header;
    rtp_video_header.SetFromMetadata(metadata);
    return rtp_video_header;
}

RTPVideoHeader::RTPVideoHeader() : video_timing() {}
RTPVideoHeader::RTPVideoHeader(const RTPVideoHeader &other) = default;
RTPVideoHeader::~RTPVideoHeader() = default;

VideoFrameMetadata RTPVideoHeader::GetAsMetadata() const
{
    VideoFrameMetadata metadata;
    metadata.SetFrameType(frame_type);
    metadata.SetWidth(width);
    metadata.SetHeight(height);
    metadata.SetRotation(rotation);
    metadata.SetContentType(content_type);
    if (generic)
    {
        metadata.SetFrameId(generic->frame_id);
        metadata.SetSpatialIndex(generic->spatial_index);
        metadata.SetTemporalIndex(generic->temporal_index);
        metadata.SetFrameDependencies(generic->dependencies);
        metadata.SetDecodeTargetIndications(generic->decode_target_indications);
    }
    metadata.SetIsLastFrameInPicture(is_last_frame_in_picture);
    metadata.SetSimulcastIdx(simulcastIdx);
    metadata.SetCodec(codec);
    switch (codec)
    {
        case VideoCodecType::kVideoCodecVP8:
            metadata.SetRTPVideoHeaderCodecSpecifics(octk::get<RTPVideoHeaderVP8>(video_type_header));
            break;
        case VideoCodecType::kVideoCodecVP9:
            metadata.SetRTPVideoHeaderCodecSpecifics(octk::get<RTPVideoHeaderVP9>(video_type_header));
            break;
        case VideoCodecType::kVideoCodecH264:
            metadata.SetRTPVideoHeaderCodecSpecifics(octk::get<RTPVideoHeaderH264>(video_type_header));
            break;
            // These codec types do not have codec-specifics.
        case VideoCodecType::kVideoCodecH265:
        case VideoCodecType::kVideoCodecAV1:
        case VideoCodecType::kVideoCodecGeneric:break;
    }
    return metadata;
}

void RTPVideoHeader::SetFromMetadata(const VideoFrameMetadata &metadata)
{
    frame_type = metadata.GetFrameType();
    width = metadata.GetWidth();
    height = metadata.GetHeight();
    rotation = metadata.GetRotation();
    content_type = metadata.GetContentType();
    if (!metadata.GetFrameId().has_value())
    {
        generic = utils::nullopt;
    }
    else
    {
        generic.emplace();
        generic->frame_id = metadata.GetFrameId().value();
        generic->spatial_index = metadata.GetSpatialIndex();
        generic->temporal_index = metadata.GetTemporalIndex();
        generic->dependencies.assign(metadata.GetFrameDependencies().begin(),
                                     metadata.GetFrameDependencies().end());
        generic->decode_target_indications.assign(metadata.GetDecodeTargetIndications().begin(),
                                                  metadata.GetDecodeTargetIndications().end());
    }
    is_last_frame_in_picture = metadata.GetIsLastFrameInPicture();
    simulcastIdx = metadata.GetSimulcastIdx();
    codec = metadata.GetCodec();
    switch (codec)
    {
        case VideoCodecType::kVideoCodecVP8:
            video_type_header = octk::get<RTPVideoHeaderVP8>(metadata.GetRTPVideoHeaderCodecSpecifics());
            break;
        case VideoCodecType::kVideoCodecVP9:
            video_type_header = octk::get<RTPVideoHeaderVP9>(metadata.GetRTPVideoHeaderCodecSpecifics());
            break;
        case VideoCodecType::kVideoCodecH264:
            video_type_header = octk::get<RTPVideoHeaderH264>(metadata.GetRTPVideoHeaderCodecSpecifics());
            break;
        default:
            // Codec-specifics are not supported for this codec.
            break;
    }
}

OCTK_END_NAMESPACE
