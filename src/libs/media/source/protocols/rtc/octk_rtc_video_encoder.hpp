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

#include <octk_rtc_video_frame.hpp>
#include <octk_rtc_types.hpp>
#include <octk_string.hpp>

OCTK_BEGIN_NAMESPACE

class RtcVideoEncoder
{
public:
    using SharedPtr = SharedPointer<RtcVideoEncoder>;

    struct Info
    {
        // The name of this particular encoder implementation, e.g. "libvpx".
        String implementationName;
        // If true, this encoder has internal support for generating simulcast streams.
        // Otherwise, an adapter class will be needed.
        bool supportsSimulcast{false};
        // If true, encoder supports working with a native handle (e.g. texture
        // handle for hw codecs) rather than requiring a raw I420 buffer.
        bool supportsNativeHandle{false};
        // If this field is true, the encoder uses hardware support and different
        // thresholds will be used in CPU adaptation.
        bool isHardwareAccelerated{false};
        // The list of pixel formats preferred by the encoder.
        Vector<RtcVideoFrame::BufferType> preferredPixelFormats{RtcVideoFrame::BufferType::kI420};
        // Quality scaling is enabled if thresholds are provided(>0).
        struct ScalingSettings
        {
            struct QpThresholds
            {
                int low{-1};  // -1 means disabled.
                int high{-1}; // -1 means disabled.
            } thresholds;
            // We will never ask for a resolution lower than this.
            int minPixelsPerFrame{320 * 180};
        } scalingSettings;
    };

    struct Settings
    {
        int numberOfCores{0};
        size_t maxPayloadSize{0};
        int encoderThreadLimit{-1}; // < 0 if the limit are unknown.
        bool lossNotification{false};
    };

    struct RateControlParameters
    {
        double framerateFps{0};
        RtcVideoBitrateAllocation::SharedPtr bitrate{nullptr};
        RtcVideoBitrateAllocation::SharedPtr targetBitrate{nullptr};
    };

    struct LossNotification
    {
        // The timestamp of the last decodable frame *prior* to the last received.
        // (The last received - described below - might itself be decodable or not.)
        uint32_t timestampOfLastDecodable{0};
        // The timestamp of the last received frame.
        uint32_t timestampOfLastReceived{0};
        // Describes whether the dependencies of the last received frame were
        // all decodable.
        // `0` if some dependencies were undecodable, `>0` if all dependencies
        // were decodable, and `<0` if the dependencies are unknown.
        int dependenciesOfLastReceivedDecodable{-1};
        // Describes whether the received frame was decodable.
        // `false` if some dependency was undecodable or if some packet belonging
        // to the last received frame was missed.
        // `true` if all dependencies were decodable and all packets belonging
        // to the last received frame were received.
        // `nullopt` if no packet belonging to the last frame was missed, but the
        // last packet in the frame was not yet received.
        int lastReceivedDecodable{-1};
    };

    class EncodedImageCallback
    {
    public:
        using SharedPtr = SharedPointer<EncodedImageCallback>;

        struct Result
        {
            // Frame ID assigned to the frame.
            uint32_t frameId{0};
            // Failed to send the packet.
            bool errored{false};
            // Tells the encoder that the next frame is should be dropped.
            bool dropNextFrame{false};
        };

        enum class DropReason : uint8_t
        {
            kDroppedByMediaOptimizations,
            kDroppedByEncoder
        };

        virtual Result onEncodedImage(const RtcEncodedImage &encodedImage,
                                      const RtcCodecSpecificInfo &codecSpecificInfo) = 0;

        virtual void onDroppedFrame(DropReason /* reason */) { }

    protected:
        virtual ~EncodedImageCallback() = default;
    };

    virtual int32_t release() = 0;

    /**
     * Returns meta-data about the encoder, such as implementation name.
     * The output of this method may change during runtime. For instance if a hardware encoder fails,
     * it may fall back to doing software encoding using an implementation with different characteristics.
     * @return The encoder info.
     */
    virtual Info getEncoderInfo() const = 0;

    /**
     * Sets rate control parameters: bitrate, framerate, etc.
     * These settings are instantaneous (i.e. not moving averages) and should apply from now until the next call to
     * SetRates().
     * @param parameters : The rate control parameters.
     */
    virtual void setRates(const RateControlParameters &parameters) = 0;

    /**
     * Initialize the encoder with the information from the codecSettings
     * @param inst : Codec settings
     * @param settings : Settings affecting the encoding itself.
     * @return Set bit rate if OK, <0 - Errors:
     */
    virtual int initEncode(const RtcVideoCodec &inst, const Settings &settings) = 0;

    /**
     * Encode an image (as a part of a video stream).
     * The encoded image will be returned to the user through the encode complete callback.
     * @param frame : Image to be encoded
     * @param frameTypes : Frame type to be generated by the encoder.
     * @return >= 0 if OK, <0 - Errors:
     */
    virtual int32_t encode(const RtcVideoFrame::SharedPtr &frame, const Vector<RtcVideoFrameType> &frameTypes) = 0;

    /**
     * Inform the encoder when the round trip time changes.
     * @param rttMSecs : The new RTT, in milliseconds.
     */
    virtual void onRttUpdate(int64_t rttMSecs) { OCTK_UNUSED(rttMSecs); }

    /**
     * Inform the encoder when the packet loss rate changes.
     * @param packetLossRate : The packet loss rate (0.0 to 1.0).
     */
    virtual void onPacketLossRateUpdate(float packetLossRate) { OCTK_UNUSED(packetLossRate); }

    /**
     * Called when a loss notification is received.
     * @param lossNotification
     */
    virtual void onLossNotification(const LossNotification &lossNotification) { OCTK_UNUSED(lossNotification); }

    /**
     * Register an encode complete callback object.
     * @param callback : Callback object which handles encoded images.
     * @return : 0 if OK, < 0 otherwise.
     */
    int32_t registerEncodeCompleteCallback(const EncodedImageCallback::SharedPtr &callback)
    {
        if (callback)
        {
            mEncodedImageCallback = callback;
            return 0;
        }
        return -1;
    }

protected:
    virtual ~RtcVideoEncoder() = 0;

    EncodedImageCallback::SharedPtr mEncodedImageCallback{nullptr};
};

OCTK_END_NAMESPACE