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

#pragma once

#include <octk_media_constants.hpp>

OCTK_BEGIN_NAMESPACE

namespace media
{

extern const int kVideoCodecClockrate;

extern const int kVideoMtu;
extern const int kVideoRtpSendBufferSize;
extern const int kVideoRtpRecvBufferSize;

// Default CPU thresholds.
extern const float kHighSystemCpuThreshold;
extern const float kLowSystemCpuThreshold;
extern const float kProcessCpuThreshold;

extern const char kRedCodecName[];
extern const char kUlpfecCodecName[];
extern const char kFlexfecCodecName[];
extern const char kMultiplexCodecName[];

extern const char kFlexfecFmtpRepairWindow[];

extern const char kRtxCodecName[];
extern const char kCodecParamRtxTime[];
extern const char kCodecParamAssociatedPayloadType[];

extern const char kCodecParamAssociatedCodecName[];
extern const char kCodecParamNotInNameValueFormat[];

extern const char kOpusCodecName[];
extern const char kL16CodecName[];
extern const char kG722CodecName[];
extern const char kIlbcCodecName[];
extern const char kPcmuCodecName[];
extern const char kPcmaCodecName[];
extern const char kCnCodecName[];
extern const char kDtmfCodecName[];

// Attribute parameters
extern const char kCodecParamPTime[];
extern const char kCodecParamMaxPTime[];
// fmtp parameters
extern const char kCodecParamMinPTime[];
extern const char kCodecParamSPropStereo[];
extern const char kCodecParamStereo[];
extern const char kCodecParamUseInbandFec[];
extern const char kCodecParamUseDtx[];
extern const char kCodecParamMaxAverageBitrate[];
extern const char kCodecParamMaxPlaybackRate[];
extern const char kCodecParamPerLayerPictureLossIndication[];

extern const char kParamValueTrue[];
// Parameters are stored as parameter/value pairs. For parameters who do not
// have a value, `kParamValueEmpty` should be used as value.
extern const char kParamValueEmpty[];

// opus parameters.
// Default value for maxptime according to
// http://tools.ietf.org/html/draft-spittka-payload-rtp-opus-03
extern const int kOpusDefaultMaxPTime;
extern const int kOpusDefaultPTime;
extern const int kOpusDefaultMinPTime;
extern const int kOpusDefaultSPropStereo;
extern const int kOpusDefaultStereo;
extern const int kOpusDefaultUseInbandFec;
extern const int kOpusDefaultUseDtx;
extern const int kOpusDefaultMaxPlaybackRate;

// Prefered values in this code base. Note that they may differ from the default
// values in http://tools.ietf.org/html/draft-spittka-payload-rtp-opus-03
// Only frames larger or equal to 10 ms are currently supported in this code
// base.
extern const int kPreferredMaxPTime;
extern const int kPreferredMinPTime;
extern const int kPreferredSPropStereo;
extern const int kPreferredStereo;
extern const int kPreferredUseInbandFec;

extern const char kPacketizationParamRaw[];

// rtcp-fb message in its first experimental stages. Documentation pending.
extern const char kRtcpFbParamLntf[];
// rtcp-fb messages according to RFC 4585
extern const char kRtcpFbParamNack[];
extern const char kRtcpFbNackParamPli[];
// rtcp-fb messages according to
// http://tools.ietf.org/html/draft-alvestrand-rmcat-remb-00
extern const char kRtcpFbParamRemb[];
// rtcp-fb messages according to
// https://tools.ietf.org/html/draft-holmer-rmcat-transport-wide-cc-extensions-01
extern const char kRtcpFbParamTransportCc[];
// ccm submessages according to RFC 5104
extern const char kRtcpFbParamCcm[];
extern const char kRtcpFbCcmParamFir[];
// Receiver reference time report
// https://tools.ietf.org/html/rfc3611 section 4.4
extern const char kRtcpFbParamRrtr[];
// Google specific parameters
extern const char kCodecParamMaxBitrate[];
extern const char kCodecParamMinBitrate[];
extern const char kCodecParamStartBitrate[];
extern const char kCodecParamMaxQuantization[];

extern const char kComfortNoiseCodecName[];

OCTK_MEDIA_API extern const char kVp8CodecName[];
OCTK_MEDIA_API extern const char kVp9CodecName[];
OCTK_MEDIA_API extern const char kAv1CodecName[];
OCTK_MEDIA_API extern const char kH264CodecName[];
OCTK_MEDIA_API extern const char kH265CodecName[];

// RFC 6184 RTP Payload Format for H.264 video
OCTK_MEDIA_API extern const char kH264FmtpProfileLevelId[];
OCTK_MEDIA_API extern const char kH264FmtpLevelAsymmetryAllowed[];
OCTK_MEDIA_API extern const char kH264FmtpPacketizationMode[];
extern const char kH264FmtpSpropParameterSets[];
extern const char kH264FmtpSpsPpsIdrInKeyframe[];
extern const char kH264ProfileLevelConstrainedBaseline[];
extern const char kH264ProfileLevelConstrainedHigh[];

// RFC 7798 RTP Payload Format for H.265 video.
// According to RFC 7742, the sprop parameters MUST NOT be included
// in SDP generated by WebRTC, so for H.265 we don't handle them, though
// current H.264 implementation honors them when receiving
// sprop-parameter-sets in SDP.
OCTK_MEDIA_API extern const char kH265FmtpProfileSpace[];
OCTK_MEDIA_API extern const char kH265FmtpTierFlag[];
OCTK_MEDIA_API extern const char kH265FmtpProfileId[];
OCTK_MEDIA_API extern const char kH265FmtpLevelId[];
OCTK_MEDIA_API extern const char kH265FmtpProfileCompatibilityIndicator[];
OCTK_MEDIA_API extern const char kH265FmtpInteropConstraints[];
OCTK_MEDIA_API extern const char kH265FmtpTxMode[];

// draft-ietf-payload-vp9
extern const char kVP9ProfileId[];

// https://aomediacodec.github.io/av1-rtp-spec/
extern const char kAv1FmtpProfile[];
extern const char kAv1FmtpLevelIdx[];
extern const char kAv1FmtpTier[];

extern const int kDefaultVideoMaxFramerate;
extern const int kDefaultVideoMaxQpVpx;
extern const int kDefaultVideoMaxQpH26x;

extern const size_t kConferenceMaxNumSpatialLayers;
extern const size_t kConferenceMaxNumTemporalLayers;
extern const size_t kConferenceDefaultNumTemporalLayers;

extern const char kApplicationSpecificBandwidth[];
extern const char kTransportSpecificBandwidth[];

} // namespace media

OCTK_END_NAMESPACE