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

#include "../../octk_rtc_audio_device.hpp"


#include <private/octk_media_config_p.hpp>
#include <octk_rtc_peerconnection_factory.hpp>
#include <octk_rtc_video_codec_factory.hpp>
#include <octk_rtc_peerconnection.hpp>
#include <octk_rtc_dtls_transport.hpp>
#include <octk_rtc_configuration.hpp>
#include <octk_rtc_dtmf_sender.hpp>
#include <octk_rtc_video_frame.hpp>
#include <octk_string_utils.hpp>
#include <octk_once_flag.hpp>
#include <octk_yuv.hpp>

#ifndef OCTK_3RDPARTY_WEBRTC_VERSION
#    error "OCTK_3RDPARTY_WEBRTC_VERSION is not defined"
#endif /* OCTK_3RDPARTY_WEBRTC_VERSION */

#include <api/audio_codecs/builtin_audio_encoder_factory.h>
#include <api/audio_codecs/builtin_audio_decoder_factory.h>
#include <api/video_codecs/builtin_video_encoder_factory.h>
#include <api/video_codecs/builtin_video_decoder_factory.h>
#include <api/video_codecs/video_decoder_factory.h>
#include <api/video_codecs/video_decoder_factory_template.h>
#include <api/video_codecs/video_decoder_factory_template_dav1d_adapter.h>
#include <api/video_codecs/video_decoder_factory_template_libvpx_vp8_adapter.h>
#include <api/video_codecs/video_decoder_factory_template_libvpx_vp9_adapter.h>
#include <api/video_codecs/video_decoder_factory_template_open_h264_adapter.h>
#include <api/video_codecs/video_encoder_factory.h>
#include <api/video_codecs/video_encoder_factory_template.h>
#include <api/video_codecs/video_encoder_factory_template_libaom_av1_adapter.h>
#include <api/video_codecs/video_encoder_factory_template_libvpx_vp8_adapter.h>
#include <api/video_codecs/video_encoder_factory_template_libvpx_vp9_adapter.h>
#include <api/video_codecs/video_encoder_factory_template_open_h264_adapter.h>
#include <api/video_codecs/video_encoder.h>

#include <common_audio/resampler/include/push_resampler.h>
#include <common_audio/vad/include/webrtc_vad.h>

#include <modules/video_capture/video_capture_factory.h>
#include <modules/audio_device/include/audio_device.h>
#include <modules/audio_device/audio_device_impl.h>
#include <modules/video_capture/video_capture.h>

#include <api/task_queue/default_task_queue_factory.h>
#include <api/task_queue/task_queue_factory.h>

#include <api/create_peerconnection_factory.h>
#include <api/video/video_sink_interface.h>
#include <api/peer_connection_interface.h>
#include <api/data_channel_interface.h>
#include <api/media_stream_interface.h>
#include <api/video/video_frame.h>
#include <api/video/i420_buffer.h>
#include <api/scoped_refptr.h>

#include <media/engine/webrtc_video_engine.h>
#include <media/engine/webrtc_voice_engine.h>
#include <media/base/video_broadcaster.h>
#include <media/base/video_adapter.h>

#include <sdk/media_constraints.h>

#include <pc/video_track_source.h>
#include <pc/media_session.h>

#include <rtc_base/ssl_adapter.h>
#include <rtc_base/logging.h>

#include <future> // std::promise, std::future

namespace webrtc
{
template <typename T>
using VideoSinkInterface = rtc::VideoSinkInterface<T>;
template <typename T>
using VideoSourceInterface = rtc::VideoSourceInterface<T>;

using rtc::Thread;
using rtc::make_ref_counted;
using rtc::VideoBroadcaster;
using cricket::VideoAdapter;

using VideoSinkWants = rtc::VideoSinkWants;
using CopyOnWriteBuffer = rtc::CopyOnWriteBuffer;
} // namespace webrtc


OCTK_BEGIN_NAMESPACE

namespace utils
{
RtcPeerConnection::PeerConnectionState fromWebRTC(webrtc::PeerConnectionInterface::PeerConnectionState state)
{
    switch (state)
    {
        case webrtc::PeerConnectionInterface::PeerConnectionState::kNew:
            return RtcPeerConnection::PeerConnectionState::kNew;
        case webrtc::PeerConnectionInterface::PeerConnectionState::kConnecting:
            return RtcPeerConnection::PeerConnectionState::kConnecting;
        case webrtc::PeerConnectionInterface::PeerConnectionState::kConnected:
            return RtcPeerConnection::PeerConnectionState::kConnected;
        case webrtc::PeerConnectionInterface::PeerConnectionState::kDisconnected:
            return RtcPeerConnection::PeerConnectionState::kDisconnected;
        case webrtc::PeerConnectionInterface::PeerConnectionState::kFailed:
            return RtcPeerConnection::PeerConnectionState::kFailed;
        case webrtc::PeerConnectionInterface::PeerConnectionState::kClosed:
            return RtcPeerConnection::PeerConnectionState::kClosed;
        default: break;
    }
    OCTK_CHECK_NOTREACHED();
    return RtcPeerConnection::PeerConnectionState::kFailed;
}
RtcPeerConnection::IceConnectionState fromWebRTC(webrtc::PeerConnectionInterface::IceConnectionState state)
{
    switch (state)
    {
        case webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionNew:
            return RtcPeerConnection::IceConnectionState::kNew;
        case webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionChecking:
            return RtcPeerConnection::IceConnectionState::kChecking;
        case webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionConnected:
            return RtcPeerConnection::IceConnectionState::kConnected;
        case webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionCompleted:
            return RtcPeerConnection::IceConnectionState::kCompleted;
        case webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionFailed:
            return RtcPeerConnection::IceConnectionState::kFailed;
        case webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionDisconnected:
            return RtcPeerConnection::IceConnectionState::kDisconnected;
        case webrtc::PeerConnectionInterface::IceConnectionState::kIceConnectionClosed:
            return RtcPeerConnection::IceConnectionState::kClosed;
        default: break;
    }
    OCTK_CHECK_NOTREACHED();
    return RtcPeerConnection::IceConnectionState::kFailed;
}
RtcPeerConnection::IceGatheringState fromWebRTC(webrtc::PeerConnectionInterface::IceGatheringState state)
{
    switch (state)
    {
        case webrtc::PeerConnectionInterface::IceGatheringState::kIceGatheringNew:
            return RtcPeerConnection::IceGatheringState::kNew;
        case webrtc::PeerConnectionInterface::IceGatheringState::kIceGatheringGathering:
            return RtcPeerConnection::IceGatheringState::kGathering;
        case webrtc::PeerConnectionInterface::IceGatheringState::kIceGatheringComplete:
            return RtcPeerConnection::IceGatheringState::kComplete;
        default: break;
    }
    OCTK_CHECK_NOTREACHED();
    return RtcPeerConnection::IceGatheringState::kNew;
}
RtcPeerConnection::SignalingState fromWebRTC(webrtc::PeerConnectionInterface::SignalingState state)
{
    switch (state)
    {
        case webrtc::PeerConnectionInterface::SignalingState::kStable:
            return RtcPeerConnection::SignalingState::kStable;
        case webrtc::PeerConnectionInterface::SignalingState::kHaveLocalOffer:
            return RtcPeerConnection::SignalingState::kHaveLocalOffer;
        case webrtc::PeerConnectionInterface::SignalingState::kHaveLocalPrAnswer:
            return RtcPeerConnection::SignalingState::kHaveLocalPrAnswer;
        case webrtc::PeerConnectionInterface::SignalingState::kHaveRemoteOffer:
            return RtcPeerConnection::SignalingState::kHaveRemoteOffer;
        case webrtc::PeerConnectionInterface::SignalingState::kHaveRemotePrAnswer:
            return RtcPeerConnection::SignalingState::kHaveRemotePrAnswer;
        case webrtc::PeerConnectionInterface::SignalingState::kClosed:
            return RtcPeerConnection::SignalingState::kClosed;
        default: break;
    }
    OCTK_CHECK_NOTREACHED();
    return RtcPeerConnection::SignalingState::kClosed;
}
static RtcDataChannel::State fromWebRTC(webrtc::DataChannelInterface::DataState state)
{
    switch (state)
    {
        case webrtc::DataChannelInterface::kConnecting: return RtcDataChannel::State::kConnecting;
        case webrtc::DataChannelInterface::kOpen: return RtcDataChannel::State::kOpen;
        case webrtc::DataChannelInterface::kClosing: return RtcDataChannel::State::kClosing;
        case webrtc::DataChannelInterface::kClosed: return RtcDataChannel::State::kClosed;
        default: break;
    }
    OCTK_CHECK_NOTREACHED();
    return RtcDataChannel::State::kClosed;
}
static RtcDtlsTransportState fromWebRTC(webrtc::DtlsTransportState state)
{
    switch (state)
    {
        case webrtc::DtlsTransportState::kNew: return RtcDtlsTransportState::kNew;
        case webrtc::DtlsTransportState::kConnecting: return RtcDtlsTransportState::kConnecting;
        case webrtc::DtlsTransportState::kConnected: return RtcDtlsTransportState::kConnected;
        case webrtc::DtlsTransportState::kClosed: return RtcDtlsTransportState::kClosed;
        case webrtc::DtlsTransportState::kFailed: return RtcDtlsTransportState::kFailed;
        case webrtc::DtlsTransportState::kNumValues: return RtcDtlsTransportState::kNumValues;
        default: break;
    }
    OCTK_CHECK_NOTREACHED();
    return RtcDtlsTransportState::kClosed;
}
static RtcPriority fromWebRTC(webrtc::Priority priority)
{
    switch (priority)
    {
        case webrtc::Priority::kVeryLow: return RtcPriority::kVeryLow;
        case webrtc::Priority::kLow: return RtcPriority::kLow;
        case webrtc::Priority::kMedium: return RtcPriority::kMedium;
        case webrtc::Priority::kHigh: return RtcPriority::kHigh;
        default: break;
    }
    OCTK_CHECK_NOTREACHED();
    return RtcPriority::kMedium;
}
static RtcMediaSource::State fromWebRTC(webrtc::MediaSourceInterface::SourceState state)
{
    switch (state)
    {
        case webrtc::MediaSourceInterface::SourceState::kInitializing: return RtcMediaSource::State::kInitializing;
        case webrtc::MediaSourceInterface::SourceState::kLive: return RtcMediaSource::State::kLive;
        case webrtc::MediaSourceInterface::SourceState::kMuted: return RtcMediaSource::State::kMuted;
        case webrtc::MediaSourceInterface::SourceState::kEnded: return RtcMediaSource::State::kEnded;
        default: break;
    }
    OCTK_CHECK_NOTREACHED();
    return RtcMediaSource::State::kEnded;
}
static RtcVideoFrameType fromWebRTC(webrtc::VideoFrameType frame_type)
{
    switch (frame_type)
    {
        case webrtc::VideoFrameType::kEmptyFrame: return RtcVideoFrameType::kEmpty;
        case webrtc::VideoFrameType::kVideoFrameKey: return RtcVideoFrameType::kKey;
        case webrtc::VideoFrameType::kVideoFrameDelta: return RtcVideoFrameType::kDelta;
    }
    return RtcVideoFrameType::kEmpty;
}
static RtcMediaType fromWebRTC(cricket::MediaType type)
{
    switch (type)
    {
        case cricket::MediaType::MEDIA_TYPE_AUDIO: return RtcMediaType::kAudio;
        case cricket::MediaType::MEDIA_TYPE_VIDEO: return RtcMediaType::kVideo;
        case cricket::MediaType::MEDIA_TYPE_DATA: return RtcMediaType::kData;
        case cricket::MediaType::MEDIA_TYPE_UNSUPPORTED: return RtcMediaType::kUnsupported;
        default: break;
    }
    return RtcMediaType::kUnsupported;
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
static RtcMediaTrack::State fromWebRTC(webrtc::MediaStreamTrackInterface::TrackState state)
{
    switch (state)
    {
        case webrtc::MediaStreamTrackInterface::TrackState::kLive: return RtcMediaTrack::State::kLive;
        case webrtc::MediaStreamTrackInterface::TrackState::kEnded: return RtcMediaTrack::State::kEnded;
    }
    return RtcMediaTrack::State::kLive;
}
static RtcRtcpFeedbackType fromWebRTC(webrtc::RtcpFeedbackType type)
{
    switch (type)
    {
        case webrtc::RtcpFeedbackType::CCM: return RtcRtcpFeedbackType::CCM;
        case webrtc::RtcpFeedbackType::LNTF: return RtcRtcpFeedbackType::LNTF;
        case webrtc::RtcpFeedbackType::NACK: return RtcRtcpFeedbackType::NACK; ;
        case webrtc::RtcpFeedbackType::REMB: return RtcRtcpFeedbackType::REMB; ;
        case webrtc::RtcpFeedbackType::TRANSPORT_CC: return RtcRtcpFeedbackType::TRANSPORT_CC; ;
        default: break;
    }
    OCTK_CHECK_NOTREACHED();
    return RtcRtcpFeedbackType::CCM;
}
static RtcRtcpFeedbackMessageType fromWebRTC(webrtc::RtcpFeedbackMessageType type)
{
    switch (type)
    {
        case webrtc::RtcpFeedbackMessageType::GENERIC_NACK: return RtcRtcpFeedbackMessageType::GENERIC_NACK;
        case webrtc::RtcpFeedbackMessageType::PLI: return RtcRtcpFeedbackMessageType::PLI;
        case webrtc::RtcpFeedbackMessageType::FIR: return RtcRtcpFeedbackMessageType::FIR;
        default: break;
    }
    OCTK_CHECK_NOTREACHED();
    return RtcRtcpFeedbackMessageType::GENERIC_NACK;
}
static RtcRtcpFeedbackMessageType fromWebRTC(const absl::optional<webrtc::RtcpFeedbackMessageType> &type)
{
    return type.has_value() ? fromWebRTC(type.value()) : RtcRtcpFeedbackMessageType::GENERIC_NACK;
}
static RtcVideoFrame::Rotation fromWebRTC(webrtc::VideoRotation rotation)
{
    switch (rotation)
    {
        case webrtc::kVideoRotation_0: return RtcVideoFrame::Rotation::kAngle0;
        case webrtc::kVideoRotation_90: return RtcVideoFrame::Rotation::kAngle90;
        case webrtc::kVideoRotation_180: return RtcVideoFrame::Rotation::kAngle180;
        case webrtc::kVideoRotation_270: return RtcVideoFrame::Rotation::kAngle270;
        default: break;
    }
    OCTK_CHECK_NOTREACHED();
    return RtcVideoFrame::Rotation::kAngle0;
}
static RtcDegradationPreference fromWebRTC(webrtc::DegradationPreference value)
{
    switch (value)
    {
        case webrtc::DegradationPreference::DISABLED: return RtcDegradationPreference::DISABLED;
        case webrtc::DegradationPreference::MAINTAIN_FRAMERATE: return RtcDegradationPreference::MAINTAIN_FRAMERATE;
        case webrtc::DegradationPreference::MAINTAIN_RESOLUTION: return RtcDegradationPreference::MAINTAIN_RESOLUTION;
        case webrtc::DegradationPreference::BALANCED: return RtcDegradationPreference::BALANCED;
        default: break;
    }
    OCTK_CHECK_NOTREACHED();
    return RtcDegradationPreference::BALANCED;
}
static RtcDegradationPreference fromWebRTC(const absl::optional<webrtc::DegradationPreference> &value)
{
    return fromWebRTC(value.value_or(webrtc::DegradationPreference::BALANCED));
}
static RtcRtpTransceiverDirection fromWebRTC(webrtc::RtpTransceiverDirection direction)
{
    switch (direction)
    {
        case webrtc::RtpTransceiverDirection::kSendRecv: return RtcRtpTransceiverDirection::kSendRecv;
        case webrtc::RtpTransceiverDirection::kSendOnly: return RtcRtpTransceiverDirection::kSendOnly;
        case webrtc::RtpTransceiverDirection::kRecvOnly: return RtcRtpTransceiverDirection::kRecvOnly;
        case webrtc::RtpTransceiverDirection::kInactive: return RtcRtpTransceiverDirection::kInactive;
        case webrtc::RtpTransceiverDirection::kStopped: return RtcRtpTransceiverDirection::kStopped;
    }
    OCTK_CHECK_NOTREACHED();
    return RtcRtpTransceiverDirection::kStopped;
}
static RtcVideoTrack::ContentHint fromWebRTC(webrtc::VideoTrackInterface::ContentHint hint)
{
    switch (hint)
    {
        case webrtc::VideoTrackInterface::ContentHint::kNone: return RtcVideoTrack::ContentHint::kNone;
        case webrtc::VideoTrackInterface::ContentHint::kFluid: return RtcVideoTrack::ContentHint::kFluid;
        case webrtc::VideoTrackInterface::ContentHint::kDetailed: return RtcVideoTrack::ContentHint::kDetailed;
        case webrtc::VideoTrackInterface::ContentHint::kText: return RtcVideoTrack::ContentHint::kText;
        default: break;
    }
    OCTK_CHECK_NOTREACHED();
    return RtcVideoTrack::ContentHint::kNone;
}


webrtc::PeerConnectionInterface::CandidateNetworkPolicy toWebRTC(RtcCandidateNetworkPolicy policy)
{
    switch (policy)
    {
        case RtcCandidateNetworkPolicy::kAll: return webrtc::PeerConnectionInterface::kCandidateNetworkPolicyAll;
        case RtcCandidateNetworkPolicy::kLowCost:
            return webrtc::PeerConnectionInterface::kCandidateNetworkPolicyLowCost;
        default: break;
    }
    OCTK_CHECK_NOTREACHED();
    return webrtc::PeerConnectionInterface::kCandidateNetworkPolicyAll;
}
webrtc::PeerConnectionInterface::TcpCandidatePolicy toWebRTC(RtcTcpCandidatePolicy policy)
{
    switch (policy)
    {
        case RtcTcpCandidatePolicy::kDisabled:
            return webrtc::PeerConnectionInterface::TcpCandidatePolicy::kTcpCandidatePolicyDisabled;
        case RtcTcpCandidatePolicy::kEnabled:
            return webrtc::PeerConnectionInterface::TcpCandidatePolicy::kTcpCandidatePolicyEnabled;
        default: break;
    }
    OCTK_CHECK_NOTREACHED();
    return webrtc::PeerConnectionInterface::TcpCandidatePolicy::kTcpCandidatePolicyDisabled;
}
webrtc::PeerConnectionInterface::IceTransportsType toWebRTC(RtcIceTransportsType type)
{
    switch (type)
    {
        case RtcIceTransportsType::kAll: return webrtc::PeerConnectionInterface::IceTransportsType::kAll;
        case RtcIceTransportsType::kNoHost: return webrtc::PeerConnectionInterface::IceTransportsType::kNoHost;
        case RtcIceTransportsType::kNone: return webrtc::PeerConnectionInterface::IceTransportsType::kNone;
        case RtcIceTransportsType::kRelay: return webrtc::PeerConnectionInterface::IceTransportsType::kRelay;
        default: break;
    }
    OCTK_CHECK_NOTREACHED();
    return webrtc::PeerConnectionInterface::IceTransportsType::kAll;
}
webrtc::PeerConnectionInterface::RtcpMuxPolicy toWebRTC(RtcRtcpMuxPolicy policy)
{
    switch (policy)
    {
        case RtcRtcpMuxPolicy::kNegotiate:
            return webrtc::PeerConnectionInterface::RtcpMuxPolicy::kRtcpMuxPolicyNegotiate;
        case RtcRtcpMuxPolicy::kRequire: return webrtc::PeerConnectionInterface::RtcpMuxPolicy::kRtcpMuxPolicyRequire;
        default: break;
    }
    OCTK_CHECK_NOTREACHED();
    return webrtc::PeerConnectionInterface::RtcpMuxPolicy::kRtcpMuxPolicyNegotiate;
}
webrtc::PeerConnectionInterface::BundlePolicy toWebRTC(RtcBundlePolicy policy)
{
    switch (policy)
    {
        case RtcBundlePolicy::kBalanced: return webrtc::PeerConnectionInterface::BundlePolicy::kBundlePolicyBalanced;
        case RtcBundlePolicy::kMaxBundle: return webrtc::PeerConnectionInterface::BundlePolicy::kBundlePolicyMaxBundle;
        case RtcBundlePolicy::kMaxCompat: return webrtc::PeerConnectionInterface::BundlePolicy::kBundlePolicyMaxCompat;
        default: break;
    }
    OCTK_CHECK_NOTREACHED();
    return webrtc::PeerConnectionInterface::BundlePolicy::kBundlePolicyBalanced;
}
webrtc::RtpTransceiverDirection toWebRTC(RtcRtpTransceiverDirection direction)
{
    switch (direction)
    {
        case RtcRtpTransceiverDirection::kSendRecv: return webrtc::RtpTransceiverDirection::kSendRecv;
        case RtcRtpTransceiverDirection::kSendOnly: return webrtc::RtpTransceiverDirection::kSendOnly;
        case RtcRtpTransceiverDirection::kRecvOnly: return webrtc::RtpTransceiverDirection::kRecvOnly;
        case RtcRtpTransceiverDirection::kInactive: return webrtc::RtpTransceiverDirection::kInactive;
        case RtcRtpTransceiverDirection::kStopped: return webrtc::RtpTransceiverDirection::kStopped;
    }
    OCTK_CHECK_NOTREACHED();
    return webrtc::RtpTransceiverDirection::kStopped;
}
webrtc::SdpSemantics toWebRTC(RtcSdpSemantics sdpSemantics)
{
    switch (sdpSemantics)
    {
        case RtcSdpSemantics::kPlanB: return webrtc::SdpSemantics::kPlanB_DEPRECATED;
        case RtcSdpSemantics::kUnifiedPlan: return webrtc::SdpSemantics::kUnifiedPlan;
    }
    OCTK_CHECK_NOTREACHED();
    return webrtc::SdpSemantics::kUnifiedPlan;
}
static webrtc::RtcpFeedbackType toWebRTC(RtcRtcpFeedbackType type)
{
    switch (type)
    {
        case RtcRtcpFeedbackType::CCM: return webrtc::RtcpFeedbackType::CCM;
        case RtcRtcpFeedbackType::LNTF: return webrtc::RtcpFeedbackType::LNTF;
        case RtcRtcpFeedbackType::NACK: return webrtc::RtcpFeedbackType::NACK; ;
        case RtcRtcpFeedbackType::REMB: return webrtc::RtcpFeedbackType::REMB; ;
        case RtcRtcpFeedbackType::TRANSPORT_CC: return webrtc::RtcpFeedbackType::TRANSPORT_CC; ;
        default: break;
    }
    OCTK_CHECK_NOTREACHED();
    return webrtc::RtcpFeedbackType::CCM;
}
static webrtc::RtcpFeedbackMessageType toWebRTC(RtcRtcpFeedbackMessageType type)
{
    switch (type)
    {
        case RtcRtcpFeedbackMessageType::GENERIC_NACK: return webrtc::RtcpFeedbackMessageType::GENERIC_NACK;
        case RtcRtcpFeedbackMessageType::PLI: return webrtc::RtcpFeedbackMessageType::PLI;
        case RtcRtcpFeedbackMessageType::FIR: return webrtc::RtcpFeedbackMessageType::FIR;
        default: break;
    }
    OCTK_CHECK_NOTREACHED();
    return webrtc::RtcpFeedbackMessageType::GENERIC_NACK;
}
static cricket::MediaType toWebRTC(RtcMediaType type)
{
    switch (type)
    {
        case RtcMediaType::kAudio: return cricket::MediaType::MEDIA_TYPE_AUDIO;
        case RtcMediaType::kVideo: return cricket::MediaType::MEDIA_TYPE_VIDEO;
        case RtcMediaType::kData: return cricket::MediaType::MEDIA_TYPE_DATA;
        case RtcMediaType::kUnsupported: return cricket::MediaType::MEDIA_TYPE_UNSUPPORTED;
        default: break;
    }
    return cricket::MediaType::MEDIA_TYPE_UNSUPPORTED;
}
static webrtc::Priority toWebRTC(RtcPriority priority)
{
    switch (priority)
    {
        case RtcPriority::kVeryLow: return webrtc::Priority::kVeryLow;
        case RtcPriority::kLow: return webrtc::Priority::kLow;
        case RtcPriority::kMedium: return webrtc::Priority::kMedium;
        case RtcPriority::kHigh: return webrtc::Priority::kHigh;
        default: break;
    }
    OCTK_CHECK_NOTREACHED();
    return webrtc::Priority::kMedium;
}
static webrtc::DegradationPreference toWebRTC(RtcDegradationPreference value)
{
    switch (value)
    {
        case RtcDegradationPreference::DISABLED: return webrtc::DegradationPreference::DISABLED;
        case RtcDegradationPreference::MAINTAIN_FRAMERATE: return webrtc::DegradationPreference::MAINTAIN_FRAMERATE;
        case RtcDegradationPreference::MAINTAIN_RESOLUTION: return webrtc::DegradationPreference::MAINTAIN_RESOLUTION;
        case RtcDegradationPreference::BALANCED: return webrtc::DegradationPreference::BALANCED;
        default: break;
    }
    OCTK_CHECK_NOTREACHED();
    return webrtc::DegradationPreference::BALANCED;
}
static webrtc::VideoTrackInterface::ContentHint toWebRTC(RtcVideoTrack::ContentHint hint)
{
    switch (hint)
    {
        case RtcVideoTrack::ContentHint::kNone: return webrtc::VideoTrackInterface::ContentHint::kNone;
        case RtcVideoTrack::ContentHint::kFluid: return webrtc::VideoTrackInterface::ContentHint::kFluid;
        case RtcVideoTrack::ContentHint::kDetailed: return webrtc::VideoTrackInterface::ContentHint::kDetailed;
        case RtcVideoTrack::ContentHint::kText: return webrtc::VideoTrackInterface::ContentHint::kText;
        default: break;
    }
    OCTK_CHECK_NOTREACHED();
    return webrtc::VideoTrackInterface::ContentHint::kNone;
}
static webrtc::VideoRotation toWebRTC(RtcVideoFrame::Rotation rotation)
{
    switch (rotation)
    {
        case RtcVideoFrame::Rotation::kAngle0: return webrtc::kVideoRotation_0;
        case RtcVideoFrame::Rotation::kAngle90: return webrtc::kVideoRotation_90;
        case RtcVideoFrame::Rotation::kAngle180: return webrtc::kVideoRotation_180;
        case RtcVideoFrame::Rotation::kAngle270: return webrtc::kVideoRotation_270;
    }
    OCTK_CHECK_NOTREACHED();
    return webrtc::kVideoRotation_0;
}


inline std::vector<std::string> split(std::string s, std::string delimiter)
{
    std::vector<std::string> tokens;
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos)
    {
        token = s.substr(0, pos);
        tokens.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    if (int(s.length()) > 0)
    {
        tokens.push_back(s);
    }
    return tokens;
}

template <class T>
std::string join(T &val, std::string delim)
{
    std::string str;
    typename T::iterator it;
    const typename T::iterator itlast = val.end() - 1;
    for (it = val.begin(); it != val.end(); it++)
    {
        str += *it;
        if (it != itlast)
        {
            str += delim;
        }
    }
    return str;
}
} // namespace utils

/***********************************************************************************************************************
 * RtcVideoFrameWebRTC
***********************************************************************************************************************/
class RtcVideoFrameWebRTC : public RtcVideoFrame
{
public:
    RtcVideoFrameWebRTC(webrtc::scoped_refptr<webrtc::I420BufferInterface> buffer,
                        webrtc::VideoRotation rotation,
                        int64_t timestamp_us,
                        uint16_t id)
        : mWebRTCI420Buffer(buffer)
        , mTimestampUSecs(timestamp_us)
        , mWebRTCRotation(rotation)
        , mId(id)
    {
    }
    RtcVideoFrameWebRTC(const webrtc::VideoFrame &frame)
        : mWebRTCI420Buffer(frame.video_frame_buffer()->ToI420())
        , mTimestampUSecs(frame.timestamp_us())
        , mWebRTCRotation(frame.rotation())
        , mId(frame.id())
    {
    }

    ~RtcVideoFrameWebRTC() override { }
    static SharedPtr create(const webrtc::VideoFrame &frame)
    {
        return SharedPtr(new RtcVideoFrameWebRTC(frame), [](RtcVideoFrameWebRTC *p) { delete p; });
    }

    SharedPtr copy() override
    {
        return SharedPtr(new RtcVideoFrameWebRTC(mWebRTCI420Buffer, mWebRTCRotation, mTimestampUSecs, mId),
                         [](RtcVideoFrameWebRTC *p) { delete p; });
    }

    int width() const override { return mWebRTCI420Buffer->width(); }
    int height() const override { return mWebRTCI420Buffer->height(); }
    Format format() const override { return Format::kI420; }

    uint16_t id() const override { return mId; }
    int64_t timestamp() const override { return mTimestampUSecs; }
    Rotation rotation() const override { return utils::fromWebRTC(mWebRTCRotation); }

    // Returns pointer to the pixel data for a given plane. The memory is owned by
    // the VideoFrameBuffer object and must not be freed by the caller.
    const uint8_t *dataY() const override { return mWebRTCI420Buffer->GetI420()->DataY(); }
    const uint8_t *dataU() const override { return mWebRTCI420Buffer->GetI420()->DataU(); }
    const uint8_t *dataV() const override { return mWebRTCI420Buffer->GetI420()->DataV(); }

    // Returns the number of bytes between successive rows for a given plane.
    int strideY() const override { return mWebRTCI420Buffer->GetI420()->StrideY(); }
    int strideU() const override { return mWebRTCI420Buffer->GetI420()->StrideU(); }
    int strideV() const override { return mWebRTCI420Buffer->GetI420()->StrideV(); }

    // int convertToARGB(BufferType type, uint8_t *dstArgb, int dstStrideArgb, int dstWidth, int dstHeight) override
    // {
    //
    // }

private:
    uint16_t mId{0};
    int64_t mTimestampUSecs{0};
    webrtc::scoped_refptr<webrtc::I420BufferInterface> mWebRTCI420Buffer;
    webrtc::VideoRotation mWebRTCRotation{webrtc::kVideoRotation_0};
};

/***********************************************************************************************************************
 * RtcIceCandidateWebRTC
***********************************************************************************************************************/
class RtcIceCandidateWebRTC : public RtcIceCandidate
{
    RtcIceCandidateWebRTC(std::unique_ptr<webrtc::IceCandidateInterface> candidate)
        : mWebRTCCandidate(std::move(candidate))
    {
        mSdpMid = mWebRTCCandidate->sdp_mid();
    }
    ~RtcIceCandidateWebRTC() override = default;

public:
    bool toString(String &out) override
    {
        std::string tmp;
        if (mWebRTCCandidate->ToString(&tmp))
        {
            out = tmp;
            return true;
        }
        return false;
    }

    int sdpMLineIndex() const override { return mWebRTCCandidate->sdp_mline_index(); }

    String candidate() const override
    {
        mWebRTCCandidate->ToString((std::string *)&mSdp);
        return mSdp;
    }

    String sdpMid() const override { return mSdpMid; }

    webrtc::IceCandidateInterface *candidate() { return mWebRTCCandidate.get(); }

private:
    std::unique_ptr<webrtc::IceCandidateInterface> mWebRTCCandidate;
    std::string mSdpMid;
    std::string mSdp;
};

/***********************************************************************************************************************
 * RtcStatsWebRTC
***********************************************************************************************************************/
class RtcStatsWebRTC : public RtcStats
{
public:
    class AttributeWebRTC : public Attribute
    {
    public:
        AttributeWebRTC(const webrtc::Attribute &attr);
        ~AttributeWebRTC() override = default;

        Type type() const override;
        bool hasValue() const override { return mWebRTCAttr.has_value(); }
        StringView name() const override { return mWebRTCAttr.name(); }

        Bool toBool() const override { return mWebRTCAttr.get<bool>(); }
        Int32 toInt32() const override { return mWebRTCAttr.get<int32_t>(); }
        Int64 toInt64() const override { return mWebRTCAttr.get<int64_t>(); }
        Uint32 toUint32() const override { return mWebRTCAttr.get<uint32_t>(); }
        Uint64 toUint64() const override { return mWebRTCAttr.get<uint64_t>(); }
        Double toDouble() const override { return mWebRTCAttr.get<double>(); }
        String toString() const override { return mWebRTCAttr.get<std::string>(); }
        BoolVector toBoolVector() const override { return mWebRTCAttr.get<std::vector<bool>>(); }
        Int32Vector toInt32Vector() const override { return mWebRTCAttr.get<std::vector<int32_t>>(); }
        Int64Vector toInt64Vector() const override { return mWebRTCAttr.get<std::vector<int64_t>>(); }
        Uint32Vector toUint32Vector() const override { return mWebRTCAttr.get<std::vector<uint32_t>>(); }
        Uint64Vector toUint64Vector() const override { return mWebRTCAttr.get<std::vector<uint64_t>>(); }
        DoubleVector toDoubleVector() const override { return mWebRTCAttr.get<std::vector<double>>(); }
        StringVector toStringVector() const override { return mWebRTCAttr.get<std::vector<std::string>>(); }
        StringUint64Map toStringUint64Map() const override;
        StringDoubleMap toStringDoubleMap() const override;

    private:
        const webrtc::Attribute mWebRTCAttr;
    };
    RtcStatsWebRTC(std::unique_ptr<webrtc::RTCStats> stats) { }
    ~RtcStatsWebRTC() override { }

    StringView id() const override { return mWebRTCStats->id(); }

    StringView type() const override { return mWebRTCStats->type(); }

    String toJson() const override { return mWebRTCStats->ToJson(); }

    int64_t timestamp() const override { return mWebRTCStats->timestamp().us(); }

    Attributes attributes() override
    {
        mAttributes.clear();
        for (const webrtc::Attribute &attr : mWebRTCStats->Attributes())
        {
            if (!attr.has_value())
            {
                continue;
            }
            mAttributes.push_back(utils::make_shared<AttributeWebRTC>(attr));
        }
        return mAttributes;
    }

private:
    std::unique_ptr<webrtc::RTCStats> mWebRTCStats;
    std::vector<SharedPointer<AttributeWebRTC>> mAttributes;
};

/***********************************************************************************************************************
 * RtcAudioSourceWebRTC
***********************************************************************************************************************/
// class RtcAudioSourceWebRTC : public RtcAudioSource
// {
// public:
//     RtcAudioSourceWebRTC(webrtc::scoped_refptr<libwebrtc::LocalAudioSource> rtc_audio_source, Type source_type);
//     ~RtcAudioSourceWebRTC() override;
//
//     virtual void captureFrame(const void *audio_data,
//                               int bits_per_sample,
//                               int sample_rate,
//                               size_t number_of_channels,
//                               size_t number_of_frames) = 0;
//
//     virtual Type sourceType() const = 0;
// };

/***********************************************************************************************************************
 * RtcAudioTrackSinkWebRTCAdapter
***********************************************************************************************************************/
class RtcAudioTrackSinkWebRTCAdapter : public webrtc::AudioTrackSinkInterface
{
public:
    RtcAudioTrackSinkWebRTCAdapter(RtcAudioSink *sink)
        : mAudioSink(sink)
    {
    }

    void OnData(const void *audio_data,
                int bits_per_sample,
                int sample_rate,
                size_t number_of_channels,
                size_t number_of_frames) override
    {
        if (mAudioSink)
        {
            // mAudioSink->onData(audio_data, bits_per_sample, sample_rate, number_of_channels, number_of_frames);
        }
    }

private:
    RtcAudioSink *mAudioSink;
};

/***********************************************************************************************************************
 * RtcAudioTrackWebRTC
***********************************************************************************************************************/
class RtcAudioTrackWebRTC : public RtcAudioTrack
{
public:
    RtcAudioTrackWebRTC(webrtc::scoped_refptr<webrtc::AudioTrackInterface> audio_track)
        : mWebRTCVideoTrack(audio_track)
    {
        RTC_LOG(LS_INFO) << __FUNCTION__ << ": ctor ";
        mId = mWebRTCVideoTrack->id();
        mKind = mWebRTCVideoTrack->kind();
    }
    ~RtcAudioTrackWebRTC() override
    {
        RTC_LOG(LS_INFO) << __FUNCTION__ << ": dtor ";
        RemoveSinks();
    }

    void setVolume(double volume) override { mWebRTCVideoTrack->GetSource()->SetVolume(volume); }

    RtcAudioTrackSource::SharedPtr getSource() const override { return nullptr; }
    RtcAudioProcessor::SharedPtr getAudioProcessor() override { return nullptr; }

    void addSink(RtcAudioSink *sink) override
    {
        webrtc::MutexLock lock(&mMutex);
        if (mVideoSinks.find(sink) != mVideoSinks.end())
        {
            return;
        }
        auto adapter = std::make_unique<RtcAudioTrackSinkWebRTCAdapter>(sink);
        mWebRTCVideoTrack->AddSink(adapter.get());
        mVideoSinks[sink] = std::move(adapter);
    }

    void removeSink(RtcAudioSink *sink) override
    {
        webrtc::MutexLock lock(&mMutex);
        auto it = mVideoSinks.find(sink);
        if (it == mVideoSinks.end())
        {
            return;
        }
        mWebRTCVideoTrack->RemoveSink(it->second.get());
        mVideoSinks.erase(it);
    }

    State state() const override { return utils::fromWebRTC(mWebRTCVideoTrack->state()); }

    String id() const override { return mId; }

    String kind() const override { return mKind; }

    bool enabled() const override { return mWebRTCVideoTrack->enabled(); }
    bool setEnabled(bool enable) override { return mWebRTCVideoTrack->set_enabled(enable); }

    webrtc::scoped_refptr<webrtc::AudioTrackInterface> rtc_track() { return mWebRTCVideoTrack; }

private:
    void RemoveSinks()
    {
        webrtc::MutexLock lock(&mMutex);
        auto it = mVideoSinks.begin();
        while (it != mVideoSinks.end())
        {
            mWebRTCVideoTrack->RemoveSink(it->second.get());
            it++;
        }
        mVideoSinks.clear();
    }

    std::map<RtcAudioSink *, std::unique_ptr<RtcAudioTrackSinkWebRTCAdapter>> mVideoSinks;
    webrtc::scoped_refptr<webrtc::AudioTrackInterface> mWebRTCVideoTrack;
    webrtc::Mutex mMutex;
    String mKind;
    String mId;
};

/***********************************************************************************************************************
 * RtcVideoSinkWebRTCAdapter
***********************************************************************************************************************/
class RtcVideoSinkWebRTCAdapter : public webrtc::VideoSinkInterface<webrtc::VideoFrame>
{
public:
    RtcVideoSinkWebRTCAdapter(webrtc::scoped_refptr<webrtc::VideoTrackInterface> track)
        : mWebRTCVideoTrack(track)
    // , mMutex(new webrtc::Mutex())
    {
        mWebRTCVideoTrack->AddOrUpdateSink(this, webrtc::VideoSinkWants());
        RTC_LOG(LS_INFO) << __FUNCTION__ << ": ctor " << (void *)this;
    }
    ~RtcVideoSinkWebRTCAdapter() override
    {
        mWebRTCVideoTrack->RemoveSink(this);
        RTC_LOG(LS_INFO) << __FUNCTION__ << ": dtor ";
    }

    // virtual void sinks(RtcVideoSink *sink)
    //
    // virtual void addSink(RtcVideoSink *sink)
    // {
    //     RTC_LOG(LS_INFO) << __FUNCTION__ << ": AddRenderer " << (void *)sink;
    //     webrtc::MutexLock cs(mMutex.get());
    //     mVideoSinks.push_back(sink);
    // }
    //
    // virtual void removeSink(RtcVideoSink *sink)
    // {
    //     RTC_LOG(LS_INFO) << __FUNCTION__ << ": RemoveRenderer " << (void *)sink;
    //     webrtc::MutexLock cs(mMutex.get());
    //     mVideoSinks.erase(
    //         std::remove_if(mVideoSinks.begin(), mVideoSinks.end(), [sink](const RtcVideoSink *p) { return p == sink; }),
    //         mVideoSinks.end());
    // }

    RtcVideoBroadcaster &broadcaster() { return mVideoBroadcaster; }

    virtual void addSink(webrtc::VideoSinkInterface<webrtc::VideoFrame> *sink)
    {
        mWebRTCVideoTrack->AddOrUpdateSink(sink, webrtc::VideoSinkWants());
    }

    virtual void removeSink(webrtc::VideoSinkInterface<webrtc::VideoFrame> *sink)
    {
        mWebRTCVideoTrack->RemoveSink(sink);
    }

protected:
    // VideoSinkInterface implementation
    void OnFrame(const webrtc::VideoFrame &frame) override
    {
        auto videoFrame = RtcVideoFrameWebRTC::create(frame);
        /*webrtc::MutexLock lock(mMutex.get());
        for (const auto &sink : mVideoSinks)
        {
            sink->onData(videoFrame);
        }*/
        mVideoBroadcaster.pushData(videoFrame);
    }

    webrtc::scoped_refptr<webrtc::VideoTrackInterface> mWebRTCVideoTrack;
    RtcVideoBroadcaster mVideoBroadcaster;
    // std::vector<RtcVideoSink *> mVideoSinks;
    // std::unique_ptr<webrtc::Mutex> mMutex;
};


/***********************************************************************************************************************
 * RtcVideoSourceWebRTCAdapter
***********************************************************************************************************************/
class RtcVideoSourceWebRTCAdapter : public webrtc::VideoSourceInterface<webrtc::VideoFrame>
{
public:
    class Sink : public RtcVideoSink
    {
    public:
        Sink(webrtc::VideoBroadcaster *broadcaster)
            : mVideoBroadcaster(broadcaster)
        {
        }
        ~Sink() override = default;

        void onData(const DataType &data) override
        {
            if (!mWebRTCI420Buffer.get() || mWebRTCI420Buffer->width() != data->width() ||
                mWebRTCI420Buffer->height() != data->height())
            {
                mWebRTCI420Buffer = webrtc::I420Buffer::Create(data->width(), data->height());
            }
            utils::yuv::copyI420(data->dataY(),
                                 data->strideY(),
                                 data->dataU(),
                                 data->strideU(),
                                 data->dataV(),
                                 data->strideV(),
                                 mWebRTCI420Buffer->MutableDataY(),
                                 mWebRTCI420Buffer->StrideY(),
                                 mWebRTCI420Buffer->MutableDataU(),
                                 mWebRTCI420Buffer->StrideU(),
                                 mWebRTCI420Buffer->MutableDataV(),
                                 mWebRTCI420Buffer->StrideV(),
                                 data->width(),
                                 data->height());
            // mAdapter->mWebRTCVideoBroadcaster.OnFrame(
            //     webrtc::VideoFrame(mWebRTCI420Buffer, webrtc::kVideoRotation_0, data->timestamp()));
            mVideoBroadcaster->OnFrame(
                webrtc::VideoFrame(mWebRTCI420Buffer, webrtc::kVideoRotation_0, data->timestamp()));
            // mAdapter->OnFrame(webrtc::VideoFrame::Builder()
            //                       .set_video_frame_buffer(mWebRTCI420Buffer)
            //                       .set_rotation(utils::toWebRTC(data->rotation()))
            //                       .set_timestamp_us(data->timestamp())
            //                       .set_id(data->id())
            //                       .build());
        }

    private:
        webrtc::scoped_refptr<webrtc::I420Buffer> mWebRTCI420Buffer;
        // RtcVideoSourceWebRTCAdapter *mAdapter;
        webrtc::VideoBroadcaster *mVideoBroadcaster{nullptr};
    };

    RtcVideoSourceWebRTCAdapter()
        : mSink(utils::make_shared<Sink>(&mWebRTCVideoBroadcaster))
    {
    }
    ~RtcVideoSourceWebRTCAdapter() override = default;

    SharedPointer<Sink> sink() const { return mSink; }

    void AddOrUpdateSink(webrtc::VideoSinkInterface<webrtc::VideoFrame> *sink,
                         const webrtc::VideoSinkWants &wants) override
    {
        mWebRTCVideoBroadcaster.AddOrUpdateSink(sink, wants);
        UpdateVideoAdapter();
    }
    void RemoveSink(webrtc::VideoSinkInterface<webrtc::VideoFrame> *sink) override
    {
        mWebRTCVideoBroadcaster.RemoveSink(sink);
        UpdateVideoAdapter();
    }
    void RequestRefreshFrame() override { mWebRTCVideoBroadcaster.RequestRefreshFrame(); }

#if 1
    void OnFrame(const webrtc::VideoFrame &frame)
    {
        int cropped_width = 0;
        int cropped_height = 0;
        int out_width = 0;
        int out_height = 0;

        if (!mWebRTCVideoAdapter.AdaptFrameResolution(frame.width(),
                                                      frame.height(),
                                                      frame.timestamp_us() * 1000,
                                                      &cropped_width,
                                                      &cropped_height,
                                                      &out_width,
                                                      &out_height))
        {
            // Drop frame in order to respect frame rate constraint.
            return;
        }

        if (out_height != frame.height() || out_width != frame.width())
        {
            // Video adapter has requested a down-scale. Allocate a new buffer and
            // return scaled version.
            webrtc::scoped_refptr<webrtc::I420Buffer> scaled_buffer = webrtc::I420Buffer::Create(out_width, out_height);
            scaled_buffer->ScaleFrom(*frame.video_frame_buffer()->ToI420());
            mWebRTCVideoBroadcaster.OnFrame(webrtc::VideoFrame::Builder()
                                                .set_video_frame_buffer(scaled_buffer)
                                                .set_rotation(webrtc::kVideoRotation_0)
                                                .set_timestamp_us(frame.timestamp_us())
                                                .set_id(frame.id())
                                                .build());
        }
        else
        {
            // No adaptations needed, just return the frame as is.
            mWebRTCVideoBroadcaster.OnFrame(frame);
        }
    }
#endif

protected:
    webrtc::VideoSinkWants GetSinkWants() { return mWebRTCVideoBroadcaster.wants(); }

private:
    friend class Sink;
    void UpdateVideoAdapter()
    {
        webrtc::VideoSinkWants wants = mWebRTCVideoBroadcaster.wants();

        if (0 < wants.resolutions.size())
        {
            auto size = wants.resolutions.at(0);
            std::pair<int, int> target_aspect_ratiot(size.width, size.height);
            mWebRTCVideoAdapter.OnOutputFormatRequest(target_aspect_ratiot,
                                                      wants.max_pixel_count,
                                                      wants.max_framerate_fps);
        }
        else
        {
            mWebRTCVideoAdapter.OnSinkWants(wants);
        }
    }

    webrtc::VideoBroadcaster mWebRTCVideoBroadcaster;
    webrtc::VideoAdapter mWebRTCVideoAdapter;
    SharedPointer<Sink> mSink;
};
//
// /***********************************************************************************************************************
//  * RtcVideoSinkWebRTC
// ***********************************************************************************************************************/
// class RtcVideoSinkWebRTC : public RtcVideoSink
// {
// public:
//     RtcVideoSinkWebRTC() = default;
//     RtcVideoSinkWebRTC(const SharedPointer<RtcVideoSourceWebRTCAdapter> &adapter)
//         : mAdapter(adapter)
//     {
//     }
//     ~RtcVideoSinkWebRTC() override = default;
//
//     void onData(const DataType &data) override
//     {
//         // auto buffer = webrtc::I420Buffer::Create(data->width(),
//         //                                          data->height(),
//         //                                          data->dataY(),
//         //                                          data->strideY(),
//         //                                          data->dataU(),
//         //                                          data->strideU(),
//         //                                          data->dataV(),
//         //                                          data->strideV());
//         // // scaled_buffer->ScaleFrom(*frame.video_frame_buffer()->ToI420());
//         // this->OnFrame(webrtc::VideoFrame::Builder()
//         //                   .set_video_frame_buffer(buffer)
//         //                   .set_rotation(webrtc::kVideoRotation_0)
//         //                   .set_timestamp_us(data->timestamp())
//         //                   // .set_id(frame.id())
//         //                   .build());
//     }
//
// private:
//     SharedPointer<RtcVideoSourceWebRTCAdapter> mAdapter;
// };

/***********************************************************************************************************************
 * RtcVideoTrackSourceWebRTCAdapter
***********************************************************************************************************************/
class RtcVideoTrackSourceWebRTCAdapter : public webrtc::VideoTrackSource
{
public:
    RtcVideoTrackSourceWebRTCAdapter(const SharedPointer<RtcVideoSourceWebRTCAdapter> &videoSourceAdapter)
        : webrtc::VideoTrackSource(/*remote=*/false)
        , mAdapter(videoSourceAdapter)
    {
    }
    ~RtcVideoTrackSourceWebRTCAdapter() override = default;

    static webrtc::scoped_refptr<RtcVideoTrackSourceWebRTCAdapter> create(
        const SharedPointer<RtcVideoSourceWebRTCAdapter> &adapter)
    {
        return webrtc::make_ref_counted<RtcVideoTrackSourceWebRTCAdapter>(adapter);
    }

protected:
    bool is_screencast() const override { return false; }
    webrtc::VideoSourceInterface<webrtc::VideoFrame> *source() override { return mAdapter.get(); }

private:
    SharedPointer<RtcVideoSourceWebRTCAdapter> mAdapter;
};

/***********************************************************************************************************************
 * RtcVideoTrackSourceWebRTC
***********************************************************************************************************************/
class RtcVideoTrackSourceWebRTC : public RtcVideoTrackSource
{
public:
    RtcVideoTrackSourceWebRTC(webrtc::scoped_refptr<webrtc::VideoTrackSourceInterface> videoTrackSource)
        : mWebRTCVideoTrackSource(videoTrackSource)
    {
    }
    RtcVideoTrackSourceWebRTC(const SharedPointer<RtcVideoSourceWebRTCAdapter> &adapter)
        : mWebRTCVideoTrackSource(RtcVideoTrackSourceWebRTCAdapter::create(adapter))
    {
    }
    ~RtcVideoTrackSourceWebRTC() override = default;

    State state() const override { return utils::fromWebRTC(mWebRTCVideoTrackSource->state()); }
    bool isRemote() const override { return mWebRTCVideoTrackSource->remote(); }

    std::set<SharedPointer<SinkType>> sinks() const override { return {}; }
    void addSink(const SharedPointer<SinkType> &sink) override { }
    void removeSink(const SharedPointer<SinkType> &sink) override { }

    bool getStats(Stats *stats) override
    {
        webrtc::VideoTrackSourceInterface::Stats rtcStats;
        const auto ret = mWebRTCVideoTrackSource->GetStats(&rtcStats);
        stats->inputHeight = rtcStats.input_height;
        stats->inputWidth = rtcStats.input_width;
        return ret;
    }

    webrtc::scoped_refptr<webrtc::VideoTrackSourceInterface> rtcVideoTrackSource() { return mWebRTCVideoTrackSource; }

private:
    webrtc::scoped_refptr<webrtc::VideoTrackSourceInterface> mWebRTCVideoTrackSource;
};

/***********************************************************************************************************************
 * RtcVideoTrackWebRTC
***********************************************************************************************************************/
class RtcVideoTrackWebRTC : public RtcVideoTrack
{
public:
    RtcVideoTrackWebRTC(webrtc::scoped_refptr<webrtc::VideoTrackInterface> rtc_track)
        : mWebRTCVideoTrack(rtc_track)
        , mVideoSinkAdapter(utils::make_unique<RtcVideoSinkWebRTCAdapter>(rtc_track))
    // , mVideoSource(utils::make_shared<RtcVideoTrackSourceWebRTC>(rtc_track->GetSource()))
    {
        RTC_LOG(LS_INFO) << __FUNCTION__ << ": ctor ";
        mId = mWebRTCVideoTrack->id();
        mKind = mWebRTCVideoTrack->kind();
    }

    ~RtcVideoTrackWebRTC() override { RTC_LOG(LS_INFO) << __FUNCTION__ << ": dtor "; }

    ContentHint contentHint() const override { return utils::fromWebRTC(mWebRTCVideoTrack->content_hint()); }
    void setContentHint(ContentHint hint) override { mWebRTCVideoTrack->set_content_hint(utils::toWebRTC(hint)); }
    RtcVideoTrackSource::SharedPtr getSource() const override { return mVideoSource; }

    std::set<SharedPointer<SinkType>> sinks() const override { return mVideoSinkAdapter->broadcaster().sinks(); }
    void addSink(const SharedPointer<SinkType> &sink) override
    {
        return mVideoSinkAdapter->broadcaster().addSink(sink);
    }
    void removeSink(const SharedPointer<SinkType> &sink) override
    {
        return mVideoSinkAdapter->broadcaster().removeSink(sink);
    }

    State state() const override { return utils::fromWebRTC(mWebRTCVideoTrack->state()); }

    String id() const override { return mId; }

    String kind() const override { return mKind; }

    bool enabled() const override { return mWebRTCVideoTrack->enabled(); }
    bool setEnabled(bool enable) override { return mWebRTCVideoTrack->set_enabled(enable); }

    webrtc::scoped_refptr<webrtc::VideoTrackInterface> rtc_track() { return mWebRTCVideoTrack; }

private:
    webrtc::scoped_refptr<webrtc::VideoTrackInterface> mWebRTCVideoTrack;
    UniquePointer<RtcVideoSinkWebRTCAdapter> mVideoSinkAdapter;
    SharedPointer<RtcVideoTrackSourceWebRTC> mVideoSource;
    String mKind;
    String mId;
};

/***********************************************************************************************************************
 * RtcMediaStreamWebRTC
***********************************************************************************************************************/
class RtcMediaStreamWebRTC : public RtcMediaStream, public webrtc::ObserverInterface
{
public:
    RtcMediaStreamWebRTC(webrtc::scoped_refptr<webrtc::MediaStreamInterface> webrtcMediaStream)
        : mWebRTCMediaStream(webrtcMediaStream)
    {
        mWebRTCMediaStream->RegisterObserver(this);

        for (auto track : webrtcMediaStream->GetAudioTracks())
        {
            auto audio_track = utils::make_shared<RtcAudioTrackWebRTC>(track);
            mAudioTracks.push_back(audio_track);
        }

        for (auto track : webrtcMediaStream->GetVideoTracks())
        {
            auto video_track = utils::make_shared<RtcVideoTrackWebRTC>(track);
            mVideoTracks.push_back(video_track);
        }
        mId = mWebRTCMediaStream->id();
        mLabel = mWebRTCMediaStream->id();
    }

    ~RtcMediaStreamWebRTC() override
    {
        RTC_LOG(LS_INFO) << __FUNCTION__ << ": dtor ";
        mWebRTCMediaStream->UnregisterObserver(this);
        mAudioTracks.clear();
        mVideoTracks.clear();
    }

    RtcAudioTrack::SharedPtr findAudioTrack(StringView trackId) override
    {
        for (auto track : mAudioTracks)
        {
            if (track->id().std_string() == trackId.data())
                return track;
        }

        return nullptr;
    }

    RtcVideoTrack::SharedPtr findVideoTrack(StringView trackId) override
    {
        for (auto track : mVideoTracks)
        {
            if (track->id().std_string() == trackId.data())
                return track;
        }

        return nullptr;
    }

    bool removeTrack(const RtcAudioTrack::SharedPtr &track) override
    {
        auto track_impl = utils::dynamic_pointer_cast<RtcAudioTrackWebRTC>(track);
        if (mWebRTCMediaStream->RemoveTrack(track_impl->rtc_track()))
        {
            auto it = std::find(mAudioTracks.begin(), mAudioTracks.end(), track);
            if (it != mAudioTracks.end())
                mAudioTracks.erase(it);
            return true;
        }
        return false;
    }

    bool removeTrack(const RtcVideoTrack::SharedPtr &track) override
    {
        auto track_impl = utils::dynamic_pointer_cast<RtcVideoTrackWebRTC>(track);
        if (mWebRTCMediaStream->RemoveTrack(track_impl->rtc_track()))
        {
            auto it = std::find(mVideoTracks.begin(), mVideoTracks.end(), track);
            if (it != mVideoTracks.end())
                mVideoTracks.erase(it);

            return true;
        }
        return false;
    }

    bool addTrack(const RtcAudioTrack::SharedPtr &track) override
    {
        auto track_impl = utils::dynamic_pointer_cast<RtcAudioTrackWebRTC>(track);
        if (mWebRTCMediaStream->AddTrack(track_impl->rtc_track()))
        {
            mAudioTracks.push_back(track);
            return true;
        }
        return false;
    }

    bool addTrack(const RtcVideoTrack::SharedPtr &track) override
    {
        auto track_impl = utils::dynamic_pointer_cast<RtcVideoTrackWebRTC>(track);
        if (mWebRTCMediaStream->AddTrack(track_impl->rtc_track()))
        {
            mVideoTracks.push_back(track);
            return true;
        }
        return false;
    }

    Vector<RtcAudioTrack::SharedPtr> audioTracks() override { return mAudioTracks; }

    Vector<RtcVideoTrack::SharedPtr> videoTracks() override { return mVideoTracks; }

    Vector<RtcMediaTrack::SharedPtr> tracks() override
    {
        std::vector<RtcMediaTrack::SharedPtr> tracks;
        for (auto track : mAudioTracks)
        {
            tracks.push_back(track);
        }
        for (auto track : mVideoTracks)
        {
            tracks.push_back(track);
        }
        return tracks;
    }

    String label() override { return mLabel; }

    String id() override { return mId; }

    webrtc::scoped_refptr<webrtc::MediaStreamInterface> webrtcMediaStream() { return mWebRTCMediaStream; }

    void RegisterRTCPeerConnectionObserver(RtcPeerConnection::Observer *observer) { mObserver = observer; }

protected:
    void OnChanged() override
    {
#if 0
         std::vector<scoped_refptr<RTCAudioTrack>> audio_tracks;
         for (auto track : mWebRTCMediaStream->GetAudioTracks()) {
             scoped_refptr<AudioTrackImpl> audio_track = scoped_refptr<AudioTrackImpl>(
                 new RefCountedObject<AudioTrackImpl>(track));
             audio_tracks.push_back(audio_track);
         }

         mAudioTracks = audio_tracks;

         std::vector<scoped_refptr<RTCVideoTrack>> video_tracks;
         for (auto track : mWebRTCMediaStream->GetVideoTracks()) {
             scoped_refptr<VideoTrackImpl> video_track = scoped_refptr<VideoTrackImpl>(
                 new RefCountedObject<VideoTrackImpl>(track));
             video_tracks.push_back(video_track);
         }

         std::vector<scoped_refptr<RTCVideoTrack>> removed_video_tracks;

         for (auto track : mVideoTracks) {
             if (std::find(video_tracks.begin(), video_tracks.end(), track) ==
                 video_tracks.end()) {
                 removed_video_tracks.push_back(track);
                 }
         }

         for (auto track : removed_video_tracks) {
             /*  if (mObserver) {
                 mObserver->OnRemoveTrack([&](OnRTCMediaStream on) { on(this); }, track);
               }*/
         }

         std::vector<scoped_refptr<RTCVideoTrack>> new_video_tracks;
         for (auto track : video_tracks) {
             if (std::find(mVideoTracks.begin(), mVideoTracks.end(), track) ==
                 mVideoTracks.end()) {
                 new_video_tracks.push_back(track);
                 }
         }

         // for (auto track : new_video_tracks) {
         //  if (mObserver)
         //    mObserver->OnAddTrack([&](OnRTCMediaStream on) { on(this); }, track);
         //}

         mVideoTracks = video_tracks;
#endif
    }

private:
    webrtc::scoped_refptr<webrtc::PeerConnectionInterface> mWebRTCPeerConnection;
    webrtc::scoped_refptr<webrtc::MediaStreamInterface> mWebRTCMediaStream;
    std::vector<RtcAudioTrack::SharedPtr> mAudioTracks;
    std::vector<RtcVideoTrack::SharedPtr> mVideoTracks;
    RtcPeerConnection::Observer *mObserver{nullptr};
    std::string mLabel;
    std::string mId;
};


/***********************************************************************************************************************
 * RtcDataChannelWebRTC
***********************************************************************************************************************/
class RtcDataChannelWebRTC : public RtcDataChannel, public webrtc::DataChannelObserver
{
public:
    RtcDataChannelWebRTC(webrtc::scoped_refptr<webrtc::DataChannelInterface> rtc_data_channel)
        : mWebRTCDataChannel(rtc_data_channel)
        , mMutex(new webrtc::Mutex())
    {
        mWebRTCDataChannel->RegisterObserver(this);
        mLabel = mWebRTCDataChannel->label();
    }
    ~RtcDataChannelWebRTC() override { mWebRTCDataChannel->UnregisterObserver(); }

    webrtc::scoped_refptr<webrtc::DataChannelInterface> rtc_data_channel() { return mWebRTCDataChannel; }

    void send(const uint8_t *data, uint32_t size, bool binary = false) override
    {
        webrtc::CopyOnWriteBuffer copyOnWriteBuffer(data, size);
        webrtc::DataBuffer buffer(copyOnWriteBuffer, binary);
        mWebRTCDataChannel->Send(buffer);
    }

    void registerObserver(Observer *observer) override
    {
        webrtc::MutexLock(mMutex.get());
        mObserver = observer;
    }

    uint64_t buffered_amount() const override { return mWebRTCDataChannel->buffered_amount(); }

    void unregisterObserver() override
    {
        webrtc::MutexLock(mMutex.get());
        mObserver = nullptr;
    }

    String label() const override { return mLabel; }

    int id() const override { return mWebRTCDataChannel->id(); }

    State state() override { return mState; }

    void close() override
    {
        mWebRTCDataChannel->UnregisterObserver();
        mWebRTCDataChannel->Close();
    }

protected:
    void OnStateChange() override
    {
        webrtc::MutexLock(mMutex.get());
        mState = utils::fromWebRTC(mWebRTCDataChannel->state());
        if (mObserver)
        {
            mObserver->OnStateChange(mState);
        }
    }

    void OnMessage(const webrtc::DataBuffer &buffer) override
    {
        if (mObserver)
        {
            mObserver->OnMessage(buffer.data.data<char>(), buffer.data.size(), buffer.binary);
        }
    }

private:
    webrtc::scoped_refptr<webrtc::DataChannelInterface> mWebRTCDataChannel;
    std::unique_ptr<webrtc::Mutex> mMutex;
    Observer *mObserver{nullptr};
    std::string mLabel;
    State mState;
};

/***********************************************************************************************************************
 * RtcAudioDeviceWebRTC
***********************************************************************************************************************/
// class RtcAudioDeviceWebRTC : public RtcAudioDevice, public webrtc::AudioDeviceObserver
// {
// public:
//     RtcAudioDeviceWebRTC(webrtc::scoped_refptr<webrtc::AudioDeviceModule> audio_device_module,
//                     webrtc::Thread *worker_thread);
//
//     virtual ~RtcAudioDeviceWebRTC();
//
//     int16_t playoutDevices() = 0;
//
//      int16_t recordingDevices() = 0;
//
//     int32_t playoutDeviceName(uint16_t index, char name[kAdmMaxDeviceNameSize], char guid[kAdmMaxGuidSize]) = 0;
//
//     int32_t recordingDeviceName(uint16_t index,
//                                         char name[kAdmMaxDeviceNameSize],
//                                         char guid[kAdmMaxGuidSize]) = 0;
//
//     int32_t setPlayoutDevice(uint16_t index) = 0;
//
//     int32_t setRecordingDevice(uint16_t index) = 0;
//
//     int32_t onDeviceChange(OnDeviceChangeCallback listener) = 0;
//
//      int32_t setMicrophoneVolume(uint32_t volume) = 0;
//
//      int32_t microphoneVolume(uint32_t &volume) = 0;
//
//      int32_t setSpeakerVolume(uint32_t volume) = 0;
//
//      int32_t speakerVolume(uint32_t &volume) = 0;
// };

/***********************************************************************************************************************
 * RtcVideoCapturerWebRTC
***********************************************************************************************************************/
class RtcVideoCapturerWebRTC : public RtcVideoCapturer
{
public:
    class Adapter : public RtcVideoSourceWebRTCAdapter, public webrtc::VideoSinkInterface<webrtc::VideoFrame>
    {
    public:
        static SharedPointer<Adapter> create(webrtc::Thread *worker_thread,
                                             size_t width,
                                             size_t height,
                                             size_t target_fps,
                                             size_t capture_device_index)
        {
            SharedPointer<Adapter> vcm_capturer(utils::make_shared<Adapter>(worker_thread));
            if (!vcm_capturer->Init(width, height, target_fps, capture_device_index))
            {
                RTC_LOG(LS_WARNING) << "Failed to create VcmCapturer(w = " << width << ", h = " << height
                                    << ", fps = " << target_fps << ")";
                return nullptr;
            }
            return vcm_capturer;
        }

        Adapter(webrtc::Thread *worker_thread)
            : vcm_(nullptr)
            , worker_thread_(worker_thread)
        {
        }
        ~Adapter() override { Destroy(); }

        bool startCapture()
        {
            int32_t result = worker_thread_->BlockingCall([&] { return vcm_->StartCapture(capability_); });

            if (result != 0)
            {
                Destroy();
                return false;
            }

            return true;
        }

        bool isCaptureStarted()
        {
            return vcm_ != nullptr && worker_thread_->BlockingCall([&] { return vcm_->CaptureStarted(); });
        }

        void stopCapture()
        {
            worker_thread_->BlockingCall(
                [&]
                {
                    vcm_->StopCapture();
                    // Release reference to VCM.
                    vcm_ = nullptr;
                });
        }

        void OnFrame(const webrtc::VideoFrame &frame) override { RtcVideoSourceWebRTCAdapter::OnFrame(frame); }

    private:
        bool Init(size_t width, size_t height, size_t target_fps, size_t capture_device_index)
        {
            std::unique_ptr<webrtc::VideoCaptureModule::DeviceInfo> device_info(
                webrtc::VideoCaptureFactory::CreateDeviceInfo());

            char device_name[256];
            char unique_name[256];
            if (device_info->GetDeviceName(static_cast<uint32_t>(capture_device_index),
                                           device_name,
                                           sizeof(device_name),
                                           unique_name,
                                           sizeof(unique_name)) != 0)
            {
                Destroy();
                return false;
            }

            vcm_ = webrtc::VideoCaptureFactory::Create(unique_name);

            if (!vcm_)
            {
                return false;
            }

            vcm_->RegisterCaptureDataCallback(this);

            device_info->GetCapability(vcm_->CurrentDeviceName(), 0, capability_);

            capability_.width = static_cast<int32_t>(width);
            capability_.height = static_cast<int32_t>(height);
            capability_.maxFPS = static_cast<int32_t>(target_fps);
            capability_.videoType = webrtc::VideoType::kI420;

            return true;
        }
        void Destroy()
        {
            if (!vcm_)
                return;

            vcm_->DeRegisterCaptureDataCallback();

            this->stopCapture();
        }

        webrtc::scoped_refptr<webrtc::VideoCaptureModule> vcm_;
        webrtc::VideoCaptureCapability capability_;
        webrtc::Thread *worker_thread_ = nullptr;
    };

    RtcVideoCapturerWebRTC(const SharedPointer<Adapter> &adapter)
        : mAdapter(adapter)
    {
    }
    ~RtcVideoCapturerWebRTC() override = default;

    bool isCaptureStarted() override { return mAdapter->isCaptureStarted(); }

    bool startCapture() override { return mAdapter->startCapture(); }

    void stopCapture() override { mAdapter->stopCapture(); }

private:
    SharedPointer<Adapter> mAdapter;
};

/***********************************************************************************************************************
 * RtcVideoDeviceWebRTC
***********************************************************************************************************************/
class RtcVideoDeviceWebRTC : public RtcVideoDevice
{
public:
    RtcVideoDeviceWebRTC(webrtc::Thread *workerThread)
        : mWebRTCDeviceInfo(webrtc::VideoCaptureFactory::CreateDeviceInfo())
        , mWebRTCWorkerThread(workerThread)
    {
    }
    ~RtcVideoDeviceWebRTC() override = default;

    uint32_t numberOfDevices() override
    {
        if (!mWebRTCDeviceInfo)
        {
            return 0;
        }
        return mWebRTCDeviceInfo->NumberOfDevices();
    }

    int32_t getDeviceName(uint32_t deviceNumber,
                          char *deviceNameUTF8,
                          uint32_t deviceNameLength,
                          char *deviceUniqueIdUTF8,
                          uint32_t deviceUniqueIdUTF8Length,
                          char *productUniqueIdUTF8 = 0,
                          uint32_t productUniqueIdUTF8Length = 0) override
    {
        if (!mWebRTCDeviceInfo)
        {
            return -1;
        }

        if (mWebRTCDeviceInfo->GetDeviceName(deviceNumber,
                                             deviceNameUTF8,
                                             deviceNameLength,
                                             deviceUniqueIdUTF8,
                                             deviceUniqueIdUTF8Length) != -1)
        {
            return 0;
        }
        return 0;
    }

    SharedPointer<RtcVideoCapturer> create(const char *name,
                                           uint32_t index,
                                           size_t width,
                                           size_t height,
                                           size_t targetFps) override
    {
        auto vcm = mWebRTCWorkerThread->BlockingCall(
            [&, width, height, targetFps]
            { return RtcVideoCapturerWebRTC::Adapter::create(mWebRTCWorkerThread, width, height, targetFps, index); });

        if (vcm == nullptr)
        {
            return nullptr;
        }

        return mWebRTCWorkerThread->BlockingCall([vcm] { return utils::make_shared<RtcVideoCapturerWebRTC>(vcm); });
    }

private:
    std::unique_ptr<webrtc::VideoCaptureModule::DeviceInfo> mWebRTCDeviceInfo;
    webrtc::Thread *mWebRTCWorkerThread{nullptr};
};

/***********************************************************************************************************************
 * RtcMediaConstraintsWebRTC
***********************************************************************************************************************/
class RtcMediaConstraintsWebRTC : public RtcMediaConstraints, public webrtc::MediaConstraints
{
public:
    RtcMediaConstraintsWebRTC() = default;
    ~RtcMediaConstraintsWebRTC() override = default;

    void addMandatoryConstraint(const StringView key, const StringView value) override
    {
        webrtc::MediaConstraints::Constraint constraint(key.data(), value.data());
        mWebRTCMandatory.push_back(constraint);
    }

    void addOptionalConstraint(const StringView key, const StringView value) override
    {
        webrtc::MediaConstraints::Constraint constraint(key.data(), value.data());
        mWebRTCOptional.push_back(constraint);
    }

    const Constraints &webrtcMandatory() const { return mWebRTCMandatory; }
    const Constraints &webrtcOptional() const { return mWebRTCOptional; }

private:
    webrtc::MediaConstraints::Constraints mWebRTCMandatory;
    webrtc::MediaConstraints::Constraints mWebRTCOptional;
};

/***********************************************************************************************************************
 * RtcDtlsTransportInformationWebRTC
***********************************************************************************************************************/
class RtcDtlsTransportInformationWebRTC : public RtcDtlsTransportInformation
{
public:
    RtcDtlsTransportInformationWebRTC(const webrtc::DtlsTransportInformation &dtls_transport_information)
        : mWebRTCDtlsTransportInformation(dtls_transport_information)
    {
    }
    ~RtcDtlsTransportInformationWebRTC() override = default;

    void copy(const SharedPointer<RtcDtlsTransportInformation> &other) override
    {
        const auto impl = utils::dynamic_pointer_cast<RtcDtlsTransportInformationWebRTC>(other);
        if (impl)
        {
            mWebRTCDtlsTransportInformation = impl->mWebRTCDtlsTransportInformation;
        }
    }
    RtcDtlsTransportState state() const override { return utils::fromWebRTC(mWebRTCDtlsTransportInformation.state()); }
    int srtpCipherSuite() const override { return mWebRTCDtlsTransportInformation.srtp_cipher_suite().value(); }
    int sslCipherSuite() const override { return mWebRTCDtlsTransportInformation.ssl_cipher_suite().value(); }

    webrtc::DtlsTransportInformation &dtls_transport_information() { return mWebRTCDtlsTransportInformation; }

private:
    webrtc::DtlsTransportInformation mWebRTCDtlsTransportInformation;
};

/***********************************************************************************************************************
 * RtcDtlsTransportWebRTC
***********************************************************************************************************************/
class RtcDtlsTransportWebRTC : public RtcDtlsTransport, public webrtc::DtlsTransportObserverInterface
{
public:
    RtcDtlsTransportWebRTC(webrtc::scoped_refptr<webrtc::DtlsTransportInterface> dtls_transport)
        : mWebRTCDtlsTransport(dtls_transport)
        , mObserver(nullptr)
    {
    }
    ~RtcDtlsTransportWebRTC() override = default;

    RtcDtlsTransportInformation::SharedPtr getInformation() override
    {
        return utils::make_shared<RtcDtlsTransportInformationWebRTC>(mWebRTCDtlsTransport->Information());
    }

    void registerObserver(Observer *observer) override
    {
        mObserver = observer;
        mWebRTCDtlsTransport->RegisterObserver(this);
    }

    void unregisterObserver() override
    {
        mWebRTCDtlsTransport->UnregisterObserver();
        mObserver = nullptr;
    }

    webrtc::scoped_refptr<webrtc::DtlsTransportInterface> dtls_transport();

protected:
    void OnStateChange(webrtc::DtlsTransportInformation info) override { }

    void OnError(webrtc::RTCError error) override
    {
        if (mObserver)
        {
            mObserver->onError(static_cast<int>(error.type()), error.message());
        }
    }

private:
    webrtc::scoped_refptr<webrtc::DtlsTransportInterface> mWebRTCDtlsTransport;
    Observer *mObserver;
};

/***********************************************************************************************************************
 * RtcDtmfSenderWebRTC
***********************************************************************************************************************/
class RtcDtmfSenderWebRTC : public RtcDtmfSender, public webrtc::DtmfSenderObserverInterface
{
public:
    RtcDtmfSenderWebRTC(webrtc::scoped_refptr<webrtc::DtmfSenderInterface> dtmf_sender)
        : mWebRTCDtmfSender(dtmf_sender)
        , mObserver(nullptr)
    {
    }
    ~RtcDtmfSenderWebRTC() override = default;

    void registerObserver(Observer *observer) override
    {
        mObserver = observer;
        mWebRTCDtmfSender->RegisterObserver(this);
    }
    void unregisterObserver() override
    {
        mWebRTCDtmfSender->UnregisterObserver();
        mObserver = nullptr;
    }

    bool insertDtmf(const StringView tones, int duration, int interToneGap, int commaDelay) override
    {
        return mWebRTCDtmfSender->InsertDtmf(tones.data(), duration, interToneGap, commaDelay);
    }
    bool insertDtmf(const StringView tones, int duration, int interToneGap) override
    {
        return mWebRTCDtmfSender->InsertDtmf(tones.data(), duration, interToneGap);
    }

    int interToneGap() const override { return mWebRTCDtmfSender->inter_tone_gap(); }

    int commaDelay() const override { return mWebRTCDtmfSender->comma_delay(); }

    bool canInsertDtmf() override { return mWebRTCDtmfSender->CanInsertDtmf(); }

    String tones() const override { return mWebRTCDtmfSender->tones(); }

    int duration() const override { return mWebRTCDtmfSender->duration(); }

    webrtc::scoped_refptr<webrtc::DtmfSenderInterface> dtmf_sender() { return mWebRTCDtmfSender; }

protected:
    void OnToneChange(const std::string &tone, const std::string &tone_buffer) override
    {
        if (mObserver)
        {
            mObserver->onToneChange(tone.c_str(), tone_buffer.c_str());
        }
    }

    void OnToneChange(const std::string &tone) override
    {
        if (mObserver)
        {
            mObserver->onToneChange(tone.c_str());
        }
    }

private:
    webrtc::scoped_refptr<webrtc::DtmfSenderInterface> mWebRTCDtmfSender;
    Observer *mObserver;
};

/***********************************************************************************************************************
 * RtcRtcpFeedbackWebRTC
***********************************************************************************************************************/
class RtcRtcpFeedbackWebRTC : public RtcRtcpFeedback
{
public:
    RtcRtcpFeedbackWebRTC(const webrtc::RtcpFeedback &rtcp_feedback)
        : mWebRTCRtcpFeedback(rtcp_feedback)
    {
    }
    ~RtcRtcpFeedbackWebRTC() override = default;

    RtcRtcpFeedbackType type() override { return utils::fromWebRTC(mWebRTCRtcpFeedback.type); }
    void setType(RtcRtcpFeedbackType value) override { mWebRTCRtcpFeedback.type = utils::toWebRTC(value); }

    RtcRtcpFeedbackMessageType messageType() override { return utils::fromWebRTC(mWebRTCRtcpFeedback.message_type); }
    void setMessageType(RtcRtcpFeedbackMessageType value) override
    {
        mWebRTCRtcpFeedback.message_type = static_cast<webrtc::RtcpFeedbackMessageType>(value);
    }

    bool isEqual(const RtcRtcpFeedback::SharedPtr &other) const override
    {
        const auto otherImpl = utils::dynamic_pointer_cast<RtcRtcpFeedbackWebRTC>(other);
        if (otherImpl)
        {
            return mWebRTCRtcpFeedback == otherImpl->mWebRTCRtcpFeedback;
        }
        return false;
    }

    webrtc::RtcpFeedback rtcp_feedback() { return mWebRTCRtcpFeedback; }

private:
    webrtc::RtcpFeedback mWebRTCRtcpFeedback;
};

/***********************************************************************************************************************
 * RtcRtpCodecParametersWebRTC
***********************************************************************************************************************/
class RtcRtpCodecParametersWebRTC : public RtcRtpCodecParameters
{
public:
    RtcRtpCodecParametersWebRTC(webrtc::RtpCodecParameters rtp_codec_parameters)
        : mWebRTCRtpCodecParameters(rtp_codec_parameters)
    {
    }
    ~RtcRtpCodecParametersWebRTC() override = default;

    String mimeType() const override { return mWebRTCRtpCodecParameters.mime_type(); }

    int pTime() const override
    {
        // return rtp_codec_parameters_.ptime.value_or(0);
        return 0;
    }
    void setPTime(int value) override
    {
        // rtp_codec_parameters_.ptime = value;
    }

    int maxPTime() const override
    {
        // return rtp_codec_parameters_.max_ptime.value_or(0);
        return 0;
    }
    void setMaxPTime(int value) override
    {
        // rtp_codec_parameters_.max_ptime = value;
    }

    int clockRate() const override { return mWebRTCRtpCodecParameters.clock_rate.value_or(0); }
    void setClockRate(int value) override { mWebRTCRtpCodecParameters.clock_rate = value; }

    String name() const override { return mWebRTCRtpCodecParameters.name; }
    void setName(StringView name) override { mWebRTCRtpCodecParameters.name = name.data(); }

    int payloadType() const override { return mWebRTCRtpCodecParameters.payload_type; }
    void setPayloadType(int value) override { mWebRTCRtpCodecParameters.payload_type = value; }

    int numChannels() const override { return mWebRTCRtpCodecParameters.num_channels.value_or(1); }
    void setNumChannels(int value) override { mWebRTCRtpCodecParameters.num_channels = value; }

    RtcMediaType kind() const override { return utils::fromWebRTC(mWebRTCRtpCodecParameters.kind); }
    void setKind(RtcMediaType value) override { mWebRTCRtpCodecParameters.kind = utils::toWebRTC(value); }

    Vector<std::pair<String, String>> parameters() const override
    {
        std::vector<std::pair<String, String>> els{};
        for (auto item : mWebRTCRtpCodecParameters.parameters)
        {
            els.push_back(std::pair<String, String>{item.first, item.second});
        }
        return els;
    }
    void setParameters(const VectorMap<String, String> &parameters) override
    {
        std::map<std::string, std::string> map;
        // for (auto item : parameters.std_map())
        // {
        //     map[item.first.c_str()] = item.second.std_string();
        // }
        mWebRTCRtpCodecParameters.parameters = map;
    }

    Vector<RtcRtcpFeedback::SharedPtr> rtcpFeedback() const override
    {
        std::vector<RtcRtcpFeedback::SharedPtr> vec;
        for (auto item : mWebRTCRtpCodecParameters.rtcp_feedback)
        {
            vec.push_back(utils::make_shared<RtcRtcpFeedbackWebRTC>(item));
        }
        return vec;
    }
    void setRtcpFeedback(const Vector<RtcRtcpFeedback::SharedPtr> &feecbacks) override
    {
        std::vector<webrtc::RtcpFeedback> rtcp_feedback;
        for (const auto &item : feecbacks.std_vector())
        {
            const auto impl = utils::dynamic_pointer_cast<RtcRtcpFeedbackWebRTC>(item);
            if (impl)
            {
                rtcp_feedback.push_back(impl->rtcp_feedback());
            }
        }
        mWebRTCRtpCodecParameters.rtcp_feedback = rtcp_feedback;
    }

    bool isEqual(const RtcRtpCodecParameters::SharedPtr &other) const override
    {
        const auto impl = utils::dynamic_pointer_cast<RtcRtpCodecParametersWebRTC>(other);
        if (impl)
        {
            return mWebRTCRtpCodecParameters == impl->mWebRTCRtpCodecParameters;
        }
        return false;
    }

    webrtc::RtpCodecParameters &rtp_codec_parameters() { return mWebRTCRtpCodecParameters; }

private:
    webrtc::RtpCodecParameters mWebRTCRtpCodecParameters;
};

/***********************************************************************************************************************
 * RtcRtpEncodingParametersWebRTC
***********************************************************************************************************************/
class RtcRtpEncodingParametersWebRTC : public RtcRtpEncodingParameters
{
public:
    RtcRtpEncodingParametersWebRTC() = default;
    RtcRtpEncodingParametersWebRTC(webrtc::RtpEncodingParameters &rtp_encoding_parameters)
        : mWebRTCRtpEncodingParameters(rtp_encoding_parameters)
    {
    }

    uint32_t ssrc() const override { return mWebRTCRtpEncodingParameters.ssrc.value_or(0); }
    void setSsrc(uint32_t value) override { mWebRTCRtpEncodingParameters.ssrc = value; }

    double bitratePriority() const override { return mWebRTCRtpEncodingParameters.bitrate_priority; }
    void setBitratePriority(double value) override { mWebRTCRtpEncodingParameters.bitrate_priority = value; }

    RtcPriority networkPriority() const override
    {
        return utils::fromWebRTC(mWebRTCRtpEncodingParameters.network_priority);
    }
    void setNetworkPriority(RtcPriority value) override
    {
        mWebRTCRtpEncodingParameters.network_priority = utils::toWebRTC(value);
    }

    int maxBitrateBps() const override { return mWebRTCRtpEncodingParameters.max_bitrate_bps.value_or(0); }
    void setMaxBitrateBps(int value) override { mWebRTCRtpEncodingParameters.max_bitrate_bps = value; }

    int minBitrateBps() const override { return mWebRTCRtpEncodingParameters.min_bitrate_bps.value_or(0); }
    void setMinBitrateBps(int value) override { mWebRTCRtpEncodingParameters.min_bitrate_bps = value; }

    double maxFramerate() const override { return mWebRTCRtpEncodingParameters.max_framerate.value_or(0); }
    void setMaxFramerate(double value) override { mWebRTCRtpEncodingParameters.max_framerate = value; }

    int numTemporalLayers() const override { return mWebRTCRtpEncodingParameters.num_temporal_layers.value_or(1); }
    void setNumTemporalLayers(int value) override { mWebRTCRtpEncodingParameters.num_temporal_layers = value; }

    double scaleResolutionDownBy() const override
    {
        return mWebRTCRtpEncodingParameters.scale_resolution_down_by.value_or(1.0);
    }
    void setScaleResolutionDownBy(double value) override
    {
        mWebRTCRtpEncodingParameters.scale_resolution_down_by = value;
    }

    String scalabilityMode() const override
    {
        auto temp = mWebRTCRtpEncodingParameters.scalability_mode.value_or("");
        return temp;
    }
    void setScalabilityMode(StringView mode) override { mWebRTCRtpEncodingParameters.scalability_mode = mode.data(); }

    bool active() const override { return mWebRTCRtpEncodingParameters.active; }
    void setActive(bool value) override { mWebRTCRtpEncodingParameters.active = value; }

    String rid() const override
    {
        auto temp = mWebRTCRtpEncodingParameters.rid;
        return temp;
    }
    void setRid(StringView rid) override { mWebRTCRtpEncodingParameters.rid = rid.data(); }


    bool adaptivePtime() const override { return mWebRTCRtpEncodingParameters.adaptive_ptime; }
    void setAdaptivePtime(bool value) override { mWebRTCRtpEncodingParameters.adaptive_ptime = value; }

    bool isEqual(const RtcRtpEncodingParameters::SharedPtr &other) const override
    {
        const auto impl = utils::dynamic_pointer_cast<RtcRtpEncodingParametersWebRTC>(other);
        if (impl)
        {
            return mWebRTCRtpEncodingParameters == impl->mWebRTCRtpEncodingParameters;
        }
        return false;
    }
    webrtc::RtpEncodingParameters &rtp_parameters() { return mWebRTCRtpEncodingParameters; }

private:
    webrtc::RtpEncodingParameters mWebRTCRtpEncodingParameters;
};

/***********************************************************************************************************************
 * RtcRtpExtensionWebRTC
***********************************************************************************************************************/
class RtcRtpExtensionWebRTC : public RtcRtpExtension
{
public:
    RtcRtpExtensionWebRTC(webrtc::RtpExtension rtp_extension)
        : mWebRTCRtpExtension(rtp_extension)
    {
    }
    ~RtcRtpExtensionWebRTC() override = default;

    String toString() const override { return mWebRTCRtpExtension.ToString(); }

    String uri() const override { return mWebRTCRtpExtension.uri; }
    void setUri(StringView uri) override { mWebRTCRtpExtension.uri = uri.data(); }

    int id() const override { return mWebRTCRtpExtension.id; }
    void setId(int value) override { mWebRTCRtpExtension.id = value; }

    bool encrypt() const override { return mWebRTCRtpExtension.encrypt; }
    void setEncrypt(bool value) override { mWebRTCRtpExtension.encrypt = value; }

    bool isEqual(const SharedPointer<RtcRtpExtension> &other) const override
    {
        const auto impl = utils::dynamic_pointer_cast<RtcRtpExtensionWebRTC>(other);
        if (impl)
        {
            return mWebRTCRtpExtension == impl->mWebRTCRtpExtension;
        }
        return false;
    }

    webrtc::RtpExtension &rtp_extension() { return mWebRTCRtpExtension; }

private:
    webrtc::RtpExtension mWebRTCRtpExtension;
};

/***********************************************************************************************************************
 * RtcRtcpParametersWebRTC
***********************************************************************************************************************/
class RtcRtcpParametersWebRTC : public RtcRtcpParameters
{
public:
    RtcRtcpParametersWebRTC(webrtc::RtcpParameters rtcp_parameters)
        : mWebRTCRtcpParameters(rtcp_parameters)
    {
    }
    ~RtcRtcpParametersWebRTC() override = default;

    uint32_t ssrc() const override { return mWebRTCRtcpParameters.ssrc.value_or(0); }
    void setSsrc(uint32_t value) override { mWebRTCRtcpParameters.ssrc = value; }

    String cname() const override { return mWebRTCRtcpParameters.cname; }
    void setCName(StringView value) override { mWebRTCRtcpParameters.cname = value.data(); }

    bool reducedSize() const override { return mWebRTCRtcpParameters.reduced_size; }
    void setReducedSize(bool value) override { mWebRTCRtcpParameters.reduced_size = value; }

    bool mux() const override { return mWebRTCRtcpParameters.mux; }
    void setMux(bool value) override { }

    bool isEqual(const SharedPointer<RtcRtcpParameters> &other) const override
    {
        const auto impl = utils::dynamic_pointer_cast<RtcRtcpParametersWebRTC>(other);
        if (impl)
        {
            return mWebRTCRtcpParameters == impl->mWebRTCRtcpParameters;
        }
        return false;
    }

    webrtc::RtcpParameters &rtcp_parameters() { return mWebRTCRtcpParameters; }

private:
    webrtc::RtcpParameters mWebRTCRtcpParameters;
};

/***********************************************************************************************************************
 * RtcRtpParametersWebRTC
***********************************************************************************************************************/
class RtcRtpParametersWebRTC : public RtcRtpParameters
{
public:
    RtcRtpParametersWebRTC(webrtc::RtpParameters rtp_parameters)
        : mWebRTCRtpParameters(rtp_parameters)
    {
    }
    ~RtcRtpParametersWebRTC() override = default;

    String transactionId() const override { return mWebRTCRtpParameters.transaction_id; }
    void setTransactionId(StringView id) override { mWebRTCRtpParameters.transaction_id = id.data(); }

    String mid() const override { return mWebRTCRtpParameters.mid; }
    void setMid(StringView mid) override { mWebRTCRtpParameters.mid = mid.data(); }

    Vector<RtcRtpCodecParameters::SharedPtr> codecs() const override
    {
        std::vector<RtcRtpCodecParameters::SharedPtr> vec;
        for (auto item : mWebRTCRtpParameters.codecs)
        {
            vec.push_back(utils::make_shared<RtcRtpCodecParametersWebRTC>(item));
        }
        return vec;
    }
    void setCodecs(const Vector<RtcRtpCodecParameters::SharedPtr> &codecs) override
    {
        std::vector<webrtc::RtpCodecParameters> list;
        for (auto item : codecs.std_vector())
        {
            const auto impl = utils::dynamic_pointer_cast<RtcRtpCodecParametersWebRTC>(item);
            if (impl)
            {
                list.push_back(impl->rtp_codec_parameters());
            }
        }
        mWebRTCRtpParameters.codecs = list;
    }

    Vector<RtcRtpExtension::SharedPtr> headerExtensions() const override
    {
        std::vector<RtcRtpExtension::SharedPtr> vec;
        for (auto item : mWebRTCRtpParameters.header_extensions)
        {
            vec.push_back(utils::make_shared<RtcRtpExtensionWebRTC>(item));
        }
        return vec;
    }
    void setHeaderExtensions(const Vector<RtcRtpExtension::SharedPtr> &header_extensions) override
    {
        std::vector<webrtc::RtpExtension> list;
        for (auto item : header_extensions.std_vector())
        {
            const auto impl = utils::dynamic_pointer_cast<RtcRtpExtensionWebRTC>(item);
            if (impl)
            {
                list.push_back(impl->rtp_extension());
            }
        }
        mWebRTCRtpParameters.header_extensions = list;
    }

    Vector<RtcRtpEncodingParameters::SharedPtr> encodings() const override
    {
        std::vector<RtcRtpEncodingParameters::SharedPtr> vec;
        for (auto item : mWebRTCRtpParameters.encodings)
        {
            vec.push_back(utils::make_shared<RtcRtpEncodingParametersWebRTC>(item));
        }
        return vec;
    }
    void setEncodings(const Vector<RtcRtpEncodingParameters::SharedPtr> &encodings) override
    {
        std::vector<webrtc::RtpEncodingParameters> list;
        for (auto item : encodings.std_vector())
        {
            const auto impl = utils::dynamic_pointer_cast<RtcRtpEncodingParametersWebRTC>(item);
            if (impl)
            {
                list.push_back(impl->rtp_parameters());
            }
        }
        mWebRTCRtpParameters.encodings = list;
    }

    RtcRtcpParameters::SharedPtr rtcpParameters() const override
    {
        return utils::make_shared<RtcRtcpParametersWebRTC>(mWebRTCRtpParameters.rtcp);
    }
    void setRtcpParameters(const SharedPointer<RtcRtcpParameters> &rtcp_parameters) override
    {
        const auto impl = utils::dynamic_pointer_cast<RtcRtcpParametersWebRTC>(rtcp_parameters);
        if (impl)
        {
            mWebRTCRtpParameters.rtcp = impl->rtcp_parameters();
        }
    }

    RtcDegradationPreference degradationPreference() const override
    {
        return utils::fromWebRTC(mWebRTCRtpParameters.degradation_preference);
    }
    void setDegradationPreference(RtcDegradationPreference value) override
    {
        mWebRTCRtpParameters.degradation_preference = utils::toWebRTC(value);
    }

    bool isEqual(const RtcRtpParameters::SharedPtr &other) const override
    {
        const auto impl = utils::dynamic_pointer_cast<RtcRtpParametersWebRTC>(other);
        if (impl)
        {
            return mWebRTCRtpParameters == impl->mWebRTCRtpParameters;
        }
        return false;
    }

    webrtc::RtpParameters rtp_parameters() { return mWebRTCRtpParameters; }

private:
    webrtc::RtpParameters mWebRTCRtpParameters;
};

/***********************************************************************************************************************
 * RtcRtpCodecCapabilityWebRTC
***********************************************************************************************************************/
class RtcRtpCodecCapabilityWebRTC : public RtcRtpCodecCapability
{
public:
    RtcRtpCodecCapabilityWebRTC() = default;
    RtcRtpCodecCapabilityWebRTC(webrtc::RtpCodecCapability rtp_codec_capability)
        : mWebRTCRtpCodecCapability(rtp_codec_capability)
    {
    }
    ~RtcRtpCodecCapabilityWebRTC() override = default;

    int channels() const override { return mWebRTCRtpCodecCapability.num_channels.value_or(-1); }
    void setChannels(int channels) override { mWebRTCRtpCodecCapability.num_channels = channels; }

    int clockRate() const override { return mWebRTCRtpCodecCapability.clock_rate.value_or(-1); }
    void setClockRate(int clockRate) override { mWebRTCRtpCodecCapability.clock_rate = clockRate; }

    String mimeType() const override { return mWebRTCRtpCodecCapability.mime_type(); }
    void setMimeType(StringView mimeType) override
    {
        std::vector<std::string> mime_type_split = utils::split(mimeType.data(), "/");
        mWebRTCRtpCodecCapability.name = mime_type_split[1];
        cricket::MediaType kind = cricket::MEDIA_TYPE_UNSUPPORTED;
        if (mime_type_split[0] == "audio")
        {
            kind = cricket::MEDIA_TYPE_AUDIO;
        }
        else if (mime_type_split[0] == "video")
        {
            kind = cricket::MEDIA_TYPE_VIDEO;
        }
        else if (mime_type_split[0] == "data")
        {
            kind = cricket::MEDIA_TYPE_DATA;
        }
        mWebRTCRtpCodecCapability.kind = kind;
    }

    String sdpFmtpLine() const override
    {
        std::vector<std::string> strarr;
        for (auto parameter : mWebRTCRtpCodecCapability.parameters)
        {
            if (parameter.first == "")
            {
                strarr.push_back(parameter.second);
            }
            else
            {
                strarr.push_back(parameter.first + "=" + parameter.second);
            }
        }
        return utils::join(strarr, ";");
    }
    void setSdpFmtpLine(StringView sdpFmtpLine) override
    {
        std::vector<std::string> parameters = utils::split(sdpFmtpLine.data(), ";");
        for (auto parameter : parameters)
        {
            if (parameter.find("=") != std::string::npos)
            {
                std::vector<std::string> parameter_split = utils::split(parameter, "=");
                mWebRTCRtpCodecCapability.parameters[parameter_split[0]] = parameter_split[1];
            }
            else
            {
                mWebRTCRtpCodecCapability.parameters[""] = parameter;
            }
        }
    }

    webrtc::RtpCodecCapability rtp_codec_capability() { return mWebRTCRtpCodecCapability; }

private:
    webrtc::RtpCodecCapability mWebRTCRtpCodecCapability;
};

/***********************************************************************************************************************
 * RtcRtpHeaderExtensionCapabilityWebRTC
***********************************************************************************************************************/
class RtcRtpHeaderExtensionCapabilityWebRTC : public RtcRtpHeaderExtensionCapability
{
public:
    RtcRtpHeaderExtensionCapabilityWebRTC(webrtc::RtpHeaderExtensionCapability rtp_header_extension_capability)
        : mWebRTCRtpHeaderExtensionCapability(rtp_header_extension_capability)
    {
    }
    ~RtcRtpHeaderExtensionCapabilityWebRTC() override = default;

    String uri() override { return mWebRTCRtpHeaderExtensionCapability.uri; }
    void setUri(StringView uri) override { mWebRTCRtpHeaderExtensionCapability.uri = uri.data(); }

    int preferredId() override { return mWebRTCRtpHeaderExtensionCapability.preferred_id.value_or(-1); }
    void setPreferredId(int value) override { mWebRTCRtpHeaderExtensionCapability.preferred_id = value; }

    bool preferredEncrypt() override { return mWebRTCRtpHeaderExtensionCapability.preferred_encrypt; }
    void setPreferredEncrypt(bool value) override { mWebRTCRtpHeaderExtensionCapability.preferred_encrypt = value; }

    webrtc::RtpHeaderExtensionCapability rtp_header_extension_capability()
    {
        return mWebRTCRtpHeaderExtensionCapability;
    }

private:
    webrtc::RtpHeaderExtensionCapability mWebRTCRtpHeaderExtensionCapability;
};

/***********************************************************************************************************************
 * RtcRtpCapabilitiesWebRTC
***********************************************************************************************************************/
class RtcRtpCapabilitiesWebRTC : public RtcRtpCapabilities
{
public:
    RtcRtpCapabilitiesWebRTC(webrtc::RtpCapabilities rtp_capabilities)
        : mWebRTCRtpCapabilities(rtp_capabilities)
    {
    }
    ~RtcRtpCapabilitiesWebRTC() override = default;

    Vector<RtcRtpCodecCapability::SharedPtr> codecs() override
    {
        std::vector<RtcRtpCodecCapability::SharedPtr> codecs;
        for (auto &codec : mWebRTCRtpCapabilities.codecs)
        {
            codecs.push_back(utils::make_shared<RtcRtpCodecCapabilityWebRTC>(codec));
        }
        return codecs;
    }
    void setCodecs(const Vector<RtcRtpCodecCapability::SharedPtr> &codecs) override
    {
        mWebRTCRtpCapabilities.codecs.clear();
        for (auto &codec : codecs.std_vector())
        {
            const auto impl = utils::dynamic_pointer_cast<RtcRtpCodecCapabilityWebRTC>(codec);
            if (impl)
            {
                mWebRTCRtpCapabilities.codecs.push_back(impl->rtp_codec_capability());
            }
        }
    }

    Vector<RtcRtpHeaderExtensionCapability::SharedPtr> headerExtensions() override
    {
        std::vector<SharedPointer<RtcRtpHeaderExtensionCapability>> header_extensions;
        for (auto &header_extension : mWebRTCRtpCapabilities.header_extensions)
        {
            header_extensions.push_back(utils::make_shared<RtcRtpHeaderExtensionCapabilityWebRTC>(header_extension));
        }
        return header_extensions;
    }
    void setHeaderExtensions(const Vector<RtcRtpHeaderExtensionCapability::SharedPtr> &headerExtensions) override
    {
        mWebRTCRtpCapabilities.header_extensions.clear();
        for (auto &header_extension : headerExtensions.std_vector())
        {
            const auto impl = utils::dynamic_pointer_cast<RtcRtpHeaderExtensionCapabilityWebRTC>(header_extension);
            if (impl)
            {
                mWebRTCRtpCapabilities.header_extensions.push_back(impl->rtp_header_extension_capability());
            }
        }
    }

    webrtc::RtpCapabilities rtp_capabilities() { return mWebRTCRtpCapabilities; }

private:
    webrtc::RtpCapabilities mWebRTCRtpCapabilities;
};


/***********************************************************************************************************************
 * RtcRtpReceiverWebRTC
***********************************************************************************************************************/
class RtcRtpReceiverWebRTC : public RtcRtpReceiver, webrtc::RtpReceiverObserverInterface
{
public:
    RtcRtpReceiverWebRTC(webrtc::scoped_refptr<webrtc::RtpReceiverInterface> rtp_receiver)
        : mWebRTCRtpReceiver(rtp_receiver)
        , mObserver(nullptr)
    {
    }

    ~RtcRtpReceiverWebRTC() override = default;

    RtcMediaTrack::SharedPtr track() const override
    {
        webrtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track = mWebRTCRtpReceiver->track();
        if (nullptr == track.get())
        {
            return nullptr;
        }
        if (track->kind() == webrtc::MediaStreamTrackInterface::kVideoKind)
        {
            return utils::make_shared<RtcVideoTrackWebRTC>(webrtc::scoped_refptr<webrtc::VideoTrackInterface>(
                static_cast<webrtc::VideoTrackInterface *>(track.get())));
        }
        else if (track->kind() == webrtc::MediaStreamTrackInterface::kAudioKind)
        {
            return utils::make_shared<RtcAudioTrackWebRTC>(webrtc::scoped_refptr<webrtc::AudioTrackInterface>(
                static_cast<webrtc::AudioTrackInterface *>(track.get())));
        }
        return nullptr;
    }

    RtcDtlsTransport::SharedPtr dtlsTransport() const override
    {
        if (nullptr == mWebRTCRtpReceiver->dtls_transport().get())
        {
            return nullptr;
        }

        return utils::make_shared<RtcDtlsTransportWebRTC>(mWebRTCRtpReceiver->dtls_transport());
    }

    Vector<String> streamIds() const override
    {
        std::vector<String> vec;
        for (auto item : mWebRTCRtpReceiver->stream_ids())
        {
            vec.push_back(item);
        }
        return vec;
    }

    Vector<RtcMediaStream::SharedPtr> streams() const override
    {
        std::vector<SharedPointer<RtcMediaStream>> streams;
        for (auto item : mWebRTCRtpReceiver->streams())
        {
            streams.push_back(utils::make_shared<RtcMediaStreamWebRTC>(item));
        }
        return streams;
    }

    RtcMediaType mediaType() const override { return utils::fromWebRTC(mWebRTCRtpReceiver->media_type()); }


    String id() const override { return mWebRTCRtpReceiver->id(); }

    RtcRtpParameters::SharedPtr parameters() const override
    {
        return utils::make_shared<RtcRtpParametersWebRTC>(mWebRTCRtpReceiver->GetParameters());
    }

    bool setParameters(const RtcRtpParameters::SharedPtr &parameters) override
    {
        const auto impl = utils::dynamic_pointer_cast<RtcRtpParametersWebRTC>(parameters);
        if (impl)
        {
            return mWebRTCRtpReceiver->SetParameters(impl->rtp_parameters());
        }
        return false;
    }

    void setObserver(Observer *observer) override
    {
        mObserver = observer;
        if (nullptr == observer)
        {
            mWebRTCRtpReceiver->SetObserver(nullptr);
        }
        else
        {
            mWebRTCRtpReceiver->SetObserver(this);
        }
    }

    void setJitterBufferMinimumDelay(double delaySeconds) override
    {
        mWebRTCRtpReceiver->SetJitterBufferMinimumDelay(delaySeconds);
    }


    webrtc::scoped_refptr<webrtc::RtpReceiverInterface> rtp_receiver() { return mWebRTCRtpReceiver; }

protected:
    void OnFirstPacketReceived(cricket::MediaType media_type) override
    {
        if (nullptr != mObserver)
        {
            mObserver->onFirstPacketReceived(utils::fromWebRTC(media_type));
        }
    }

private:
    webrtc::scoped_refptr<webrtc::RtpReceiverInterface> mWebRTCRtpReceiver;
    Observer *mObserver;
};

/***********************************************************************************************************************
 * RtcRtpSenderWebRTC
***********************************************************************************************************************/
class RtcRtpSenderWebRTC : public RtcRtpSender
{
public:
    RtcRtpSenderWebRTC(webrtc::scoped_refptr<webrtc::RtpSenderInterface> rtp_sender)
        : mWebRTCRtpSender(rtp_sender)
    {
    }
    ~RtcRtpSenderWebRTC() override = default;

    Vector<RtcRtpEncodingParameters::SharedPtr> initSendEncodings() const override
    {
        std::vector<SharedPointer<RtcRtpEncodingParameters>> vec;
        for (webrtc::RtpEncodingParameters item : mWebRTCRtpSender->init_send_encodings())
        {
            vec.push_back(utils::make_shared<RtcRtpEncodingParametersWebRTC>(item));
        }
        return vec;
    }

    RtcRtpParameters::SharedPtr parameters() const override
    {
        return utils::make_shared<RtcRtpParametersWebRTC>(mWebRTCRtpSender->GetParameters());
    }
    bool setParameters(const SharedPointer<RtcRtpParameters> &parameters) override
    {
        const auto impl = utils::dynamic_pointer_cast<RtcRtpParametersWebRTC>(parameters);
        if (impl)
        {
            return mWebRTCRtpSender->SetParameters(impl->rtp_parameters()).ok();
        }
        return false;
    }

    RtcMediaTrack::SharedPtr track() const override
    {
        webrtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track = mWebRTCRtpSender->track();

        if (nullptr == track.get())
        {
            return nullptr;
        }

        if (track->kind() == webrtc::MediaStreamTrackInterface::kVideoKind)
        {
            return utils::make_shared<RtcVideoTrackWebRTC>(webrtc::scoped_refptr<webrtc::VideoTrackInterface>(
                static_cast<webrtc::VideoTrackInterface *>(track.get())));
        }
        else if (track->kind() == webrtc::MediaStreamTrackInterface::kAudioKind)
        {
            return utils::make_shared<RtcAudioTrackWebRTC>(webrtc::scoped_refptr<webrtc::AudioTrackInterface>(
                static_cast<webrtc::AudioTrackInterface *>(track.get())));
        }
        return nullptr;
    }
    bool setTrack(const RtcMediaTrack::SharedPtr &track) override
    {
        if (track == nullptr)
        {
            return mWebRTCRtpSender->SetTrack(nullptr);
        }
        if (std::string(webrtc::MediaStreamTrackInterface::kVideoKind) == track->kind().std_string())
        {
            const auto impl = utils::dynamic_pointer_cast<RtcVideoTrackWebRTC>(track);
            if (impl)
            {
                return mWebRTCRtpSender->SetTrack(impl->rtc_track().get());
            }
        }
        else if (std::string(webrtc::MediaStreamTrackInterface::kAudioKind) == track->kind().std_string())
        {
            const auto impl = utils::dynamic_pointer_cast<RtcAudioTrackWebRTC>(track);
            if (impl)
            {
                return mWebRTCRtpSender->SetTrack(impl->rtc_track().get());
            }
        }
        return false;
    }

    Vector<String> streamIds() const override
    {
        std::vector<String> vec;
        for (std::string item : mWebRTCRtpSender->stream_ids())
        {
            vec.push_back(item.c_str());
        }
        return vec;
    }
    void setStreamIds(const Vector<String> &streamIds) const override
    {
        std::vector<std::string> list;
        for (auto id : streamIds.std_vector())
        {
            list.push_back(id.std_string());
        }
        mWebRTCRtpSender->SetStreams(list);
    }

    RtcDtmfSender::SharedPtr dtmfSender() const override
    {
        if (nullptr == mWebRTCRtpSender->GetDtmfSender().get())
        {
            return nullptr;
        }
        return utils::make_shared<RtcDtmfSenderWebRTC>(mWebRTCRtpSender->GetDtmfSender());
    }
    RtcDtlsTransport::SharedPtr dtlsTransport() const override
    {
        if (nullptr == mWebRTCRtpSender->dtls_transport().get())
        {
            return nullptr;
        }
        return utils::make_shared<RtcDtlsTransportWebRTC>(mWebRTCRtpSender->dtls_transport());
    }

    RtcMediaType mediaType() const override { return utils::fromWebRTC(mWebRTCRtpSender->media_type()); }
    uint32_t ssrc() const override { return mWebRTCRtpSender->ssrc(); }
    String id() const override { return mWebRTCRtpSender->id(); }

    webrtc::scoped_refptr<webrtc::RtpSenderInterface> rtc_rtp_sender() { return mWebRTCRtpSender; }

private:
    webrtc::scoped_refptr<webrtc::RtpSenderInterface> mWebRTCRtpSender;
};

/***********************************************************************************************************************
 * RtcRtpTransceiverInitWebRTC
***********************************************************************************************************************/
class RtcRtpTransceiverInitWebRTC : public RtcRtpTransceiverInit
{
public:
    RtcRtpTransceiverInitWebRTC() = default;
    ~RtcRtpTransceiverInitWebRTC() override = default;

    RtcRtpTransceiverDirection direction() const override
    {
        return utils::fromWebRTC(mWebRTCRtpTransceiverInit.direction);
    }
    void setDirection(RtcRtpTransceiverDirection value) override
    {
        mWebRTCRtpTransceiverInit.direction = utils::toWebRTC(value);
    }

    Vector<String> streamIds() const override
    {
        std::vector<String> vec;
        for (std::string item : mWebRTCRtpTransceiverInit.stream_ids)
        {
            vec.push_back(item.c_str());
        }
        return vec;
    }
    void setStreamIds(const Vector<String> &ids) override
    {
        std::vector<std::string> list;
        for (auto id : ids.std_vector())
        {
            list.push_back(id.std_string());
        }
        mWebRTCRtpTransceiverInit.stream_ids = list;
    }

    Vector<RtcRtpEncodingParameters::SharedPtr> sendEncodings() const override
    {
        std::vector<RtcRtpEncodingParameters::SharedPtr> vec;
        for (auto item : mWebRTCRtpTransceiverInit.send_encodings)
        {
            vec.push_back(utils::make_shared<RtcRtpEncodingParametersWebRTC>(item));
        }
        return vec;
    }
    void setSendEncodings(const Vector<RtcRtpEncodingParameters::SharedPtr> &sendEncodings) override
    {
        std::vector<webrtc::RtpEncodingParameters> list;
        for (const auto &param : sendEncodings.std_vector())
        {
            const auto impl = utils::dynamic_pointer_cast<RtcRtpEncodingParametersWebRTC>(param);
            if (impl)
            {
                list.push_back(impl->rtp_parameters());
            }
        }
        mWebRTCRtpTransceiverInit.send_encodings = list;
    }

    webrtc::RtpTransceiverInit rtp_transceiver_init() { return mWebRTCRtpTransceiverInit; }

private:
    webrtc::RtpTransceiverInit mWebRTCRtpTransceiverInit;
};

/***********************************************************************************************************************
 * RtcRtpTransceiverWebRTC
***********************************************************************************************************************/
class RtcRtpTransceiverWebRTC : public RtcRtpTransceiver
{
public:
    RtcRtpTransceiverWebRTC(webrtc::scoped_refptr<webrtc::RtpTransceiverInterface> rtp_transceiver)
        : mWebRTCRtpTransceiver(rtp_transceiver)
    {
    }
    ~RtcRtpTransceiverWebRTC() override = default;

    void stopInternal() override { mWebRTCRtpTransceiver->StopInternal(); }
    String stopStandard() override
    {
        std::string val = mWebRTCRtpTransceiver->StopStandard().message();
        return val;
    }

    String mid() const override
    {
        auto temp = mWebRTCRtpTransceiver->mid();
        if (temp.has_value())
        {
            return temp.value();
        }
        return "";
    }

    bool isStopped() const override { return mWebRTCRtpTransceiver->stopped(); }
    bool isStopping() const override { return mWebRTCRtpTransceiver->stopping(); }

    String transceiverId() const override
    {
        std::stringstream ss;
        ss << "transceiver_" << mWebRTCRtpTransceiver.get();
        return ss.str();
    }
    RtcMediaType mediaType() const override { return utils::fromWebRTC(mWebRTCRtpTransceiver->media_type()); }


    SharedPointer<RtcRtpSender> sender() const override
    {
        if (nullptr == mWebRTCRtpTransceiver->sender().get())
        {
            return nullptr;
        }
        return utils::make_shared<RtcRtpSenderWebRTC>(mWebRTCRtpTransceiver->sender());
    }
    SharedPointer<RtcRtpReceiver> receiver() const override
    {
        if (nullptr == mWebRTCRtpTransceiver->receiver().get())
        {
            return nullptr;
        }
        return utils::make_shared<RtcRtpReceiverWebRTC>(mWebRTCRtpTransceiver->receiver());
    }

    RtcRtpTransceiverDirection firedDirection() const override
    {
        auto temp = mWebRTCRtpTransceiver->fired_direction();
        if (temp.has_value())
        {
            return utils::fromWebRTC(temp.value());
        }
        return RtcRtpTransceiverDirection::kInactive;
    }
    RtcRtpTransceiverDirection currentCirection() const override
    {
        auto temp = mWebRTCRtpTransceiver->current_direction();
        if (temp.has_value())
        {
            return utils::fromWebRTC(temp.value());
        }
        return RtcRtpTransceiverDirection::kInactive;
    }

    RtcRtpTransceiverDirection direction() const override
    {
        return utils::fromWebRTC(mWebRTCRtpTransceiver->direction());
    }

    Status setDirection(RtcRtpTransceiverDirection newDirection) override
    {
        const auto error = mWebRTCRtpTransceiver->SetDirectionWithError(utils::toWebRTC(newDirection));
        return error.ok() ? Status::ok : Status(Error::create(error.message()));
    }

    void setCodecPreferences(const Vector<SharedPointer<RtcRtpCodecCapability>> &codecs) override
    {
        std::vector<webrtc::RtpCodecCapability> list;
        for (auto codec : codecs.std_vector())
        {
            const auto impl = utils::dynamic_pointer_cast<RtcRtpCodecCapabilityWebRTC>(codec);
            if (impl)
            {
                list.push_back(impl->rtp_codec_capability());
            }
        }
        mWebRTCRtpTransceiver->SetCodecPreferences(list);
    }

    webrtc::scoped_refptr<webrtc::RtpTransceiverInterface> rtp_transceiver() { return mWebRTCRtpTransceiver; }

private:
    webrtc::scoped_refptr<webrtc::RtpTransceiverInterface> mWebRTCRtpTransceiver;
};

/***********************************************************************************************************************
 * RtcPeerConnectionWebRTC
***********************************************************************************************************************/
class RtcPeerConnectionWebRTC : public RtcPeerConnection, public webrtc::PeerConnectionObserver
{
public:
    RtcPeerConnectionWebRTC(const RtcConfiguration &configuration,
                            RtcMediaConstraints::SharedPtr constraints,
                            webrtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> peer_connection_factory);
    ~RtcPeerConnectionWebRTC() override;

    Status initialize() override;
    void restartIce() override;
    void close() override;

    int addStream(const RtcMediaStream::SharedPtr &stream) override;
    int removeStream(const RtcMediaStream::SharedPtr &stream) override;

    RtcMediaStream::SharedPtr createLocalMediaStream(StringView streamId) override;
    RtcDataChannel::SharedPtr createDataChannel(StringView label, RtcDataChannelInit *dataChannelDict) override;

    void createOffer(OnSdpCreateSuccess success,
                     OnSdpCreateFailure failure,
                     const RtcMediaConstraints::SharedPtr &constraints) override;

    void createAnswer(OnSdpCreateSuccess success,
                      OnSdpCreateFailure failure,
                      const RtcMediaConstraints::SharedPtr &constraints) override;

    void setLocalDescription(StringView sdp,
                             StringView type,
                             OnSetSdpSuccess success,
                             OnSetSdpFailure failure) override;
    void setRemoteDescription(StringView sdp,
                              StringView type,
                              OnSetSdpSuccess success,
                              OnSetSdpFailure failure) override;

    void addCandidate(StringView mid, int midMLineIndex, StringView candidate) override;
    void getLocalDescription(OnGetSdpSuccess success, OnGetSdpFailure failure) override;
    void getRemoteDescription(OnGetSdpSuccess success, OnGetSdpFailure failure) override;

    void registerObserver(Observer *observer) override;
    void deregisterObserver() override;

    Vector<RtcMediaStream::SharedPtr> localStreams() override;
    Vector<RtcMediaStream::SharedPtr> remoteStreams() override;

    bool getStats(const RtcRtpSender::SharedPtr &sender,
                  OnStatsCollectorSuccess success,
                  OnStatsCollectorFailure failure) override;
    bool getStats(const RtcRtpReceiver::SharedPtr &receiver,
                  OnStatsCollectorSuccess success,
                  OnStatsCollectorFailure failure) override;
    void getStats(OnStatsCollectorSuccess success, OnStatsCollectorFailure failure) override;

    Result<RtcRtpTransceiver::SharedPtr> addTransceiver(const RtcMediaTrack::SharedPtr &track,
                                                        const RtcRtpTransceiverInit::SharedPtr &init) override;
    Result<RtcRtpTransceiver::SharedPtr> addTransceiver(const RtcMediaTrack::SharedPtr &track) override;
    Result<RtcRtpTransceiver::SharedPtr> addTransceiver(RtcMediaType mediaType) override;
    Result<RtcRtpTransceiver::SharedPtr> addTransceiver(RtcMediaType mediaType,
                                                        const RtcRtpTransceiverInit::SharedPtr &init) override;

    Result<RtcRtpSender::SharedPtr> addTrack(const RtcMediaTrack::SharedPtr &track,
                                             const Vector<String> &streamIds) override;
    bool RemoveTrack(const RtcRtpSender::SharedPtr &render) override;

    Vector<RtcRtpSender::SharedPtr> senders() override;
    Vector<RtcRtpReceiver::SharedPtr> receivers() override;
    Vector<RtcRtpTransceiver::SharedPtr> transceivers() override;

    SignalingState signalingState() override;
    IceGatheringState iceGatheringState() override;
    IceConnectionState iceConnectionState() override;
    PeerConnectionState peerConnectionState() override;

protected:
    void OnAddTrack(webrtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver,
                    const std::vector<webrtc::scoped_refptr<webrtc::MediaStreamInterface>> &streams) override;
    void OnTrack(webrtc::scoped_refptr<webrtc::RtpTransceiverInterface> transceiver) override;
    void OnRemoveTrack(webrtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver) override;
    void OnAddStream(webrtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override;
    void OnRemoveStream(webrtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override;
    void OnDataChannel(webrtc::scoped_refptr<webrtc::DataChannelInterface> data_channel) override;
    void OnRenegotiationNeeded() override;
    void OnConnectionChange(webrtc::PeerConnectionInterface::PeerConnectionState new_state) override;
    void OnIceCandidate(const webrtc::IceCandidateInterface *candidate) override;
    void OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state) override;
    void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state) override;
    void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state) override;

    webrtc::scoped_refptr<webrtc::PeerConnectionInterface> mWebRTCPeerConnection;
    webrtc::PeerConnectionInterface::RTCOfferAnswerOptions mWebRTCOfferAnswerOptions;
    webrtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> mWebRTCPeerConnectionFactory;

private:
    OnceFlag mInitOnceFlag;
    std::string mLastError;
    Observer *mObserver{nullptr};
    bool mInitializeOfferSent{false};
    const RtcConfiguration &mConfiguration;
    RtcDataChannel::SharedPtr mDataChannel;
    std::unique_ptr<webrtc::Mutex> mCallbackMutex;
    RtcMediaConstraints::SharedPtr mConstraints;
    std::vector<RtcMediaStream::SharedPtr> mLocalStreams;
    std::vector<RtcMediaStream::SharedPtr> mRemoteStreams;
};

/***********************************************************************************************************************
 * RtcPeerConnectionFactoryWebRTC
***********************************************************************************************************************/
class RtcPeerConnectionFactoryWebRTC : public RtcPeerConnectionFactory
{
public:
    RtcPeerConnectionFactoryWebRTC() = default;
    ~RtcPeerConnectionFactoryWebRTC() override = default;

    Status terminate() override;
    Status initialize() override;

    uint32_t version() const override;
    StringView versionName() const override;
    StringView backendName() const override;

    RtcPeerConnection::SharedPtr create(const RtcConfiguration &configuration,
                                        const RtcMediaConstraints::SharedPtr &constraints) override;

    void destroy(const RtcPeerConnection::SharedPtr &peerConnection) override;

    RtcAudioDevice::SharedPtr getAudioDevice() override;
    RtcVideoDevice::SharedPtr getVideoDevice() override;
    RtcAudioProcessor::SharedPtr getAudioProcessor() override;

    RtcMediaConstraints::SharedPtr createMediaConstraints() override;

    Result<RtcAudioTrackSource::SharedPtr> createAudioTrackSource(const RtcAudioSource::SharedPtr &source,
                                                                  StringView label) override;
    Result<RtcVideoTrackSource::SharedPtr> createVideoTrackSource(const RtcVideoSource::SharedPtr &source,
                                                                  StringView label) override;

    Result<RtcAudioTrack::SharedPtr> createAudioTrack(const RtcAudioTrackSource::SharedPtr &source,
                                                      StringView trackId) override;
    Result<RtcVideoTrack::SharedPtr> createVideoTrack(const RtcVideoTrackSource::SharedPtr &source,
                                                      StringView trackId) override;
    Result<RtcVideoTrack::SharedPtr> createVideoTrack(const RtcVideoSource::SharedPtr &source,
                                                      StringView trackId) override;

    RtcMediaStream::SharedPtr createLocalMediaStream(StringView streamId) override;

    RtcRtpCapabilities::SharedPtr getRtpSenderCapabilities(RtcMediaType mediaType) override;
    RtcRtpCapabilities::SharedPtr getRtpReceiverCapabilities(RtcMediaType mediaType) override;


    webrtc::Thread *webrtcSignalingThread() { return mWebRTCSignalingThread.get(); }
    webrtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> webrtcPeerConnectionFactory()
    {
        return mWebRTCPeerConnectionFactory;
    }

protected:
    void createAudioDeviceModule_w();

    void destroyAudioDeviceModule_w();

    // webrtc::scoped_refptr<libwebrtc::LocalAudioSource> CreateAudioSourceWithOptions(webrtc::AudioOptions *options,
    //                                                                                 bool is_custom_source = false);

    // scoped_refptr<RTCVideoSource> CreateVideoSource_s(SharedPointer<RTCVideoCapturer> capturer,
    //                                                   const char *video_source_label,
    //                                                   SharedPointer<RTCMediaConstraints> constraints);
    // #ifdef RTC_DESKTOP_DEVICE
    //     scoped_refptr<RTCVideoSource> CreateDesktopSource_d(scoped_refptr<RTCDesktopCapturer> capturer,
    //                                                         const char *video_source_label,
    //                                                         scoped_refptr<RTCMediaConstraints> constraints);
    // #endif
private:
    OnceFlag mInitOnceFlag;
    std::string mLastError;
    std::unique_ptr<rtc::Thread> mWebRTCWorkerThread;
    std::unique_ptr<rtc::Thread> mWebRTCNetworkThread;
    std::unique_ptr<rtc::Thread> mWebRTCSignalingThread;
    std::unique_ptr<webrtc::TaskQueueFactory> mWebRTCTaskQueueFactory;
    webrtc::scoped_refptr<webrtc::AudioDeviceModule> mWebRTCAudioDeviceModule;
    webrtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> mWebRTCPeerConnectionFactory;
    // webrtc::scoped_refptr<webrtc::CustomAudioTransportFactory> audio_transport_factory_;

    // SharedPointer<RtcAudioDeviceWebRTC> audio_device_impl_;
    // SharedPointer<RtcAudioProcessorWebRTC> audio_processing_impl_;
    SharedPointer<RtcVideoDeviceWebRTC> video_device_impl_;
    std::unordered_map<SharedPointer<RtcVideoSource>, SharedPointer<RtcVideoTrackSource>> mVideoTrackSourceMap;
#ifdef RTC_DESKTOP_DEVICE
    SharedPointer<RtcDesktopDeviceWebRTC> desktop_device_impl_;
#endif
    std::list<SharedPointer<RtcPeerConnectionWebRTC>> mPeerConnections;
};

OCTK_END_NAMESPACE