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

#include <octk_media_context_factory.hpp>
#include <octk_rtc_video_encoder.hpp>
#include <octk_memory.hpp>

#if OCTK_FEATURE_MEDIA_USE_H264
#    include <private/octk_video_encoder_openh264_p.hpp>
#endif

OCTK_BEGIN_NAMESPACE

#if OCTK_FEATURE_MEDIA_USE_H264
class RtcVideoEncoderOpenh264 : public RtcVideoEncoder
{
    //RtcVideoEncoderOpenh264()
public:
    using SharedPtr = SharedPointer<RtcVideoEncoderOpenh264>;
    ~RtcVideoEncoderOpenh264() override = default;

    int32_t release() override { return mOpenh264Encoder->release(); }

    Info getEncoderInfo() const override
    {
        Info result;
        const auto encoderInfo = mOpenh264Encoder->getEncoderInfo();
        result.implementationName = encoderInfo.implementationName;
        result.supportsSimulcast = encoderInfo.supportsSimulcast;
        result.supportsNativeHandle = encoderInfo.supportsNativeHandle;
        result.isHardwareAccelerated = encoderInfo.isHardwareAccelerated;
        result.preferredPixelFormats = {RtcVideoFrame::BufferType::kI420};
        result.scalingSettings.thresholds.low = encoderInfo.scalingSettings.thresholds.has_value()
                                                    ? encoderInfo.scalingSettings.thresholds.value().low
                                                    : -1;
        result.scalingSettings.thresholds.high = encoderInfo.scalingSettings.thresholds.has_value()
                                                     ? encoderInfo.scalingSettings.thresholds.value().high
                                                     : -1;
        result.scalingSettings.minPixelsPerFrame = encoderInfo.scalingSettings.minPixelsPerFrame;
        return result;
    }

    void setRates(const RateControlParameters &parameters) override { }

    int initEncode(const RtcVideoCodec &inst, const Settings &settings) override
    {
        // VideoCodec videoCodec;
        // videoCodec.return mOpenh264Encoder->initEncode();
        return -1;
    }

    int32_t encode(const RtcVideoFrame::SharedPtr &frame, const Vector<RtcVideoFrameType> &frameTypes) override
    {
        return -1;
    }

    void onRttUpdate(int64_t rttMSecs) override { OCTK_UNUSED(rttMSecs); }

    void onPacketLossRateUpdate(float packetLossRate) override { OCTK_UNUSED(packetLossRate); }

    void onLossNotification(const LossNotification &lossNotification) override { OCTK_UNUSED(lossNotification); }

protected:
    std::unique_ptr<VideoEncoder> mOpenh264Encoder{utils::make_unique<VideoEncoderOpenh264>(CreateMediaContext())};
};
#endif

OCTK_END_NAMESPACE