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

#include <octk_rtc_types.hpp>

OCTK_BEGIN_NAMESPACE

class RtcVideoDecoder
{
public:
    using SharedPtr = std::shared_ptr<RtcVideoDecoder>;

    struct Info
    {
        // Descriptive name of the decoder implementation.
        String implementationName;
        // True if the decoder is backed by hardware acceleration.
        bool isHardwareAccelerated{false};
    };

    struct Settings
    {
        int bufferPoolSize{-1};
        struct RenderResolution
        {
            int width{0};
            int height{0};
        } maxResolution;
        int numberOfCores{1};
        RtcVideoCodec::Type codecType{RtcVideoCodec::Type::kGeneric};
    };

    virtual int32_t release() = 0;

    virtual Info getDecoderInfo() = 0;

    /**
     * Prepares decoder to handle incoming encoded frames. Can be called multiple times,
     * in such case only latest `settings` are in effect.
     * @param settings
     * @return
     */
    virtual bool configure(const Settings &settings) = 0;

    virtual int32_t decode(const RtcEncodedImage::SharedPtr &inputImage, int64_t renderTimeMSecs) = 0;

    //int32_t RegisterDecodeCompleteCallback(webrtc::DecodedImageCallback *callback) override;

protected:
    virtual ~RtcVideoDecoder() { }
};

OCTK_END_NAMESPACE