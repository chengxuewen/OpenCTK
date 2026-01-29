/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2026~Present ChengXueWen.
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

#include <octk_rtc_video_decoder.hpp>
#include <octk_rtc_video_encoder.hpp>
#include <octk_rtc_types.hpp>

OCTK_BEGIN_NAMESPACE

class RtcVideoCodecFactory
{
public:
    using SharedPtr = SharedPointer<RtcVideoCodecFactory>;

    virtual Vector<RtcSdpVideoFormat::SharedPtr> getSupportedEncoderFormats() const = 0;
    virtual Vector<RtcSdpVideoFormat::SharedPtr> getSupportedDecoderFormats() const = 0;

    virtual RtcVideoEncoder::SharedPtr createVideoEncoder(const RtcSdpVideoFormat::SharedPtr &format) = 0;
    virtual RtcVideoDecoder::SharedPtr createVideoDecoder(const RtcSdpVideoFormat::SharedPtr &format) = 0;

protected:
    virtual ~RtcVideoCodecFactory() = default;
};

OCTK_END_NAMESPACE