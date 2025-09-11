/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
** Copyright 2016 The WebRTC Project Authors.
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

#ifndef _OCTK_RTC_STATS_OBJECTS_HPP
#define _OCTK_RTC_STATS_OBJECTS_HPP

#include <octk_rtc_stats_report.hpp>

OCTK_BEGIN_NAMESPACE

// https://w3c.github.io/webrtc-stats/#certificatestats-dict*
class OCTK_MEDIA_API RtcCertificateStats final : public RtcStats
{
public:
    OCTK_DECLARE_RTCSTATS();
    RtcCertificateStats(std::string id, Timestamp timestamp);
    ~RtcCertificateStats() override;

    Optional<std::string> fingerprint;
    Optional<std::string> fingerprintAlgorithm;
    Optional<std::string> base64Certificate;
    Optional<std::string> issuerCertificateId;
};

// https://w3c.github.io/webrtc-stats/#codec-dict*
class OCTK_MEDIA_API RtcCodecStats final : public RtcStats
{
public:
    OCTK_DECLARE_RTCSTATS();
    RtcCodecStats(std::string id, Timestamp timestamp);
    ~RtcCodecStats() override;

    Optional<std::string> transportId;
    Optional<uint32_t> payloadType;
    Optional<std::string> mimeType;
    Optional<uint32_t> clockRate;
    Optional<uint32_t> channels;
    Optional<std::string> sdpFmtpLine;
};

// https://w3c.github.io/webrtc-stats/#dcstats-dict*
class OCTK_MEDIA_API RtcDataChannelStats final : public RtcStats
{
public:
    OCTK_DECLARE_RTCSTATS();
    RtcDataChannelStats(std::string id, Timestamp timestamp);
    ~RtcDataChannelStats() override;

    Optional<std::string> label;
    Optional<std::string> protocol;
    Optional<int32_t> dataChannelIdentifier;
    Optional<std::string> state;
    Optional<uint32_t> messagesSent;
    Optional<uint64_t> bytesSent;
    Optional<uint32_t> messagesReceived;
    Optional<uint64_t> bytesReceived;
};

// https://w3c.github.io/webrtc-stats/#candidatepair-dict*
class OCTK_MEDIA_API RtcIceCandidatePairStats final : public RtcStats
{
public:
    OCTK_DECLARE_RTCSTATS();
    RtcIceCandidatePairStats(std::string id, Timestamp timestamp);
    ~RtcIceCandidatePairStats() override;

    Optional<std::string> transportId;
    Optional<std::string> localCandidateId;
    Optional<std::string> remoteCandidateId;
    Optional<std::string> state;
    // Obsolete: priority
    Optional<uint64_t> priority;
    Optional<bool> nominated;
    // `writable` does not exist in the spec and old comments suggest it used to
    // exist but was incorrectly implemented.
    // TODO(https://crbug.com/webrtc/14171): Standardize and/or modify
    // implementation.
    Optional<bool> writable;
    Optional<uint64_t> packetsSent;
    Optional<uint64_t> packetsReceived;
    Optional<uint64_t> bytesSent;
    Optional<uint64_t> bytesReceived;
    Optional<double> totalRoundTripTime;
    Optional<double> currentRoundTripTime;
    Optional<double> availableOutgoingBitrate;
    Optional<double> availableIncomingBitrate;
    Optional<uint64_t> requestsReceived;
    Optional<uint64_t> requestsSent;
    Optional<uint64_t> responsesReceived;
    Optional<uint64_t> responsesSent;
    Optional<uint64_t> consentRequestsSent;
    Optional<uint64_t> packetsDiscardedOnSend;
    Optional<uint64_t> bytesDiscardedOnSend;
    Optional<double> lastPacketReceivedTimestamp;
    Optional<double> lastPacketSentTimestamp;
};

// https://w3c.github.io/webrtc-stats/#icecandidate-dict*
class OCTK_MEDIA_API RtcIceCandidateStats : public RtcStats
{
public:
    OCTK_DECLARE_RTCSTATS();
    ~RtcIceCandidateStats() override;

    Optional<std::string> transportId;
    // Obsolete: is_remote
    Optional<bool> isRemote;
    Optional<std::string> networkType;
    Optional<std::string> ip;
    Optional<std::string> address;
    Optional<int32_t> port;
    Optional<std::string> protocol;
    Optional<std::string> relayProtocol;
    Optional<std::string> candidateType;
    Optional<int32_t> priority;
    Optional<std::string> url;
    Optional<std::string> foundation;
    Optional<std::string> relatedAddress;
    Optional<int32_t> relatedPort;
    Optional<std::string> usernameFragment;
    Optional<std::string> tcpType;

    // The following metrics are NOT exposed to JavaScript. We should consider
    // standardizing or removing them.
    Optional<bool> vpn;
    Optional<std::string> networkAdapterType;

protected:
    RtcIceCandidateStats(std::string id, Timestamp timestamp, bool isRemote);
};

// In the spec both local and remote varieties are of type RtcIceCandidateStats.
// But here we define them as subclasses of `RtcIceCandidateStats` because the
// `kType` need to be different ("RtcStatsType type") in the local/remote case.
// https://w3c.github.io/webrtc-stats/#rtcstatstype-str*
// This forces us to have to override copy() and type().
class OCTK_MEDIA_API RtcLocalIceCandidateStats final : public RtcIceCandidateStats
{
public:
    static const char kType[];
    RtcLocalIceCandidateStats(std::string id, Timestamp timestamp);
    std::unique_ptr<RtcStats> copy() const override;
    const char *type() const override;
};

class OCTK_MEDIA_API RtcRemoteIceCandidateStats final : public RtcIceCandidateStats
{
public:
    static const char kType[];
    RtcRemoteIceCandidateStats(std::string id, Timestamp timestamp);
    std::unique_ptr<RtcStats> copy() const override;
    const char *type() const override;
};

// https://w3c.github.io/webrtc-stats/#pcstats-dict*
class OCTK_MEDIA_API RtcPeerConnectionStats final : public RtcStats
{
public:
    OCTK_DECLARE_RTCSTATS();
    RtcPeerConnectionStats(std::string id, Timestamp timestamp);
    ~RtcPeerConnectionStats() override;

    Optional<uint32_t> dataChannelsOpened;
    Optional<uint32_t> dataChannelsClosed;
};

// https://w3c.github.io/webrtc-stats/#streamstats-dict*
class OCTK_MEDIA_API RtcRtpStreamStats : public RtcStats
{
public:
    OCTK_DECLARE_RTCSTATS();
    ~RtcRtpStreamStats() override;

    Optional<uint32_t> ssrc;
    Optional<std::string> kind;
    Optional<std::string> transportId;
    Optional<std::string> codecId;

protected:
    RtcRtpStreamStats(std::string id, Timestamp timestamp);
};

// https://www.w3.org/TR/webrtc-stats/#receivedrtpstats-dict*
class OCTK_MEDIA_API RtcReceivedRtpStreamStats : public RtcRtpStreamStats
{
public:
    OCTK_DECLARE_RTCSTATS();
    ~RtcReceivedRtpStreamStats() override;

    Optional<double> jitter;
    Optional<int32_t> packetsLost; // Signed per RFC 3550

protected:
    RtcReceivedRtpStreamStats(std::string id, Timestamp timestamp);
};

// https://www.w3.org/TR/webrtc-stats/#sentrtpstats-dict*
class OCTK_MEDIA_API RtcSentRtpStreamStats : public RtcRtpStreamStats
{
public:
    OCTK_DECLARE_RTCSTATS();
    ~RtcSentRtpStreamStats() override;

    Optional<uint64_t> packetsSent;
    Optional<uint64_t> bytesSent;

protected:
    RtcSentRtpStreamStats(std::string id, Timestamp timestamp);
};

// https://w3c.github.io/webrtc-stats/#inboundrtpstats-dict*
class OCTK_MEDIA_API RtcInboundRtpStreamStats final : public RtcReceivedRtpStreamStats
{
public:
    OCTK_DECLARE_RTCSTATS();
    RtcInboundRtpStreamStats(std::string id, Timestamp timestamp);
    ~RtcInboundRtpStreamStats() override;

    Optional<std::string> playoutId;
    Optional<std::string> trackIdentifier;
    Optional<std::string> mid;
    Optional<std::string> remoteId;
    Optional<uint32_t> packetsReceived;
    Optional<uint64_t> packetsDiscarded;
    Optional<uint64_t> fecPacketsReceived;
    Optional<uint64_t> fecBytesReceived;
    Optional<uint64_t> fecPacketsDiscarded;
    // Inbound FEC SSRC. Only present if a mechanism like FlexFEC is negotiated.
    Optional<uint32_t> fecSsrc;
    Optional<uint64_t> bytesReceived;
    Optional<uint64_t> headerBytesReceived;
    // Inbound RTX stats. Only defined when RTX is used and it is therefore
    // possible to distinguish retransmissions.
    Optional<uint64_t> retransmittedPacketsReceived;
    Optional<uint64_t> retransmittedBytesReceived;
    Optional<uint32_t> rtxSsrc;

    Optional<double> lastPacketReceivedTimestamp;
    Optional<double> jitterBufferDelay;
    Optional<double> jitterBufferTargetDelay;
    Optional<double> jitterBufferMinimumDelay;
    Optional<uint64_t> jitterBufferEmittedCount;
    Optional<uint64_t> totalSamplesReceived;
    Optional<uint64_t> concealedSamples;
    Optional<uint64_t> silentConcealedSamples;
    Optional<uint64_t> concealmentEvents;
    Optional<uint64_t> insertedSamplesForDeceleration;
    Optional<uint64_t> removedSamplesForAcceleration;
    Optional<double> audioLevel;
    Optional<double> totalAudioEnergy;
    Optional<double> totalSamplesDuration;
    // Stats below are only implemented or defined for video.
    Optional<uint32_t> framesReceived;
    Optional<uint32_t> frameWidth;
    Optional<uint32_t> frameHeight;
    Optional<double> framesPerSecond;
    Optional<uint32_t> framesDecoded;
    Optional<uint32_t> keyFramesDecoded;
    Optional<uint32_t> framesDropped;
    Optional<double> totalDecodeTime;
    Optional<double> totalProcessingDelay;
    Optional<double> totalAssemblyTime;
    Optional<uint32_t> framesAssembledFromMultiplePackets;
    // TODO(https://crbug.com/webrtc/15600): Implement framesRendered, which is
    // incremented at the same time that totalInterFrameDelay and
    // totalSquaredInterFrameDelay is incremented. (Dividing inter-frame delay by
    // framesDecoded is slightly wrong.)
    // https://w3c.github.io/webrtc-stats/#dom-rtcinboundrtpstreamstats-framesrendered
    //
    // TODO(https://crbug.com/webrtc/15601): Inter-frame, pause and freeze metrics
    // all related to when the frame is rendered, but our implementation measures
    // at delivery to sink, not at actual render time. When we have an actual
    // frame rendered callback, move the calculating of these metrics to there in
    // order to make them more accurate.
    Optional<double> totalInterFrameDelay;
    Optional<double> totalSquaredInterFrameDelay;
    Optional<uint32_t> pauseCount;
    Optional<double> totalPausesDuration;
    Optional<uint32_t> freezeCount;
    Optional<double> totalFreezesDuration;
    // https://w3c.github.io/webrtc-provisional-stats/#dom-rtcinboundrtpstreamstats-contenttype
    Optional<std::string> contentType;
    // Only populated if audio/video sync is enabled.
    // TODO(https://crbug.com/webrtc/14177): Expose even if A/V sync is off?
    Optional<double> estimatedPlayoutTimestamp;
    // Only defined for video.
    // In JavaScript, this is only exposed if HW exposure is allowed.
    Optional<std::string> decoderImplementation;
    // FIR and PLI counts are only defined for |kind == "video"|.
    Optional<uint32_t> firCount;
    Optional<uint32_t> pliCount;
    Optional<uint32_t> nackCount;
    Optional<uint64_t> qpSum;
    Optional<double> totalCorruptionProbability;
    Optional<double> totalSquaredCorruptionProbability;
    Optional<uint64_t> corruptionMeasurements;
    // This is a remnant of the legacy getStats() API. When the "video-timing"
    // header extension is used,
    // https://webrtc.github.io/webrtc-org/experiments/rtp-hdrext/video-timing/,
    // `googTimingFrameInfo` is exposed with the value of
    // TimingFrameInfo::ToString().
    // TODO(https://crbug.com/webrtc/14586): Unship or standardize this metric.
    Optional<std::string> googTimingFrameInfo;
    // In JavaScript, this is only exposed if HW exposure is allowed.
    Optional<bool> powerEfficientDecoder;

    // The following metrics are NOT exposed to JavaScript. We should consider
    // standardizing or removing them.
    Optional<uint64_t> jitterBufferFlushes;
    Optional<uint64_t> delayedPacketOutageSamples;
    Optional<double> relativePacketArrivalDelay;
    Optional<uint32_t> interruptionCount;
    Optional<double> totalInterruptionDuration;
    Optional<double> minPlayoutDelay;
};

// https://w3c.github.io/webrtc-stats/#outboundrtpstats-dict*
class OCTK_MEDIA_API RtcOutboundRtpStreamStats final : public RtcSentRtpStreamStats
{
public:
    OCTK_DECLARE_RTCSTATS();
    RtcOutboundRtpStreamStats(std::string id, Timestamp timestamp);
    ~RtcOutboundRtpStreamStats() override;

    Optional<std::string> mediaSourceId;
    Optional<std::string> remoteId;
    Optional<std::string> mid;
    Optional<std::string> rid;
    Optional<uint64_t> retransmittedPacketsSent;
    Optional<uint64_t> headerBytesSent;
    Optional<uint64_t> retransmittedBytesSent;
    Optional<double> targetBitrate;
    Optional<uint32_t> framesEncoded;
    Optional<uint32_t> keyFramesEncoded;
    Optional<double> totalEncodeTime;
    Optional<uint64_t> totalEncodedBytesTarget;
    Optional<uint32_t> frameWidth;
    Optional<uint32_t> frameHeight;
    Optional<double> framesPerSecond;
    Optional<uint32_t> framesSent;
    Optional<uint32_t> hugeFramesSent;
    Optional<double> totalPacketSendDelay;
    Optional<std::string> qualityLimitationReason;
    Optional<std::map<std::string, double>> qualityLimitationDurations;
    // https://w3c.github.io/webrtc-stats/#dom-rtcoutboundrtpstreamstats-qualitylimitationresolutionchanges
    Optional<uint32_t> qualityLimitationResolutionChanges;
    // https://w3c.github.io/webrtc-provisional-stats/#dom-rtcoutboundrtpstreamstats-contenttype
    Optional<std::string> contentType;
    // In JavaScript, this is only exposed if HW exposure is allowed.
    // Only implemented for video.
    // TODO(https://crbug.com/webrtc/14178): Implement for audio as well.
    Optional<std::string> encoderImplementation;
    // FIR and PLI counts are only defined for |kind == "video"|.
    Optional<uint32_t> firCount;
    Optional<uint32_t> pliCount;
    Optional<uint32_t> nackCount;
    Optional<uint64_t> qpSum;
    Optional<bool> active;
    // In JavaScript, this is only exposed if HW exposure is allowed.
    Optional<bool> powerEfficientEncoder;
    Optional<std::string> scalabilityMode;

    // RTX ssrc. Only present if RTX is negotiated.
    Optional<uint32_t> rtxSsrc;
};

// https://w3c.github.io/webrtc-stats/#remoteinboundrtpstats-dict*
class OCTK_MEDIA_API RtcRemoteInboundRtpStreamStats final : public RtcReceivedRtpStreamStats
{
public:
    OCTK_DECLARE_RTCSTATS();
    RtcRemoteInboundRtpStreamStats(std::string id, Timestamp timestamp);
    ~RtcRemoteInboundRtpStreamStats() override;

    Optional<std::string> localId;
    Optional<double> roundTripTime;
    Optional<double> fractionLost;
    Optional<double> totalRoundTripTime;
    Optional<int32_t> roundTripTimeMeasurements;
};

// https://w3c.github.io/webrtc-stats/#remoteoutboundrtpstats-dict*
class OCTK_MEDIA_API RtcRemoteOutboundRtpStreamStats final : public RtcSentRtpStreamStats
{
public:
    OCTK_DECLARE_RTCSTATS();
    RtcRemoteOutboundRtpStreamStats(std::string id, Timestamp timestamp);
    ~RtcRemoteOutboundRtpStreamStats() override;

    Optional<std::string> localId;
    Optional<double> remoteTimestamp;
    Optional<uint64_t> reportsSent;
    Optional<double> roundTripTime;
    Optional<uint64_t> roundTripTimeMeasurements;
    Optional<double> totalRoundTripTime;
};

// https://w3c.github.io/webrtc-stats/#dom-rtcmediasourcestats
class OCTK_MEDIA_API RtcMediaSourceStats : public RtcStats
{
public:
    OCTK_DECLARE_RTCSTATS();
    ~RtcMediaSourceStats() override;

    Optional<std::string> trackIdentifier;
    Optional<std::string> kind;

protected:
    RtcMediaSourceStats(std::string id, Timestamp timestamp);
};

// https://w3c.github.io/webrtc-stats/#dom-rtcaudiosourcestats
class OCTK_MEDIA_API RtcAudioSourceStats final : public RtcMediaSourceStats
{
public:
    OCTK_DECLARE_RTCSTATS();
    RtcAudioSourceStats(std::string id, Timestamp timestamp);
    ~RtcAudioSourceStats() override;

    Optional<double> audioLevel;
    Optional<double> totalAudioEnergy;
    Optional<double> totalSamplesDuration;
    Optional<double> echoReturnLoss;
    Optional<double> echoReturnLossEnhancement;
};

// https://w3c.github.io/webrtc-stats/#dom-rtcvideosourcestats
class OCTK_MEDIA_API RtcVideoSourceStats final : public RtcMediaSourceStats
{
public:
    OCTK_DECLARE_RTCSTATS();
    RtcVideoSourceStats(std::string id, Timestamp timestamp);
    ~RtcVideoSourceStats() override;

    Optional<uint32_t> width;
    Optional<uint32_t> height;
    Optional<uint32_t> frames;
    Optional<double> framesPerSecond;
};

// https://w3c.github.io/webrtc-stats/#transportstats-dict*
class OCTK_MEDIA_API RtcTransportStats final : public RtcStats
{
public:
    OCTK_DECLARE_RTCSTATS();
    RtcTransportStats(std::string id, Timestamp timestamp);
    ~RtcTransportStats() override;

    Optional<uint64_t> bytesSent;
    Optional<uint64_t> packetsSent;
    Optional<uint64_t> bytesReceived;
    Optional<uint64_t> packetsReceived;
    Optional<std::string> rtcpTransportStatsId;
    Optional<std::string> dtlsState;
    Optional<std::string> selectedCandidatePairId;
    Optional<std::string> localCertificateId;
    Optional<std::string> remoteCertificateId;
    Optional<std::string> tlsVersion;
    Optional<std::string> dtlsCipher;
    Optional<std::string> dtlsRole;
    Optional<std::string> srtpCipher;
    Optional<uint32_t> selectedCandidatePairChanges;
    Optional<std::string> iceRole;
    Optional<std::string> iceLocalUsernameFragment;
    Optional<std::string> iceState;
};

// https://w3c.github.io/webrtc-stats/#playoutstats-dict*
class OCTK_MEDIA_API RtcAudioPlayoutStats final : public RtcStats
{
public:
    OCTK_DECLARE_RTCSTATS();
    RtcAudioPlayoutStats(const std::string &id, Timestamp timestamp);
    ~RtcAudioPlayoutStats() override;

    Optional<std::string> kind;
    Optional<double> synthesizedSamplesDuration;
    Optional<uint64_t> synthesizedSamplesEvents;
    Optional<double> totalSamplesDuration;
    Optional<double> totalPlayoutDelay;
    Optional<uint64_t> totalSamplesCount;
};

OCTK_END_NAMESPACE

#endif // _OCTK_RTC_STATS_OBJECTS_HPP
