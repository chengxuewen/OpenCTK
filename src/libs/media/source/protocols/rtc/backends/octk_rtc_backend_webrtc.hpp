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

#include <octk_rtc_video_codec_factory.hpp>
#include <octk_rtc_video_frame.hpp>

#include <api/video_codecs/video_encoder.h>
#include <api/video_codecs/video_encoder_factory.h>

OCTK_BEGIN_NAMESPACE

namespace utils
{
static RtcVideoCodec::Type fromWebRTC(const webrtc::VideoCodecType &type)
{
    switch (type)
    {
        case webrtc::VideoCodecType::kVideoCodecH265: return RtcVideoCodec::Type::kH265;
        case webrtc::VideoCodecType::kVideoCodecH264: return RtcVideoCodec::Type::kH264;
        case webrtc::VideoCodecType::kVideoCodecVP8: return RtcVideoCodec::Type::kVP8;
        case webrtc::VideoCodecType::kVideoCodecVP9: return RtcVideoCodec::Type::kVP9;
        case webrtc::VideoCodecType::kVideoCodecAV1: return RtcVideoCodec::Type::kAV1;
        default: break;
    }
    return RtcVideoCodec::Type::kGeneric;
}
static RtcVideoCodec::Mode fromWebRTC(const webrtc::VideoCodecMode &mode)
{
    switch (mode)
    {
        case webrtc::VideoCodecMode::kRealtimeVideo: return RtcVideoCodec::Mode::kRealtimeVideo;
        case webrtc::VideoCodecMode::kScreensharing: return RtcVideoCodec::Mode::kScreenSharing;
        default: break;
    }
    return RtcVideoCodec::Mode::kRealtimeVideo;
}
static RtcVideoCodec::H264 fromWebRTC(const webrtc::VideoCodecH264 &h264)
{
    RtcVideoCodec::H264 dst;
    dst.keyFrameInterval = h264.keyFrameInterval;
    dst.numberOfTemporalLayers = h264.numberOfTemporalLayers;
    return dst;
}
static RtcVideoCodec::VP8 fromWebRTC(const webrtc::VideoCodecVP8 &vp8)
{
    RtcVideoCodec::VP8 dst;
    dst.denoisingOn = vp8.denoisingOn;
    dst.automaticResizeOn = vp8.automaticResizeOn;
    dst.keyFrameInterval = vp8.keyFrameInterval;
    dst.numberOfTemporalLayers = vp8.numberOfTemporalLayers;
    return dst;
}
static RtcVideoCodec::VP9::InterLayerPredMode fromWebRTC(const webrtc::InterLayerPredMode &mode)
{
    switch (mode)
    {
        case webrtc::InterLayerPredMode::kOn: return RtcVideoCodec::VP9::InterLayerPredMode::kOn;
        case webrtc::InterLayerPredMode::kOff: return RtcVideoCodec::VP9::InterLayerPredMode::kOff;
        case webrtc::InterLayerPredMode::kOnKeyPic: return RtcVideoCodec::VP9::InterLayerPredMode::kOnKeyPic;
    }
    return RtcVideoCodec::VP9::InterLayerPredMode::kOn;
}
static RtcVideoCodec::VP9 fromWebRTC(const webrtc::VideoCodecVP9 &vp9)
{
    RtcVideoCodec::VP9 dst;
    dst.denoisingOn = vp9.denoisingOn;
    dst.flexibleMode = vp9.flexibleMode;
    dst.adaptiveQpMode = vp9.adaptiveQpMode;
    dst.automaticResizeOn = vp9.automaticResizeOn;
    dst.keyFrameInterval = vp9.keyFrameInterval;
    dst.numberOfSpatialLayers = vp9.numberOfSpatialLayers;
    dst.numberOfTemporalLayers = vp9.numberOfTemporalLayers;
    dst.interLayerPred = fromWebRTC(vp9.interLayerPred);
    return dst;
}
static RtcVideoCodec::AV1 fromWebRTC(const webrtc::VideoCodecAV1 &av1)
{
    RtcVideoCodec::AV1 dst;
    dst.automaticResizeOn = av1.automatic_resize_on;
    return dst;
}
static RtcVideoCodec fromWebRTC(const webrtc::VideoCodec &videoCodec)
{
    RtcVideoCodec dst;
    dst.type = fromWebRTC(videoCodec.codecType);
    dst.mode = fromWebRTC(videoCodec.mode);
    dst.width = videoCodec.width;
    dst.height = videoCodec.height;
    dst.maxFramerate = videoCodec.maxFramerate;
    dst.maxBitrate = videoCodec.maxBitrate;
    dst.minBitrate = videoCodec.minBitrate;
    dst.startBitrate = videoCodec.startBitrate;
    dst.frameDropEnabled = videoCodec.GetFrameDropEnabled();
    dst.h264 = fromWebRTC(videoCodec.H264());
    dst.vp8 = fromWebRTC(videoCodec.VP8());
    dst.vp9 = fromWebRTC(videoCodec.VP9());
    dst.av1 = fromWebRTC(videoCodec.AV1());
    return dst;
}
static RtcVideoEncoder::Settings fromWebRTC(const webrtc::VideoEncoder::Settings &settings)
{
    RtcVideoEncoder::Settings dst;
    dst.numberOfCores = settings.number_of_cores;
    dst.maxPayloadSize = settings.max_payload_size;
    dst.encoderThreadLimit = settings.encoder_thread_limit.has_value() ? settings.encoder_thread_limit.value() : -1;
    dst.lossNotification = settings.capabilities.loss_notification;
    return dst;
}
static RtcVideoFrameType fromWebRTC(const webrtc::VideoFrameType &frame_type)
{
    switch (frame_type)
    {
        case webrtc::VideoFrameType::kEmptyFrame: return RtcVideoFrameType::kEmpty;
        case webrtc::VideoFrameType::kVideoFrameKey: return RtcVideoFrameType::kKey;
        case webrtc::VideoFrameType::kVideoFrameDelta: return RtcVideoFrameType::kDelta;
    }
    return RtcVideoFrameType::kEmpty;
}
static Vector<RtcVideoFrameType> fromWebRTC(const std::vector<webrtc::VideoFrameType> *frame_types)
{
    std::vector<RtcVideoFrameType> dst;
    if (frame_types)
    {
        for (const auto &type : *frame_types)
        {
            dst.push_back(fromWebRTC(type));
        }
    }
    return dst;
}
static RtcVideoEncoder::LossNotification fromWebRTC(const webrtc::VideoEncoder::LossNotification &loss_notification)
{
    RtcVideoEncoder::LossNotification dst;
    dst.timestampOfLastDecodable = loss_notification.timestamp_of_last_decodable;
    dst.timestampOfLastReceived = loss_notification.timestamp_of_last_received;
    dst.dependenciesOfLastReceivedDecodable = loss_notification.dependencies_of_last_received_decodable.has_value()
                                                  ? loss_notification.dependencies_of_last_received_decodable.value()
                                                  : -1;
    dst.lastReceivedDecodable = loss_notification.last_received_decodable.has_value()
                                    ? loss_notification.last_received_decodable.value()
                                    : -1;
    return dst;
}
static RtcVideoEncoder::EncodedImageCallback::Result fromWebRTC(const webrtc::EncodedImageCallback::Result &result)
{
    RtcVideoEncoder::EncodedImageCallback::Result dst;
    dst.errored = result.error == webrtc::EncodedImageCallback::Result::OK;
    dst.dropNextFrame = result.drop_next_frame;
    dst.frameId = result.frame_id;
    return dst;
}

// static webrtc::SdpVideoFormat toWebRTC(const RtcSdpVideoFormat::SharedPtr &format)
// {
//     webrtc::SdpVideoFormat dst(format->name().data(), );
//     webrtc::CreateH264Format(webrtc::H264Profile::kProfileBaseline, webrtc::H264Level::kLevel3_1, "1"), return dst;
// }
static std::vector<webrtc::SdpVideoFormat> toWebRTC(const Vector<RtcSdpVideoFormat::SharedPtr> &formats)
{
    std::vector<webrtc::SdpVideoFormat> dst;
    for (size_t i = 0; i < formats.size(); ++i)
    {
        // dst.push_back(toWebRTC(formats[i]));
    }
    return dst;
}
static webrtc::VideoCodecType toWebRTC(RtcVideoCodec::Type type)
{
    switch (type)
    {
        case RtcVideoCodec::Type::kGeneric: return webrtc::kVideoCodecGeneric;
        case RtcVideoCodec::Type::kH264: return webrtc::kVideoCodecH264;
        case RtcVideoCodec::Type::kH265: return webrtc::kVideoCodecH265;
        case RtcVideoCodec::Type::kVP8: return webrtc::kVideoCodecVP8;
        case RtcVideoCodec::Type::kVP9: return webrtc::kVideoCodecVP9;
        case RtcVideoCodec::Type::kAV1: return webrtc::kVideoCodecAV1;
    }
    return webrtc::kVideoCodecGeneric;
}
static webrtc::H264PacketizationMode toWebRTC(RtcH264PacketizationMode mode)
{
    switch (mode)
    {
        case RtcH264PacketizationMode::kSingleNalUnit: return webrtc::H264PacketizationMode::SingleNalUnit;
        case RtcH264PacketizationMode::kNonInterleaved: return webrtc::H264PacketizationMode::NonInterleaved;
    }
    return webrtc::H264PacketizationMode::SingleNalUnit;
}
static void toWebRTC(const RtcCodecSpecificInfo &codecSpecificInfo, webrtc::CodecSpecificInfo &dst)
{
    dst.codecType = toWebRTC(codecSpecificInfo.codecType);
    dst.end_of_picture = codecSpecificInfo.endOfPicture;
    dst.codecSpecific.H264.packetization_mode = toWebRTC(codecSpecificInfo.codecSpecific.h264.packetizationMode);
    dst.codecSpecific.H264.temporal_idx = codecSpecificInfo.codecSpecific.h264.temporalIndex;
    dst.codecSpecific.H264.base_layer_sync = codecSpecificInfo.codecSpecific.h264.baseLayerSync;
    dst.codecSpecific.H264.idr_frame = codecSpecificInfo.codecSpecific.h264.idrFrame;
    dst.codecSpecific.VP8.nonReference = codecSpecificInfo.codecSpecific.vp8.nonReference;
    dst.codecSpecific.VP8.temporalIdx = codecSpecificInfo.codecSpecific.vp8.temporalIdx;
    dst.codecSpecific.VP8.layerSync = codecSpecificInfo.codecSpecific.vp8.layerSync;
    dst.codecSpecific.VP8.keyIdx = codecSpecificInfo.codecSpecific.vp8.keyIdx;
    dst.codecSpecific.VP8.useExplicitDependencies = codecSpecificInfo.codecSpecific.vp8.useExplicitDependencies;
    dst.codecSpecific.VP8.referencedBuffersCount = codecSpecificInfo.codecSpecific.vp8.referencedBuffersCount;
    dst.codecSpecific.VP8.updatedBuffersCount = codecSpecificInfo.codecSpecific.vp8.updatedBuffersCount;
    for (size_t i = 0; i < dst.codecSpecific.VP8.kBuffersCount; ++i)
    {
        dst.codecSpecific.VP8.referencedBuffers[i] = codecSpecificInfo.codecSpecific.vp8.referencedBuffers[i];
        dst.codecSpecific.VP8.updatedBuffers[i] = codecSpecificInfo.codecSpecific.vp8.updatedBuffers[i];
    }
}
static webrtc::EncodedImageCallback::DropReason toWebRTC(RtcVideoEncoder::EncodedImageCallback::DropReason dropReason)
{
    webrtc::EncodedImageCallback::DropReason dst;
    switch (dropReason)
    {
        case RtcVideoEncoder::EncodedImageCallback::DropReason::kDroppedByEncoder:
            return webrtc::EncodedImageCallback::DropReason::kDroppedByEncoder;
        case RtcVideoEncoder::EncodedImageCallback::DropReason::kDroppedByMediaOptimizations:
            return webrtc::EncodedImageCallback::DropReason::kDroppedByMediaOptimizations;
    }
    return webrtc::EncodedImageCallback::DropReason::kDroppedByMediaOptimizations;
}
} // namespace utils

class RtcEncodedImageFromWebRTC : public RtcEncodedImage
{
    RtcEncodedImageFromWebRTC(const webrtc::EncodedImage &image)
        : mWebRTCEncodedImage(image)
    {
    }

public:
    using SharedPtr = SharedPointer<RtcEncodedImageFromWebRTC>;
    static SharedPtr create(const webrtc::EncodedImage &image = {})
    {
        return SharedPtr(new RtcEncodedImageFromWebRTC(image), [](RtcEncodedImageFromWebRTC *p) { delete p; });
    }
    ~RtcEncodedImageFromWebRTC() override = default;

    size_t size() const override { return mWebRTCEncodedImage.size(); }

    const uint8_t *data() const override { return mWebRTCEncodedImage.data(); }

    webrtc::EncodedImage &encodedImage() { return mWebRTCEncodedImage; }
    const webrtc::EncodedImage &encodedImage() const { return mWebRTCEncodedImage; }

protected:
    webrtc::EncodedImage mWebRTCEncodedImage;
};

class RtcSdpVideoFormatFromWebRTC : public RtcSdpVideoFormat
{
    RtcSdpVideoFormatFromWebRTC(const webrtc::SdpVideoFormat &format)
        : mWebRTCSdpVideoFormat(format)
    {
    }

public:
    static SharedPtr create(const webrtc::SdpVideoFormat &format)
    {
        return SharedPtr(new RtcSdpVideoFormatFromWebRTC(format), [](RtcSdpVideoFormatFromWebRTC *p) { delete p; });
    }
    ~RtcSdpVideoFormatFromWebRTC() override = default;

    StringView name() const override { return mWebRTCSdpVideoFormat.name; }
    void setName(StringView name) override { mWebRTCSdpVideoFormat.name = name.data(); }

    RtcCodecParameterMap parameters() const override { return {}; }
    void setParameters(const RtcCodecParameterMap &parameters) override { }

    Vector<uint8_t> scalabilityModes() const override { return {}; }
    void setScalabilityModes(const Vector<uint8_t> &scalabilityModes) override { }

    String toString() const override { return mWebRTCSdpVideoFormat.ToString(); }
    bool isSameCodec(const SharedPtr &other) const override
    {
        const auto impl = std::dynamic_pointer_cast<RtcSdpVideoFormatFromWebRTC>(other);
        if (impl)
        {
            return mWebRTCSdpVideoFormat == impl->mWebRTCSdpVideoFormat;
        }
        return false;
    }

protected:
    webrtc::SdpVideoFormat mWebRTCSdpVideoFormat;
};

class RtcVideoBitrateAllocationFromWebRTC : public RtcVideoBitrateAllocation
{
    explicit RtcVideoBitrateAllocationFromWebRTC(const webrtc::VideoBitrateAllocation &allocation)
        : mWebRTCAllocation(allocation)
    {
    }
    ~RtcVideoBitrateAllocationFromWebRTC() override { }

public:
    static SharedPtr create(const webrtc::VideoBitrateAllocation &allocation)
    {
        return SharedPtr(new RtcVideoBitrateAllocationFromWebRTC(allocation),
                         [](RtcVideoBitrateAllocationFromWebRTC *p) { delete p; });
    }

    uint32_t getBitrate(size_t spatial_index, size_t temporal_index) const override
    {
        return mWebRTCAllocation.GetBitrate(spatial_index, temporal_index);
    }

    uint32_t getSpatialLayerSum(size_t spatial_index) const override
    {
        return mWebRTCAllocation.GetSpatialLayerSum(spatial_index);
    }

    bool isSpatialLayerUsed(size_t spatial_index) const override
    {
        return mWebRTCAllocation.IsSpatialLayerUsed(spatial_index);
    }

    uint32_t getTemporalLayerSum(size_t spatial_index, size_t temporal_index) const override
    {
        return mWebRTCAllocation.GetTemporalLayerSum(spatial_index, temporal_index);
    }

    Vector<uint32_t> getTemporalLayerAllocation(size_t spatial_index) const override
    {
        return mWebRTCAllocation.GetTemporalLayerAllocation(spatial_index);
    }

    uint32_t getSumBps() const override { return mWebRTCAllocation.get_sum_bps(); }

protected:
    webrtc::VideoBitrateAllocation mWebRTCAllocation;
};

class RtcVideoFrameFromWebRTC : public RtcVideoFrame
{
    RtcVideoFrameFromWebRTC(rtc::scoped_refptr<webrtc::VideoFrameBuffer> buffer)
        : mWebRTCVideoFrameBuffer(buffer)
    {
    }
    ~RtcVideoFrameFromWebRTC() override { }

public:
    static SharedPtr create(const webrtc::VideoFrame &frame)
    {
        return SharedPtr(new RtcVideoFrameFromWebRTC(frame.video_frame_buffer()),
                         [](RtcVideoFrameFromWebRTC *p) { delete p; });
    }

    SharedPtr copy() override
    {
        return SharedPtr(new RtcVideoFrameFromWebRTC(mWebRTCVideoFrameBuffer),
                         [](RtcVideoFrameFromWebRTC *p) { delete p; });
    }

    int width() const override { return mWebRTCVideoFrameBuffer->width(); }
    int height() const override { return mWebRTCVideoFrameBuffer->height(); }

    // Returns pointer to the pixel data for a given plane. The memory is owned by
    // the VideoFrameBuffer object and must not be freed by the caller.
    const uint8_t *dataY() const override { return mWebRTCVideoFrameBuffer->GetI420()->DataY(); }
    const uint8_t *dataU() const override { return mWebRTCVideoFrameBuffer->GetI420()->DataU(); }
    const uint8_t *dataV() const override { return mWebRTCVideoFrameBuffer->GetI420()->DataV(); }

    // Returns the number of bytes between successive rows for a given plane.
    int strideY() const override { return mWebRTCVideoFrameBuffer->GetI420()->StrideY(); }
    int strideU() const override { return mWebRTCVideoFrameBuffer->GetI420()->StrideU(); }
    int strideV() const override { return mWebRTCVideoFrameBuffer->GetI420()->StrideV(); }

    // int convertToARGB(BufferType type, uint8_t *dstArgb, int dstStrideArgb, int dstWidth, int dstHeight) override
    // {
    //
    // }

private:
    rtc::scoped_refptr<webrtc::VideoFrameBuffer> mWebRTCVideoFrameBuffer;
};

class RtcEncodedImageCallbackFromWebRTC : public RtcVideoEncoder::EncodedImageCallback
{
    explicit RtcEncodedImageCallbackFromWebRTC(webrtc::EncodedImageCallback *callback)
        : mWebRTCEncodedImageCallback(callback)
    {
    }

public:
    static SharedPtr create(webrtc::EncodedImageCallback *callback)
    {
        return SharedPtr(new RtcEncodedImageCallbackFromWebRTC(callback),
                         [](RtcEncodedImageCallbackFromWebRTC *p) { delete p; });
    }
    ~RtcEncodedImageCallbackFromWebRTC() override = default;

    Result onEncodedImage(const RtcEncodedImage &encodedImage, const RtcCodecSpecificInfo &codecSpecificInfo) override
    {
        if (mWebRTCEncodedImageCallback)
        {
            utils::toWebRTC(codecSpecificInfo, mWebRTCCodecSpecificInfo);
            const auto result = mWebRTCEncodedImageCallback->OnEncodedImage(mRtcEncodedImage->encodedImage(),
                                                                            &mWebRTCCodecSpecificInfo);
            return utils::fromWebRTC(result);
        }
        return {};
    }

    void onDroppedFrame(DropReason reason) override
    {
        if (mWebRTCEncodedImageCallback)
        {
            mWebRTCEncodedImageCallback->OnDroppedFrame(utils::toWebRTC(reason));
        }
    }

protected:
    webrtc::CodecSpecificInfo mWebRTCCodecSpecificInfo;
    webrtc::EncodedImageCallback *mWebRTCEncodedImageCallback{nullptr};
    RtcEncodedImageFromWebRTC::SharedPtr mRtcEncodedImage{RtcEncodedImageFromWebRTC::create()};
};

class RtcVideoEncoderToWebRTC : public webrtc::VideoEncoder
{
    RtcVideoEncoderToWebRTC(const RtcVideoEncoder::SharedPtr &encoder)
        : mRtcVideoEncoder(encoder)
    {
    }

public:
    static std::unique_ptr<webrtc::VideoEncoder> create(const RtcVideoEncoder::SharedPtr &encoder)
    {
        return std::unique_ptr<RtcVideoEncoderToWebRTC>(new RtcVideoEncoderToWebRTC(encoder));
    }
    ~RtcVideoEncoderToWebRTC() override = default;

    int32_t Release() override { return mRtcVideoEncoder->release(); }

    EncoderInfo GetEncoderInfo() const override
    {
        EncoderInfo encoderInfo;
        auto implInfo = mRtcVideoEncoder->getEncoderInfo();
        encoderInfo.supports_simulcast = implInfo.supportsSimulcast;
        encoderInfo.supports_native_handle = implInfo.supportsNativeHandle;
        encoderInfo.implementation_name = implInfo.implementationName.c_str();
        encoderInfo.is_hardware_accelerated = implInfo.isHardwareAccelerated;
        encoderInfo.scaling_settings = {implInfo.scalingSettings.thresholds.low,
                                        implInfo.scalingSettings.thresholds.high,
                                        implInfo.scalingSettings.minPixelsPerFrame};
        for (size_t i = 0; i < implInfo.preferredPixelFormats.size(); ++i)
        {
            const auto &format = implInfo.preferredPixelFormats[i];
            switch (format)
            {
                case RtcVideoFrame::BufferType::kI420:
                {
                    encoderInfo.preferred_pixel_formats[i] = webrtc::VideoFrameBuffer::Type::kI420;
                    break;
                }
                case RtcVideoFrame::BufferType::kNV12:
                {
                    encoderInfo.preferred_pixel_formats[i] = webrtc::VideoFrameBuffer::Type::kNV12;
                    break;
                }
                default: break;
            }
        }
        return encoderInfo;
    }

    void SetRates(const RateControlParameters &parameters) override
    {
        RtcVideoEncoder::RateControlParameters implParameters;
        implParameters.framerateFps = parameters.framerate_fps;
        implParameters.bitrate = RtcVideoBitrateAllocationFromWebRTC::create(parameters.bitrate);
        implParameters.targetBitrate = RtcVideoBitrateAllocationFromWebRTC::create(parameters.target_bitrate);
        mRtcVideoEncoder->setRates(implParameters);
    }

    int InitEncode(const webrtc::VideoCodec *codec_settings, const Settings &settings) override
    {
        const auto ret = mRtcVideoEncoder->initEncode(utils::fromWebRTC(*codec_settings), utils::fromWebRTC(settings));
        return ret >= 0 ? WEBRTC_VIDEO_CODEC_OK : WEBRTC_VIDEO_CODEC_ERROR;
    }

    int32_t Encode(const webrtc::VideoFrame &frame, const std::vector<webrtc::VideoFrameType> *frame_types) override
    {
        const auto ret = mRtcVideoEncoder->encode(RtcVideoFrameFromWebRTC::create(frame),
                                                  utils::fromWebRTC(frame_types));
        return ret >= 0 ? WEBRTC_VIDEO_CODEC_OK : WEBRTC_VIDEO_CODEC_ERROR;
    }

    int32_t RegisterEncodeCompleteCallback(webrtc::EncodedImageCallback *callback) override
    {
        const auto wraper = RtcEncodedImageCallbackFromWebRTC::create(callback);
        const auto ret = mRtcVideoEncoder->registerEncodeCompleteCallback(wraper);
        return ret >= 0 ? WEBRTC_VIDEO_CODEC_OK : WEBRTC_VIDEO_CODEC_ERROR;
    }

    void OnRttUpdate(int64_t rtt_ms) override
    {
        // rtt_ms is in milliseconds
        mRtcVideoEncoder->onRttUpdate(rtt_ms);
    }

    void OnPacketLossRateUpdate(float packet_loss_rate) override
    {
        mRtcVideoEncoder->onPacketLossRateUpdate(packet_loss_rate);
    }

    void OnLossNotification(const LossNotification &loss_notification) override
    {
        mRtcVideoEncoder->onLossNotification(utils::fromWebRTC(loss_notification));
    }

protected:
    RtcVideoEncoder::SharedPtr mRtcVideoEncoder{nullptr};
};

class RtcVideoEncoderFactoryToWebRTC : public webrtc::VideoEncoderFactory
{
    RtcVideoEncoderFactoryToWebRTC(const RtcVideoCodecFactory::SharedPtr &factory)
        : mRtcVideoCodecFactory(factory)
    {
    }

public:
    ~RtcVideoEncoderFactoryToWebRTC() override = default;

    std::vector<webrtc::SdpVideoFormat> GetSupportedFormats() const override
    {
        std::vector<webrtc::SdpVideoFormat> supportedFormats{webrtc::SdpVideoFormat(cricket::kVp8CodecName)};

        for (const webrtc::SdpVideoFormat &format : webrtc::SupportedVP9Codecs())
        {
            supportedFormats.push_back(format);
        }
        for (const webrtc::SdpVideoFormat &format : webrtc::SupportedH264Codecs())
        {
            supportedFormats.push_back(format);
        }
        return supportedFormats;
    }

    std::unique_ptr<webrtc::VideoEncoder> Create(const webrtc::Environment &env,
                                                 const webrtc::SdpVideoFormat &format) override
    {
        auto videoEncoder = mRtcVideoCodecFactory->createVideoEncoder(RtcSdpVideoFormatFromWebRTC::create(format));
        if (videoEncoder)
        {
            return RtcVideoEncoderToWebRTC::create(videoEncoder);
        }
        if (utils::stringEqualsIgnoreCase(format.name, cricket::kVp8CodecName))
        {
            return webrtc::LibvpxVp8EncoderTemplateAdapter::CreateEncoder(env, format);
        }
        if (utils::stringEqualsIgnoreCase(format.name, cricket::kVp9CodecName))
        {
            return webrtc::LibvpxVp9EncoderTemplateAdapter::CreateEncoder(env, format);
        }
        if (utils::stringEqualsIgnoreCase(format.name, cricket::kH264CodecName))
        {
            return webrtc::OpenH264EncoderTemplateAdapter::CreateEncoder(env, format);
        }
        OCTK_WARNING() << "create video encoder failed, format not supported," << "format: " << format.name;
        return nullptr;
    }

    static std::unique_ptr<RtcVideoEncoderFactoryToWebRTC> create() { }

protected:
    RtcVideoCodecFactory::SharedPtr mRtcVideoCodecFactory{nullptr};
};

OCTK_END_NAMESPACE