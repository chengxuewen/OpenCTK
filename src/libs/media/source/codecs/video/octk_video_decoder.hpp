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

#pragma once

#include <octk_encoded_image.hpp>
#include <octk_video_frame.hpp>
#include <octk_size_base.hpp>

#include <cstdint>
#include <string>

//#include "api/video/encoded_image.h
//#include "api/video/render_resolution.h"

OCTK_BEGIN_NAMESPACE

class OCTK_MEDIA_API DecodedImageCallback
{
public:
    virtual ~DecodedImageCallback() { }

    virtual int32_t Decoded(VideoFrame &decodedImage) = 0;
    // Provides an alternative interface that allows the decoder to specify the
    // decode time excluding waiting time for any previous pending frame to
    // return. This is necessary for breaking positive feedback in the delay
    // estimation when the decoder has a single output buffer.
    virtual int32_t Decoded(VideoFrame &decodedImage, int64_t decode_time_ms);

    // TODO(sakal): Remove other implementations when upstream projects have been
    // updated.
    virtual void Decoded(VideoFrame &decodedImage, Optional<int32_t> decode_time_ms, Optional<uint8_t> qp);
};

class OCTK_MEDIA_API VideoDecoder
{
public:
    struct DecoderInfo
    {
        // Descriptive name of the decoder implementation.
        std::string implementation_name;

        // True if the decoder is backed by hardware acceleration.
        bool is_hardware_accelerated = false;

        std::string ToString() const;
        bool operator==(const DecoderInfo &rhs) const;
        bool operator!=(const DecoderInfo &rhs) const { return !(*this == rhs); }
    };

    class Settings
    {
    public:
        Settings() = default;
        Settings(const Settings &) = default;
        Settings &operator=(const Settings &) = default;
        ~Settings() = default;

        // The size of pool which is used to store video frame buffers inside
        // decoder. If value isn't present some codec-default value will be used. If
        // value is present and decoder doesn't have buffer pool the value will be
        // ignored.
        Optional<int> buffer_pool_size() const;
        void set_buffer_pool_size(Optional<int> value);

        // When valid, user of the VideoDecoder interface shouldn't `Decode`
        // encoded images with render resolution larger than width and height
        // specified here.
        Resolution max_render_resolution() const;
        void set_max_render_resolution(Resolution value);

        // Maximum number of cpu cores the decoder is allowed to use in parallel.
        // Must be positive.
        int number_of_cores() const { return number_of_cores_; }
        void set_number_of_cores(int value);

        // Codec of encoded images user of the VideoDecoder interface will `Decode`.
        VideoCodecType codec_type() const { return codec_type_; }
        void set_codec_type(VideoCodecType value) { codec_type_ = value; }

    private:
        Optional<int> buffer_pool_size_;
        Resolution max_resolution_;
        int number_of_cores_ = 1;
        VideoCodecType codec_type_ = kVideoCodecGeneric;
    };

    virtual ~VideoDecoder() = default;

    // Prepares decoder to handle incoming encoded frames. Can be called multiple
    // times, in such case only latest `settings` are in effect.
    virtual bool Configure(const Settings &settings) = 0;

    // TODO(bugs.webrtc.org/15444): Make pure virtual once all subclasses have
    // migrated to implementing this class.
    virtual int32_t Decode(const EncodedImage &input_image, int64_t render_time_ms)
    {
        return Decode(input_image, /*missing_frame=*/false, render_time_ms);
    }

    // TODO(bugs.webrtc.org/15444): Migrate all subclasses to Decode() without
    // missing_frame and delete this.
    virtual int32_t Decode(const EncodedImage &input_image, bool /* missing_frames */, int64_t render_time_ms)
    {
        return Decode(input_image, render_time_ms);
    }

    virtual int32_t RegisterDecodeCompleteCallback(DecodedImageCallback *callback) = 0;

    virtual int32_t Release() = 0;

    virtual DecoderInfo GetDecoderInfo() const;

    // Deprecated, use GetDecoderInfo().implementation_name instead.
    virtual const char *ImplementationName() const;
};

inline Optional<int> VideoDecoder::Settings::buffer_pool_size() const
{
    return buffer_pool_size_;
}

inline void VideoDecoder::Settings::set_buffer_pool_size(Optional<int> value)
{
    buffer_pool_size_ = value;
}

inline Resolution VideoDecoder::Settings::max_render_resolution() const
{
    return max_resolution_;
}

inline void VideoDecoder::Settings::set_max_render_resolution(Resolution value)
{
    max_resolution_ = value;
}

OCTK_END_NAMESPACE