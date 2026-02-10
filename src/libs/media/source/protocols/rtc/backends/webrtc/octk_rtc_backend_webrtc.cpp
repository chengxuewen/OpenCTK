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

#include <private/octk_rtc_backend_webrtc_p.hpp>
#include <octk_rtc_engine.hpp>
#include <octk_scope_guard.hpp>

#include "codecs/jetson/jetson_video_encoder.h"
#define USE_JETSON_HW_ENCODER 1

OCTK_DEFINE_LOGGER_WITH_LEVEL("WebRTC", WEBRTC_LOGGER, octk::LogLevel::Warning)

OCTK_BEGIN_NAMESPACE

namespace detail
{
class WebRTCRedirectLogSink : virtual public rtc::LogSink
{
public:
    static LogSink *logSink()
    {
        static WebRTCRedirectLogSink logSink;
        return &logSink;
    }

    void OnLogMessage(const std::string &message) override { }
    void OnLogMessage(const rtc::LogLineRef &line) override
    {
        const auto threadId = line.thread_id().has_value() ? std::to_string(line.thread_id().value()) : "";
        std::string msg = std::string(line.tag().data()) + ":" + threadId + ": " + line.message().data();
        switch (line.severity())
        {
            case rtc::LoggingSeverity::LS_VERBOSE:
            {
                OCTK_LOGGING_FULL(WEBRTC_LOGGER(),
                                  octk::LogLevel::Trace,
                                  line.filename().data(),
                                  "",
                                  line.line(),
                                  msg.data());
                break;
            }
            case rtc::LoggingSeverity::LS_INFO:
            {
                OCTK_LOGGING_FULL(WEBRTC_LOGGER(),
                                  octk::LogLevel::Debug,
                                  line.filename().data(),
                                  "",
                                  line.line(),
                                  msg.data());
                break;
            }
            case rtc::LoggingSeverity::LS_WARNING:
            {
                OCTK_LOGGING_FULL(WEBRTC_LOGGER(),
                                  octk::LogLevel::Warning,
                                  line.filename().data(),
                                  "",
                                  line.line(),
                                  msg.data());
                break;
            }
            case rtc::LoggingSeverity::LS_ERROR:
            {
                OCTK_LOGGING_FULL(WEBRTC_LOGGER(),
                                  octk::LogLevel::Error,
                                  line.filename().data(),
                                  "",
                                  line.line(),
                                  msg.data());
                break;
            }
            default: break;
        }
    }
};

bool findConstraintsFirst(const webrtc::MediaConstraints::Constraints &constraints,
                          const std::string &key,
                          std::string *value)
{
    for (auto iter = constraints.begin(); iter != constraints.end(); ++iter)
    {
        if (iter->key == key)
        {
            *value = iter->value;
            return true;
        }
    }
    return false;
}
// Find the highest-priority instance of the T-valued constraint named by `key` and return its value as `value`.
// `constraints` can be null.
// If `mandatory_constraints` is non-null, it is incremented if the key appears among the mandatory constraints.
// Returns true if the key was found and has a valid value for type T.
// If the key appears multiple times as an optional constraint, appearances after the first are ignored.
// Note: Because this uses FindFirst, repeated optional constraints whose first instance has an unrecognized value
// are not handled precisely in accordance with the specification.
template <typename T>
bool findConstraint(const webrtc::MediaConstraints *constraints,
                    const std::string &key,
                    T *value,
                    size_t *mandatory_constraints)
{
    std::string string_value;
    if (!findConstraint(constraints, key, &string_value, mandatory_constraints))
    {
        return false;
    }
    return rtc::FromString(string_value, value);
}
// Specialization for std::string, since a string doesn't need conversion.
template <>
bool findConstraint(const webrtc::MediaConstraints *constraints,
                    const std::string &key,
                    std::string *value,
                    size_t *mandatory_constraints)
{
    if (!constraints)
    {
        return false;
    }
    if (findConstraintsFirst(constraints->GetMandatory(), key, value))
    {
        if (mandatory_constraints)
        {
            ++*mandatory_constraints;
        }
        return true;
    }
    if (findConstraintsFirst(constraints->GetOptional(), key, value))
    {
        return true;
    }
    return false;
}
bool findConstraint(const webrtc::MediaConstraints *constraints,
                    const std::string &key,
                    bool *value,
                    size_t *mandatory_constraints)
{
    return findConstraint<bool>(constraints, key, value, mandatory_constraints);
}
bool findConstraint(const webrtc::MediaConstraints *constraints,
                    const std::string &key,
                    int *value,
                    size_t *mandatory_constraints)
{
    return findConstraint<int>(constraints, key, value, mandatory_constraints);
}
// Converts a constraint (mandatory takes precedence over optional) to an std::optional.
template <typename T>
void constraintToOptional(const webrtc::MediaConstraints *constraints,
                          const std::string &key,
                          std::optional<T> *value_out)
{
    T value;
    bool present = findConstraint<T>(constraints, key, &value, nullptr);
    if (present)
    {
        *value_out = value;
    }
}
void copyConstraintsIntoRtcConfiguration(const webrtc::MediaConstraints *constraints,
                                         webrtc::PeerConnectionInterface::RTCConfiguration *configuration)
{
    findConstraint(constraints, RtcMediaConstraints::kEnableDscp, &configuration->media_config.enable_dscp, nullptr);
    findConstraint(constraints,
                   RtcMediaConstraints::kCpuOveruseDetection,
                   &configuration->media_config.video.enable_cpu_adaptation,
                   nullptr);
    // Find Suspend Below Min Bitrate constraint.
    findConstraint(constraints,
                   RtcMediaConstraints::kEnableVideoSuspendBelowMinBitrate,
                   &configuration->media_config.video.suspend_below_min_bitrate,
                   nullptr);
    constraintToOptional<int>(constraints,
                              RtcMediaConstraints::kScreencastMinBitrate,
                              &configuration->screencast_min_bitrate);
}
void copyIntoAudioOptions(const webrtc::MediaConstraints *constraints, cricket::AudioOptions *options)
{
}
bool copyConstraintsIntoOfferAnswerOptions(const webrtc::MediaConstraints *constraints,
                                           webrtc::PeerConnectionInterface::RTCOfferAnswerOptions *offerAnswerOptions)
{
    if (!constraints)
    {
        return true;
    }

    bool value = false;
    size_t mandatory_constraints_satisfied = 0;

    if (findConstraint(constraints,
                       RtcMediaConstraints::kOfferToReceiveAudio,
                       &value,
                       &mandatory_constraints_satisfied))
    {
        offerAnswerOptions->offer_to_receive_audio =
            value ? webrtc::PeerConnectionInterface::RTCOfferAnswerOptions::kOfferToReceiveMediaTrue : 0;
    }

    if (findConstraint(constraints,
                       RtcMediaConstraints::kOfferToReceiveVideo,
                       &value,
                       &mandatory_constraints_satisfied))
    {
        offerAnswerOptions->offer_to_receive_video =
            value ? webrtc::PeerConnectionInterface::RTCOfferAnswerOptions::kOfferToReceiveMediaTrue : 0;
    }
    if (findConstraint(constraints,
                       RtcMediaConstraints::kVoiceActivityDetection,
                       &value,
                       &mandatory_constraints_satisfied))
    {
        offerAnswerOptions->voice_activity_detection = value;
    }
    if (findConstraint(constraints, RtcMediaConstraints::kUseRtpMux, &value, &mandatory_constraints_satisfied))
    {
        offerAnswerOptions->use_rtp_mux = value;
    }
    if (findConstraint(constraints, RtcMediaConstraints::kIceRestart, &value, &mandatory_constraints_satisfied))
    {
        offerAnswerOptions->ice_restart = value;
    }

    if (findConstraint(constraints,
                       RtcMediaConstraints::kRawPacketizationForVideoEnabled,
                       &value,
                       &mandatory_constraints_satisfied))
    {
        offerAnswerOptions->raw_packetization_for_video = value;
    }

    int layers;
    if (findConstraint(constraints,
                       RtcMediaConstraints::kNumSimulcastLayers,
                       &layers,
                       &mandatory_constraints_satisfied))
    {
        offerAnswerOptions->num_simulcast_layers = layers;
    }

    return mandatory_constraints_satisfied == constraints->GetMandatory().size();
}


namespace
{
std::vector<webrtc::SdpVideoFormat> SupportedH264Codecs(bool add_scalability_modes = false)
{
    //    if (!IsH264CodecSupported())
    //      return std::vector<SdpVideoFormat>();
    //    return {
    //        webrtc::CreateH264Format(webrtc::H264Profile::kProfileBaseline, webrtc::H264Level::kLevel3_1, "1"),
    //        webrtc::CreateH264Format(webrtc::H264Profile::kProfileBaseline, webrtc::H264Level::kLevel3_1, "0"),
    //        webrtc::CreateH264Format(webrtc::H264Profile::kProfileConstrainedBaseline, webrtc::H264Level::kLevel3_1, "1"),
    //        webrtc::CreateH264Format(webrtc::H264Profile::kProfileConstrainedBaseline, webrtc::H264Level::kLevel3_1, "0")};

    return {webrtc::CreateH264Format(webrtc::H264Profile::kProfileBaseline,
                                     webrtc::H264Level::kLevel3_1,
                                     "1",
                                     add_scalability_modes),
            webrtc::CreateH264Format(webrtc::H264Profile::kProfileBaseline,
                                     webrtc::H264Level::kLevel3_1,
                                     "0",
                                     add_scalability_modes),
            webrtc::CreateH264Format(webrtc::H264Profile::kProfileConstrainedBaseline,
                                     webrtc::H264Level::kLevel3_1,
                                     "1",
                                     add_scalability_modes),
            webrtc::CreateH264Format(webrtc::H264Profile::kProfileConstrainedBaseline,
                                     webrtc::H264Level::kLevel3_1,
                                     "0",
                                     add_scalability_modes),
            webrtc::CreateH264Format(webrtc::H264Profile::kProfileMain,
                                     webrtc::H264Level::kLevel3_1,
                                     "1",
                                     add_scalability_modes),
            webrtc::CreateH264Format(webrtc::H264Profile::kProfileMain,
                                     webrtc::H264Level::kLevel3_1,
                                     "0",
                                     add_scalability_modes)};
}
std::vector<webrtc::SdpVideoFormat> SupportedH264DecoderCodecs()
{
    //    if (!IsH264CodecSupported())
    //      return std::vector<SdpVideoFormat>();

    std::vector<webrtc::SdpVideoFormat> supportedCodecs = SupportedH264Codecs();

    // OpenH264 doesn't yet support High Predictive 4:4:4 encoding but it does
    // support decoding.
    supportedCodecs.push_back(
        CreateH264Format(webrtc::H264Profile::kProfilePredictiveHigh444, webrtc::H264Level::kLevel3_1, "1"));
    supportedCodecs.push_back(
        CreateH264Format(webrtc::H264Profile::kProfilePredictiveHigh444, webrtc::H264Level::kLevel3_1, "0"));

    return supportedCodecs;
}
} // namespace

class ExternalVideoEncoderFactory : public webrtc::VideoEncoderFactory
{
public:
    static std::unique_ptr<ExternalVideoEncoderFactory> Create()
    {
        return std::make_unique<ExternalVideoEncoderFactory>();
    }

    std::vector<webrtc::SdpVideoFormat> GetSupportedFormats() const override
    {
        std::vector<webrtc::SdpVideoFormat> supportedFormats;
#if USE_JETSON_HW_ENCODER
        supportedFormats.push_back(
            CreateH264Format(webrtc::H264Profile::kProfileConstrainedBaseline, webrtc::H264Level::kLevel4, "1"));
        supportedFormats.push_back(
            CreateH264Format(webrtc::H264Profile::kProfileConstrainedBaseline, webrtc::H264Level::kLevel4, "0"));
        supportedFormats.push_back(
            CreateH264Format(webrtc::H264Profile::kProfileBaseline, webrtc::H264Level::kLevel4, "1"));
        supportedFormats.push_back(
            CreateH264Format(webrtc::H264Profile::kProfileBaseline, webrtc::H264Level::kLevel4, "0"));
#else
        supportedFormats.push_back(webrtc::SdpVideoFormat(cricket::kVp8CodecName));
        //        for (const webrtc::SdpVideoFormat &format : webrtc::SupportedVP9Codecs())
        //        {
        //            supportedFormats.push_back(format);
        //        }
        for (const webrtc::SdpVideoFormat &format : webrtc::SupportedH264Codecs())
        {
            supportedFormats.push_back(format);
        }
#endif
        return supportedFormats;
    }

    std::unique_ptr<webrtc::VideoEncoder> Create(const webrtc::Environment &env,
                                                 const webrtc::SdpVideoFormat &format) override
    {
#if USE_JETSON_HW_ENCODER
        Args args_;
        return JetsonVideoEncoder::Create(args_);
#else
        if (utils::stringEqualsIgnoreCase(format.name, cricket::kVp8CodecName))
        {
            return webrtc::LibvpxVp8EncoderTemplateAdapter::CreateEncoder(env, format);
        }
        //        if (utils::stringEqualsIgnoreCase(format.name, cricket::kVp9CodecName))
        //        {
        //            return webrtc::LibvpxVp9EncoderTemplateAdapter::CreateEncoder(env, format);
        //        }
        if (utils::stringEqualsIgnoreCase(format.name, cricket::kH264CodecName))
        {
            return webrtc::OpenH264EncoderTemplateAdapter::CreateEncoder(env, format);
        }
#endif
        RTC_LOG(LS_WARNING) << "create video encoder failed, format not supported," << "format: " << format.name;
        return nullptr;
    }
};

class ExternalVideoDecoderFactory : public webrtc::VideoDecoderFactory
{
public:
    static std::unique_ptr<ExternalVideoDecoderFactory> Create()
    {
        return std::make_unique<ExternalVideoDecoderFactory>();
    }

    std::vector<webrtc::SdpVideoFormat> GetSupportedFormats() const override
    {
        std::vector<webrtc::SdpVideoFormat> supportedFormats;
        supportedFormats.push_back(webrtc::SdpVideoFormat(cricket::kVp8CodecName));
        //        for (const webrtc::SdpVideoFormat &format : webrtc::SupportedVP9Codecs())
        //        {
        //            supportedFormats.push_back(format);
        //        }
        for (const webrtc::SdpVideoFormat &format : SupportedH264DecoderCodecs())
        {
            supportedFormats.push_back(format);
        }

        //        if (kDav1dIsIncluded)
        //        {
        //            supportedFormats.push_back(SdpVideoFormat::AV1Profile0());
        //            supportedFormats.push_back(SdpVideoFormat::AV1Profile1());
        //        }

        return supportedFormats;
    }

    CodecSupport QueryCodecSupport(const webrtc::SdpVideoFormat &format, bool reference_scaling) const override
    {
        // Query for supported formats and check if the specified format is supported.
        // Return unsupported if an invalid combination of format and
        // reference_scaling is specified.
        if (reference_scaling)
        {
            VideoCodecType codec = PayloadStringToCodecType(format.name);
            if (codec != kVideoCodecVP9 && codec != kVideoCodecAV1)
            {
                return {/*is_supported=*/false, /*is_power_efficient=*/false};
            }
        }

        CodecSupport codec_support;
        codec_support.is_supported = format.IsCodecInList(this->GetSupportedFormats());
        return codec_support;
    }

    std::unique_ptr<webrtc::VideoDecoder> Create(const webrtc::Environment &env,
                                                 const webrtc::SdpVideoFormat &format) override
    {
        if (utils::stringEqualsIgnoreCase(format.name, cricket::kVp8CodecName))
        {
            return webrtc::LibvpxVp8DecoderTemplateAdapter::CreateDecoder(env, format);
        }
        //        if (utils::stringEqualsIgnoreCase(format.name, cricket::kVp9CodecName))
        //        {
        //            return webrtc::LibvpxVp9DecoderTemplateAdapter::CreateDecoder(format);
        //        }
        if (utils::stringEqualsIgnoreCase(format.name, cricket::kH264CodecName))
        {
            return webrtc::OpenH264DecoderTemplateAdapter::CreateDecoder(format);
        }
        RTC_LOG(LS_WARNING) << "create video decoder failed, format not supported," << "format: " << format.name;
        return nullptr;
    }
};
} // namespace detail

/***********************************************************************************************************************
 * RtcStatsWebRTC
***********************************************************************************************************************/
RtcStatsWebRTC::AttributeWebRTC::AttributeWebRTC(const webrtc::Attribute &attr)
    : mWebRTCAttr(attr)
{
}
RtcStats::Attribute::Type RtcStatsWebRTC::AttributeWebRTC::type() const
{
    if (mWebRTCAttr.holds_alternative<bool>())
    {
        return Type::kBool;
    }
    else if (mWebRTCAttr.holds_alternative<int32_t>())
    {
        return Type::kInt32;
    }
    else if (mWebRTCAttr.holds_alternative<uint32_t>())
    {
        return Type::kUint32;
    }
    else if (mWebRTCAttr.holds_alternative<int64_t>())
    {
        return Type::kInt64;
    }
    else if (mWebRTCAttr.holds_alternative<uint64_t>())
    {
        return Type::kUint64;
    }
    else if (mWebRTCAttr.holds_alternative<double>())
    {
        return Type::kDouble;
    }
    else if (mWebRTCAttr.holds_alternative<std::string>())
    {
        return Type::kString;
    }
    else if (mWebRTCAttr.holds_alternative<std::vector<bool>>())
    {
        return Type::kBoolVector;
    }
    else if (mWebRTCAttr.holds_alternative<std::vector<int32_t>>())
    {
        return Type::kInt32Vector;
    }
    else if (mWebRTCAttr.holds_alternative<std::vector<uint32_t>>())
    {
        return Type::kUint32Vector;
    }
    else if (mWebRTCAttr.holds_alternative<std::vector<int64_t>>())
    {
        return Type::kInt64Vector;
    }
    else if (mWebRTCAttr.holds_alternative<std::vector<uint64_t>>())
    {
        return Type::kUint64Vector;
    }
    else if (mWebRTCAttr.holds_alternative<std::vector<double>>())
    {
        return Type::kDoubleVector;
    }
    else if (mWebRTCAttr.holds_alternative<std::vector<std::string>>())
    {
        return Type::kStringVector;
    }
    return Type::kString;
}
RtcStats::Attribute::StringUint64Map RtcStatsWebRTC::AttributeWebRTC::toStringUint64Map() const
{
    // return attr_.get<StringUint64Map>();
    return {};
}
RtcStats::Attribute::StringDoubleMap RtcStatsWebRTC::AttributeWebRTC::toStringDoubleMap() const
{
    // return attr_.get<StringDoubleMap>();
    return {};
}

/***********************************************************************************************************************
 * RtcPeerConnectionWebRTC
***********************************************************************************************************************/
namespace
{
class SetLocalDescriptionObserver : public webrtc::SetLocalDescriptionObserverInterface
{
public:
    using OnSetSdpSuccess = RtcPeerConnection::OnSetSdpSuccess;
    using OnSetSdpFailure = RtcPeerConnection::OnSetSdpFailure;

    static webrtc::scoped_refptr<SetLocalDescriptionObserver> create(OnSetSdpSuccess success, OnSetSdpFailure failure)
    {
        return webrtc::make_ref_counted<SetLocalDescriptionObserver>(std::move(success), std::move(failure));
    }

    SetLocalDescriptionObserver(OnSetSdpSuccess success, OnSetSdpFailure failure)
        : mOnSuccess(std::move(success))
        , mOnFailure(std::move(failure))
    {
    }
    ~SetLocalDescriptionObserver() override { }

    void OnSetLocalDescriptionComplete(webrtc::RTCError error) override
    {
        RTC_LOG(LS_INFO) << __FUNCTION__;
        if (error.ok())
        {
            mOnSuccess();
        }
        else
        {
            mOnFailure(error.message());
        }
    }

    virtual void OnSuccess()
    {
        RTC_LOG(LS_INFO) << __FUNCTION__;
        mOnSuccess();
    }
    virtual void OnFailure(webrtc::RTCError error)
    {
        RTC_LOG(LS_INFO) << __FUNCTION__ << " " << error.message();
        mOnFailure(error.message());
    }

private:
    OnSetSdpSuccess mOnSuccess;
    OnSetSdpFailure mOnFailure;
};

class SetRemoteDescriptionObserver : public webrtc::SetRemoteDescriptionObserverInterface
{
public:
    using OnSetSdpSuccess = RtcPeerConnection::OnSetSdpSuccess;
    using OnSetSdpFailure = RtcPeerConnection::OnSetSdpFailure;

    static webrtc::scoped_refptr<SetRemoteDescriptionObserver> create(OnSetSdpSuccess success, OnSetSdpFailure failure)
    {
        return webrtc::make_ref_counted<SetRemoteDescriptionObserver>(std::move(success), std::move(failure));
    }

    SetRemoteDescriptionObserver(OnSetSdpSuccess success, OnSetSdpFailure failure)
        : mOnSuccess(std::move(success))
        , mOnFailure(std::move(failure))
    {
    }
    ~SetRemoteDescriptionObserver() override { }

    void OnSetRemoteDescriptionComplete(webrtc::RTCError error) override
    {
        RTC_LOG(LS_INFO) << __FUNCTION__;
        if (error.ok())
        {
            mOnSuccess();
        }
        else
        {
            mOnFailure(error.message());
        }
    }

    virtual void OnSuccess()
    {
        RTC_LOG(LS_INFO) << __FUNCTION__;
        mOnSuccess();
    }
    virtual void OnFailure(webrtc::RTCError error)
    {
        RTC_LOG(LS_INFO) << __FUNCTION__ << " " << error.message();
        mOnFailure(error.message());
    }

private:
    OnSetSdpSuccess mOnSuccess;
    OnSetSdpFailure mOnFailure;
};

class CreateSessionDescriptionObserver : public webrtc::CreateSessionDescriptionObserver
{
public:
    using OnSdpCreateSuccess = RtcPeerConnection::OnSdpCreateSuccess;
    using OnSdpCreateFailure = RtcPeerConnection::OnSdpCreateFailure;

    static webrtc::scoped_refptr<CreateSessionDescriptionObserver> create(OnSdpCreateSuccess success,
                                                                          OnSdpCreateFailure failure)
    {
        return webrtc::make_ref_counted<CreateSessionDescriptionObserver>(std::move(success), std::move(failure));
    }

    CreateSessionDescriptionObserver(OnSdpCreateSuccess success, OnSdpCreateFailure failure)
        : mOnSuccess(std::move(success))
        , mOnFailure(std::move(failure))
    {
    }
    ~CreateSessionDescriptionObserver() override { }

    void OnSuccess(webrtc::SessionDescriptionInterface *desc) override
    {
        std::string sdp;
        desc->ToString(&sdp);
        std::string type = desc->type();
        mOnSuccess(sdp.c_str(), type.c_str());
    }

    void OnFailure(webrtc::RTCError error) override { mOnFailure(error.message()); }

private:
    OnSdpCreateSuccess mOnSuccess;
    OnSdpCreateFailure mOnFailure;
};

class RTCStatsCollectorCallback : public webrtc::RTCStatsCollectorCallback
{
public:
    using OnStatsCollectorSuccess = RtcPeerConnection::OnStatsCollectorSuccess;
    using OnStatsCollectorFailure = RtcPeerConnection::OnStatsCollectorFailure;

    RTCStatsCollectorCallback(OnStatsCollectorSuccess success, OnStatsCollectorFailure failure)
        : mOnSuccess(std::move(success))
        , mOnFailure(std::move(failure))
    {
    }
    ~RTCStatsCollectorCallback() override { }

    static webrtc::scoped_refptr<RTCStatsCollectorCallback> Create(OnStatsCollectorSuccess success,
                                                                   OnStatsCollectorFailure failure)
    {
        auto instance = webrtc::make_ref_counted<RTCStatsCollectorCallback>(std::move(success), std::move(failure));
        instance->AddRef();
        return instance;
    }

    void OnStatsDelivered(const webrtc::scoped_refptr<const webrtc::RTCStatsReport> &report) override
    {
        webrtc::RTCStatsReport::ConstIterator iter = report->begin();
        std::vector<RtcStats::SharedPtr> reports;
        while (iter != report->end())
        {
            reports.push_back(utils::make_shared<RtcStatsWebRTC>(iter->copy()));
            iter++;
        }
        mOnSuccess(reports);
    }

private:
    OnStatsCollectorSuccess mOnSuccess;
    OnStatsCollectorFailure mOnFailure;
};
} // namespace
RtcPeerConnectionWebRTC::RtcPeerConnectionWebRTC(
    const RtcConfiguration &configuration,
    RtcMediaConstraints::SharedPtr constraints,
    webrtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> peer_connection_factory)
    : mWebRTCPeerConnectionFactory(peer_connection_factory)
    , mConfiguration(configuration)
    , mConstraints(constraints)
    , mCallbackMutex(new webrtc::Mutex())
{
    OCTK_TRACE() << __FUNCTION__ << ": ctor" << this;
}

RtcPeerConnectionWebRTC::~RtcPeerConnectionWebRTC()
{
    OCTK_TRACE() << __FUNCTION__ << ": dtor" << this;
}

Status RtcPeerConnectionWebRTC::initialize()
{
    Status status = mInitOnceFlag.isNeverCalled() ? Status::ok : Status(mLastError);
    if (mInitOnceFlag.enter())
    {
        auto scope = utils::makeScopeGuard([this]() { mInitOnceFlag.leave(); });
        if (!mWebRTCPeerConnectionFactory.get())
        {
            mLastError = "PeerConnectionFactory is null";
            OCTK_WARNING("%s", mLastError.c_str());
            status = Status(mLastError);
            return status;
        }

        webrtc::PeerConnectionInterface::IceServers servers;
        webrtc::PeerConnectionInterface::RTCConfiguration config;
        config.rtcp_mux_policy = webrtc::PeerConnectionInterface::kRtcpMuxPolicyNegotiate;
        config.candidate_network_policy = webrtc::PeerConnectionInterface::kCandidateNetworkPolicyAll;

        for (int i = 0; i < RtcIceServer::kMaxSize; i++)
        {
            const auto &ice_server = mConfiguration.ice_servers[i];
            if (ice_server.uri.size() > 0)
            {
                webrtc::PeerConnectionInterface::IceServer server;
                server.uri = ice_server.uri.std_string();
                server.username = ice_server.username.std_string();
                server.password = ice_server.password.std_string();
                config.servers.push_back(server);
            }
        }
        config.candidate_network_policy = utils::toWebRTC(mConfiguration.candidate_network_policy);
        config.tcp_candidate_policy = utils::toWebRTC(mConfiguration.tcp_candidate_policy);
        config.rtcp_mux_policy = utils::toWebRTC(mConfiguration.rtcp_mux_policy);
        config.bundle_policy = utils::toWebRTC(mConfiguration.bundle_policy);
        config.sdp_semantics = utils::toWebRTC(mConfiguration.sdp_semantics);
        config.type = utils::toWebRTC(mConfiguration.type);

        mWebRTCOfferAnswerOptions.offer_to_receive_audio = mConfiguration.offer_to_receive_audio;
        mWebRTCOfferAnswerOptions.offer_to_receive_video = mConfiguration.offer_to_receive_video;
        mWebRTCOfferAnswerOptions.use_rtp_mux = mConfiguration.use_rtp_mux;

        // config.disable_ipv6 = configuration_.disable_ipv6;
        config.disable_ipv6_on_wifi = mConfiguration.disable_ipv6_on_wifi;
        config.disable_link_local_networks = mConfiguration.disable_link_local_networks;
        config.max_ipv6_networks = mConfiguration.max_ipv6_networks;

        if (mConfiguration.screencast_min_bitrate > 0)
        {
            config.screencast_min_bitrate = mConfiguration.screencast_min_bitrate;
        }

        const auto mediaConstraints = utils::dynamic_pointer_cast<RtcMediaConstraintsWebRTC>(mConstraints);
        if (mediaConstraints)
        {
            webrtc::MediaConstraints webRTCConstraints(mediaConstraints->webrtcMandatory(),
                                                       mediaConstraints->webrtcOptional());
            detail::copyConstraintsIntoRtcConfiguration(&webRTCConstraints, &config);
        }

        webrtc::PeerConnectionFactoryInterface::Options options;
        options.disable_encryption = (mConfiguration.srtp_type == RtcMediaSecurityType::kSRTP_None);
        // options.network_ignore_mask |= ADAPTER_TYPE_CELLULAR;
        mWebRTCPeerConnectionFactory->SetOptions(options);

        webrtc::PeerConnectionDependencies dependencies(this);
        auto result = mWebRTCPeerConnectionFactory->CreatePeerConnectionOrError(config, std::move(dependencies));
        if (!result.ok())
        {
            mLastError = std::string("CreatePeerConnection failed: ") + result.error().message();
            OCTK_WARNING("%s", mLastError.c_str());
            status = Status(mLastError);
            return status;
        }
        mWebRTCPeerConnection = result.MoveValue();
    }
    return status;
}

void RtcPeerConnectionWebRTC::restartIce()
{
    RTC_LOG(LS_INFO) << __FUNCTION__;
    if (mWebRTCPeerConnection.get())
    {
        mWebRTCPeerConnection->RestartIce();
    }
}

void RtcPeerConnectionWebRTC::close()
{
    RTC_LOG(LS_INFO) << __FUNCTION__;
    if (mWebRTCPeerConnection.get())
    {
        mWebRTCPeerConnection = nullptr;
        mDataChannel = nullptr;
        mLocalStreams.clear();
        for (auto stream : mRemoteStreams)
        {
            if (mObserver)
            {
                mObserver->onRemoveStream(stream);
            }
            /*   stream->GetAudioTracks([&](scoped_refptr<RTCMediaTrack> track) {
                 observer_->OnRemoveTrack([&](OnRTCMediaStream on) { on(stream); },
                                          track);
               });
               stream->GetVideoTracks([&](scoped_refptr<RTCMediaTrack> track) {
                 observer_->OnRemoveTrack([&](OnRTCMediaStream on) { on(stream); },
                                          track);
               });*/
        }
        mRemoteStreams.clear();
    }
}

int RtcPeerConnectionWebRTC::addStream(const RtcMediaStream::SharedPtr &stream)
{
    auto send_stream = utils::dynamic_pointer_cast<RtcMediaStreamWebRTC>(stream);
    auto rtc_media_stream = send_stream->webrtcMediaStream();

    send_stream->RegisterRTCPeerConnectionObserver(mObserver);

    if (std::find(mLocalStreams.begin(), mLocalStreams.end(), stream) != mLocalStreams.end())
    {
        return -1; // Already added.
    }

    if (!mWebRTCPeerConnection->AddStream(rtc_media_stream.get()))
    {
        RTC_LOG(LS_ERROR) << "Adding stream to PeerConnection failed";
    }

    mLocalStreams.push_back(stream);
    return 0;
}

int RtcPeerConnectionWebRTC::removeStream(const RtcMediaStream::SharedPtr &stream)
{
    auto send_stream = utils::dynamic_pointer_cast<RtcMediaStreamWebRTC>(stream);
    auto rtc_media_stream = send_stream->webrtcMediaStream();

    if (std::find(mLocalStreams.begin(), mLocalStreams.end(), stream) == mLocalStreams.end())
    {
        return -1; // Not found.
    }

    mWebRTCPeerConnection->RemoveStream(rtc_media_stream.get());

    mLocalStreams.erase(std::find(mLocalStreams.begin(), mLocalStreams.end(), stream));
    return 0;
}

RtcMediaStream::SharedPtr RtcPeerConnectionWebRTC::createLocalMediaStream(StringView streamId)
{
    if (!mWebRTCPeerConnectionFactory.get())
    {
        return nullptr;
    }
    auto stream = mWebRTCPeerConnectionFactory->CreateLocalMediaStream(streamId.data());
    auto rtc_stream = utils::make_shared<RtcMediaStreamWebRTC>(stream);
    mLocalStreams.push_back(rtc_stream);
    return rtc_stream;
}

RtcDataChannel::SharedPtr RtcPeerConnectionWebRTC::createDataChannel(StringView label,
                                                                     RtcDataChannelInit *dataChannelDict)
{
    webrtc::DataChannelInit init;
    init.id = dataChannelDict->id;
    init.maxRetransmits = dataChannelDict->maxRetransmits;
    init.protocol = dataChannelDict->protocol.std_string();
    init.negotiated = dataChannelDict->negotiated;
    init.reliable = dataChannelDict->reliable;
    init.ordered = dataChannelDict->ordered;
    init.id = dataChannelDict->id;

    auto result = mWebRTCPeerConnection->CreateDataChannelOrError(label.data(), &init);
    if (!result.ok())
    {
        RTC_LOG(LS_ERROR) << "CreateDataChannel failed: " << ToString(result.error().type()) << " "
                          << result.error().message();
        return nullptr;
    }

    mDataChannel = utils::make_shared<RtcDataChannelWebRTC>(result.MoveValue());
    dataChannelDict->id = init.id;
    return mDataChannel;
}

void RtcPeerConnectionWebRTC::createOffer(OnSdpCreateSuccess success,
                                          OnSdpCreateFailure failure,
                                          const RtcMediaConstraints::SharedPtr &constraints)
{
    if (!mWebRTCPeerConnection.get() || !mWebRTCPeerConnectionFactory.get())
    {
        webrtc::MutexLock cs(mCallbackMutex.get());
        failure("Failed to initialize PeerConnection");
        return;
    }

    webrtc::PeerConnectionInterface::RTCOfferAnswerOptions offerAnswerOptions;
    RtcMediaConstraintsWebRTC *mediaConstraints = static_cast<RtcMediaConstraintsWebRTC *>(constraints.get());
    if (mediaConstraints)
    {
        webrtc::MediaConstraints webRTCConstraints(mediaConstraints->GetMandatory(), mediaConstraints->GetOptional());
        if (detail::copyConstraintsIntoOfferAnswerOptions(&webRTCConstraints, &offerAnswerOptions) == false)
        {
            offerAnswerOptions = mWebRTCOfferAnswerOptions;
        }
    }
    const auto observer = CreateSessionDescriptionObserver::create(std::move(success), std::move(failure));
    mWebRTCPeerConnection->CreateOffer(observer.get(), offerAnswerOptions);
}

void RtcPeerConnectionWebRTC::createAnswer(OnSdpCreateSuccess success,
                                           OnSdpCreateFailure failure,
                                           const RtcMediaConstraints::SharedPtr &constraints)
{
    if (!mWebRTCPeerConnection.get() || !mWebRTCPeerConnectionFactory.get())
    {
        webrtc::MutexLock cs(mCallbackMutex.get());
        failure("Failed to initialize PeerConnection");
        return;
    }
    webrtc::PeerConnectionInterface::RTCOfferAnswerOptions offerAnswerOptions;
    RtcMediaConstraintsWebRTC *mediaConstraints = static_cast<RtcMediaConstraintsWebRTC *>(constraints.get());
    if (mediaConstraints)
    {
        webrtc::MediaConstraints webRTCConstraints(mediaConstraints->GetMandatory(), mediaConstraints->GetOptional());
        if (detail::copyConstraintsIntoOfferAnswerOptions(&webRTCConstraints, &offerAnswerOptions) == false)
        {
            offerAnswerOptions = mWebRTCOfferAnswerOptions;
        }
    }
    const auto observer = CreateSessionDescriptionObserver::create(std::move(success), std::move(failure));
    mWebRTCPeerConnection->CreateAnswer(observer.get(), offerAnswerOptions);
}

void RtcPeerConnectionWebRTC::setLocalDescription(StringView sdp,
                                                  StringView type,
                                                  OnSetSdpSuccess success,
                                                  OnSetSdpFailure failure)
{
    webrtc::SdpParseError error;
    std::optional<webrtc::SdpType> maybe_type = webrtc::SdpTypeFromString(type.data());
    if (!maybe_type)
    {
        return;
    }
    auto session_description(webrtc::CreateSessionDescription(*maybe_type, sdp.data(), &error));
    if (!session_description)
    {
        std::string error = "Can't parse received session description message.";
        RTC_LOG(LS_WARNING) << error;
        failure(error.c_str());
        return;
    }
    auto observer = SetLocalDescriptionObserver::create(std::move(success), std::move(failure));
    mWebRTCPeerConnection->SetLocalDescription(std::move(session_description), observer);
}

void RtcPeerConnectionWebRTC::setRemoteDescription(StringView sdp,
                                                   StringView type,
                                                   OnSetSdpSuccess success,
                                                   OnSetSdpFailure failure)
{
    RTC_LOG(LS_INFO) << " Received session description :" << sdp.data();
    webrtc::SdpParseError error;
    webrtc::SdpParseError sdp_error;
    std::optional<webrtc::SdpType> maybe_type = webrtc::SdpTypeFromString(type.data());
    if (!maybe_type)
    {
        return;
    }
    auto session_description(webrtc::CreateSessionDescription(*maybe_type, sdp.data(), &error));
    if (!session_description)
    {
        std::string error = "Can't parse received session description message.";
        RTC_LOG(LS_WARNING) << error;
        failure(error.c_str());
        return;
    }

    auto media_content_desc = session_description->description()->GetContentDescriptionByName("video");
    // auto media_content_desc = (webrtc::MediaContentDescription *)content_desc;

    if (media_content_desc && mConfiguration.local_video_bandwidth > 0)
    {
        media_content_desc->set_bandwidth(mConfiguration.local_video_bandwidth * 1000);
    }
    auto observer = SetRemoteDescriptionObserver::create(std::move(success), std::move(failure));
    mWebRTCPeerConnection->SetRemoteDescription(std::move(session_description), observer);
}

void RtcPeerConnectionWebRTC::getLocalDescription(OnGetSdpSuccess success, OnGetSdpFailure failure)
{
    auto local_description = mWebRTCPeerConnection->local_description();
    if (!local_description)
    {
        if (failure)
        {
            failure("not local description");
        }
        return;
    }

    if (success)
    {
        std::string dsp;
        local_description->ToString(&dsp);
        success(dsp.c_str(), webrtc::SdpTypeToString(local_description->GetType()));
    }
}

void RtcPeerConnectionWebRTC::getRemoteDescription(OnGetSdpSuccess success, OnGetSdpFailure failure)
{
    auto remote_description = mWebRTCPeerConnection->remote_description();
    if (!remote_description)
    {
        if (failure)
        {
            failure("not remote description");
        }
        return;
    }

    if (success)
    {
        std::string dsp;
        remote_description->ToString(&dsp);
        success(dsp.c_str(), webrtc::SdpTypeToString(remote_description->GetType()));
    }
}

void RtcPeerConnectionWebRTC::addCandidate(StringView mid, int midMLineIndex, StringView candiate)
{
    webrtc::SdpParseError error;
    auto candidate = webrtc::CreateIceCandidate(mid.data(), midMLineIndex, candiate.data(), &error);
    if (candidate)
    {
        mWebRTCPeerConnection->AddIceCandidate(candidate);
    }
}

void RtcPeerConnectionWebRTC::registerObserver(Observer *observer)
{
    webrtc::MutexLock cs(mCallbackMutex.get());
    mObserver = observer;
}

void RtcPeerConnectionWebRTC::deregisterObserver()
{
    webrtc::MutexLock cs(mCallbackMutex.get());
    mObserver = nullptr;
}

Vector<RtcMediaStream::SharedPtr> RtcPeerConnectionWebRTC::localStreams()
{
    return mLocalStreams;
}

Vector<RtcMediaStream::SharedPtr> RtcPeerConnectionWebRTC::remoteStreams()
{
    return mRemoteStreams;
}

bool RtcPeerConnectionWebRTC::getStats(const RtcRtpSender::SharedPtr &sender,
                                       OnStatsCollectorSuccess success,
                                       OnStatsCollectorFailure failure)
{
    auto rtc_callback = RTCStatsCollectorCallback::Create(std::move(success), std::move(failure));
    if (!mWebRTCPeerConnection.get() || !mWebRTCPeerConnectionFactory.get())
    {
        webrtc::MutexLock cs(mCallbackMutex.get());
        failure("Failed to initialize PeerConnection");
        return false;
    }
    const auto impl = utils::dynamic_pointer_cast<RtcRtpSenderWebRTC>(sender);
    if (impl)
    {
        mWebRTCPeerConnection->GetStats(impl->rtc_rtp_sender(), rtc_callback);
        return true;
    }
    return false;
}

bool RtcPeerConnectionWebRTC::getStats(const RtcRtpReceiver::SharedPtr &receiver,
                                       OnStatsCollectorSuccess success,
                                       OnStatsCollectorFailure failure)
{
    auto rtc_callback = RTCStatsCollectorCallback::Create(std::move(success), std::move(failure));
    if (!mWebRTCPeerConnection.get() || !mWebRTCPeerConnectionFactory.get())
    {
        webrtc::MutexLock cs(mCallbackMutex.get());
        failure("Failed to initialize PeerConnection");
        return false;
    }
    const auto impl = utils::dynamic_pointer_cast<RtcRtpReceiverWebRTC>(receiver);
    if (impl)
    {
        mWebRTCPeerConnection->GetStats(impl->rtp_receiver(), rtc_callback);
        return true;
    }
    return false;
}

void RtcPeerConnectionWebRTC::getStats(OnStatsCollectorSuccess success, OnStatsCollectorFailure failure)
{
    auto rtc_callback = RTCStatsCollectorCallback::Create(std::move(success), std::move(failure));
    if (!mWebRTCPeerConnection.get() || !mWebRTCPeerConnectionFactory.get())
    {
        webrtc::MutexLock cs(mCallbackMutex.get());
        failure("Failed to initialize PeerConnection");
        return;
    }
    mWebRTCPeerConnection->GetStats(rtc_callback.get());
}

Result<RtcRtpTransceiver::SharedPtr> RtcPeerConnectionWebRTC::addTransceiver(
    const RtcMediaTrack::SharedPtr &track,
    const RtcRtpTransceiverInit::SharedPtr &init)
{
    const auto initImpl = utils::dynamic_pointer_cast<RtcRtpTransceiverInitWebRTC>(init);
    if (!initImpl)
    {
        return Error::create("init type error, not RtcRtpTransceiverInitWebRTC type");
    }

    webrtc::RTCErrorOr<webrtc::scoped_refptr<webrtc::RtpTransceiverInterface>> errorOr;
    std::string kind = track->kind().std_string();
    if (0 == kind.compare(webrtc::MediaStreamTrackInterface::kVideoKind))
    {
        const auto impl = utils::dynamic_pointer_cast<RtcVideoTrackWebRTC>(track);
        if (impl)
        {
            errorOr = mWebRTCPeerConnection->AddTransceiver(impl->rtc_track(), initImpl->rtp_transceiver_init());
        }
        else
        {
            return Error::create("track type error, not RtcVideoTrackWebRTC type");
        }
    }
    else if (0 == kind.compare(webrtc::MediaStreamTrackInterface::kAudioKind))
    {
        const auto impl = utils::dynamic_pointer_cast<RtcAudioTrackWebRTC>(track);
        if (impl)
        {
            errorOr = mWebRTCPeerConnection->AddTransceiver(impl->rtc_track(), initImpl->rtp_transceiver_init());
        }
        else
        {
            return Error::create("track type error, not RtcAudioTrackWebRTC type");
        }
    }

    if (errorOr.ok())
    {
        RtcRtpTransceiver::SharedPtr transceiver = utils::make_shared<RtcRtpTransceiverWebRTC>(errorOr.value());
        return transceiver;
    }
    return Error::create(errorOr.error().message());
}

Result<RtcRtpTransceiver::SharedPtr> RtcPeerConnectionWebRTC::addTransceiver(const RtcMediaTrack::SharedPtr &track)
{
    webrtc::RTCErrorOr<webrtc::scoped_refptr<webrtc::RtpTransceiverInterface>> errorOr;
    std::string kind = track->kind().std_string();
    if (0 == kind.compare(webrtc::MediaStreamTrackInterface::kVideoKind))
    {
        const auto impl = utils::dynamic_pointer_cast<RtcVideoTrackWebRTC>(track);
        if (impl)
        {
            errorOr = mWebRTCPeerConnection->AddTransceiver(impl->rtc_track());
        }
        else
        {
            return Error::create("track type error, not RtcVideoTrackWebRTC type");
        }
    }
    else if (0 == kind.compare(webrtc::MediaStreamTrackInterface::kAudioKind))
    {
        const auto impl = utils::dynamic_pointer_cast<RtcAudioTrackWebRTC>(track);
        if (impl)
        {
            errorOr = mWebRTCPeerConnection->AddTransceiver(impl->rtc_track());
        }
        else
        {
            return Error::create("track type error, not RtcAudioTrackWebRTC type");
        }
    }

    if (errorOr.ok())
    {
        RtcRtpTransceiver::SharedPtr transceiver = utils::make_shared<RtcRtpTransceiverWebRTC>(errorOr.value());
        return transceiver;
    }
    return Error::create(errorOr.error().message());
}

Result<RtcRtpTransceiver::SharedPtr> RtcPeerConnectionWebRTC::addTransceiver(RtcMediaType mediaType)
{
    webrtc::RTCErrorOr<webrtc::scoped_refptr<webrtc::RtpTransceiverInterface>> errorOr;
    if (RtcMediaType::kAudio == mediaType)
    {
        errorOr = mWebRTCPeerConnection->AddTransceiver(cricket::MEDIA_TYPE_AUDIO);
    }
    else if (RtcMediaType::kVideo == mediaType)
    {
        errorOr = mWebRTCPeerConnection->AddTransceiver(cricket::MEDIA_TYPE_VIDEO);
    }
    if (errorOr.ok())
    {
        RtcRtpTransceiver::SharedPtr transceiver = utils::make_shared<RtcRtpTransceiverWebRTC>(errorOr.value());
        return transceiver;
    }
    return Error::create(errorOr.error().message());
}

Result<RtcRtpTransceiver::SharedPtr> RtcPeerConnectionWebRTC::addTransceiver(
    RtcMediaType mediaType,
    const SharedPointer<RtcRtpTransceiverInit> &init)
{
    const auto initImpl = utils::dynamic_pointer_cast<RtcRtpTransceiverInitWebRTC>(init);
    if (!initImpl)
    {
        return Error::create("init type error, not RtcRtpTransceiverInitWebRTC type");
    }
    webrtc::RTCErrorOr<webrtc::scoped_refptr<webrtc::RtpTransceiverInterface>> errorOr;
    if (RtcMediaType::kAudio == mediaType)
    {
        errorOr = mWebRTCPeerConnection->AddTransceiver(cricket::MEDIA_TYPE_AUDIO, initImpl->rtp_transceiver_init());
    }
    else if (RtcMediaType::kVideo == mediaType)
    {
        errorOr = mWebRTCPeerConnection->AddTransceiver(cricket::MEDIA_TYPE_VIDEO, initImpl->rtp_transceiver_init());
    }
    if (errorOr.ok())
    {
        RtcRtpTransceiver::SharedPtr transceiver = utils::make_shared<RtcRtpTransceiverWebRTC>(errorOr.value());
        return transceiver;
    }
    return Error::create(errorOr.error().message());
}

Result<RtcRtpSender::SharedPtr> RtcPeerConnectionWebRTC::addTrack(const RtcMediaTrack::SharedPtr &track,
                                                                  const Vector<String> &streamIds)
{
    webrtc::RTCErrorOr<webrtc::scoped_refptr<webrtc::RtpSenderInterface>> errorOr;

    std::vector<std::string> stream_ids;
    for (auto id : streamIds.std_vector())
    {
        stream_ids.push_back(id.std_string());
    }
    std::string kind = track->kind().std_string();
    if (0 == kind.compare(webrtc::MediaStreamTrackInterface::kVideoKind))
    {
        const auto impl = utils::dynamic_pointer_cast<RtcVideoTrackWebRTC>(track);
        if (impl)
        {
            errorOr = mWebRTCPeerConnection->AddTrack(impl->rtc_track(), stream_ids);
        }
        else
        {
            return Error::create("track type error, not RtcVideoTrackWebRTC type");
        }
    }
    else if (0 == kind.compare(webrtc::MediaStreamTrackInterface::kAudioKind))
    {
        const auto impl = utils::dynamic_pointer_cast<RtcAudioTrackWebRTC>(track);
        if (impl)
        {
            errorOr = mWebRTCPeerConnection->AddTrack(impl->rtc_track(), stream_ids);
        }
        else
        {
            return Error::create("track type error, not RtcAudioTrackWebRTC type");
        }
    }

    if (errorOr.ok())
    {
        RtcRtpSender::SharedPtr sender = utils::make_shared<RtcRtpSenderWebRTC>(errorOr.value());
        return sender;
    }
    return Error::create(errorOr.error().message());
}

bool RtcPeerConnectionWebRTC::RemoveTrack(const RtcRtpSender::SharedPtr &sender)
{
    const auto impl = utils::dynamic_pointer_cast<RtcRtpSenderWebRTC>(sender);
    if (impl)
    {
        webrtc::RTCError err = mWebRTCPeerConnection->RemoveTrackOrError(impl->rtc_rtp_sender());
        if (err.ok())
        {
            return true;
        }
    }
    return false;
}

Vector<RtcRtpSender::SharedPtr> RtcPeerConnectionWebRTC::senders()
{
    std::vector<RtcRtpSender::SharedPtr> vec;
    for (auto item : mWebRTCPeerConnection->GetSenders())
    {
        vec.push_back(utils::make_shared<RtcRtpSenderWebRTC>(item));
    }
    return vec;
}

Vector<RtcRtpReceiver::SharedPtr> RtcPeerConnectionWebRTC::receivers()
{
    std::vector<RtcRtpReceiver::SharedPtr> vec;
    for (auto item : mWebRTCPeerConnection->GetReceivers())
    {
        vec.push_back(utils::make_shared<RtcRtpReceiverWebRTC>(item));
    }
    return vec;
}

Vector<RtcRtpTransceiver::SharedPtr> RtcPeerConnectionWebRTC::transceivers()
{
    std::vector<RtcRtpTransceiver::SharedPtr> vec;
    for (auto item : mWebRTCPeerConnection->GetTransceivers())
    {
        vec.push_back(utils::make_shared<RtcRtpTransceiverWebRTC>(item));
    }
    return vec;
}

RtcPeerConnection::SignalingState RtcPeerConnectionWebRTC::signalingState()
{
    return utils::fromWebRTC(mWebRTCPeerConnection->signaling_state());
}

RtcPeerConnection::IceGatheringState RtcPeerConnectionWebRTC::iceGatheringState()
{
    return utils::fromWebRTC(mWebRTCPeerConnection->ice_gathering_state());
}

RtcPeerConnection::IceConnectionState RtcPeerConnectionWebRTC::iceConnectionState()
{
    return utils::fromWebRTC(mWebRTCPeerConnection->ice_connection_state());
}

RtcPeerConnection::PeerConnectionState RtcPeerConnectionWebRTC::peerConnectionState()
{
    return utils::fromWebRTC(mWebRTCPeerConnection->peer_connection_state());
}

void RtcPeerConnectionWebRTC::OnAddTrack(
    webrtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver,
    const std::vector<webrtc::scoped_refptr<webrtc::MediaStreamInterface>> &streams)
{
    OCTK_TRACE("[%p]%s(%p, %l)", this, OCTK_STRFUNC_NAME, receiver.get(), streams.size());
    if (mObserver)
    {
        std::vector<SharedPointer<RtcMediaStream>> out_streams;
        for (auto item : streams)
        {
            out_streams.push_back(utils::make_shared<RtcMediaStreamWebRTC>(item));
        }
        auto rtc_receiver = utils::make_shared<RtcRtpReceiverWebRTC>(receiver);
        mObserver->onAddTrack(out_streams, rtc_receiver);
    }
}

void RtcPeerConnectionWebRTC::OnTrack(webrtc::scoped_refptr<webrtc::RtpTransceiverInterface> transceiver)
{
    OCTK_TRACE("[%p]%s(%p)", this, OCTK_STRFUNC_NAME, transceiver.get());
    if (mObserver)
    {
        mObserver->onTrack(utils::make_shared<RtcRtpTransceiverWebRTC>(transceiver));
    }
}

void RtcPeerConnectionWebRTC::OnRemoveTrack(webrtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver)
{
    OCTK_TRACE("[%p]%s(%p)", this, OCTK_STRFUNC_NAME, receiver.get());
    if (mObserver)
    {
        mObserver->onRemoveTrack(utils::make_shared<RtcRtpReceiverWebRTC>(receiver));
    }
}

void RtcPeerConnectionWebRTC::OnAddStream(webrtc::scoped_refptr<webrtc::MediaStreamInterface> stream)
{
    OCTK_TRACE("[%p]%s(%p, %s)", this, OCTK_STRFUNC_NAME, stream.get(), stream->id().c_str());

    auto remote_stream = utils::make_shared<RtcMediaStreamWebRTC>(stream);
    remote_stream->RegisterRTCPeerConnectionObserver(mObserver);
    mRemoteStreams.push_back(remote_stream);

    if (mObserver)
    {
        mObserver->onAddStream(remote_stream);
    }
}

void RtcPeerConnectionWebRTC::OnRemoveStream(webrtc::scoped_refptr<webrtc::MediaStreamInterface> stream)
{
    OCTK_TRACE("[%p]%s(%p, %s)", this, OCTK_STRFUNC_NAME, stream.get(), stream->id().c_str());
    SharedPointer<RtcMediaStreamWebRTC> recv_stream;

    for (auto kv : mRemoteStreams)
    {
        const auto impl = utils::dynamic_pointer_cast<RtcMediaStreamWebRTC>(kv);
        if (impl && impl->webrtcMediaStream() == stream)
        {
            recv_stream = impl;
        }
    }

    if (recv_stream)
    {
        if (mObserver)
        {
            mObserver->onRemoveStream(recv_stream);
        }
        mRemoteStreams.erase(std::find(mRemoteStreams.begin(), mRemoteStreams.end(), recv_stream));
    }
}

void RtcPeerConnectionWebRTC::OnDataChannel(webrtc::scoped_refptr<webrtc::DataChannelInterface> data_channel)
{
    OCTK_TRACE("[%p]%s(%p, %s)", this, OCTK_STRFUNC_NAME, data_channel.get(), data_channel->label().c_str());
    mDataChannel = utils::make_shared<RtcDataChannelWebRTC>(data_channel);
    if (mObserver)
    {
        mObserver->onDataChannel(mDataChannel);
    }
}

void RtcPeerConnectionWebRTC::OnRenegotiationNeeded()
{
    OCTK_TRACE("[%p]%s()", this, OCTK_STRFUNC_NAME);
    if (mObserver)
    {
        mObserver->onRenegotiationNeeded();
    }
}

void RtcPeerConnectionWebRTC::OnIceCandidate(const webrtc::IceCandidateInterface *candidate)
{
    OCTK_TRACE("[%p]%s(%p)", this, OCTK_STRFUNC_NAME, candidate);
    // if (!rtc_peerconnection_)
    //     return;

#if 0
    if (candidate->candidate().protocol() != "tcp")
        return;


    // For loopback test. To save some connecting delay.
    if (type_ == kLoopBack) {
        if (!mWebRTCPeerConnection->AddIceCandidate(candidate)) {
            RTC_LOG(LS_WARNING) << "Failed to apply the received candidate";
        }
        return;
    }
#endif

    std::string cand_sdp;
    if (mObserver && candidate->ToString(&cand_sdp))
    {
        // SdpParseError error;
        // scoped_refptr<RTCIceCandidate> cand = RTCIceCandidate::Create(cand_sdp.c_str(),
        //                                                               candidate->sdp_mid().c_str(),
        //                                                               candidate->sdp_mline_index(),
        //                                                               &error);
        // observer_->onIceCandidate(cand);
    }

    RTC_LOG(LS_INFO) << __FUNCTION__ << ", mid " << candidate->sdp_mid() << ", mline " << candidate->sdp_mline_index()
                     << ", sdp" << cand_sdp;
}

void RtcPeerConnectionWebRTC::OnConnectionChange(webrtc::PeerConnectionInterface::PeerConnectionState new_state)
{
    OCTK_TRACE("[%p]%s(%s)", this, OCTK_STRFUNC_NAME, toString(utils::fromWebRTC(new_state)).data());
    if (mObserver)
    {
        mObserver->onPeerConnectionState(utils::fromWebRTC(new_state));
    }
}

void RtcPeerConnectionWebRTC::OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state)
{
    OCTK_TRACE("[%p]%s(%s)", this, OCTK_STRFUNC_NAME, toString(utils::fromWebRTC(new_state)).data());
    if (mObserver)
    {
        mObserver->onIceGatheringState(utils::fromWebRTC(new_state));
    }
}

void RtcPeerConnectionWebRTC::OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state)
{
    OCTK_TRACE("[%p]%s(%s)", this, OCTK_STRFUNC_NAME, toString(utils::fromWebRTC(new_state)).data());
    if (mObserver)
    {
        mObserver->onIceConnectionState(utils::fromWebRTC(new_state));
    }
}

void RtcPeerConnectionWebRTC::OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state)
{
    OCTK_TRACE("[%p]%s(%s)", this, OCTK_STRFUNC_NAME, toString(utils::fromWebRTC(new_state)).data());
    if (mObserver)
    {
        mObserver->onSignalingState(utils::fromWebRTC(new_state));
    }
}

/***********************************************************************************************************************
 * RtcPeerConnectionFactoryWebRTC
***********************************************************************************************************************/
Status RtcPeerConnectionFactoryWebRTC::terminate()
{
    mWebRTCWorkerThread->BlockingCall(
        [&]
        {
            // audio_device_impl_ = nullptr;
            // video_device_impl_ = nullptr;
            // audio_processing_impl_ = nullptr;
        });
    mWebRTCPeerConnectionFactory = NULL;
    if (mWebRTCAudioDeviceModule)
    {
        mWebRTCWorkerThread->BlockingCall(
            [this]
            {
                mWebRTCAudioDeviceModule = nullptr;
                // DestroyAudioDeviceModule_w();
            });
    }

    return Status::ok;
}

Status RtcPeerConnectionFactoryWebRTC::initialize()
{
    Status status = mInitOnceFlag.isNeverCalled() ? Status::ok : Status(mLastError);
    utils::callOnce(mInitOnceFlag,
                    [this, &status]()
                    {
                        mWebRTCNetworkThread = rtc::Thread::CreateWithSocketServer();
                        mWebRTCNetworkThread->SetName("network_thread", nullptr);
                        if (!mWebRTCNetworkThread->Start())
                        {
                            mLastError = "WebRTC network thread start failed";
                            OCTK_WARNING("%s", mLastError.c_str());
                            status = mLastError;
                            return;
                        }

                        mWebRTCSignalingThread = rtc::Thread::Create();
                        mWebRTCSignalingThread->SetName("signaling_thread", nullptr);
                        if (!mWebRTCSignalingThread->Start())
                        {
                            mLastError = "WebRTC signaling thread start failed";
                            OCTK_WARNING("%s", mLastError.c_str());
                            status = mLastError;
                            return;
                        }

                        mWebRTCWorkerThread = rtc::Thread::Create();
                        mWebRTCWorkerThread->SetName("worker_thread", nullptr);
                        if (!mWebRTCWorkerThread->Start())
                        {
                            mLastError = "WebRTC worker thread start failed";
                            OCTK_WARNING("%s", mLastError.c_str());
                            status = mLastError;
                            return;
                        }

                        if (!mWebRTCAudioDeviceModule)
                        {
                            mWebRTCTaskQueueFactory = webrtc::CreateDefaultTaskQueueFactory();
                            mWebRTCWorkerThread->BlockingCall(
                                [&]
                                {
                                    if (!mWebRTCAudioDeviceModule)
                                    {
                                        //                                        mWebRTCAudioDeviceModule = webrtc::AudioDeviceModule::Create(
                                        //                                            webrtc::AudioDeviceModule::kPlatformDefaultAudio,
                                        //                                            mWebRTCTaskQueueFactory.get());
                                    }
                                    // CreateAudioDeviceModule_w();
                                });
                        }

                        // if (!audio_processing_impl_)
                        // {
                        //     worker_thread_->BlockingCall([this]
                        //                                  { audio_processing_impl_ =
                        //                                  //new RefCountedObject<RTCAudioProcessingImpl>(); });
                        // }
                        //
                        // if (!audio_transport_factory_)
                        // {
                        //     worker_thread_->BlockingCall(
                        //         [this] { audio_transport_factory_ = //
                        //         webrtc::make_ref_counted<CustomAudioTransportFactory>(); });
                        // }

                        mWebRTCPeerConnectionFactory = webrtc::CreatePeerConnectionFactory(
                            mWebRTCNetworkThread.get(),                 // network_thread
                            mWebRTCWorkerThread.get(),                  // worker_thread
                            mWebRTCSignalingThread.get(),               // signaling_thread
                            mWebRTCAudioDeviceModule,                   // default_adm
                            webrtc::CreateBuiltinAudioEncoderFactory(), // audio_encoder_factory
                            webrtc::CreateBuiltinAudioDecoderFactory(), // audio_decoder_factory
#if 0
                    webrtc::CreateBuiltinVideoEncoderFactory(), // video_encoder_factory
                    webrtc::CreateBuiltinVideoDecoderFactory(), // video_decoder_factory
#else
                            detail::ExternalVideoEncoderFactory::Create(), // video_encoder_factory
                            detail::ExternalVideoDecoderFactory::Create(), // video_decoder_factory
        //                            std::make_unique<webrtc::InternalDecoderFactory>(),
#endif
                            nullptr, // audio_mixer
                            nullptr, // audio_processing
                            nullptr  // owned_audio_frame_processor
                        );
                        if (!mWebRTCPeerConnectionFactory.get())
                        {
                            mLastError = "WebRTC create peerconnection factory failed";
                            OCTK_WARNING("%s", mLastError.c_str());
                            status = mLastError;
                            this->terminate();
                        }
                    });
    return status;
}

uint32_t RtcPeerConnectionFactoryWebRTC::version() const
{
    return OCTK_3RDPARTY_WEBRTC_VERSION;
}

StringView RtcPeerConnectionFactoryWebRTC::versionName() const
{
    static OnceFlag onceFlag;
    static std::string versionName;
    if (onceFlag.enter())
    {
#ifdef OCTK_3RDPARTY_WEBRTC_MILESTONE
        versionName = OCTK_3RDPARTY_WEBRTC_MILESTONE;
        versionName += ".";
#endif
        versionName += std::to_string(OCTK_3RDPARTY_WEBRTC_VERSION);
        onceFlag.leave();
    }
    return versionName;
}

StringView RtcPeerConnectionFactoryWebRTC::backendName() const
{
    return RtcEngine::kBackendNameWebRTC;
}

RtcPeerConnection::SharedPtr RtcPeerConnectionFactoryWebRTC::create(const RtcConfiguration &configuration,
                                                                    const RtcMediaConstraints::SharedPtr &constraints)
{
    auto peerConnection = utils::make_shared<RtcPeerConnectionWebRTC>(configuration,
                                                                      constraints,
                                                                      mWebRTCPeerConnectionFactory);
    mPeerConnections.push_back(peerConnection);
    return peerConnection;
}

void RtcPeerConnectionFactoryWebRTC::destroy(const RtcPeerConnection::SharedPtr &peerconnection)
{
    mPeerConnections.erase(std::remove_if(mPeerConnections.begin(),
                                          mPeerConnections.end(),
                                          [peerconnection](const SharedPointer<RtcPeerConnection> &pc_)
                                          { return pc_ == peerconnection; }),
                           mPeerConnections.end());
}

RtcAudioDevice::SharedPtr RtcPeerConnectionFactoryWebRTC::getAudioDevice()
{
    // if (!audio_device_module_)
    // {
    //     worker_thread_->BlockingCall([this] { CreateAudioDeviceModule_w(); });
    // }
    //
    // if (!audio_device_impl_)
    //     audio_device_impl_ = scoped_refptr<AudioDeviceImpl>(
    //         new RefCountedObject<AudioDeviceImpl>(audio_device_module_, worker_thread_.get()));
    //
    // return audio_device_impl_;
    return nullptr;
}

RtcVideoDevice::SharedPtr RtcPeerConnectionFactoryWebRTC::getVideoDevice()
{
    if (!video_device_impl_)
    {
        video_device_impl_ = utils::make_shared<RtcVideoDeviceWebRTC>(mWebRTCWorkerThread.get());
    }
    return video_device_impl_;
}

RtcAudioProcessor::SharedPtr RtcPeerConnectionFactoryWebRTC::getAudioProcessor()
{
    //     // if (!audio_processing_impl_)
    //     // {
    //     //     worker_thread_->BlockingCall([this]
    //     //                                  { audio_processing_impl_ = new RefCountedObject<RTCAudioProcessingImpl>(); });
    //     // }
    //     //
    //     // return audio_processing_impl_;
    return nullptr;
}

// SharedPointer<RtcAudioSource> RtcPeerConnectionFactoryWebRTC::createAudioSource(StringView audioSourceLabel,
//                                                                                 RtcAudioSource::Type sourceType)
// {
//     return nullptr;
// }
//
// SharedPointer<RtcVideoSource> RtcPeerConnectionFactoryWebRTC::createVideoSource(
//     const SharedPointer<RtcVideoCapturer> &capturer,
//     StringView videoSourceLabel,
//     const SharedPointer<RtcMediaConstraints> &constraints)
// {
//     auto func = [](const SharedPointer<RtcVideoCapturer> &capturer,
//                    StringView videoSourceLabel,
//                    const SharedPointer<RtcMediaConstraints> &constraints) -> SharedPointer<RtcVideoSource>
//     {
//         const auto impl = utils::dynamic_pointer_cast<RtcVideoCapturerWebRTC>(capturer);
//         if (impl)
//         {
//             auto rtc_source_track = webrtc::scoped_refptr<webrtc::VideoTrackSourceInterface>(
//                 new webrtc::RefCountedObject<webrtc::internal::CapturerTrackSource>(impl->video_capturer()));
//             auto source = scoped_refptr<RTCVideoSourceImpl>(new RefCountedObject<RTCVideoSourceImpl>(rtc_source_track));
//             return source;
//         }
//         return nullptr;
//     };
//     if (webrtc::Thread::Current() != signaling_thread_.get())
//     {
//         auto source = signaling_thread_->BlockingCall([this, func, capturer, videoSourceLabel, constraints]
//                                                       { return func(capturer, videoSourceLabel, constraints); });
//         return source;
//     }
//
//     return func(capturer, videoSourceLabel, constraints);
// }

RtcMediaConstraints::SharedPtr RtcPeerConnectionFactoryWebRTC::createMediaConstraints()
{
    return utils::make_shared<RtcMediaConstraintsWebRTC>();
}

Result<RtcAudioTrackSource::SharedPtr> RtcPeerConnectionFactoryWebRTC::createAudioTrackSource(
    const RtcAudioSource::SharedPtr &source,
    StringView label)
{
    return Error::create("not impl");
}

Result<RtcVideoTrackSource::SharedPtr> RtcPeerConnectionFactoryWebRTC::createVideoTrackSource(
    const RtcVideoSource::SharedPtr &source,
    StringView label)
{
    if (utils::dynamic_pointer_cast<RtcVideoTrackSource>(source))
    {
        return Error::create("RtcVideoTrackSource source backend type error");
    }
    if (utils::dynamic_pointer_cast<RtcVideoTrack>(source))
    {
        return Error::create("RtcVideoTrack source backend type error");
    }
    const auto iter = mVideoTrackSourceMap.find(source);
    if (mVideoTrackSourceMap.end() != iter)
    {
        return iter->second;
    }
    auto adapter = utils::make_shared<RtcVideoSourceWebRTCAdapter>();
    RtcVideoTrackSource::SharedPtr trackSource = utils::make_shared<RtcVideoTrackSourceWebRTC>(adapter);
    mVideoTrackSourceMap.insert(std::make_pair(source, trackSource));
    source->addSink(adapter->sink().get());
    return trackSource;
}

Result<RtcAudioTrack::SharedPtr> RtcPeerConnectionFactoryWebRTC::createAudioTrack(
    const RtcAudioTrackSource::SharedPtr &source,
    StringView trackId)
{
    return Error::create("not impl");
}

Result<RtcVideoTrack::SharedPtr> RtcPeerConnectionFactoryWebRTC::createVideoTrack(
    const RtcVideoTrackSource::SharedPtr &source,
    StringView trackId)
{
    const auto impl = utils::dynamic_pointer_cast<RtcVideoTrackSourceWebRTC>(source);
    if (impl)
    {
        auto rtcVideoTrack = mWebRTCPeerConnectionFactory->CreateVideoTrack(impl->rtcVideoTrackSource(),
                                                                            trackId.data());
        RtcVideoTrack::SharedPtr videoTrack = utils::make_shared<RtcVideoTrackWebRTC>(rtcVideoTrack);
        return videoTrack;
    }
    return Error::create("source backend type error");
}

Result<RtcVideoTrack::SharedPtr> RtcPeerConnectionFactoryWebRTC::createVideoTrack(
    const RtcVideoSource::SharedPtr &source,
    StringView trackId)
{
    const auto result = this->createVideoTrackSource(source, trackId);
    if (result)
    {
        return this->createVideoTrack(result.value(), trackId);
    }
    return Error::create("source backend type error");
}

RtcMediaStream::SharedPtr RtcPeerConnectionFactoryWebRTC::createLocalMediaStream(StringView streamId)
{
    auto mediaStream = mWebRTCPeerConnectionFactory->CreateLocalMediaStream(streamId.data());
    return utils::make_shared<RtcMediaStreamWebRTC>(mediaStream);
}

RtcRtpCapabilities::SharedPtr RtcPeerConnectionFactoryWebRTC::getRtpSenderCapabilities(RtcMediaType mediaType)
{
    if (webrtc::Thread::Current() != mWebRTCSignalingThread.get())
    {
        auto capabilities = mWebRTCSignalingThread->BlockingCall([this, mediaType]
                                                                 { return this->getRtpSenderCapabilities(mediaType); });
        return capabilities;
    }
    auto rtpCapabilities = mWebRTCPeerConnectionFactory->GetRtpSenderCapabilities(utils::toWebRTC(mediaType));
    return utils::make_shared<RtcRtpCapabilitiesWebRTC>(rtpCapabilities);
}

RtcRtpCapabilities::SharedPtr RtcPeerConnectionFactoryWebRTC::getRtpReceiverCapabilities(RtcMediaType mediaType)
{
    if (webrtc::Thread::Current() != mWebRTCSignalingThread.get())
    {
        auto capabilities = mWebRTCSignalingThread->BlockingCall(
            [this, mediaType] { return this->getRtpReceiverCapabilities(mediaType); });
        return capabilities;
    }
    auto rtpCapabilities = mWebRTCPeerConnectionFactory->GetRtpReceiverCapabilities(utils::toWebRTC(mediaType));
    return utils::make_shared<RtcRtpCapabilitiesWebRTC>(rtpCapabilities);
}
//
// void RtcPeerConnectionFactoryWebRTC::createAudioDeviceModule_w()
// {
//     // if (!mWebRTCAudioDeviceModule)
//     // {
//     //     mWebRTCAudioDeviceModule = webrtc::AudioDeviceModule::Create(webrtc::AudioDeviceModule::kPlatformDefaultAudio,
//     //                                                                  mWebRTCTaskQueueFactory.get());
//     // }
// }
//
// void RtcPeerConnectionFactoryWebRTC::destroyAudioDeviceModule_w()
// {
//     // if (mWebRTCAudioDeviceModule)
//     // {
//     //     mWebRTCAudioDeviceModule = nullptr;
//     // }
// }

OCTK_RTC_ENGINE_REGISTER_FACTORY(
    RtcPeerConnectionFactoryWebRTC,
    RtcEngine::kBackendNameWebRTC,
    []()
    {
        rtc::InitializeSSL();
        rtc::LogMessage::LogThreads(true);
        rtc::LogMessage::LogToDebug(rtc::LoggingSeverity::LS_NONE);
        rtc::LogMessage::AddLogToStream(detail::WebRTCRedirectLogSink::logSink(), rtc::LoggingSeverity::LS_VERBOSE);
    },
    [](LogLevel level) { WEBRTC_LOGGER().switchLevel(level); });

OCTK_END_NAMESPACE
