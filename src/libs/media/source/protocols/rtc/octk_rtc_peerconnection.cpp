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

#include <octk_rtc_peerconnection.hpp>

#include <future>

OCTK_BEGIN_NAMESPACE

StringView RtcPeerConnection::peerConnectionStateToString(PeerConnectionState state)
{
    switch (state)
    {
        case PeerConnectionState::kNew: return "New";
        case PeerConnectionState::kConnecting: return "Connecting";
        case PeerConnectionState::kConnected: return "Connected";
        case PeerConnectionState::kDisconnected: return "Disconnected";
        case PeerConnectionState::kFailed: return "Failed";
        case PeerConnectionState::kClosed: return "Closed";
        default: break;
    }
    OCTK_CHECK_NOTREACHED();
    return "";
}

StringView RtcPeerConnection::iceConnectionStateToString(IceConnectionState state)
{
    switch (state)
    {
        case IceConnectionState::kNew: return "New";
        case IceConnectionState::kChecking: return "Checking";
        case IceConnectionState::kConnected: return "Connected";
        case IceConnectionState::kCompleted: return "Completed";
        case IceConnectionState::kFailed: return "Failed";
        case IceConnectionState::kDisconnected: return "Disconnected";
        case IceConnectionState::kClosed: return "Closed";
        default: break;
    }
    OCTK_CHECK_NOTREACHED();
    return "";
}

StringView RtcPeerConnection::iceGatheringStateToString(IceGatheringState state)
{
    switch (state)
    {
        case IceGatheringState::kNew: return "New";
        case IceGatheringState::kGathering: return "Gathering";
        case IceGatheringState::kComplete: return "Complete";
        default: break;
    }
    OCTK_CHECK_NOTREACHED();
    return "";
}

StringView RtcPeerConnection::signalingStateToString(SignalingState state)
{
    switch (state)
    {
        case SignalingState::kClosed: return "Closed";
        case SignalingState::kStable: return "Stable";
        case SignalingState::kHaveLocalOffer: return "HaveLocalOffer";
        case SignalingState::kHaveRemoteOffer: return "HaveRemoteOffer";
        case SignalingState::kHaveLocalPrAnswer: return "HaveLocalPranswer";
        case SignalingState::kHaveRemotePrAnswer: return "HaveRemotePranswer";
        default: break;
    }
    OCTK_CHECK_NOTREACHED();
    return "";
}

Result<RtcSessionDescription::Data> RtcPeerConnection::createOffer(const RtcMediaConstraints::SharedPtr &constraints)
{
    std::promise<Result<RtcSessionDescription::Data>> promise;
    this->createOffer(
        [&promise](const char *sdp, const char *type)
        {
            RtcSessionDescription::Data data{sdp, type};
            promise.set_value(data);
        },
        [&promise](const char *error) { promise.set_value(Error::create(error)); },
        constraints);
    return promise.get_future().get();
}

Result<RtcSessionDescription::Data> RtcPeerConnection::createAnswer(const RtcMediaConstraints::SharedPtr &constraints)
{
    std::promise<Result<RtcSessionDescription::Data>> promise;
    this->createAnswer(
        [&promise](const char *sdp, const char *type)
        {
            RtcSessionDescription::Data data{sdp, type};
            promise.set_value(data);
        },
        [&promise](const char *error) { promise.set_value(Error::create(error)); },
        constraints);
    return promise.get_future().get();
}

Status RtcPeerConnection::setLocalDescription(StringView sdp, StringView type)
{
    std::promise<Status> promise;
    this->setLocalDescription(
        sdp.data(),
        type.data(),
        [&promise]() { promise.set_value(Status::ok); },
        [&promise](const char *error) { promise.set_value(Error::create(error)); });
    return promise.get_future().get();
}

Status RtcPeerConnection::setRemoteDescription(StringView sdp, StringView type)
{
    std::promise<Status> promise;
    this->setRemoteDescription(
        sdp.data(),
        type.data(),
        [&promise]() { promise.set_value(Status::ok); },
        [&promise](const char *error) { promise.set_value(Error::create(error)); });
    return promise.get_future().get();
}


Result<RtcSessionDescription::Data> RtcPeerConnection::getLocalDescription()
{
    std::promise<Result<RtcSessionDescription::Data>> promise;
    this->getLocalDescription(
        [&promise](const char *sdp, const char *type)
        {
            RtcSessionDescription::Data data{sdp, type};
            promise.set_value(data);
        },
        [&promise](const char *error) { promise.set_value(Error::create(error)); });
    return promise.get_future().get();
}

Result<RtcSessionDescription::Data> RtcPeerConnection::getRemoteDescription()
{
    std::promise<Result<RtcSessionDescription::Data>> promise;
    this->getRemoteDescription(
        [&promise](const char *sdp, const char *type)
        {
            RtcSessionDescription::Data data{sdp, type};
            promise.set_value(data);
        },
        [&promise](const char *error) { promise.set_value(Error::create(error)); });
    return promise.get_future().get();
}

OCTK_END_NAMESPACE
