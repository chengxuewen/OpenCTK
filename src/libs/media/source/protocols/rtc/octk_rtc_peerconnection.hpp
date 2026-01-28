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
#include <octk_rtc_rtp_sender.hpp>
#include <octk_rtc_rtp_transceiver.hpp>
#include <octk_rtc_session_description.hpp>
#include <octk_rtc_video_source.hpp>
#include <octk_rtc_video_track.hpp>
#include <octk_rtc_ice_transport.hpp>
#include <octk_rtc_stats.hpp>
#include <octk_unique_function.hpp>

#include <octk_shared_pointer.hpp>

OCTK_BEGIN_NAMESPACE

class RtcPeerConnection
{
public:
    using OnStatsCollectorSuccess = UniqueFunction<void(const Vector<SharedPointer<RtcStats> &> reports)>;
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

    enum class IceGatheringState
    {
        kNew,
        kGathering,
        kComplete,
    };

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

    enum class SignalingState
    {
        kStable,
        kHaveLocalOffer,
        kHaveRemoteOffer,
        kHaveLocalPrAnswer,
        kHaveRemotePrAnswer,
        kClosed,
    };

    class Observer
    {
    public:
        virtual void onSignalingState(SignalingState state) = 0;

        virtual void onPeerConnectionState(PeerConnectionState state) = 0;

        virtual void onIceGatheringState(IceGatheringState state) = 0;

        virtual void onIceConnectionState(IceConnectionState state) = 0;

        virtual void onIceCandidate(const SharedPointer<RtcIceCandidate> &candidate) = 0;

        virtual void onAddStream(const SharedPointer<RtcMediaStream> &stream) = 0;

        virtual void onRemoveStream(const SharedPointer<RtcMediaStream> &stream) = 0;

        virtual void onDataChannel(const SharedPointer<RtcDataChannel> &data_channel) = 0;

        virtual void onRenegotiationNeeded() = 0;

        virtual void onTrack(const SharedPointer<RtcRtpTransceiver> &transceiver) = 0;

        virtual void onAddTrack(Vector<const SharedPointer<RtcMediaStream>> streams,
                                const SharedPointer<RtcRtpReceiver> &receiver) = 0;

        virtual void onRemoveTrack(const SharedPointer<RtcRtpReceiver> &receiver) = 0;

    protected:
        virtual ~Observer() = 0;
    };

    virtual int addStream(const SharedPointer<RtcMediaStream> &stream) = 0;
    virtual int removeStream(const SharedPointer<RtcMediaStream> &stream) = 0;

    virtual SharedPointer<RtcMediaStream> createLocalMediaStream(const String &stream_id) = 0;

    virtual SharedPointer<RtcDataChannel> createDataChannel(const String &label,
                                                            RtcDataChannelInit *dataChannelDict) = 0;

    virtual void createOffer(OnSdpCreateSuccess success,
                             OnSdpCreateFailure failure,
                             const SharedPointer<RtcMediaConstraints> &constraints) = 0;

    virtual void createAnswer(OnSdpCreateSuccess success,
                              OnSdpCreateFailure failure,
                              const SharedPointer<RtcMediaConstraints> &constraints) = 0;

    virtual void restartIce() = 0;
    virtual void close() = 0;

    virtual void setLocalDescription(const String &sdp,
                                     const String &type,
                                     OnSetSdpSuccess success,
                                     OnSetSdpFailure failure) = 0;
    virtual void setRemoteDescription(const String &sdp,
                                      const String &type,
                                      OnSetSdpSuccess success,
                                      OnSetSdpFailure failure) = 0;

    virtual void getLocalDescription(OnGetSdpSuccess success, OnGetSdpFailure failure) = 0;
    virtual void getRemoteDescription(OnGetSdpSuccess success, OnGetSdpFailure failure) = 0;
    virtual void addCandidate(const String &mid, int mid_mline_index, const String &candiate) = 0;

    virtual void registerObserver(Observer *observer) = 0;
    virtual void deregisterObserver() = 0;

    virtual Vector<const SharedPointer<RtcMediaStream>> localStreams() = 0;
    virtual Vector<const SharedPointer<RtcMediaStream>> remoteStreams() = 0;

    virtual bool getStats(const SharedPointer<RtcRtpSender> &sender,
                          OnStatsCollectorSuccess success,
                          OnStatsCollectorFailure failure) = 0;
    virtual bool getStats(const SharedPointer<RtcRtpReceiver> &receiver,
                          OnStatsCollectorSuccess success,
                          OnStatsCollectorFailure failure) = 0;
    virtual void getStats(OnStatsCollectorSuccess success, OnStatsCollectorFailure failure) = 0;

    virtual SharedPointer<RtcRtpTransceiver> addTransceiver(const SharedPointer<RtcMediaTrack> &track,
                                                            const SharedPointer<RtcRtpTransceiverInit> &init) = 0;
    virtual SharedPointer<RtcRtpTransceiver> addTransceiver(const SharedPointer<RtcMediaTrack> &track) = 0;
    virtual SharedPointer<RtcRtpTransceiver> addTransceiver(MediaType media_type) = 0;
    virtual SharedPointer<RtcRtpTransceiver> addTransceiver(MediaType media_type,
                                                            const SharedPointer<RtcRtpTransceiverInit> &init) = 0;

    virtual SharedPointer<RtcRtpSender> addTrack(const SharedPointer<RtcMediaTrack> &track,
                                                 const Vector<String> &streamIds) = 0;
    virtual bool RemoveTrack(const SharedPointer<RtcRtpSender> &render) = 0;

    virtual Vector<const SharedPointer<RtcRtpSender>> senders() = 0;
    virtual Vector<const SharedPointer<RtcRtpReceiver>> receivers() = 0;
    virtual Vector<const SharedPointer<RtcRtpTransceiver>> transceivers() = 0;

    virtual SignalingState signalingState() = 0;
    virtual IceGatheringState iceGatheringState() = 0;
    virtual IceConnectionState iceConnectionState() = 0;
    virtual PeerConnectionState peerConnectionState() = 0;

protected:
    virtual ~RtcPeerConnection() = 0;
};

OCTK_END_NAMESPACE
