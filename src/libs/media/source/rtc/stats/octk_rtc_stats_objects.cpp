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

#include <octk_rtc_stats_objects.hpp>

OCTK_BEGIN_NAMESPACE

OCTK_IMPLEMENT_RTCSTATS(RtcCertificateStats,
                        RtcStats,
                        "certificate",
                        AttributeInit("fingerprint", &fingerprint),
                        AttributeInit("fingerprintAlgorithm", &fingerprintAlgorithm),
                        AttributeInit("base64Certificate", &base64Certificate),
                        AttributeInit("issuerCertificateId", &issuerCertificateId))

RtcCertificateStats::RtcCertificateStats(std::string id, Timestamp timestamp)
    : RtcStats(std::move(id), timestamp)
{
}

RtcCertificateStats::~RtcCertificateStats() { }

OCTK_IMPLEMENT_RTCSTATS(RtcCodecStats,
                        RtcStats,
                        "codec",
                        AttributeInit("transportId", &transportId),
                        AttributeInit("payloadType", &payloadType),
                        AttributeInit("mimeType", &mimeType),
                        AttributeInit("clockRate", &clockRate),
                        AttributeInit("channels", &channels),
                        AttributeInit("sdpFmtpLine", &sdpFmtpLine))

RtcCodecStats::RtcCodecStats(std::string id, Timestamp timestamp)
    : RtcStats(std::move(id), timestamp)
{
}

RtcCodecStats::~RtcCodecStats() { }

OCTK_IMPLEMENT_RTCSTATS(RtcDataChannelStats,
                        RtcStats,
                        "data-channel",
                        AttributeInit("label", &label),
                        AttributeInit("protocol", &protocol),
                        AttributeInit("dataChannelIdentifier", &dataChannelIdentifier),
                        AttributeInit("state", &state),
                        AttributeInit("messagesSent", &messagesSent),
                        AttributeInit("bytesSent", &bytesSent),
                        AttributeInit("messagesReceived", &messagesReceived),
                        AttributeInit("bytesReceived", &bytesReceived))

RtcDataChannelStats::RtcDataChannelStats(std::string id, Timestamp timestamp)
    : RtcStats(std::move(id), timestamp)
{
}

RtcDataChannelStats::~RtcDataChannelStats() { }

OCTK_IMPLEMENT_RTCSTATS(RtcIceCandidatePairStats,
                        RtcStats,
                        "candidate-pair",
                        AttributeInit("transportId", &transportId),
                        AttributeInit("localCandidateId", &localCandidateId),
                        AttributeInit("remoteCandidateId", &remoteCandidateId),
                        AttributeInit("state", &state),
                        AttributeInit("priority", &priority),
                        AttributeInit("nominated", &nominated),
                        AttributeInit("writable", &writable),
                        AttributeInit("packetsSent", &packetsSent),
                        AttributeInit("packetsReceived", &packetsReceived),
                        AttributeInit("bytesSent", &bytesSent),
                        AttributeInit("bytesReceived", &bytesReceived),
                        AttributeInit("totalRoundTripTime", &totalRoundTripTime),
                        AttributeInit("currentRoundTripTime", &currentRoundTripTime),
                        AttributeInit("availableOutgoingBitrate", &availableOutgoingBitrate),
                        AttributeInit("availableIncomingBitrate", &availableIncomingBitrate),
                        AttributeInit("requestsReceived", &requestsReceived),
                        AttributeInit("requestsSent", &requestsSent),
                        AttributeInit("responsesReceived", &responsesReceived),
                        AttributeInit("responsesSent", &responsesSent),
                        AttributeInit("consentRequestsSent", &consentRequestsSent),
                        AttributeInit("packetsDiscardedOnSend", &packetsDiscardedOnSend),
                        AttributeInit("bytesDiscardedOnSend", &bytesDiscardedOnSend),
                        AttributeInit("lastPacketReceivedTimestamp", &lastPacketReceivedTimestamp),
                        AttributeInit("lastPacketSentTimestamp", &lastPacketSentTimestamp))

RtcIceCandidatePairStats::RtcIceCandidatePairStats(std::string id, Timestamp timestamp)
    : RtcStats(std::move(id), timestamp)
{
}

RtcIceCandidatePairStats::~RtcIceCandidatePairStats() { }

OCTK_IMPLEMENT_RTCSTATS(RtcIceCandidateStats,
                        RtcStats,
                        "abstract-ice-candidate",
                        AttributeInit("transportId", &transportId),
                        AttributeInit("isRemote", &isRemote),
                        AttributeInit("networkType", &networkType),
                        AttributeInit("ip", &ip),
                        AttributeInit("address", &address),
                        AttributeInit("port", &port),
                        AttributeInit("protocol", &protocol),
                        AttributeInit("relayProtocol", &relayProtocol),
                        AttributeInit("candidateType", &candidateType),
                        AttributeInit("priority", &priority),
                        AttributeInit("url", &url),
                        AttributeInit("foundation", &foundation),
                        AttributeInit("relatedAddress", &relatedAddress),
                        AttributeInit("relatedPort", &relatedPort),
                        AttributeInit("usernameFragment", &usernameFragment),
                        AttributeInit("tcpType", &tcpType),
                        AttributeInit("vpn", &vpn),
                        AttributeInit("networkAdapterType", &networkAdapterType))

RtcIceCandidateStats::RtcIceCandidateStats(std::string id, Timestamp timestamp, bool isRemote)
    : RtcStats(std::move(id), timestamp)
    , isRemote(isRemote)
{
}

RtcIceCandidateStats::~RtcIceCandidateStats() { }

const char RtcLocalIceCandidateStats::kType[] = "local-candidate";

RtcLocalIceCandidateStats::RtcLocalIceCandidateStats(std::string id, Timestamp timestamp)
    : RtcIceCandidateStats(std::move(id), timestamp, false)
{
}

std::unique_ptr<RtcStats> RtcLocalIceCandidateStats::copy() const
{
    return utils::make_unique<RtcLocalIceCandidateStats>(*this);
}

const char *RtcLocalIceCandidateStats::type() const { return kType; }

const char RtcRemoteIceCandidateStats::kType[] = "remote-candidate";

RtcRemoteIceCandidateStats::RtcRemoteIceCandidateStats(std::string id, Timestamp timestamp)
    : RtcIceCandidateStats(std::move(id), timestamp, true)
{
}

std::unique_ptr<RtcStats> RtcRemoteIceCandidateStats::copy() const
{
    return utils::make_unique<RtcRemoteIceCandidateStats>(*this);
}

const char *RtcRemoteIceCandidateStats::type() const { return kType; }

OCTK_IMPLEMENT_RTCSTATS(RtcPeerConnectionStats,
                        RtcStats,
                        "peer-connection",
                        AttributeInit("dataChannelsOpened", &dataChannelsOpened),
                        AttributeInit("dataChannelsClosed", &dataChannelsClosed))

RtcPeerConnectionStats::RtcPeerConnectionStats(std::string id, Timestamp timestamp)
    : RtcStats(std::move(id), timestamp)
{
}

RtcPeerConnectionStats::~RtcPeerConnectionStats() { }

OCTK_IMPLEMENT_RTCSTATS(RtcRtpStreamStats,
                        RtcStats,
                        "rtp",
                        AttributeInit("ssrc", &ssrc),
                        AttributeInit("kind", &kind),
                        AttributeInit("transportId", &transportId),
                        AttributeInit("codecId", &codecId))

RtcRtpStreamStats::RtcRtpStreamStats(std::string id, Timestamp timestamp)
    : RtcStats(std::move(id), timestamp)
{
}

RtcRtpStreamStats::~RtcRtpStreamStats() { }

OCTK_IMPLEMENT_RTCSTATS(RtcReceivedRtpStreamStats,
                        RtcRtpStreamStats,
                        "received-rtp",
                        AttributeInit("jitter", &jitter),
                        AttributeInit("packetsLost", &packetsLost))

RtcReceivedRtpStreamStats::RtcReceivedRtpStreamStats(std::string id, Timestamp timestamp)
    : RtcRtpStreamStats(std::move(id), timestamp)
{
}

RtcReceivedRtpStreamStats::~RtcReceivedRtpStreamStats() { }

OCTK_IMPLEMENT_RTCSTATS(RtcSentRtpStreamStats,
                        RtcRtpStreamStats,
                        "sent-rtp",
                        AttributeInit("packetsSent", &packetsSent),
                        AttributeInit("bytesSent", &bytesSent))

RtcSentRtpStreamStats::RtcSentRtpStreamStats(std::string id, Timestamp timestamp)
    : RtcRtpStreamStats(std::move(id), timestamp)
{
}

RtcSentRtpStreamStats::~RtcSentRtpStreamStats() { }

OCTK_IMPLEMENT_RTCSTATS(RtcInboundRtpStreamStats,
                        RtcReceivedRtpStreamStats,
                        "inbound-rtp",
                        AttributeInit("playoutId", &playoutId),
                        AttributeInit("trackIdentifier", &trackIdentifier),
                        AttributeInit("mid", &mid),
                        AttributeInit("remoteId", &remoteId),
                        AttributeInit("packetsReceived", &packetsReceived),
                        AttributeInit("packetsDiscarded", &packetsDiscarded),
                        AttributeInit("fecPacketsReceived", &fecPacketsReceived),
                        AttributeInit("fecBytesReceived", &fecBytesReceived),
                        AttributeInit("fecPacketsDiscarded", &fecPacketsDiscarded),
                        AttributeInit("fecSsrc", &fecSsrc),
                        AttributeInit("bytesReceived", &bytesReceived),
                        AttributeInit("headerBytesReceived", &headerBytesReceived),
                        AttributeInit("retransmittedPacketsReceived", &retransmittedPacketsReceived),
                        AttributeInit("retransmittedBytesReceived", &retransmittedBytesReceived),
                        AttributeInit("rtxSsrc", &rtxSsrc),
                        AttributeInit("lastPacketReceivedTimestamp", &lastPacketReceivedTimestamp),
                        AttributeInit("jitterBufferDelay", &jitterBufferDelay),
                        AttributeInit("jitterBufferTargetDelay", &jitterBufferTargetDelay),
                        AttributeInit("jitterBufferMinimumDelay", &jitterBufferMinimumDelay),
                        AttributeInit("jitterBufferEmittedCount", &jitterBufferEmittedCount),
                        AttributeInit("totalSamplesReceived", &totalSamplesReceived),
                        AttributeInit("concealedSamples", &concealedSamples),
                        AttributeInit("silentConcealedSamples", &silentConcealedSamples),
                        AttributeInit("concealmentEvents", &concealmentEvents),
                        AttributeInit("insertedSamplesForDeceleration", &insertedSamplesForDeceleration),
                        AttributeInit("removedSamplesForAcceleration", &removedSamplesForAcceleration),
                        AttributeInit("audioLevel", &audioLevel),
                        AttributeInit("totalAudioEnergy", &totalAudioEnergy),
                        AttributeInit("totalSamplesDuration", &totalSamplesDuration),
                        AttributeInit("framesReceived", &framesReceived),
                        AttributeInit("frameWidth", &frameWidth),
                        AttributeInit("frameHeight", &frameHeight),
                        AttributeInit("framesPerSecond", &framesPerSecond),
                        AttributeInit("framesDecoded", &framesDecoded),
                        AttributeInit("keyFramesDecoded", &keyFramesDecoded),
                        AttributeInit("framesDropped", &framesDropped),
                        AttributeInit("totalDecodeTime", &totalDecodeTime),
                        AttributeInit("totalProcessingDelay", &totalProcessingDelay),
                        AttributeInit("totalAssemblyTime", &totalAssemblyTime),
                        AttributeInit("framesAssembledFromMultiplePackets", &framesAssembledFromMultiplePackets),
                        AttributeInit("totalInterFrameDelay", &totalInterFrameDelay),
                        AttributeInit("totalSquaredInterFrameDelay", &totalSquaredInterFrameDelay),
                        AttributeInit("pauseCount", &pauseCount),
                        AttributeInit("totalPausesDuration", &totalPausesDuration),
                        AttributeInit("freezeCount", &freezeCount),
                        AttributeInit("totalFreezesDuration", &totalFreezesDuration),
                        AttributeInit("contentType", &contentType),
                        AttributeInit("estimatedPlayoutTimestamp", &estimatedPlayoutTimestamp),
                        AttributeInit("decoderImplementation", &decoderImplementation),
                        AttributeInit("firCount", &firCount),
                        AttributeInit("pliCount", &pliCount),
                        AttributeInit("nackCount", &nackCount),
                        AttributeInit("qpSum", &qpSum),
                        AttributeInit("totalCorruptionProbability", &totalCorruptionProbability),
                        AttributeInit("totalSquaredCorruptionProbability", &totalSquaredCorruptionProbability),
                        AttributeInit("corruptionMeasurements", &corruptionMeasurements),
                        AttributeInit("googTimingFrameInfo", &googTimingFrameInfo),
                        AttributeInit("powerEfficientDecoder", &powerEfficientDecoder),
                        AttributeInit("jitterBufferFlushes", &jitterBufferFlushes),
                        AttributeInit("delayedPacketOutageSamples", &delayedPacketOutageSamples),
                        AttributeInit("relativePacketArrivalDelay", &relativePacketArrivalDelay),
                        AttributeInit("interruptionCount", &interruptionCount),
                        AttributeInit("totalInterruptionDuration", &totalInterruptionDuration),
                        AttributeInit("minPlayoutDelay", &minPlayoutDelay))

RtcInboundRtpStreamStats::RtcInboundRtpStreamStats(std::string id, Timestamp timestamp)
    : RtcReceivedRtpStreamStats(std::move(id), timestamp)
{
}

RtcInboundRtpStreamStats::~RtcInboundRtpStreamStats() { }

OCTK_IMPLEMENT_RTCSTATS(RtcOutboundRtpStreamStats,
                        RtcSentRtpStreamStats,
                        "outbound-rtp",
                        AttributeInit("mediaSourceId", &mediaSourceId),
                        AttributeInit("remoteId", &remoteId),
                        AttributeInit("mid", &mid),
                        AttributeInit("rid", &rid),
                        AttributeInit("retransmittedPacketsSent", &retransmittedPacketsSent),
                        AttributeInit("headerBytesSent", &headerBytesSent),
                        AttributeInit("retransmittedBytesSent", &retransmittedBytesSent),
                        AttributeInit("targetBitrate", &targetBitrate),
                        AttributeInit("framesEncoded", &framesEncoded),
                        AttributeInit("keyFramesEncoded", &keyFramesEncoded),
                        AttributeInit("totalEncodeTime", &totalEncodeTime),
                        AttributeInit("totalEncodedBytesTarget", &totalEncodedBytesTarget),
                        AttributeInit("frameWidth", &frameWidth),
                        AttributeInit("frameHeight", &frameHeight),
                        AttributeInit("framesPerSecond", &framesPerSecond),
                        AttributeInit("framesSent", &framesSent),
                        AttributeInit("hugeFramesSent", &hugeFramesSent),
                        AttributeInit("totalPacketSendDelay", &totalPacketSendDelay),
                        AttributeInit("qualityLimitationReason", &qualityLimitationReason),
                        AttributeInit("qualityLimitationDurations", &qualityLimitationDurations),
                        AttributeInit("qualityLimitationResolutionChanges", &qualityLimitationResolutionChanges),
                        AttributeInit("contentType", &contentType),
                        AttributeInit("encoderImplementation", &encoderImplementation),
                        AttributeInit("firCount", &firCount),
                        AttributeInit("pliCount", &pliCount),
                        AttributeInit("nackCount", &nackCount),
                        AttributeInit("qpSum", &qpSum),
                        AttributeInit("active", &active),
                        AttributeInit("powerEfficientEncoder", &powerEfficientEncoder),
                        AttributeInit("scalabilityMode", &scalabilityMode),
                        AttributeInit("rtxSsrc", &rtxSsrc))

RtcOutboundRtpStreamStats::RtcOutboundRtpStreamStats(std::string id, Timestamp timestamp)
    : RtcSentRtpStreamStats(std::move(id), timestamp)
{
}

RtcOutboundRtpStreamStats::~RtcOutboundRtpStreamStats() { }


OCTK_IMPLEMENT_RTCSTATS(RtcRemoteInboundRtpStreamStats,
                        RtcReceivedRtpStreamStats,
                        "remote-inbound-rtp",
                        AttributeInit("localId", &localId),
                        AttributeInit("roundTripTime", &roundTripTime),
                        AttributeInit("fractionLost", &fractionLost),
                        AttributeInit("totalRoundTripTime", &totalRoundTripTime),
                        AttributeInit("roundTripTimeMeasurements", &roundTripTimeMeasurements))

RtcRemoteInboundRtpStreamStats::RtcRemoteInboundRtpStreamStats(std::string id, Timestamp timestamp)
    : RtcReceivedRtpStreamStats(std::move(id), timestamp)
{
}

RtcRemoteInboundRtpStreamStats::~RtcRemoteInboundRtpStreamStats() { }

OCTK_IMPLEMENT_RTCSTATS(RtcRemoteOutboundRtpStreamStats,
                        RtcSentRtpStreamStats,
                        "remote-outbound-rtp",
                        AttributeInit("localId", &localId),
                        AttributeInit("remoteTimestamp", &remoteTimestamp),
                        AttributeInit("reportsSent", &reportsSent),
                        AttributeInit("roundTripTime", &roundTripTime),
                        AttributeInit("roundTripTimeMeasurements", &roundTripTimeMeasurements),
                        AttributeInit("totalRoundTripTime", &totalRoundTripTime))

RtcRemoteOutboundRtpStreamStats::RtcRemoteOutboundRtpStreamStats(std::string id, Timestamp timestamp)
    : RtcSentRtpStreamStats(std::move(id), timestamp)
{
}

RtcRemoteOutboundRtpStreamStats::~RtcRemoteOutboundRtpStreamStats() { }


OCTK_IMPLEMENT_RTCSTATS(RtcMediaSourceStats,
                        RtcStats,
                        "parent-media-source",
                        AttributeInit("trackIdentifier", &trackIdentifier),
                        AttributeInit("kind", &kind))

RtcMediaSourceStats::RtcMediaSourceStats(std::string id, Timestamp timestamp)
    : RtcStats(std::move(id), timestamp)
{
}

RtcMediaSourceStats::~RtcMediaSourceStats() { }

OCTK_IMPLEMENT_RTCSTATS(RtcAudioSourceStats,
                        RtcMediaSourceStats,
                        "media-source",
                        AttributeInit("audioLevel", &audioLevel),
                        AttributeInit("totalAudioEnergy", &totalAudioEnergy),
                        AttributeInit("totalSamplesDuration", &totalSamplesDuration),
                        AttributeInit("echoReturnLoss", &echoReturnLoss),
                        AttributeInit("echoReturnLossEnhancement", &echoReturnLossEnhancement))

RtcAudioSourceStats::RtcAudioSourceStats(std::string id, Timestamp timestamp)
    : RtcMediaSourceStats(std::move(id), timestamp)
{
}

RtcAudioSourceStats::~RtcAudioSourceStats() { }

OCTK_IMPLEMENT_RTCSTATS(RtcVideoSourceStats,
                        RtcMediaSourceStats,
                        "media-source",
                        AttributeInit("width", &width),
                        AttributeInit("height", &height),
                        AttributeInit("frames", &frames),
                        AttributeInit("framesPerSecond", &framesPerSecond))

RtcVideoSourceStats::RtcVideoSourceStats(std::string id, Timestamp timestamp)
    : RtcMediaSourceStats(std::move(id), timestamp)
{
}

RtcVideoSourceStats::~RtcVideoSourceStats() { }

OCTK_IMPLEMENT_RTCSTATS(RtcTransportStats,
                        RtcStats,
                        "transport",
                        AttributeInit("bytesSent", &bytesSent),
                        AttributeInit("packetsSent", &packetsSent),
                        AttributeInit("bytesReceived", &bytesReceived),
                        AttributeInit("packetsReceived", &packetsReceived),
                        AttributeInit("rtcpTransportStatsId", &rtcpTransportStatsId),
                        AttributeInit("dtlsState", &dtlsState),
                        AttributeInit("selectedCandidatePairId", &selectedCandidatePairId),
                        AttributeInit("localCertificateId", &localCertificateId),
                        AttributeInit("remoteCertificateId", &remoteCertificateId),
                        AttributeInit("tlsVersion", &tlsVersion),
                        AttributeInit("dtlsCipher", &dtlsCipher),
                        AttributeInit("dtlsRole", &dtlsRole),
                        AttributeInit("srtpCipher", &srtpCipher),
                        AttributeInit("selectedCandidatePairChanges", &selectedCandidatePairChanges),
                        AttributeInit("iceRole", &iceRole),
                        AttributeInit("iceLocalUsernameFragment", &iceLocalUsernameFragment),
                        AttributeInit("iceState", &iceState))

RtcTransportStats::RtcTransportStats(std::string id, Timestamp timestamp)
    : RtcStats(std::move(id), timestamp)
{
}

RtcTransportStats::~RtcTransportStats() { }

OCTK_IMPLEMENT_RTCSTATS(RtcAudioPlayoutStats,
                        RtcStats,
                        "media-playout",
                        AttributeInit("kind", &kind),
                        AttributeInit("synthesizedSamplesDuration", &synthesizedSamplesDuration),
                        AttributeInit("synthesizedSamplesEvents", &synthesizedSamplesEvents),
                        AttributeInit("totalSamplesDuration", &totalSamplesDuration),
                        AttributeInit("totalPlayoutDelay", &totalPlayoutDelay),
                        AttributeInit("totalSamplesCount", &totalSamplesCount))

RtcAudioPlayoutStats::RtcAudioPlayoutStats(const std::string &id, Timestamp timestamp)
    : RtcStats(std::move(id), timestamp)
    , kind("audio")
{
}

RtcAudioPlayoutStats::~RtcAudioPlayoutStats() { }


OCTK_END_NAMESPACE
