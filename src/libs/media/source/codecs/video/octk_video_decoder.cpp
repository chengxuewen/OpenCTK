/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
** Copyright (c) 2014 The WebRTC project authors. All Rights Reserved.
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

#include <octk_video_decoder.hpp>
#include <octk_string_builder.hpp>
#include <octk_optional.hpp>
#include <octk_checks.hpp>

#include <cstdint>
#include <string>

OCTK_BEGIN_NAMESPACE

int32_t DecodedImageCallback::Decoded(VideoFrame &decodedImage, int64_t /* decode_time_ms */)
{
    // The default implementation ignores custom decode time value.
    return Decoded(decodedImage);
}

void DecodedImageCallback::Decoded(VideoFrame &decodedImage,
                                   Optional<int32_t> decode_time_ms,
                                   Optional<uint8_t> /* qp */)
{
    Decoded(decodedImage, decode_time_ms.value_or(-1));
}

VideoDecoder::DecoderInfo VideoDecoder::GetDecoderInfo() const
{
    DecoderInfo info;
    info.implementation_name = ImplementationName();
    return info;
}

const char *VideoDecoder::ImplementationName() const
{
    return "unknown";
}

std::string VideoDecoder::DecoderInfo::ToString() const
{
    char string_buf[2048];
    SimpleStringBuilder oss(string_buf);

    oss << "DecoderInfo { "
        << "prefers_late_decoding = "
        << "implementation_name = '" << implementation_name << "', "
        << "is_hardware_accelerated = " << (is_hardware_accelerated ? "true" : "false") << " }";
    return oss.str();
}

bool VideoDecoder::DecoderInfo::operator==(const DecoderInfo &rhs) const
{
    return is_hardware_accelerated == rhs.is_hardware_accelerated && implementation_name == rhs.implementation_name;
}

void VideoDecoder::Settings::set_number_of_cores(int value)
{
    OCTK_DCHECK_GT(value, 0);
    number_of_cores_ = value;
}

OCTK_END_NAMESPACE
