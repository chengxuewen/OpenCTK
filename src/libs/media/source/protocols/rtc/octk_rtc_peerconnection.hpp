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

#include <octk_rtc_audio_track.hpp>
#include <octk_rtc_data_channel.hpp>
#include <octk_rtc_ice_candidate.hpp>
#include <octk_rtc_media_stream.hpp>
#include <octk_rtc_media_constraints.hpp>
#include <octk_rtc_session_description.hpp>
#include <octk_rtc_rtp_transceiver.hpp>
#include <octk_rtc_session_description.hpp>
#include <octk_rtc_video_frame.hpp>
#include <octk_rtc_video_track.hpp>
#include <octk_rtc_ice_transport.hpp>
#include <octk_rtc_stats.hpp>
#include <octk_unique_function.hpp>
#include <octk_status.hpp>
#include <octk_result.hpp>

#include <octk_shared_pointer.hpp>

OCTK_BEGIN_NAMESPACE

class OCTK_MEDIA_API RtcPeerConnection
{
public:
    OCTK_DEFINE_SHARED_PTR(RtcPeerConnection)

    using OnStatsCollectorSuccess = UniqueFunction<void(const Vector<SharedPointer<RtcStats>> &reports)>;
    using OnStatsCollectorFailure = UniqueFunction<void(const char *error)>;

    using OnSdpCreateSuccess = UniqueFunction<void(const char *sdp, const char *type)>;
    using OnSdpCreateFailure = UniqueFunction<void(const char *error)>;

    using OnSetSdpSuccess = UniqueFunction<void()>;
    using OnSetSdpFailure = UniqueFunction<void(const char *error)>;

    using OnGetSdpSuccess = UniqueFunction<void(const char *sdp, const char *type)>;
    using OnGetSdpFailure = UniqueFunction<void(const char *error)>;

    enum class PeerConnectionState
    {
        kNew,
        kConnecting,
        kConnected,
        kDisconnected,
        kFailed,
        kClosed,
    };
    static StringView peerConnectionStateToString(PeerConnectionState state);
    static StringView toString(PeerConnectionState state) { return peerConnectionStateToString(state); }

    enum class IceGatheringState
    {
        kNew,
        kGathering,
        kComplete,
    };
    static StringView iceGatheringStateToString(IceGatheringState state);
    static StringView toString(IceGatheringState state) { return iceGatheringStateToString(state); }

    enum class IceConnectionState
    {
        kNew,
        kChecking,
        kCompleted,
        kConnected,
        kFailed,
        kDisconnected,
        kClosed,
        kMax,
    };
    static StringView iceConnectionStateToString(IceConnectionState state);
    static StringView toString(IceConnectionState state) { return iceConnectionStateToString(state); }

    enum class SignalingState
    {
        kStable,
        kHaveLocalOffer,
        kHaveRemoteOffer,
        kHaveLocalPrAnswer,
        kHaveRemotePrAnswer,
        kClosed,
    };
    static StringView signalingStateToString(SignalingState state);
    static StringView toString(SignalingState state) { return signalingStateToString(state); }

    class Observer
    {
    public:
        virtual void onSignalingState(SignalingState state) { OCTK_UNUSED(state); }

        virtual void onPeerConnectionState(PeerConnectionState state) { OCTK_UNUSED(state); }

        virtual void onIceGatheringState(IceGatheringState state) { OCTK_UNUSED(state); }

        virtual void onIceConnectionState(IceConnectionState state) { OCTK_UNUSED(state); }

        virtual void onIceCandidate(const RtcIceCandidate::SharedPtr &candidate) { OCTK_UNUSED(candidate); }

        virtual void onAddStream(const RtcMediaStream::SharedPtr &stream) { OCTK_UNUSED(stream); }

        virtual void onRemoveStream(const RtcMediaStream::SharedPtr &stream) { OCTK_UNUSED(stream); }

        virtual void onDataChannel(const RtcDataChannel::SharedPtr &data_channel) { OCTK_UNUSED(data_channel); }

        virtual void onRenegotiationNeeded() { }

        virtual void onTrack(const RtcRtpTransceiver::SharedPtr &transceiver) { OCTK_UNUSED(transceiver); }

        virtual void onAddTrack(const Vector<RtcMediaStream::SharedPtr> &streams,
                                const RtcRtpReceiver::SharedPtr &receiver)
        {
            OCTK_UNUSED(streams);
            OCTK_UNUSED(receiver);
        }

        virtual void onRemoveTrack(const RtcRtpReceiver::SharedPtr &receiver) { OCTK_UNUSED(receiver); }

    protected:
        virtual ~Observer() = default;
    };

    virtual Status initialize() = 0;
    virtual void restartIce() = 0;
    virtual void close() = 0;

    virtual int addStream(const RtcMediaStream::SharedPtr &stream) = 0;
    virtual int removeStream(const RtcMediaStream::SharedPtr &stream) = 0;

    virtual RtcMediaStream::SharedPtr createLocalMediaStream(StringView streamId) = 0;
    virtual RtcDataChannel::SharedPtr createDataChannel(StringView label, RtcDataChannelInit *dataChannelDict) = 0;

    Result<RtcSessionDescription::Data> createOffer(const RtcMediaConstraints::SharedPtr &constraints = nullptr);
    virtual void createOffer(OnSdpCreateSuccess success,
                             OnSdpCreateFailure failure,
                             const RtcMediaConstraints::SharedPtr &constraints) = 0;

    Result<RtcSessionDescription::Data> createAnswer(const RtcMediaConstraints::SharedPtr &constraints = nullptr);
    virtual void createAnswer(OnSdpCreateSuccess success,
                              OnSdpCreateFailure failure,
                              const RtcMediaConstraints::SharedPtr &constraints) = 0;

    Status setLocalDescription(StringView sdp, StringView type);
    virtual void setLocalDescription(StringView sdp,
                                     StringView type,
                                     OnSetSdpSuccess success,
                                     OnSetSdpFailure failure) = 0;

    Status setRemoteDescription(StringView sdp, StringView type);
    virtual void setRemoteDescription(StringView sdp,
                                      StringView type,
                                      OnSetSdpSuccess success,
                                      OnSetSdpFailure failure) = 0;

    virtual void addCandidate(StringView mid, int midLineIndex, StringView candiate) = 0;

    Result<RtcSessionDescription::Data> getLocalDescription();
    virtual void getLocalDescription(OnGetSdpSuccess success, OnGetSdpFailure failure) = 0;

    Result<RtcSessionDescription::Data> getRemoteDescription();
    virtual void getRemoteDescription(OnGetSdpSuccess success, OnGetSdpFailure failure) = 0;

    virtual void registerObserver(Observer *observer) = 0;
    virtual void deregisterObserver() = 0;

    virtual Vector<RtcMediaStream::SharedPtr> localStreams() = 0;
    virtual Vector<RtcMediaStream::SharedPtr> remoteStreams() = 0;

    virtual bool getStats(const RtcRtpSender::SharedPtr &sender,
                          OnStatsCollectorSuccess success,
                          OnStatsCollectorFailure failure) = 0;
    virtual bool getStats(const RtcRtpReceiver::SharedPtr &receiver,
                          OnStatsCollectorSuccess success,
                          OnStatsCollectorFailure failure) = 0;
    virtual void getStats(OnStatsCollectorSuccess success, OnStatsCollectorFailure failure) = 0;

    virtual Result<RtcRtpTransceiver::SharedPtr> addTransceiver(const RtcMediaTrack::SharedPtr &track,
                                                                const RtcRtpTransceiverInit::SharedPtr &init) = 0;
    virtual Result<RtcRtpTransceiver::SharedPtr> addTransceiver(const RtcMediaTrack::SharedPtr &track) = 0;
    virtual Result<RtcRtpTransceiver::SharedPtr> addTransceiver(RtcMediaType mediaType) = 0;
    virtual Result<RtcRtpTransceiver::SharedPtr> addTransceiver(RtcMediaType mediaType,
                                                                const RtcRtpTransceiverInit::SharedPtr &init) = 0;

    virtual Result<RtcRtpSender::SharedPtr> addTrack(const RtcMediaTrack::SharedPtr &track,
                                                     const Vector<String> &streamIds) = 0;
    virtual bool RemoveTrack(const RtcRtpSender::SharedPtr &render) = 0;

    virtual Vector<RtcRtpSender::SharedPtr> senders() = 0;
    virtual Vector<RtcRtpReceiver::SharedPtr> receivers() = 0;
    virtual Vector<RtcRtpTransceiver::SharedPtr> transceivers() = 0;

    virtual SignalingState signalingState() = 0;
    virtual IceGatheringState iceGatheringState() = 0;
    virtual IceConnectionState iceConnectionState() = 0;
    virtual PeerConnectionState peerConnectionState() = 0;

protected:
    virtual ~RtcPeerConnection() = default;
};

OCTK_END_NAMESPACE
