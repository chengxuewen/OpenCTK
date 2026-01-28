/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
** Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
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

#include <octk_media_context.hpp>
#include <octk_video_decoder.hpp>
#include <octk_sdp_video_format.hpp>

#include <memory>
#include <vector>

OCTK_BEGIN_NAMESPACE

// A factory that creates VideoDecoders.
// NOTE: This class is still under development and may change without notice.
class OCTK_MEDIA_API VideoDecoderFactory
{
public:
    struct CodecSupport
    {
        bool is_supported = false;
        bool is_power_efficient = false;
    };

    virtual ~VideoDecoderFactory() = default;

    // Returns a list of supported video formats in order of preference, to use
    // for signaling etc.
    virtual std::vector<SdpVideoFormat> GetSupportedFormats() const = 0;

    // Query whether the specifed format is supported or not and if it will be
    // power efficient, which is currently interpreted as if there is support for
    // hardware acceleration.
    // The parameter `reference_scaling` is used to query support for prediction
    // across spatial layers. An example where support for reference scaling is
    // needed is if the video stream is produced with a scalability mode that has
    // a dependency between the spatial layers. See
    // https://w3c.github.io/webrtc-svc/#scalabilitymodes* for a specification of
    // different scalabilty modes. NOTE: QueryCodecSupport is currently an
    // experimental feature that is subject to change without notice.
    virtual CodecSupport QueryCodecSupport(const SdpVideoFormat &format, bool reference_scaling) const
    {
        // Default implementation, query for supported formats and check if the
        // specified format is supported. Returns false if `reference_scaling` is
        // true.
        return {!reference_scaling && format.IsCodecInList(GetSupportedFormats()), false};
    }

    // Creates a VideoDecoder for the specified `format`.
    virtual std::unique_ptr<VideoDecoder> Create(const MediaContext &env, const SdpVideoFormat &format) = 0;
};

OCTK_END_NAMESPACE