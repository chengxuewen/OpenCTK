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

#ifndef _OCTK_RTP_RECEIVER_INTERFACE_HPP
#define _OCTK_RTP_RECEIVER_INTERFACE_HPP


#include <octk_frame_transformer_interface.hpp>
#include <octk_frame_decryptor_interface.hpp>
#include <octk_media_stream_interface.hpp>
#include <octk_rtp_parameters.hpp>
#include <octk_shared_ref_ptr.hpp>
#include <octk_media_types.hpp>
#include <octk_rtp_source.hpp>
#include <octk_ref_count.hpp>

#include <utility>
#include <string>
#include <vector>

// #include "api/dtls_transport_interface.h"

OCTK_BEGIN_NAMESPACE

class RtpReceiverObserverInterface
{
public:
    // Note: Currently if there are multiple RtpReceivers of the same media type,
    // they will all call OnFirstPacketReceived at once.
    //
    // In the future, it's likely that an RtpReceiver will only call
    // OnFirstPacketReceived when a packet is received specifically for its
    // SSRC/mid.
    virtual void OnFirstPacketReceived(MediaType media_type) = 0;

protected:
    virtual ~RtpReceiverObserverInterface() { }
};

class OCTK_MEDIA_API RtpReceiverInterface : public RefCountInterface, public FrameTransformerHost
{
public:
    virtual SharedRefPtr<MediaStreamTrackInterface> track() const = 0;

    // The dtlsTransport attribute exposes the DTLS transport on which the
    // media is received. It may be null.
    // https://w3c.github.io/webrtc-pc/#dom-rtcrtpreceiver-transport
    // TODO(https://bugs.webrtc.org/907849) remove default implementation
    // virtual SharedRefPtr<DtlsTransportInterface> dtls_transport() const;

    // The list of streams that `track` is associated with. This is the same as
    // the [[AssociatedRemoteMediaStreams]] internal slot in the spec.
    // https://w3c.github.io/webrtc-pc/#dfn-associatedremotemediastreams
    // TODO(hbos): Make pure virtual as soon as Chromium's mock implements this.
    // TODO(https://crbug.com/webrtc/9480): Remove streams() in favor of
    // stream_ids() as soon as downstream projects are no longer dependent on
    // stream objects.
    virtual std::vector<std::string> stream_ids() const;
//    virtual std::vector<SharedRefPtr<MediaStreamInterface>> streams() const;

    // Audio or video receiver?
    virtual MediaType media_type() const = 0;

    // Not to be confused with "mid", this is a field we can temporarily use
    // to uniquely identify a receiver until we implement Unified Plan SDP.
    virtual std::string id() const = 0;

    // The WebRTC specification only defines RTCRtpParameters in terms of senders,
    // but this API also applies them to receivers, similar to ORTC:
    // http://ortc.org/wp-content/uploads/2016/03/ortc.html#rtcrtpparameters*.
    virtual RtpParameters GetParameters() const = 0;
    // TODO(dinosaurav): Delete SetParameters entirely after rolling to Chromium.
    // Currently, doesn't support changing any parameters.
    virtual bool SetParameters(const RtpParameters & /* parameters */) { return false; }

    // Does not take ownership of observer.
    // Must call SetObserver(nullptr) before the observer is destroyed.
    virtual void SetObserver(RtpReceiverObserverInterface *observer) = 0;

    // Sets the jitter buffer minimum delay until media playout. Actual observed
    // delay may differ depending on the congestion control. `delay_seconds` is a
    // positive value including 0.0 measured in seconds. `nullopt` means default
    // value must be used.
    virtual void SetJitterBufferMinimumDelay(Optional<double> delay_seconds) = 0;

    // TODO(zhihuang): Remove the default implementation once the subclasses
    // implement this. Currently, the only relevant subclass is the
    // content::FakeRtpReceiver in Chromium.
    virtual std::vector<RtpSource> GetSources() const;

    // Sets a user defined frame decryptor that will decrypt the entire frame
    // before it is sent across the network. This will decrypt the entire frame
    // using the user provided decryption mechanism regardless of whether SRTP is
    // enabled or not.
    // TODO(bugs.webrtc.org/12772): Remove.
    virtual void SetFrameDecryptor(SharedRefPtr<FrameDecryptorInterface> frame_decryptor);

    // Returns a pointer to the frame decryptor set previously by the
    // user. This can be used to update the state of the object.
    // TODO(bugs.webrtc.org/12772): Remove.
    virtual SharedRefPtr<FrameDecryptorInterface> GetFrameDecryptor() const;

    // Sets a frame transformer between the depacketizer and the decoder to enable
    // client code to transform received frames according to their own processing
    // logic.
    // TODO: bugs.webrtc.org/15929 - add [[deprecated("Use SetFrameTransformer")]]
    // when usage in Chrome is removed
    virtual void SetDepacketizerToDecoderFrameTransformer(SharedRefPtr<FrameTransformerInterface> frame_transformer)
    {
        SetFrameTransformer(std::move(frame_transformer));
    }

    // Default implementation of SetFrameTransformer.
    // TODO: bugs.webrtc.org/15929 - Make pure virtual.
    void SetFrameTransformer(SharedRefPtr<FrameTransformerInterface> frame_transformer) override;

protected:
    ~RtpReceiverInterface() override = default;
};

OCTK_END_NAMESPACE

#endif // _OCTK_RTP_RECEIVER_INTERFACE_HPP
