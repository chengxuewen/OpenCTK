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

#pragma once

#include <octk_encoded_image.hpp>
#include <octk_file_wrapper.hpp>
#include <octk_video_codec.hpp>
#include <octk_optional.hpp>

#include <memory>
#include <utility>

// #include "api/video/encoded_image.h"
// #include "api/video_codecs/video_codec.h"
// #include "rtc_base/system/file_wrapper.h"

OCTK_BEGIN_NAMESPACE

class IvfFileReader
{
public:
    // Creates IvfFileReader. Returns nullptr if error acquired.
    static std::unique_ptr<IvfFileReader> Create(FileWrapper file);
    ~IvfFileReader();

    IvfFileReader(const IvfFileReader &) = delete;
    IvfFileReader &operator=(const IvfFileReader &) = delete;

    // Reinitializes reader. Returns false if any error acquired.
    bool Reset();

    // Returns codec type which was used to create this IVF file and which should
    // be used to decode EncodedImages from this file.
    VideoCodecType GetVideoCodecType() const { return codec_type_; }
    // Returns count of frames in this file.
    size_t GetFramesCount() const { return num_frames_; }

    // Returns next frame or std::nullopt if any error acquired. Always returns
    // std::nullopt after first error was spotted.
    Optional<EncodedImage> NextFrame();
    bool HasMoreFrames() const { return num_read_frames_ < num_frames_; }
    bool HasError() const { return has_error_; }

    uint16_t GetFrameWidth() const { return width_; }
    uint16_t GetFrameHeight() const { return height_; }

    bool Close();

private:
    struct FrameHeader
    {
        size_t frame_size;
        int64_t timestamp;
    };

    explicit IvfFileReader(FileWrapper file)
        : file_(std::move(file))
    {
    }

    // Parses codec type from specified position of the buffer. Codec type
    // contains kCodecTypeBytesCount bytes and caller has to ensure that buffer
    // won't overflow.
    Optional<VideoCodecType> ParseCodecType(uint8_t *buffer, size_t start_pos);
    Optional<FrameHeader> ReadNextFrameHeader();

    VideoCodecType codec_type_;
    size_t num_frames_;
    size_t num_read_frames_;
    uint16_t width_;
    uint16_t height_;
    uint32_t time_scale_;
    FileWrapper file_;

    Optional<FrameHeader> next_frame_header_;
    bool has_error_;
};

OCTK_END_NAMESPACE