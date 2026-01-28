/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
** Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
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

#include <octk_string_view.hpp>
#include <octk_encoded_image.hpp>
#include <octk_video_codec_type.hpp>
#include <octk_string_view.hpp>

#include <stddef.h>
#include <stdint.h>

#include <memory>

// #include "absl/strings/string_view.h"
// #include "api/video/encoded_image.h"
// #include "api/video/video_codec_type.h"
// #include "rtc_base/numerics/sequence_number_unwrapper.h"
// #include "rtc_base/system/file_wrapper.h"

OCTK_BEGIN_NAMESPACE

class IvfFileWriter
{
public:
    // Takes ownership of the file, which will be closed either through
    // Close or ~IvfFileWriter. If writing a frame would take the file above the
    // `byte_limit` the file will be closed, the write (and all future writes)
    // will fail. A `byte_limit` of 0 is equivalent to no limit.
    static std::unique_ptr<IvfFileWriter> Wrap(FileWrapper file, size_t byte_limit);
    static std::unique_ptr<IvfFileWriter> Wrap(absl::string_view filename, size_t byte_limit);
    ~IvfFileWriter();

    IvfFileWriter(const IvfFileWriter &) = delete;
    IvfFileWriter &operator=(const IvfFileWriter &) = delete;

    bool WriteFrame(const EncodedImage &encoded_image, VideoCodecType codec_type);
    bool Close();

private:
    explicit IvfFileWriter(FileWrapper file, size_t byte_limit);

    bool WriteHeader();
    bool InitFromFirstFrame(const EncodedImage &encoded_image, VideoCodecType codec_type);
    bool WriteOneSpatialLayer(int64_t timestamp, const uint8_t *data, size_t size);

    VideoCodecType codec_type_;
    size_t bytes_written_;
    size_t byte_limit_;
    size_t num_frames_;
    uint16_t width_;
    uint16_t height_;
    int64_t last_timestamp_;
    bool using_capture_timestamps_;
    RtpTimestampUnwrapper wrap_handler_;
    FileWrapper file_;
};

OCTK_END_NAMESPACE