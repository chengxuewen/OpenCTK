/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
** Copyright (c) 2019 The WebRTC project authors. All Rights Reserved.
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

#include <private/octk_ivf_file_reader_p.hpp>
#include <private/octk_ivf_defines_p.hpp>
#include <octk_logging.hpp>

#include <string>
#include <vector>

//#include "modules/rtp_rtcp/source/byte_io.h"

OCTK_BEGIN_NAMESPACE

namespace
{

constexpr size_t kIvfFrameHeaderSize = 12;
constexpr int kCodecTypeBytesCount = 4;

constexpr uint8_t kFileHeaderStart[kCodecTypeBytesCount] = {'D', 'K', 'I', 'F'};
constexpr uint8_t kVp8Header[kCodecTypeBytesCount] = {'V', 'P', '8', '0'};
constexpr uint8_t kVp9Header[kCodecTypeBytesCount] = {'V', 'P', '9', '0'};
constexpr uint8_t kAv1Header[kCodecTypeBytesCount] = {'A', 'V', '0', '1'};
constexpr uint8_t kH264Header[kCodecTypeBytesCount] = {'H', '2', '6', '4'};
constexpr uint8_t kH265Header[kCodecTypeBytesCount] = {'H', '2', '6', '5'};

// RTP standard required 90kHz clock rate.
constexpr int32_t kRtpClockRateHz = 90000;

} // namespace

std::unique_ptr<IvfFileReader> IvfFileReader::Create(FileWrapper file)
{
    auto reader = std::unique_ptr<IvfFileReader>(new IvfFileReader(std::move(file)));
    if (!reader->Reset())
    {
        return nullptr;
    }
    return reader;
}
IvfFileReader::~IvfFileReader()
{
    Close();
}

bool IvfFileReader::Reset()
{
    // Set error to true while initialization.
    has_error_ = true;
    if (!file_.Rewind())
    {
        OCTK_ERROR() << "Failed to rewind IVF file";
        return false;
    }

    uint8_t ivf_header[kIvfHeaderSize] = {0};
    size_t read = file_.Read(&ivf_header, kIvfHeaderSize);
    if (read != kIvfHeaderSize)
    {
        OCTK_ERROR() << "Failed to read IVF header";
        return false;
    }

    if (memcmp(&ivf_header[0], kFileHeaderStart, 4) != 0)
    {
        OCTK_ERROR() << "File is not in IVF format: DKIF header expected";
        return false;
    }

    Optional<VideoCodecType> codec_type = ParseCodecType(ivf_header, 8);
    if (!codec_type)
    {
        return false;
    }
    codec_type_ = *codec_type;

    width_ = ByteReader<uint16_t>::ReadLittleEndian(&ivf_header[12]);
    height_ = ByteReader<uint16_t>::ReadLittleEndian(&ivf_header[14]);
    if (width_ == 0 || height_ == 0)
    {
        OCTK_ERROR() << "Invalid IVF header: width or height is 0";
        return false;
    }

    time_scale_ = ByteReader<uint32_t>::ReadLittleEndian(&ivf_header[16]);
    if (time_scale_ == 0)
    {
        OCTK_ERROR() << "Invalid IVF header: time scale can't be 0";
        return false;
    }

    num_frames_ = static_cast<size_t>(ByteReader<uint32_t>::ReadLittleEndian(&ivf_header[24]));
    if (num_frames_ <= 0)
    {
        OCTK_ERROR() << "Invalid IVF header: number of frames 0 or negative";
        return false;
    }

    num_read_frames_ = 0;
    next_frame_header_ = ReadNextFrameHeader();
    if (!next_frame_header_)
    {
        OCTK_ERROR() << "Failed to read 1st frame header";
        return false;
    }
    // Initialization succeed: reset error.
    has_error_ = false;

    const char *codec_name = CodecTypeToPayloadString(codec_type_);
    OCTK_INFO() << "Opened IVF file with codec data of type " << codec_name << " at resolution " << width_ << " x "
                << height_ << ", using " << time_scale_ << "Hz clock resolution.";

    return true;
}

Optional<EncodedImage> IvfFileReader::NextFrame()
{
    if (has_error_ || !HasMoreFrames())
    {
        return utils::nullopt;
    }

    auto payload = EncodedImageBuffer::Create();
    std::vector<size_t> layer_sizes;
    // next_frame_header_ have to be presented by the way how it was loaded. If it
    // is missing it means there is a bug in error handling.
    OCTK_DCHECK(next_frame_header_);
    int64_t current_timestamp = next_frame_header_->timestamp;
    // The first frame from the file should be marked as Key frame.
    bool is_first_frame = num_read_frames_ == 0;
    while (next_frame_header_ && current_timestamp == next_frame_header_->timestamp)
    {
        // Resize payload to fit next spatial layer.
        size_t current_layer_size = next_frame_header_->frame_size;
        size_t current_layer_start_pos = payload->size();
        payload->Realloc(payload->size() + current_layer_size);
        layer_sizes.push_back(current_layer_size);

        // Read next layer into payload
        size_t read = file_.Read(&payload->data()[current_layer_start_pos], current_layer_size);
        if (read != current_layer_size)
        {
            OCTK_ERROR() << "Frame #" << num_read_frames_ << ": failed to read frame payload";
            has_error_ = true;
            return utils::nullopt;
        }
        num_read_frames_++;

        current_timestamp = next_frame_header_->timestamp;
        next_frame_header_ = ReadNextFrameHeader();
    }
    if (!next_frame_header_)
    {
        // If EOF was reached, we need to check that all frames were met.
        if (!has_error_ && num_read_frames_ != num_frames_)
        {
            OCTK_ERROR() << "Unexpected EOF";
            has_error_ = true;
            return utils::nullopt;
        }
    }

    EncodedImage image;
    image.capture_time_ms_ = current_timestamp;
    image.setRtpTimestamp(static_cast<uint32_t>(current_timestamp * kRtpClockRateHz / time_scale_));
    image.setEncodedData(payload);
    image.setSpatialIndex(static_cast<int>(layer_sizes.size()) - 1);
    for (size_t i = 0; i < layer_sizes.size(); ++i)
    {
        image.setSpatialLayerFrameSize(static_cast<int>(i), layer_sizes[i]);
    }
    if (is_first_frame)
    {
        image._frameType = VideoFrameType::kKey;
    }

    return image;
}

bool IvfFileReader::Close()
{
    if (!file_.is_open())
        return false;

    file_.Close();
    return true;
}

Optional<VideoCodecType> IvfFileReader::ParseCodecType(uint8_t *buffer, size_t start_pos)
{
    if (memcmp(&buffer[start_pos], kVp8Header, kCodecTypeBytesCount) == 0)
    {
        return VideoCodecType::kVideoCodecVP8;
    }
    if (memcmp(&buffer[start_pos], kVp9Header, kCodecTypeBytesCount) == 0)
    {
        return VideoCodecType::kVideoCodecVP9;
    }
    if (memcmp(&buffer[start_pos], kAv1Header, kCodecTypeBytesCount) == 0)
    {
        return VideoCodecType::kVideoCodecAV1;
    }
    if (memcmp(&buffer[start_pos], kH264Header, kCodecTypeBytesCount) == 0)
    {
        return VideoCodecType::kVideoCodecH264;
    }
    if (memcmp(&buffer[start_pos], kH265Header, kCodecTypeBytesCount) == 0)
    {
        return VideoCodecType::kVideoCodecH265;
    }
    has_error_ = true;
    OCTK_ERROR() << "Unknown codec type: "
                 << std::string(reinterpret_cast<char const *>(&buffer[start_pos]), kCodecTypeBytesCount);
    return utils::nullopt;
}

Optional<IvfFileReader::FrameHeader> IvfFileReader::ReadNextFrameHeader()
{
    uint8_t ivf_frame_header[kIvfFrameHeaderSize] = {0};
    size_t read = file_.Read(&ivf_frame_header, kIvfFrameHeaderSize);
    if (read != kIvfFrameHeaderSize)
    {
        if (read != 0 || !file_.ReadEof())
        {
            has_error_ = true;
            OCTK_ERROR() << "Frame #" << num_read_frames_ << ": failed to read IVF frame header";
        }
        return utils::nullopt;
    }
    FrameHeader header;
    header.frame_size = static_cast<size_t>(ByteReader<uint32_t>::ReadLittleEndian(&ivf_frame_header[0]));
    header.timestamp = ByteReader<uint64_t>::ReadLittleEndian(&ivf_frame_header[4]);

    if (header.frame_size == 0)
    {
        has_error_ = true;
        OCTK_ERROR() << "Frame #" << num_read_frames_ << ": invalid frame size";
        return utils::nullopt;
    }

    if (header.timestamp < 0)
    {
        has_error_ = true;
        OCTK_ERROR() << "Frame #" << num_read_frames_ << ": negative timestamp";
        return utils::nullopt;
    }

    return header;
}

OCTK_END_NAMESPACE