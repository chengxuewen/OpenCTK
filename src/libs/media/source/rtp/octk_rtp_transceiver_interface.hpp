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

#ifndef _OCTK_RTP_TRANSCEIVER_INTERFACE_HPP
#define _OCTK_RTP_TRANSCEIVER_INTERFACE_HPP

#include <octk_rtp_transceiver_direction.hpp>
#include <octk_rtp_receiver_interface.hpp>
#include <octk_rtp_sender_interface.hpp>
#include <octk_ref_counted_object.hpp>
#include <octk_rtp_parameters.hpp>
#include <octk_media_types.hpp>
#include <octk_rtc_error.hpp>
#include <octk_optional.hpp>

OCTK_BEGIN_NAMESPACE

// Structure for initializing an RtpTransceiver in a call to
// PeerConnectionInterface::AddTransceiver.
// https://w3c.github.io/webrtc-pc/#dom-rtcrtptransceiverinit
struct OCTK_MEDIA_API RtpTransceiverInit final
{
    RtpTransceiverInit();
    RtpTransceiverInit(const RtpTransceiverInit &);
    ~RtpTransceiverInit();
    // Direction of the RtpTransceiver. See RtpTransceiverInterface::direction().
    RtpTransceiverDirection direction = RtpTransceiverDirection::SendRecv;

    // The added RtpTransceiver will be added to these streams.
    std::vector<std::string> streamIds;

    std::vector<RtpEncodingParameters> sendEncodings;
};

// The RtpTransceiverInterface maps to the RTCRtpTransceiver defined by the
// WebRTC specification. A transceiver represents a combination of an RtpSender
// and an RtpReceiver than share a common mid. As defined in JSEP, an
// RtpTransceiver is said to be associated with a media description if its mid
// property is non-null; otherwise, it is said to be disassociated.
// JSEP: https://tools.ietf.org/html/draft-ietf-rtcweb-jsep-24
//
// Note that RtpTransceivers are only supported when using PeerConnection with
// Unified Plan SDP.
//
// This class is thread-safe.
//
// WebRTC specification for RTCRtpTransceiver, the JavaScript analog:
// https://w3c.github.io/webrtc-pc/#dom-rtcrtptransceiver
class OCTK_MEDIA_API RtpTransceiverInterface : public RefCountInterface
{
public:
    // Media type of the transceiver. Any sender(s)/receiver(s) will have this
    // type as well.
    virtual MediaType media_type() const = 0;

    // The mid attribute is the mid negotiated and present in the local and
    // remote descriptions. Before negotiation is complete, the mid value may be
    // null. After rollbacks, the value may change from a non-null value to null.
    // https://w3c.github.io/webrtc-pc/#dom-rtcrtptransceiver-mid
    virtual Optional<std::string> mid() const = 0;

    // The sender attribute exposes the RtpSender corresponding to the RTP media
    // that may be sent with the transceiver's mid. The sender is always present,
    // regardless of the direction of media.
    // https://w3c.github.io/webrtc-pc/#dom-rtcrtptransceiver-sender
    virtual ScopedRefPtr<RtpSenderInterface> sender() const = 0;

    // The receiver attribute exposes the RtpReceiver corresponding to the RTP
    // media that may be received with the transceiver's mid. The receiver is
    // always present, regardless of the direction of media.
    // https://w3c.github.io/webrtc-pc/#dom-rtcrtptransceiver-receiver
    virtual ScopedRefPtr<RtpReceiverInterface> receiver() const = 0;

    // The stopped attribute indicates that the sender of this transceiver will no
    // longer send, and that the receiver will no longer receive. It is true if
    // either stop has been called or if setting the local or remote description
    // has caused the RtpTransceiver to be stopped.
    // https://w3c.github.io/webrtc-pc/#dom-rtcrtptransceiver-stopped
    virtual bool stopped() const = 0;

    // The stopping attribute indicates that the user has indicated that the
    // sender of this transceiver will stop sending, and that the receiver will
    // no longer receive. It is always true if stopped() is true.
    // If stopping() is true and stopped() is false, it means that the
    // transceiver's stop() method has been called, but the negotiation with
    // the other end for shutting down the transceiver is not yet done.
    // https://w3c.github.io/webrtc-pc/#dfn-stopping-0
    virtual bool stopping() const = 0;

    // The direction attribute indicates the preferred direction of this
    // transceiver, which will be used in calls to CreateOffer and CreateAnswer.
    // https://w3c.github.io/webrtc-pc/#dom-rtcrtptransceiver-direction
    virtual RtpTransceiverDirection direction() const = 0;

    // Sets the preferred direction of this transceiver. An update of
    // directionality does not take effect immediately. Instead, future calls to
    // CreateOffer and CreateAnswer mark the corresponding media descriptions as
    // sendrecv, sendonly, recvonly, or inactive.
    // https://w3c.github.io/webrtc-pc/#dom-rtcrtptransceiver-direction
    // TODO(hta): Deprecate SetDirection without error and rename
    // SetDirectionWithError to SetDirection, remove default implementations.
    ABSL_DEPRECATED("Use SetDirectionWithError instead")
    virtual void SetDirection(RtpTransceiverDirection new_direction);
    virtual RTCError SetDirectionWithError(RtpTransceiverDirection new_direction);

    // The current_direction attribute indicates the current direction negotiated
    // for this transceiver. If this transceiver has never been represented in an
    // offer/answer exchange, or if the transceiver is stopped, the value is null.
    // https://w3c.github.io/webrtc-pc/#dom-rtcrtptransceiver-currentdirection
    virtual Optional<RtpTransceiverDirection> current_direction() const = 0;

    // An internal slot designating for which direction the relevant
    // PeerConnection events have been fired. This is to ensure that events like
    // OnAddTrack only get fired once even if the same session description is
    // applied again.
    // Exposed in the public interface for use by Chromium.
    virtual Optional<RtpTransceiverDirection> fired_direction() const;

    // Initiates a stop of the transceiver.
    // The stop is complete when stopped() returns true.
    // A stopped transceiver can be reused for a different track.
    // https://w3c.github.io/webrtc-pc/#dom-rtcrtptransceiver-stop
    // TODO(hta): Rename to Stop() when users of the non-standard Stop() are
    // updated.
    virtual RTCError StopStandard();

    // Stops a transceiver immediately, without waiting for signalling.
    // This is an internal function, and is exposed for historical reasons.
    // https://w3c.github.io/webrtc-pc/#dfn-stop-the-rtcrtptransceiver
    virtual void StopInternal();
    ABSL_DEPRECATED("Use StopStandard instead") virtual void Stop();

    // The SetCodecPreferences method overrides the default codec preferences used
    // by WebRTC for this transceiver.
    // https://w3c.github.io/webrtc-pc/#dom-rtcrtptransceiver-setcodecpreferences
    virtual RTCError SetCodecPreferences(ArrayView<RtpCodecCapability> codecs) = 0;
    virtual std::vector<RtpCodecCapability> codec_preferences() const = 0;

    // Returns the set of header extensions that was set
    // with SetHeaderExtensionsToNegotiate, or a default set if it has not been
    // called.
    // https://w3c.github.io/webrtc-extensions/#rtcrtptransceiver-interface
    virtual std::vector<RtpHeaderExtensionCapability> GetHeaderExtensionsToNegotiate() const = 0;

    // Returns either the empty set if negotation has not yet
    // happened, or a vector of the negotiated header extensions.
    // https://w3c.github.io/webrtc-extensions/#rtcrtptransceiver-interface
    virtual std::vector<RtpHeaderExtensionCapability> GetNegotiatedHeaderExtensions() const = 0;

    // The SetHeaderExtensionsToNegotiate method modifies the next SDP negotiation
    // so that it negotiates use of header extensions which are not kStopped.
    // https://w3c.github.io/webrtc-extensions/#rtcrtptransceiver-interface
    virtual RTCError SetHeaderExtensionsToNegotiate(ArrayView<const RtpHeaderExtensionCapability> headerExtensions) = 0;

protected:
    ~RtpTransceiverInterface() override = default;
};

OCTK_END_NAMESPACE

#endif // _OCTK_RTP_TRANSCEIVER_INTERFACE_HPP
