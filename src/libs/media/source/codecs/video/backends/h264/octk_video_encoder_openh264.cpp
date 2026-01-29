/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2026~Present ChengXueWen.
** Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
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

#include <private/octk_scalable_video_controller_p.hpp>
#include <private/octk_video_encoder_openh264_p.hpp>
#include <private/octk_h264_bitstream_parser_p.hpp>
#include <private/octk_scalability_structure_p.hpp>
#include <private/octk_simulcast_utility_p.hpp>
#include <private/octk_media_constants_p.hpp>
#include <octk_simulcast_rate_allocator.hpp>
#include <octk_metrics.hpp>
#include <octk_yuv.hpp>

#include <openh264/wels/codec_app_def.h>
#include <openh264/wels/codec_api.h>
#include <openh264/wels/codec_def.h>
#include <openh264/wels/codec_ver.h>

#if OCTK_FEATURE_MEDIA_USE_H264

OCTK_BEGIN_NAMESPACE

namespace
{
const bool kOpenH264EncoderDetailedLogging = false;

// QP scaling thresholds.
static const int kLowH264QpThreshold = 24;
static const int kHighH264QpThreshold = 37;

// Used by histograms. Values of entries should not be changed.
enum VideoEncoderOpenh264Event
{
    kH264EncoderEventInit = 0,
    kH264EncoderEventError = 1,
    kH264EncoderEventMax = 16,
};

int NumberOfThreads(Optional<int> encoder_thread_limit, int width, int height, int number_of_cores)
{
    // TODO(hbos): In Chromium, multiple threads do not work with sandbox on Mac,
    // see crbug.com/583348. Until further investigated, only use one thread.
    // While this limitation is gone, this changes the bitstream format (see
    // bugs.webrtc.org/14368) so still guarded by field trial to allow for
    // experimentation using th experimental
    // WebRTC-VideoEncoderSettings/encoder_thread_limit trial.
    if (encoder_thread_limit.has_value())
    {
        int limit = encoder_thread_limit.value();
        OCTK_DCHECK_GE(limit, 1);
        if (width * height >= 1920 * 1080 && number_of_cores > 8)
        {
            return std::min(limit, 8); // 8 threads for 1080p on high perf machines.
        }
        else if (width * height > 1280 * 960 && number_of_cores >= 6)
        {
            return std::min(limit, 3); // 3 threads for 1080p.
        }
        else if (width * height > 640 * 480 && number_of_cores >= 3)
        {
            return std::min(limit, 2); // 2 threads for qHD/HD.
        }
        else
        {
            return 1; // 1 thread for VGA or less.
        }
    }
    // TODO(sprang): Also check sSliceArgument.uiSliceNum on GetEncoderParams(),
    //               before enabling multithreading here.
    return 1;
}

VideoFrameType ConvertToVideoFrameType(EVideoFrameType type)
{
    switch (type)
    {
        case videoFrameTypeIDR: return VideoFrameType::kKey;
        case videoFrameTypeSkip:
        case videoFrameTypeI:
        case videoFrameTypeP:
        case videoFrameTypeIPMixed: return VideoFrameType::kDelta;
        case videoFrameTypeInvalid: break;
    }
    OCTK_DCHECK_NOTREACHED() << "Unexpected/invalid frame type: " << type;
    return VideoFrameType::kEmpty;
}

Optional<ScalabilityMode> ScalabilityModeFromTemporalLayers(int num_temporal_layers)
{
    switch (num_temporal_layers)
    {
        case 0: break;
        case 1: return ScalabilityMode::kL1T1;
        case 2: return ScalabilityMode::kL1T2;
        case 3: return ScalabilityMode::kL1T3;
        default: OCTK_DCHECK_NOTREACHED();
    }
    return utils::nullopt;
}


// Helper method used by VideoEncoderOpenh264::Encode.
// Copies the encoded bytes from `info` to `encoded_image`. The
// `encoded_image->_buffer` may be deleted and reallocated if a bigger buffer is
// required.
//
// After OpenH264 encoding, the encoded bytes are stored in `info` spread out
// over a number of layers and "NAL units". Each NAL unit is a fragment starting
// with the four-byte start code {0,0,0,1}. All of this data (including the
// start codes) is copied to the `encoded_image->_buffer`.
static void RtpFragmentize(EncodedImage *encoded_image, SFrameBSInfo *info)
{
    // Calculate minimum buffer size required to hold encoded data.
    size_t required_capacity = 0;
    size_t fragments_count = 0;
    for (int layer = 0; layer < info->iLayerNum; ++layer)
    {
        const SLayerBSInfo &layerInfo = info->sLayerInfo[layer];
        for (int nal = 0; nal < layerInfo.iNalCount; ++nal, ++fragments_count)
        {
            OCTK_CHECK_GE(layerInfo.pNalLengthInByte[nal], 0);
            // Ensure `required_capacity` will not overflow.
            OCTK_CHECK_LE(layerInfo.pNalLengthInByte[nal], std::numeric_limits<size_t>::max() - required_capacity);
            required_capacity += layerInfo.pNalLengthInByte[nal];
        }
    }
    auto buffer = EncodedImageBuffer::Create(required_capacity);
    encoded_image->setEncodedData(buffer);

    // Iterate layers and NAL units, note each NAL unit as a fragment and copy
    // the data to `encoded_image->_buffer`.
    const uint8_t start_code[4] = {0, 0, 0, 1};
    size_t frag = 0;
    encoded_image->setSize(0);
    for (int layer = 0; layer < info->iLayerNum; ++layer)
    {
        const SLayerBSInfo &layerInfo = info->sLayerInfo[layer];
        // Iterate NAL units making up this layer, noting fragments.
        size_t layer_len = 0;
        for (int nal = 0; nal < layerInfo.iNalCount; ++nal, ++frag)
        {
            // Because the sum of all layer lengths, `required_capacity`, fits in a
            // `size_t`, we know that any indices in-between will not overflow.
            OCTK_DCHECK_GE(layerInfo.pNalLengthInByte[nal], 4);
            OCTK_DCHECK_EQ(layerInfo.pBsBuf[layer_len + 0], start_code[0]);
            OCTK_DCHECK_EQ(layerInfo.pBsBuf[layer_len + 1], start_code[1]);
            OCTK_DCHECK_EQ(layerInfo.pBsBuf[layer_len + 2], start_code[2]);
            OCTK_DCHECK_EQ(layerInfo.pBsBuf[layer_len + 3], start_code[3]);
            layer_len += layerInfo.pNalLengthInByte[nal];
        }
        // Copy the entire layer's data (including start codes).
        memcpy(buffer->data() + encoded_image->size(), layerInfo.pBsBuf, layer_len);
        encoded_image->setSize(encoded_image->size() + layer_len);
    }
}
} // namespace

class VideoEncoderOpenh264Private
{
public:
    using LayerConfig = VideoEncoderOpenh264::LayerConfig;

    VideoEncoderOpenh264Private(VideoEncoderOpenh264 *p, const MediaContext &mediaContext);
    virtual ~VideoEncoderOpenh264Private();

    SEncParamExt createEncoderParams(size_t i) const;
    // Reports statistics with histograms.
    void reportError();
    void reportInit();

    H264BitStreamParser h264_bitstream_parser_;

    std::vector<ISVCEncoder *> encoders_;
    std::vector<SSourcePicture> pictures_;
    std::vector<std::shared_ptr<I420Buffer>> downscaled_buffers_;
    std::vector<LayerConfig> configurations_;
    std::vector<EncodedImage> encoded_images_;
    std::vector<std::unique_ptr<ScalableVideoController>> svc_controllers_;
    InlinedVector<Optional<ScalabilityMode>, kMaxSimulcastStreams> scalability_modes_;

    const MediaContext mediaContext_;
    VideoCodec codec_;
    H264PacketizationMode packetization_mode_;
    size_t max_payload_size_{0};
    int32_t number_of_cores_{0};
    Optional<int> encoder_thread_limit_;
    EncodedImageCallback *encoded_image_callback_{nullptr};

    bool has_reported_init_{false};
    bool has_reported_error_{false};

    std::vector<uint8_t> tl0sync_limit_;

protected:
    OCTK_DEFINE_PPTR(VideoEncoderOpenh264)
    OCTK_DECLARE_PUBLIC(VideoEncoderOpenh264)
    OCTK_DISABLE_COPY_MOVE(VideoEncoderOpenh264Private)
};

VideoEncoderOpenh264Private::VideoEncoderOpenh264Private(VideoEncoderOpenh264 *p, const MediaContext &mediaContext)
    : mPPtr(p)
    , mediaContext_(mediaContext)
{
}

VideoEncoderOpenh264Private::~VideoEncoderOpenh264Private()
{
}

// Initialization parameters.
// There are two ways to initialize. There is SEncParamBase (cleared with
// memset(&p, 0, sizeof(SEncParamBase)) used in Initialize, and SEncParamExt
// which is a superset of SEncParamBase (cleared with GetDefaultParams) used
// in InitializeExt.
SEncParamExt VideoEncoderOpenh264Private::createEncoderParams(size_t i) const
{
    SEncParamExt encoder_params;
    encoders_[i]->GetDefaultParams(&encoder_params);
    if (codec_.mode == VideoCodecMode::kRealtimeVideo)
    {
        encoder_params.iUsageType = CAMERA_VIDEO_REAL_TIME;
    }
    else if (codec_.mode == VideoCodecMode::kScreensharing)
    {
        encoder_params.iUsageType = SCREEN_CONTENT_REAL_TIME;
    }
    else
    {
        OCTK_DCHECK_NOTREACHED();
    }
    encoder_params.iPicWidth = configurations_[i].width;
    encoder_params.iPicHeight = configurations_[i].height;
    encoder_params.iTargetBitrate = configurations_[i].target_bps;
    // Keep unspecified. WebRTC's max codec bitrate is not the same setting
    // as OpenH264's iMaxBitrate. More details in https://crbug.com/webrtc/11543
    encoder_params.iMaxBitrate = UNSPECIFIED_BIT_RATE;
    // Rate Control mode
    encoder_params.iRCMode = RC_BITRATE_MODE;
    encoder_params.fMaxFrameRate = configurations_[i].max_frame_rate;

    // The following parameters are extension parameters (they're in SEncParamExt,
    // not in SEncParamBase).
    encoder_params.bEnableFrameSkip = configurations_[i].frame_dropping_on;
    // `uiIntraPeriod`    - multiple of GOP size
    // `keyFrameInterval` - number of frames
    encoder_params.uiIntraPeriod = configurations_[i].key_frame_interval;
    // Reuse SPS id if possible. This helps to avoid reset of chromium HW decoder
    // on each key-frame.
    // Note that WebRTC resets encoder on resolution change which makes all
    // EParameterSetStrategy modes except INCREASING_ID (default) essentially
    // equivalent to CONSTANT_ID.
    encoder_params.eSpsPpsIdStrategy = SPS_LISTING;
    encoder_params.uiMaxNalSize = 0;
    // Threading model: use auto.
    //  0: auto (dynamic imp. internal encoder)
    //  1: single thread (default value)
    // >1: number of threads
    encoder_params.iMultipleThreadIdc = NumberOfThreads(encoder_thread_limit_,
                                                        encoder_params.iPicWidth,
                                                        encoder_params.iPicHeight,
                                                        number_of_cores_);
    // The base spatial layer 0 is the only one we use.
    encoder_params.sSpatialLayers[0].iVideoWidth = encoder_params.iPicWidth;
    encoder_params.sSpatialLayers[0].iVideoHeight = encoder_params.iPicHeight;
    encoder_params.sSpatialLayers[0].fFrameRate = encoder_params.fMaxFrameRate;
    encoder_params.sSpatialLayers[0].iSpatialBitrate = encoder_params.iTargetBitrate;
    encoder_params.sSpatialLayers[0].iMaxSpatialBitrate = encoder_params.iMaxBitrate;
    encoder_params.iTemporalLayerNum = configurations_[i].num_temporal_layers;
    if (encoder_params.iTemporalLayerNum > 1)
    {
        // iNumRefFrame specifies total number of reference buffers to allocate.
        // For N temporal layers we need at least (N - 1) buffers to store last
        // encoded frames of all reference temporal layers.
        // Note that there is no API in OpenH264 encoder to specify exact set of
        // references to be used to prediction of a given frame. Encoder can
        // theoretically use all available reference buffers.
        encoder_params.iNumRefFrame = encoder_params.iTemporalLayerNum - 1;
    }
    OCTK_INFO() << "OpenH264 version is " << OPENH264_MAJOR << "." << OPENH264_MINOR;
    switch (packetization_mode_)
    {
        case H264PacketizationMode::SingleNalUnit:
        {
            // Limit the size of the packets produced.
            encoder_params.sSpatialLayers[0].sSliceArgument.uiSliceNum = 1;
            encoder_params.sSpatialLayers[0].sSliceArgument.uiSliceMode = SM_SIZELIMITED_SLICE;
            encoder_params.sSpatialLayers[0].sSliceArgument.uiSliceSizeConstraint = static_cast<unsigned int>(
                max_payload_size_);
            OCTK_INFO() << "Encoder is configured with NALU constraint: " << max_payload_size_ << " bytes";
            break;
        }
        case H264PacketizationMode::NonInterleaved:
        {
            // When uiSliceMode = SM_FIXEDSLCNUM_SLICE, uiSliceNum = 0 means auto
            // design it with cpu core number.
            // TODO(sprang): Set to 0 when we understand why the rate controller borks
            //               when uiSliceNum > 1.
            encoder_params.sSpatialLayers[0].sSliceArgument.uiSliceNum = 1;
            encoder_params.sSpatialLayers[0].sSliceArgument.uiSliceMode = SM_FIXEDSLCNUM_SLICE;
            break;
        }
    }
    return encoder_params;
}

void VideoEncoderOpenh264Private::reportInit()
{
    if (has_reported_init_)
    {
        return;
    }
    OCTK_HISTOGRAM_ENUMERATION("WebRTC.Video.VideoEncoderOpenh264.Event", kH264EncoderEventInit, kH264EncoderEventMax);
    has_reported_init_ = true;
}

void VideoEncoderOpenh264Private::reportError()
{
    if (has_reported_error_)
    {
        return;
    }
    OCTK_HISTOGRAM_ENUMERATION("WebRTC.Video.VideoEncoderOpenh264.Event", kH264EncoderEventError, kH264EncoderEventMax);
    has_reported_error_ = true;
}

H264PacketizationMode VideoEncoderOpenh264::parseSdpVideoFormat(const SdpVideoFormat &format)
{
    const auto iter = format.parameters.find(media::kH264FmtpPacketizationMode);
    if (format.parameters.end() != iter)
    {
        if (iter->second == "0")
        {
            return H264PacketizationMode::SingleNalUnit;
        }
        else if (iter->second == "1")
        {
            return H264PacketizationMode::NonInterleaved;
        }
    }
    return H264PacketizationMode::NonInterleaved;
}

VideoEncoderOpenh264::VideoEncoderOpenh264(const MediaContext &mediaContext, H264PacketizationMode mode)
    : mDPtr(new VideoEncoderOpenh264Private(this, mediaContext))
{
    OCTK_D(VideoEncoderOpenh264);
    d->packetization_mode_ = mode;
    d->downscaled_buffers_.reserve(kMaxSimulcastStreams - 1);
    d->encoded_images_.reserve(kMaxSimulcastStreams);
    d->encoders_.reserve(kMaxSimulcastStreams);
    d->configurations_.reserve(kMaxSimulcastStreams);
    d->tl0sync_limit_.reserve(kMaxSimulcastStreams);
    d->svc_controllers_.reserve(kMaxSimulcastStreams);
}

VideoEncoderOpenh264::~VideoEncoderOpenh264()
{
    this->release();
}

int32_t VideoEncoderOpenh264::initEncode(const VideoCodec *inst, const VideoEncoder::Settings &settings)
{
    OCTK_D(VideoEncoderOpenh264);
    d->reportInit();
    if (!inst || inst->codecType != kVideoCodecH264)
    {
        d->reportError();
        return WEBRTC_VIDEO_CODEC_ERR_PARAMETER;
    }
    if (inst->maxFramerate == 0)
    {
        d->reportError();
        return WEBRTC_VIDEO_CODEC_ERR_PARAMETER;
    }
    if (inst->width < 1 || inst->height < 1)
    {
        d->reportError();
        return WEBRTC_VIDEO_CODEC_ERR_PARAMETER;
    }

    int32_t release_ret = this->release();
    if (release_ret != WEBRTC_VIDEO_CODEC_OK)
    {
        d->reportError();
        return release_ret;
    }

    int number_of_streams = SimulcastUtility::NumberOfSimulcastStreams(*inst);
    bool doing_simulcast = (number_of_streams > 1);

    if (doing_simulcast && !SimulcastUtility::ValidSimulcastParameters(*inst, number_of_streams))
    {
        return WEBRTC_VIDEO_CODEC_ERR_SIMULCAST_PARAMETERS_NOT_SUPPORTED;
    }
    d->downscaled_buffers_.resize(number_of_streams - 1);
    d->encoded_images_.resize(number_of_streams);
    d->encoders_.resize(number_of_streams);
    d->pictures_.resize(number_of_streams);
    d->svc_controllers_.resize(number_of_streams);
    d->scalability_modes_.resize(number_of_streams);
    d->configurations_.resize(number_of_streams);
    d->tl0sync_limit_.resize(number_of_streams);

    d->max_payload_size_ = settings.maxPayloadSize;
    d->number_of_cores_ = settings.numberOfCores;
    d->encoder_thread_limit_ = settings.encoderThreadLimit;
    d->codec_ = *inst;

    // Code expects simulcastStream resolutions to be correct, make sure they are
    // filled even when there are no simulcast layers.
    if (d->codec_.numberOfSimulcastStreams == 0)
    {
        d->codec_.simulcastStream[0].width = d->codec_.width;
        d->codec_.simulcastStream[0].height = d->codec_.height;
    }

    for (int i = 0, idx = number_of_streams - 1; i < number_of_streams; ++i, --idx)
    {
        ISVCEncoder *openh264_encoder;
        // Create encoder.
        if (WelsCreateSVCEncoder(&openh264_encoder) != 0)
        {
            // Failed to create encoder.
            OCTK_ERROR() << "Failed to create OpenH264 encoder";
            OCTK_DCHECK(!openh264_encoder);
            this->release();
            d->reportError();
            return WEBRTC_VIDEO_CODEC_ERROR;
        }
        OCTK_DCHECK(openh264_encoder);
        if (kOpenH264EncoderDetailedLogging)
        {
            int trace_level = WELS_LOG_DETAIL;
            openh264_encoder->SetOption(ENCODER_OPTION_TRACE_LEVEL, &trace_level);
        }
        // else WELS_LOG_DEFAULT is used by default.

        // Store h264 encoder.
        d->encoders_[i] = openh264_encoder;

        // Set internal settings from codec_settings
        d->configurations_[i].simulcast_idx = idx;
        d->configurations_[i].sending = false;
        d->configurations_[i].width = d->codec_.simulcastStream[idx].width;
        d->configurations_[i].height = d->codec_.simulcastStream[idx].height;
        d->configurations_[i].max_frame_rate = static_cast<float>(d->codec_.maxFramerate);
        d->configurations_[i].frame_dropping_on = d->codec_.GetFrameDropEnabled();
        d->configurations_[i].key_frame_interval = d->codec_.h264()->keyFrameInterval;
        d->configurations_[i].num_temporal_layers = std::max(d->codec_.h264()->numberOfTemporalLayers,
                                                             d->codec_.simulcastStream[idx].numberOfTemporalLayers);

        // Create downscaled image buffers.
        if (i > 0)
        {
            d->downscaled_buffers_[i - 1] = I420Buffer::create(d->configurations_[i].width,
                                                               d->configurations_[i].height,
                                                               d->configurations_[i].width,
                                                               d->configurations_[i].width / 2,
                                                               d->configurations_[i].width / 2);
        }

        // Codec_settings uses kbits/second; encoder uses bits/second.
        d->configurations_[i].max_bps = d->codec_.maxBitrate * 1000;
        d->configurations_[i].target_bps = d->codec_.startBitrate * 1000;

        // Create encoder parameters based on the layer configuration.
        SEncParamExt encoder_params = d->createEncoderParams(i);

        // Initialize.
        if (openh264_encoder->InitializeExt(&encoder_params) != 0)
        {
            OCTK_ERROR() << "Failed to initialize OpenH264 encoder";
            this->release();
            d->reportError();
            return WEBRTC_VIDEO_CODEC_ERROR;
        }
        // TODO(pbos): Base init params on these values before submitting.
        int video_format = EVideoFormatType::videoFormatI420;
        openh264_encoder->SetOption(ENCODER_OPTION_DATAFORMAT, &video_format);

        // Initialize encoded image. Default buffer size: size of unencoded data.

        const size_t new_capacity = utils::videoTypeBufferSize(VideoType::kI420,
                                                               d->codec_.simulcastStream[idx].width,
                                                               d->codec_.simulcastStream[idx].height);
        d->encoded_images_[i].setEncodedData(EncodedImageBuffer::Create(new_capacity));
        d->encoded_images_[i]._encodedWidth = d->codec_.simulcastStream[idx].width;
        d->encoded_images_[i]._encodedHeight = d->codec_.simulcastStream[idx].height;
        d->encoded_images_[i].setSize(0);

        d->tl0sync_limit_[i] = d->configurations_[i].num_temporal_layers;
        d->scalability_modes_[i] = ScalabilityModeFromTemporalLayers(d->configurations_[i].num_temporal_layers);
        if (d->scalability_modes_[i].has_value())
        {
            d->svc_controllers_[i] = CreateScalabilityStructure(*d->scalability_modes_[i]);
            if (d->svc_controllers_[i] == nullptr)
            {
                OCTK_ERROR() << "Failed to create scalability structure";
                this->release();
                d->reportError();
                return WEBRTC_VIDEO_CODEC_ERROR;
            }
        }
    }

    const FieldTrialsView *tst = d->mediaContext_.field_trials_ptr();
    auto s = tst->Lookup("s");
    SimulcastRateAllocator init_allocator(d->mediaContext_, d->codec_);
    VideoBitrateAllocation allocation = init_allocator.Allocate(
        VideoBitrateAllocationParameters(DataRate::KilobitsPerSec(d->codec_.startBitrate), d->codec_.maxFramerate));
    this->setRates(RateControlParameters(allocation, d->codec_.maxFramerate));
    return WEBRTC_VIDEO_CODEC_OK;
}

int32_t VideoEncoderOpenh264::release()
{
    OCTK_D(VideoEncoderOpenh264);
    while (!d->encoders_.empty())
    {
        ISVCEncoder *openh264_encoder = d->encoders_.back();
        if (openh264_encoder)
        {
            OCTK_CHECK_EQ(0, openh264_encoder->Uninitialize());
            WelsDestroySVCEncoder(openh264_encoder);
        }
        d->encoders_.pop_back();
    }
    d->downscaled_buffers_.clear();
    d->configurations_.clear();
    d->encoded_images_.clear();
    d->pictures_.clear();
    d->tl0sync_limit_.clear();
    d->svc_controllers_.clear();
    d->scalability_modes_.clear();
    return WEBRTC_VIDEO_CODEC_OK;
}

int32_t VideoEncoderOpenh264::registerEncodeCompleteCallback(EncodedImageCallback *callback)
{
    OCTK_D(VideoEncoderOpenh264);
    d->encoded_image_callback_ = callback;
    return WEBRTC_VIDEO_CODEC_OK;
}

void VideoEncoderOpenh264::setRates(const RateControlParameters &parameters)
{
    OCTK_D(VideoEncoderOpenh264);
    if (d->encoders_.empty())
    {
        OCTK_WARNING() << "SetRates() while uninitialized.";
        return;
    }

    if (parameters.framerateFps < 1.0)
    {
        OCTK_WARNING() << "Invalid frame rate: " << parameters.framerateFps;
        return;
    }

    if (parameters.bitrate.get_sum_bps() == 0)
    {
        // Encoder paused, turn off all encoding.
        for (size_t i = 0; i < d->configurations_.size(); ++i)
        {
            d->configurations_[i].SetStreamState(false);
        }
        return;
    }

    d->codec_.maxFramerate = static_cast<uint32_t>(parameters.framerateFps);

    size_t stream_idx = d->encoders_.size() - 1;
    for (size_t i = 0; i < d->encoders_.size(); ++i, --stream_idx)
    {
        // Update layer config.
        d->configurations_[i].target_bps = parameters.bitrate.GetSpatialLayerSum(stream_idx);
        d->configurations_[i].max_frame_rate = parameters.framerateFps;

        if (d->configurations_[i].target_bps)
        {
            d->configurations_[i].SetStreamState(true);

            // Update h264 encoder.
            SBitrateInfo target_bitrate;
            memset(&target_bitrate, 0, sizeof(SBitrateInfo));
            target_bitrate.iLayer = SPATIAL_LAYER_ALL, target_bitrate.iBitrate = d->configurations_[i].target_bps;
            d->encoders_[i]->SetOption(ENCODER_OPTION_BITRATE, &target_bitrate);
            d->encoders_[i]->SetOption(ENCODER_OPTION_FRAME_RATE, &d->configurations_[i].max_frame_rate);
        }
        else
        {
            d->configurations_[i].SetStreamState(false);
        }
    }
}

int32_t VideoEncoderOpenh264::encode(const VideoFrame &input_frame, const std::vector<VideoFrameType> *frame_types)
{
    OCTK_D(VideoEncoderOpenh264);
    if (d->encoders_.empty())
    {
        d->reportError();
        return WEBRTC_VIDEO_CODEC_UNINITIALIZED;
    }
    if (!d->encoded_image_callback_)
    {
        OCTK_WARNING() << "InitEncode() has been called, but a callback function "
                          "has not been set with RegisterEncodeCompleteCallback()";
        d->reportError();
        return WEBRTC_VIDEO_CODEC_UNINITIALIZED;
    }

    auto frame_buffer = input_frame.videoFrameBuffer()->toI420();
    if (!frame_buffer)
    {
        OCTK_ERROR() << "Failed to convert " << videoFrameBufferTypeToString(input_frame.videoFrameBuffer()->type())
                     << " image to I420. Can't encode frame.";
        return WEBRTC_VIDEO_CODEC_ENCODER_FAILURE;
    }
    OCTK_CHECK(frame_buffer->type() == VideoFrameBuffer::Type::kI420 ||
               frame_buffer->type() == VideoFrameBuffer::Type::kI420A);

    bool is_keyframe_needed = false;
    for (size_t i = 0; i < d->configurations_.size(); ++i)
    {
        if (d->configurations_[i].key_frame_request && d->configurations_[i].sending)
        {
            // This is legacy behavior, generating a keyframe on all layers
            // when generating one for a layer that became active for the first time
            // or after being disabled.
            is_keyframe_needed = true;
            break;
        }
    }

    OCTK_DCHECK_EQ(d->configurations_[0].width, frame_buffer->width());
    OCTK_DCHECK_EQ(d->configurations_[0].height, frame_buffer->height());

    // Encode image for each layer.
    for (size_t i = 0; i < d->encoders_.size(); ++i)
    {
        // EncodeFrame input.
        d->pictures_[i] = {0};
        d->pictures_[i].iPicWidth = d->configurations_[i].width;
        d->pictures_[i].iPicHeight = d->configurations_[i].height;
        d->pictures_[i].iColorFormat = EVideoFormatType::videoFormatI420;
        d->pictures_[i].uiTimeStamp = input_frame.ntpTimeMSecs();
        // Downscale images on second and ongoing layers.
        if (i == 0)
        {
            d->pictures_[i].iStride[0] = frame_buffer->strideY();
            d->pictures_[i].iStride[1] = frame_buffer->strideU();
            d->pictures_[i].iStride[2] = frame_buffer->strideV();
            d->pictures_[i].pData[0] = const_cast<uint8_t *>(frame_buffer->dataY());
            d->pictures_[i].pData[1] = const_cast<uint8_t *>(frame_buffer->dataU());
            d->pictures_[i].pData[2] = const_cast<uint8_t *>(frame_buffer->dataV());
        }
        else
        {
            d->pictures_[i].iStride[0] = d->downscaled_buffers_[i - 1]->strideY();
            d->pictures_[i].iStride[1] = d->downscaled_buffers_[i - 1]->strideU();
            d->pictures_[i].iStride[2] = d->downscaled_buffers_[i - 1]->strideV();
            d->pictures_[i].pData[0] = const_cast<uint8_t *>(d->downscaled_buffers_[i - 1]->dataY());
            d->pictures_[i].pData[1] = const_cast<uint8_t *>(d->downscaled_buffers_[i - 1]->dataU());
            d->pictures_[i].pData[2] = const_cast<uint8_t *>(d->downscaled_buffers_[i - 1]->dataV());
            // Scale the image down a number of times by downsampling factor.
            utils::yuv::scaleI420(d->pictures_[i - 1].pData[0],
                                  d->pictures_[i - 1].iStride[0],
                                  d->pictures_[i - 1].pData[1],
                                  d->pictures_[i - 1].iStride[1],
                                  d->pictures_[i - 1].pData[2],
                                  d->pictures_[i - 1].iStride[2],
                                  d->configurations_[i - 1].width,
                                  d->configurations_[i - 1].height,
                                  d->pictures_[i].pData[0],
                                  d->pictures_[i].iStride[0],
                                  d->pictures_[i].pData[1],
                                  d->pictures_[i].iStride[1],
                                  d->pictures_[i].pData[2],
                                  d->pictures_[i].iStride[2],
                                  d->configurations_[i].width,
                                  d->configurations_[i].height,
                                  utils::yuv::FilterMode::kFilterBox);
        }

        if (!d->configurations_[i].sending)
        {
            continue;
        }
        if (frame_types != nullptr && i < frame_types->size())
        {
            // Skip frame?
            if ((*frame_types)[i] == VideoFrameType::kEmpty)
            {
                continue;
            }
        }
        // Send a key frame either when this layer is configured to require one
        // or we have explicitly been asked to.
        const size_t simulcast_idx = static_cast<size_t>(d->configurations_[i].simulcast_idx);
        bool send_key_frame = is_keyframe_needed || (frame_types && simulcast_idx < frame_types->size() &&
                                                     (*frame_types)[simulcast_idx] == VideoFrameType::kKey);
        if (send_key_frame)
        {
            // API doc says ForceIntraFrame(false) does nothing, but calling this
            // function forces a key frame regardless of the `bIDR` argument's value.
            // (If every frame is a key frame we get lag/delays.)
            d->encoders_[i]->ForceIntraFrame(true);
            d->configurations_[i].key_frame_request = false;
        }
        // EncodeFrame output.
        SFrameBSInfo info;
        memset(&info, 0, sizeof(SFrameBSInfo));

        std::vector<ScalableVideoController::LayerFrameConfig> layer_frames;
        if (d->svc_controllers_[i])
        {
            layer_frames = d->svc_controllers_[i]->NextFrameConfig(send_key_frame);
            OCTK_CHECK_EQ(layer_frames.size(), 1);
        }

        // Encode!
        int enc_ret = d->encoders_[i]->EncodeFrame(&d->pictures_[i], &info);
        if (enc_ret != 0)
        {
            OCTK_ERROR() << "OpenH264 frame encoding failed, EncodeFrame returned " << enc_ret << ".";
            d->reportError();
            return WEBRTC_VIDEO_CODEC_ERROR;
        }
        else if (videoFrameTypeInvalid == info.eFrameType)
        {
            OCTK_WARNING() << "OpenH264 frame encoding failed, Invalid EncodeFrame returned.";
            continue;
        }

        d->encoded_images_[i]._encodedWidth = d->configurations_[i].width;
        d->encoded_images_[i]._encodedHeight = d->configurations_[i].height;
        d->encoded_images_[i].setRtpTimestamp(input_frame.rtpTimestamp());
        d->encoded_images_[i].setColorSpace(input_frame.colorSpace());
        d->encoded_images_[i]._frameType = ConvertToVideoFrameType(info.eFrameType);
        d->encoded_images_[i].setSimulcastIndex(d->configurations_[i].simulcast_idx);

        // Split encoded image up into fragments. This also updates
        // `encoded_image_`.
        RtpFragmentize(&d->encoded_images_[i], &info);

        // Encoder can skip frames to save bandwidth in which case
        // `encoded_images_[i]._length` == 0.
        if (d->encoded_images_[i].size() > 0)
        {
            // Parse QP.
            d->h264_bitstream_parser_.ParseBitstream(d->encoded_images_[i]);
            d->encoded_images_[i].qp_ = d->h264_bitstream_parser_.GetLastSliceQp().value_or(-1);

            // Deliver encoded image.
            CodecSpecificInfo codec_specific;
            codec_specific.codecType = kVideoCodecH264;
            codec_specific.codecSpecific.H264.packetization_mode = d->packetization_mode_;
            codec_specific.codecSpecific.H264.temporal_idx = media::codecs::kNoTemporalIdx;
            codec_specific.codecSpecific.H264.idr_frame = info.eFrameType == videoFrameTypeIDR;
            codec_specific.codecSpecific.H264.base_layer_sync = false;
            if (d->configurations_[i].num_temporal_layers > 1)
            {
                const uint8_t tid = info.sLayerInfo[0].uiTemporalId;
                codec_specific.codecSpecific.H264.temporal_idx = tid;
                codec_specific.codecSpecific.H264.base_layer_sync = tid > 0 && tid < d->tl0sync_limit_[i];
                if (d->svc_controllers_[i])
                {
                    if (d->encoded_images_[i]._frameType == VideoFrameType::kKey)
                    {
                        // Reset the ScalableVideoController on key frame
                        // to reset the expected dependency structure.
                        layer_frames = d->svc_controllers_[i]->NextFrameConfig(/* restart= */ true);
                        OCTK_CHECK_EQ(layer_frames.size(), 1);
                        OCTK_DCHECK_EQ(layer_frames[0].TemporalId(), 0);
                        OCTK_DCHECK_EQ(layer_frames[0].IsKeyframe(), true);
                    }

                    if (layer_frames[0].TemporalId() != tid)
                    {
                        OCTK_WARNING() << "Encoder produced a frame with temporal id " << tid << ", expected "
                                       << layer_frames[0].TemporalId() << ".";
                        continue;
                    }
                    d->encoded_images_[i].setTemporalIndex(tid);
                }
                if (codec_specific.codecSpecific.H264.base_layer_sync)
                {
                    d->tl0sync_limit_[i] = tid;
                }
                if (tid == 0)
                {
                    d->tl0sync_limit_[i] = d->configurations_[i].num_temporal_layers;
                }
            }
            if (d->svc_controllers_[i])
            {
                codec_specific.generic_frame_info = d->svc_controllers_[i]->OnEncodeDone(layer_frames[0]);
                if (d->encoded_images_[i]._frameType == VideoFrameType::kKey &&
                    codec_specific.generic_frame_info.has_value())
                {
                    codec_specific.template_structure = d->svc_controllers_[i]->DependencyStructure();
                }
                codec_specific.scalability_mode = d->scalability_modes_[i];
            }
            d->encoded_image_callback_->OnEncodedImage(d->encoded_images_[i], &codec_specific);
        }
    }
    return WEBRTC_VIDEO_CODEC_OK;
}

VideoEncoder::EncoderInfo VideoEncoderOpenh264::getEncoderInfo() const
{
    EncoderInfo info;
    info.supportsNativeHandle = false;
    info.implementationName = "OpenH264";
    info.scalingSettings = VideoEncoder::ScalingSettings(kLowH264QpThreshold, kHighH264QpThreshold);
    info.isHardwareAccelerated = false;
    info.supportsSimulcast = true;
    info.preferredPixelFormats = {VideoFrameBuffer::Type::kI420};
    return info;
}

void VideoEncoderOpenh264::LayerConfig::SetStreamState(bool send_stream)
{
    if (send_stream && !sending)
    {
        // Need a key frame if we have not sent this stream before.
        key_frame_request = true;
    }
    sending = send_stream;
}

H264PacketizationMode VideoEncoderOpenh264::packetizationMode() const
{
    OCTK_D(const VideoEncoderOpenh264);
    return d->packetization_mode_;
}

OCTK_END_NAMESPACE

#endif // #if OCTK_FEATURE_MEDIA_USE_H264