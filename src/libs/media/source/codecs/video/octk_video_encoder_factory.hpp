/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
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

#ifndef _OCTK_VIDEO_ENCODER_FACTORY_HPP
#define _OCTK_VIDEO_ENCODER_FACTORY_HPP

#include <octk_render_resolution.hpp>
#include <octk_sdp_video_format.hpp>
#include <octk_video_encoder.hpp>
#include <octk_rtc_context.hpp>
#include <octk_data_rate.hpp>
#include <optional>

#include <memory>
#include <string>
#include <vector>

OCTK_BEGIN_NAMESPACE

// A factory that creates VideoEncoders.
// NOTE: This class is still under development and may change without notice.
class VideoEncoderFactory
{
public:
    struct CodecSupport
    {
        bool is_supported = false;
        bool is_power_efficient = false;
    };

    // An injectable class that is continuously updated with encoding conditions
    // and selects the best encoder given those conditions. An implementation is
    // typically stateful to avoid toggling between different encoders, which is
    // costly due to recreation of objects, a new codec will always start with a
    // key-frame.
    class EncoderSelectorInterface
    {
    public:
        virtual ~EncoderSelectorInterface() { }

        // Informs the encoder selector about which encoder that is currently being
        // used.
        virtual void OnCurrentEncoder(const SdpVideoFormat &format) = 0;

        // Called every time the available bitrate is updated. Should return a
        // non-empty if an encoder switch should be performed.
        virtual Optional<SdpVideoFormat> OnAvailableBitrate(const DataRate &rate) = 0;

        // Called every time the encoder input resolution change. Should return a
        // non-empty if an encoder switch should be performed.
        virtual Optional<SdpVideoFormat> OnResolutionChange(const RenderResolution & /* resolution */)
        {
            return utils::nullopt;
        }

        // Called if the currently used encoder reports itself as broken. Should
        // return a non-empty if an encoder switch should be performed.
        virtual Optional<SdpVideoFormat> OnEncoderBroken() = 0;
    };

    // Returns a list of supported video formats in order of preference, to use
    // for signaling etc.
    virtual std::vector<SdpVideoFormat> GetSupportedFormats() const = 0;

    // Returns a list of supported video formats in order of preference, that can
    // also be tagged with additional information to allow the VideoEncoderFactory
    // to separate between different implementations when CreateVideoEncoder is
    // called.
    virtual std::vector<SdpVideoFormat> GetImplementations() const { return GetSupportedFormats(); }

    // Query whether the specifed format is supported or not and if it will be
    // power efficient, which is currently interpreted as if there is support for
    // hardware acceleration.
    // See https://w3c.github.io/webrtc-svc/#scalabilitymodes* for a specification
    // of valid values for `scalability_mode`.
    // NOTE: QueryCodecSupport is currently an experimental feature that is
    // subject to change without notice.
    virtual CodecSupport QueryCodecSupport(const SdpVideoFormat &format, Optional<std::string> scalability_mode) const
    {
        // Default implementation, query for supported formats and check if the
        // specified format is supported. Returns false if scalability_mode is
        // specified.
        CodecSupport codec_support;
        if (!scalability_mode)
        {
            codec_support.is_supported = format.IsCodecInList(GetSupportedFormats());
        }
        return codec_support;
    }

    // Creates a VideoEncoder for the specified format.
    //virtual std::unique_ptr<VideoEncoder> Create(const RtcContext &env, const SdpVideoFormat &format) = 0;

    // This method creates a EncoderSelector to use for a VideoSendStream.
    // (and hence should probably been called CreateEncoderSelector()).
    //
    // Note: This method is unsuitable if encoding several streams that
    // are using same VideoEncoderFactory (either by several streams in one
    // PeerConnection or streams with different PeerConnection but same
    // PeerConnectionFactory). This is due to the fact that the method is not
    // given any stream identifier, nor is the EncoderSelectorInterface given any
    // stream identifiers, i.e one does not know which stream is being encoded
    // with help of the selector.
    //
    // In such scenario, the `RtpSenderInterface::SetEncoderSelector` is
    // recommended.
    //
    // TODO(bugs.webrtc.org:14122): Deprecate and remove in favor of
    // `RtpSenderInterface::SetEncoderSelector`.
    virtual std::unique_ptr<EncoderSelectorInterface> GetEncoderSelector() const { return nullptr; }

    virtual ~VideoEncoderFactory() { }
};

OCTK_END_NAMESPACE

#endif // _OCTK_VIDEO_ENCODER_FACTORY_HPP
