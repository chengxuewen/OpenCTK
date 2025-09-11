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

#include <octk_rtp_parameters.hpp>
#include <octk_media_constants.hpp>

OCTK_BEGIN_NAMESPACE

const char *DegradationPreferenceToString(DegradationPreference degradation_preference)
{
    switch (degradation_preference)
    {
        case DegradationPreference::DISABLED: return "disabled";
        case DegradationPreference::MAINTAIN_FRAMERATE: return "maintain-framerate";
        case DegradationPreference::MAINTAIN_RESOLUTION: return "maintain-resolution";
        case DegradationPreference::BALANCED: return "balanced";
    }
    OCTK_CHECK_NOTREACHED();
}

const double kDefaultBitratePriority = 1.0;

RtcpFeedback::RtcpFeedback() = default;

RtcpFeedback::RtcpFeedback(RtcpFeedbackType type)
    : type(type)
{
}

RtcpFeedback::RtcpFeedback(RtcpFeedbackType type, RtcpFeedbackMessageType message_type)
    : type(type)
    , message_type(message_type)
{
}

RtcpFeedback::RtcpFeedback(const RtcpFeedback &rhs) = default;

RtcpFeedback::~RtcpFeedback() = default;

RtpCodec::RtpCodec() = default;
RtpCodec::RtpCodec(const RtpCodec &) = default;
RtpCodec::~RtpCodec() = default;
bool RtpCodec::IsResiliencyCodec() const
{
    return name == media::kRtxCodecName || name == media::kRedCodecName || name == media::kUlpfecCodecName ||
           name == media::kFlexfecCodecName;
}
bool RtpCodec::IsMediaCodec() const { return !IsResiliencyCodec() && name != media::kComfortNoiseCodecName; }
RtpCodecCapability::RtpCodecCapability() = default;
RtpCodecCapability::~RtpCodecCapability() = default;

RtpHeaderExtensionCapability::RtpHeaderExtensionCapability() = default;
RtpHeaderExtensionCapability::RtpHeaderExtensionCapability(StringView uri)
    : uri(uri)
{
}
RtpHeaderExtensionCapability::RtpHeaderExtensionCapability(StringView uri, int preferredId)
    : uri(uri)
    , preferredId(preferredId)
{
}
RtpHeaderExtensionCapability::RtpHeaderExtensionCapability(StringView uri,
                                                           int preferredId,
                                                           RtpTransceiverDirection direction)
    : uri(uri)
    , preferredId(preferredId)
    , direction(direction)
{
}
RtpHeaderExtensionCapability::RtpHeaderExtensionCapability(StringView uri,
                                                           int preferredId,
                                                           bool preferredEncrypt,
                                                           RtpTransceiverDirection direction)
    : uri(uri)
    , preferredId(preferredId)
    , preferredEncrypt(preferredEncrypt)
    , direction(direction)
{
}
RtpHeaderExtensionCapability::~RtpHeaderExtensionCapability() = default;

RtpExtension::RtpExtension() = default;
RtpExtension::RtpExtension(StringView uri, int id)
    : uri(uri)
    , id(id)
{
}
RtpExtension::RtpExtension(StringView uri, int id, bool encrypt)
    : uri(uri)
    , id(id)
    , encrypt(encrypt)
{
}
RtpExtension::~RtpExtension() = default;

RtpFecParameters::RtpFecParameters() = default;
RtpFecParameters::RtpFecParameters(FecMechanism mechanism)
    : mechanism(mechanism)
{
}
RtpFecParameters::RtpFecParameters(FecMechanism mechanism, uint32_t ssrc)
    : ssrc(ssrc)
    , mechanism(mechanism)
{
}
RtpFecParameters::RtpFecParameters(const RtpFecParameters &rhs) = default;
RtpFecParameters::~RtpFecParameters() = default;

RtpRtxParameters::RtpRtxParameters() = default;
RtpRtxParameters::RtpRtxParameters(uint32_t ssrc)
    : ssrc(ssrc)
{
}
RtpRtxParameters::RtpRtxParameters(const RtpRtxParameters &rhs) = default;
RtpRtxParameters::~RtpRtxParameters() = default;

RtpEncodingParameters::RtpEncodingParameters() = default;
RtpEncodingParameters::RtpEncodingParameters(const RtpEncodingParameters &rhs) = default;
RtpEncodingParameters::~RtpEncodingParameters() = default;

RtpCodecParameters::RtpCodecParameters() = default;
RtpCodecParameters::RtpCodecParameters(const RtpCodecParameters &rhs) = default;
RtpCodecParameters::~RtpCodecParameters() = default;

RtpCapabilities::RtpCapabilities() = default;
RtpCapabilities::~RtpCapabilities() = default;

RtcpParameters::RtcpParameters() = default;
RtcpParameters::RtcpParameters(const RtcpParameters &rhs) = default;
RtcpParameters::~RtcpParameters() = default;

RtpParameters::RtpParameters() = default;
RtpParameters::RtpParameters(const RtpParameters &rhs) = default;
RtpParameters::~RtpParameters() = default;

std::string RtpExtension::toString() const
{
    char buf[256];
    std::stringstream ss;
    ss << "{uri: " << uri;
    ss << ", id: " << id;
    if (encrypt)
    {
        ss << ", encrypt";
    }
    ss << '}';
    return ss.str();
}

constexpr char RtpExtension::kEncryptHeaderExtensionsUri[];
constexpr char RtpExtension::kAudioLevelUri[];
constexpr char RtpExtension::kTimestampOffsetUri[];
constexpr char RtpExtension::kAbsSendTimeUri[];
constexpr char RtpExtension::kAbsoluteCaptureTimeUri[];
constexpr char RtpExtension::kVideoRotationUri[];
constexpr char RtpExtension::kVideoContentTypeUri[];
constexpr char RtpExtension::kVideoTimingUri[];
constexpr char RtpExtension::kGenericFrameDescriptorUri00[];
constexpr char RtpExtension::kDependencyDescriptorUri[];
constexpr char RtpExtension::kVideoLayersAllocationUri[];
constexpr char RtpExtension::kTransportSequenceNumberUri[];
constexpr char RtpExtension::kTransportSequenceNumberV2Uri[];
constexpr char RtpExtension::kPlayoutDelayUri[];
constexpr char RtpExtension::kColorSpaceUri[];
constexpr char RtpExtension::kMidUri[];
constexpr char RtpExtension::kRidUri[];
constexpr char RtpExtension::kRepairedRidUri[];
constexpr char RtpExtension::kVideoFrameTrackingIdUri[];
constexpr char RtpExtension::kCsrcAudioLevelsUri[];
constexpr char RtpExtension::kCorruptionDetectionUri[];

constexpr int RtpExtension::kMinId;
constexpr int RtpExtension::kMaxId;
constexpr int RtpExtension::kMaxValueSize;
constexpr int RtpExtension::kOneByteHeaderExtensionMaxId;
constexpr int RtpExtension::kOneByteHeaderExtensionMaxValueSize;

bool RtpExtension::IsSupportedForAudio(StringView uri)
{
    return uri == RtpExtension::kAudioLevelUri || uri == RtpExtension::kAbsSendTimeUri ||
           uri == RtpExtension::kAbsoluteCaptureTimeUri || uri == RtpExtension::kTransportSequenceNumberUri ||
           uri == RtpExtension::kTransportSequenceNumberV2Uri || uri == RtpExtension::kMidUri ||
           uri == RtpExtension::kRidUri || uri == RtpExtension::kRepairedRidUri;
}

bool RtpExtension::IsSupportedForVideo(StringView uri)
{
    return uri == RtpExtension::kTimestampOffsetUri || uri == RtpExtension::kAbsSendTimeUri ||
           uri == RtpExtension::kAbsoluteCaptureTimeUri || uri == RtpExtension::kVideoRotationUri ||
           uri == RtpExtension::kTransportSequenceNumberUri || uri == RtpExtension::kTransportSequenceNumberV2Uri ||
           uri == RtpExtension::kPlayoutDelayUri || uri == RtpExtension::kVideoContentTypeUri ||
           uri == RtpExtension::kVideoTimingUri || uri == RtpExtension::kMidUri ||
           uri == RtpExtension::kGenericFrameDescriptorUri00 || uri == RtpExtension::kDependencyDescriptorUri ||
           uri == RtpExtension::kColorSpaceUri || uri == RtpExtension::kRidUri ||
           uri == RtpExtension::kRepairedRidUri || uri == RtpExtension::kVideoLayersAllocationUri ||
           uri == RtpExtension::kVideoFrameTrackingIdUri || uri == RtpExtension::kCorruptionDetectionUri;
}

bool RtpExtension::IsEncryptionSupported(StringView uri)
{
    return
#if defined(ENABLE_EXTERNAL_AUTH)
        // TODO(jbauch): Figure out a way to always allow "kAbsSendTimeUri"
        // here and filter out later if external auth is really used in
        // srtpfilter. External auth is used by Chromium and replaces the
        // extension header value of "kAbsSendTimeUri", so it must not be
        // encrypted (which can't be done by Chromium).
        uri != RtpExtension::kAbsSendTimeUri &&
#endif
        uri != RtpExtension::kEncryptHeaderExtensionsUri;
}

// Returns whether a header extension with the given URI exists.
// Note: This does not differentiate between encrypted and non-encrypted
// extensions, so use with care!
static bool HeaderExtensionWithUriExists(const std::vector<RtpExtension> &extensions, StringView uri)
{
    for (const auto &extension : extensions)
    {
        if (extension.uri == uri)
        {
            return true;
        }
    }
    return false;
}

const RtpExtension *
RtpExtension::FindHeaderExtensionByUri(const std::vector<RtpExtension> &extensions, StringView uri, Filter filter)
{
    const RtpExtension *fallback_extension = nullptr;
    for (const auto &extension : extensions)
    {
        if (extension.uri != uri)
        {
            continue;
        }

        switch (filter)
        {
            case kDiscardEncryptedExtension:
            {
                // We only accept an unencrypted extension.
                if (!extension.encrypt)
                {
                    return &extension;
                }
                break;
            }
            case kPreferEncryptedExtension:
            {
                // We prefer an encrypted extension but we can fall back to an
                // unencrypted extension.
                if (extension.encrypt)
                {
                    return &extension;
                }
                else
                {
                    fallback_extension = &extension;
                }
                break;
            }
            case kRequireEncryptedExtension:
            {
                // We only accept an encrypted extension.
                if (extension.encrypt)
                {
                    return &extension;
                }
                break;
            }
            default: break;
        }
    }

    // Returning fallback extension (if any)
    return fallback_extension;
}

const RtpExtension *RtpExtension::FindHeaderExtensionByUriAndEncryption(const std::vector<RtpExtension> &extensions,
                                                                        StringView uri,
                                                                        bool encrypt)
{
    for (const auto &extension : extensions)
    {
        if (extension.uri == uri && extension.encrypt == encrypt)
        {
            return &extension;
        }
    }
    return nullptr;
}

const std::vector<RtpExtension> RtpExtension::DeduplicateHeaderExtensions(const std::vector<RtpExtension> &extensions,
                                                                          Filter filter)
{
    std::vector<RtpExtension> filtered;

    // If we do not discard encrypted extensions, add them first
    if (filter != kDiscardEncryptedExtension)
    {
        for (const auto &extension : extensions)
        {
            if (!extension.encrypt)
            {
                continue;
            }
            if (!HeaderExtensionWithUriExists(filtered, extension.uri))
            {
                filtered.push_back(extension);
            }
        }
    }

    // If we do not require encrypted extensions, add missing, non-encrypted
    // extensions.
    if (filter != kRequireEncryptedExtension)
    {
        for (const auto &extension : extensions)
        {
            if (extension.encrypt)
            {
                continue;
            }
            if (!HeaderExtensionWithUriExists(filtered, extension.uri))
            {
                filtered.push_back(extension);
            }
        }
    }

    // Sort the returned vector to make comparisons of header extensions reliable.
    // In order of priority, we sort by uri first, then encrypt and id last.
    std::sort(filtered.begin(),
              filtered.end(),
              [](const RtpExtension &a, const RtpExtension &b)
              { return std::tie(a.uri, a.encrypt, a.id) < std::tie(b.uri, b.encrypt, b.id); });

    return filtered;
}

OCTK_END_NAMESPACE
