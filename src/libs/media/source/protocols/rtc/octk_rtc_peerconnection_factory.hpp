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

#include <octk_rtc_media_constraints.hpp>
#include <octk_rtc_peerconnection.hpp>
#include <octk_rtc_configuration.hpp>
#include <octk_rtc_audio_device.hpp>
#include <octk_rtc_media_stream.hpp>
#include <octk_rtc_video_device.hpp>
#include <octk_rtc_audio_track.hpp>
#include <octk_rtc_audio_frame.hpp>
#include <octk_rtc_types.hpp>
#include <octk_result.hpp>
#include <octk_status.hpp>

OCTK_BEGIN_NAMESPACE

class RtcAudioDevice;
class RtcVideoDevice;
class RtcPeerConnection;
class RtcAudioProcessor;
class RtcRtpCapabilities;

class RtcPeerConnectionFactory
{
public:
    using SharedPtr = SharedPointer<RtcPeerConnectionFactory>;

    struct Settings final
    {
        // codecs
        bool useHardwareCodec{true};
    };

    virtual Status terminate() = 0;
    virtual Status initialize(const Settings &settings) = 0;

    virtual uint32_t version() const = 0;
    virtual StringView versionName() const = 0;
    virtual StringView backendName() const = 0;

    virtual RtcPeerConnection::SharedPtr create(const RtcConfiguration &configuration,
                                                const RtcMediaConstraints::SharedPtr &constraints) = 0;

    virtual void destroy(const RtcPeerConnection::SharedPtr &peerConnection) = 0;

    virtual RtcAudioDevice::SharedPtr getAudioDevice() = 0;
    virtual RtcVideoDevice::SharedPtr getVideoDevice() = 0;
    virtual RtcAudioProcessor::SharedPtr getAudioProcessor() = 0;

    virtual RtcMediaConstraints::SharedPtr createMediaConstraints() = 0;

    virtual Result<RtcAudioTrackSource::SharedPtr> createAudioTrackSource(const RtcAudioSource::SharedPtr &source,
                                                                          StringView label) = 0;
    virtual Result<RtcVideoTrackSource::SharedPtr> createVideoTrackSource(const RtcVideoSource::SharedPtr &source,
                                                                          StringView label) = 0;

    virtual Result<RtcAudioTrack::SharedPtr> createAudioTrack(const RtcAudioTrackSource::SharedPtr &source,
                                                              StringView trackId) = 0;
    virtual Result<RtcVideoTrack::SharedPtr> createVideoTrack(const RtcVideoTrackSource::SharedPtr &source,
                                                              StringView trackId) = 0;
    virtual Result<RtcVideoTrack::SharedPtr> createVideoTrack(const RtcVideoSource::SharedPtr &source,
                                                              StringView trackId) = 0;

    virtual RtcMediaStream::SharedPtr createLocalMediaStream(StringView streamId) = 0;

    virtual RtcRtpCapabilities::SharedPtr getRtpSenderCapabilities(RtcMediaType mediaType) = 0;
    virtual RtcRtpCapabilities::SharedPtr getRtpReceiverCapabilities(RtcMediaType mediaType) = 0;

protected:
    virtual ~RtcPeerConnectionFactory() = default;
};

OCTK_END_NAMESPACE
