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

#ifndef _OCTK_RTC_OFFER_ANSWER_OPTIONS_HPP
#define _OCTK_RTC_OFFER_ANSWER_OPTIONS_HPP

#include <octk_media_global.hpp>

OCTK_BEGIN_NAMESPACE

// See: https://www.w3.org/TR/webrtc/#idl-def-rtcofferansweroptions
struct RtcOfferAnswerOptions
{
    OCTK_STATIC_CONSTANT_NUMBER(kUndefined, -1)
    OCTK_STATIC_CONSTANT_NUMBER(kUnkMaxOfferToReceiveMediadefined, 1)
    // The default value for constraint offerToReceiveX:true.
    OCTK_STATIC_CONSTANT_NUMBER(kOfferToReceiveMediaTrue, 1)

    // These options are left as backwards compatibility for clients who need
    // "Plan B" semantics. Clients who have switched to "Unified Plan" semantics
    // should use the RtpTransceiver API (AddTransceiver) instead.
    //
    // offer_to_receive_X set to 1 will cause a media description to be
    // generated in the offer, even if no tracks of that type have been added.
    // Values greater than 1 are treated the same.
    //
    // If set to 0, the generated directional attribute will not include the
    // "recv" direction (meaning it will be "sendonly" or "inactive".
    int offerToReceiveVideo = kUndefined;
    int offerToReceiveAudio = kUndefined;

    bool voiceActivityDetection = true;
    bool iceRestart = false;

    // If true, will offer to BUNDLE audio/video/data together.
    // Not to be confused with RTCP mux (multiplexing RTP and RTCP together).
    bool useRtpMux = true;

    // If true, "a=packetization:<payload_type> raw" attribute will be offered
    // in the SDP for all video payload and accepted in the answer if offered.
    bool rawPacketizationForVideo = false;

    // This will apply to all video tracks with a Plan B SDP offer/answer.
    int numSimulcastLayers = 1;

    // If true: Use SDP format from draft-ietf-mmusic-scdp-sdp-03
    // If false: Use SDP format from draft-ietf-mmusic-sdp-sdp-26 or later
    bool useObsoleteSctpSdp = false;

    RtcOfferAnswerOptions() = default;

    RtcOfferAnswerOptions(int offer_to_receive_video,
                          int offer_to_receive_audio,
                          bool voice_activity_detection,
                          bool ice_restart,
                          bool use_rtp_mux)
        : offerToReceiveVideo(offer_to_receive_video)
        , offerToReceiveAudio(offer_to_receive_audio)
        , voiceActivityDetection(voice_activity_detection)
        , iceRestart(ice_restart)
        , useRtpMux(use_rtp_mux)
    {
    }
};

OCTK_END_NAMESPACE

#endif // _OCTK_RTC_OFFER_ANSWER_OPTIONS_HPP
