/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
** Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
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

#include <octk_media_context.hpp>
#include <private/octk_media_context_factory_p.hpp>
#include <octk_frame_generator_interface.hpp>
#include <octk_video_encoder.hpp>
#include <octk_mutex.hpp>
#include <octk_time_delta.hpp>

#include <memory>
#include <vector>

#if 0
#    include "api/video_codecs/video_decoder.h"
#    include "api/video_codecs/video_encoder.h"
#    include "modules/video_coding/include/video_codec_interface.h"
#    include "modules/video_coding/utility/vp8_header_parser.h"
#    include "modules/video_coding/utility/vp9_uncompressed_header_parser.h"
#    include "rtc_base/event.h"
#    include "rtc_base/synchronization/mutex.h"
#    include "rtc_base/thread_annotations.h"
#    include "test/gtest.h"
#endif

OCTK_BEGIN_NAMESPACE

namespace
{
static constexpr TimeDelta kEncodeTimeout = TimeDelta::Millis(100);
static constexpr TimeDelta kDecodeTimeout = TimeDelta::Millis(25);
// Set bitrate to get higher quality.
static const int kStartBitrate = 300;
static const int kMaxBitrate = 4000;
static const int kWidth = 176;       // Width of the input image.
static const int kHeight = 144;      // Height of the input image.
static const int kMaxFramerate = 30; // Arbitrary value.

const VideoEncoder::Capabilities kCapabilities(false);
}

class VideoCodecUnitTest : public ::testing::Test
{
public:
    VideoCodecUnitTest()
        : env_(CreateEnvironment())
        , encode_complete_callback_(this)
        , decode_complete_callback_(this)
        , wait_for_encoded_frames_threshold_(1)
        , last_input_frame_timestamp_(0)
    {
    }

protected:
    class FakeEncodeCompleteCallback : public webrtc::EncodedImageCallback
    {
    public:
        explicit FakeEncodeCompleteCallback(VideoCodecUnitTest *test)
            : test_(test)
        {
        }

        Result OnEncodedImage(const EncodedImage &frame, const CodecSpecificInfo *codec_specific_info)
        {
            MutexLock lock(&test_->encoded_frame_section_);
            test_->encoded_frames_.push_back(frame);
            RTC_DCHECK(codec_specific_info);
            test_->codec_specific_infos_.push_back(*codec_specific_info);
            if (!test_->wait_for_encoded_frames_threshold_)
            {
                test_->encoded_frame_event_.Set();
                return Result(Result::OK);
            }

            if (test_->encoded_frames_.size() == test_->wait_for_encoded_frames_threshold_)
            {
                test_->wait_for_encoded_frames_threshold_ = 1;
                test_->encoded_frame_event_.Set();
            }
            return Result(Result::OK);
        }

    private:
        VideoCodecUnitTest *const test_;
    };

    class FakeDecodeCompleteCallback : public webrtc::DecodedImageCallback
    {
    public:
        explicit FakeDecodeCompleteCallback(VideoCodecUnitTest *test)
            : test_(test)
        {
        }

        int32_t Decoded(VideoFrame & /* frame */) override
        {
            RTC_DCHECK_NOTREACHED();
            return -1;
        }
        int32_t Decoded(VideoFrame & /* frame */, int64_t /* decode_time_ms */) override
        {
            RTC_DCHECK_NOTREACHED();
            return -1;
        }
        void Decoded(VideoFrame &frame, std::optional<int32_t> decode_time_ms, std::optional<uint8_t> qp) override
        {
            MutexLock lock(&test_->decoded_frame_section_);
            test_->decoded_frame_.emplace(frame);
            test_->decoded_qp_ = qp;
            test_->decoded_frame_event_.Set();
        }

    private:
        VideoCodecUnitTest *const test_;
    };

    virtual std::unique_ptr<VideoEncoder> CreateEncoder() = 0;
    virtual std::unique_ptr<VideoDecoder> CreateDecoder() = 0;

    void SetUp() override
    {
        webrtc::test::CodecSettings(kVideoCodecVP8, &codec_settings_);
        codec_settings_.startBitrate = kStartBitrate;
        codec_settings_.maxBitrate = kMaxBitrate;
        codec_settings_.maxFramerate = kMaxFramerate;
        codec_settings_.width = kWidth;
        codec_settings_.height = kHeight;

        ModifyCodecSettings(&codec_settings_);

        input_frame_generator_ = test::CreateSquareFrameGenerator(codec_settings_.width,
                                                                  codec_settings_.height,
                                                                  test::FrameGeneratorInterface::OutputType::kI420,
                                                                  std::optional<int>());

        encoder_ = CreateEncoder();
        decoder_ = CreateDecoder();
        encoder_->RegisterEncodeCompleteCallback(&encode_complete_callback_);
        decoder_->RegisterDecodeCompleteCallback(&decode_complete_callback_);

        EXPECT_EQ(
            WEBRTC_VIDEO_CODEC_OK,
            encoder_->InitEncode(
                &codec_settings_,
                VideoEncoder::Settings(kCapabilities, 1 /* number of cores */, 0 /* max payload size (unused) */)));

        VideoDecoder::Settings decoder_settings;
        decoder_settings.set_codec_type(codec_settings_.codecType);
        decoder_settings.set_max_render_resolution({codec_settings_.width, codec_settings_.height});
        EXPECT_TRUE(decoder_->Configure(decoder_settings));
    }

    virtual void ModifyCodecSettings(VideoCodec *codec_settings);

    VideoFrame NextInputFrame()
    {
        test::FrameGeneratorInterface::VideoFrameData frame_data = input_frame_generator_->NextFrame();
        VideoFrame input_frame = VideoFrame::Builder()
                                     .set_video_frame_buffer(frame_data.buffer)
                                     .set_update_rect(frame_data.update_rect)
                                     .build();

        const uint32_t timestamp = last_input_frame_timestamp_ +
                                   kVideoPayloadTypeFrequency / codec_settings_.maxFramerate;
        input_frame.set_rtp_timestamp(timestamp);
        input_frame.set_timestamp_us(timestamp * (1000 / 90));

        last_input_frame_timestamp_ = timestamp;
        return input_frame;
    }

    // Helper method for waiting a single encoded frame.
    bool WaitForEncodedFrame(EncodedImage *frame, CodecSpecificInfo *codec_specific_info)
    {
        std::vector<EncodedImage> frames;
        std::vector<CodecSpecificInfo> codec_specific_infos;
        if (!WaitForEncodedFrames(&frames, &codec_specific_infos))
            return false;
        EXPECT_EQ(frames.size(), static_cast<size_t>(1));
        EXPECT_EQ(frames.size(), codec_specific_infos.size());
        *frame = frames[0];
        *codec_specific_info = codec_specific_infos[0];
        return true;
    }

    // Helper methods for waiting for multiple encoded frames. Caller must
    // define how many frames are to be waited for via `num_frames` before calling
    // Encode(). Then, they can expect to retrive them via WaitForEncodedFrames().
    void SetWaitForEncodedFramesThreshold(size_t num_frames)
    {
        MutexLock lock(&encoded_frame_section_);
        wait_for_encoded_frames_threshold_ = num_frames;
    }
    bool WaitForEncodedFrames(std::vector<EncodedImage> *frames, std::vector<CodecSpecificInfo> *codec_specific_info)
    {
        EXPECT_TRUE(encoded_frame_event_.Wait(kEncodeTimeout)) << "Timed out while waiting for encoded frame.";
        // This becomes unsafe if there are multiple threads waiting for frames.
        MutexLock lock(&encoded_frame_section_);
        EXPECT_FALSE(encoded_frames_.empty());
        EXPECT_FALSE(codec_specific_infos_.empty());
        EXPECT_EQ(encoded_frames_.size(), codec_specific_infos_.size());
        if (!encoded_frames_.empty())
        {
            *frames = encoded_frames_;
            encoded_frames_.clear();
            RTC_DCHECK(!codec_specific_infos_.empty());
            *codec_specific_info = codec_specific_infos_;
            codec_specific_infos_.clear();
            return true;
        }
        else
        {
            return false;
        }
    }

    // Helper method for waiting a single decoded frame.
    bool WaitForDecodedFrame(std::unique_ptr<VideoFrame> *frame, std::optional<uint8_t> *qp)
    {
        bool ret = decoded_frame_event_.Wait(kDecodeTimeout);
        EXPECT_TRUE(ret) << "Timed out while waiting for a decoded frame.";
        // This becomes unsafe if there are multiple threads waiting for frames.
        MutexLock lock(&decoded_frame_section_);
        EXPECT_TRUE(decoded_frame_);
        if (decoded_frame_)
        {
            frame->reset(new VideoFrame(std::move(*decoded_frame_)));
            *qp = decoded_qp_;
            decoded_frame_.reset();
            return true;
        }
        else
        {
            return false;
        }
    }

    size_t GetNumEncodedFrames()
    {
        MutexLock lock(&encoded_frame_section_);
        return encoded_frames_.size();
    }

    const Environment env_;
    VideoCodec codec_settings_;

    std::unique_ptr<VideoEncoder> encoder_;
    std::unique_ptr<VideoDecoder> decoder_;
    std::unique_ptr<test::FrameGeneratorInterface> input_frame_generator_;

private:
    FakeEncodeCompleteCallback encode_complete_callback_;
    FakeDecodeCompleteCallback decode_complete_callback_;

    rtc::Event encoded_frame_event_;
    Mutex encoded_frame_section_;
    size_t wait_for_encoded_frames_threshold_;
    std::vector<EncodedImage> encoded_frames_ RTC_GUARDED_BY(encoded_frame_section_);
    std::vector<CodecSpecificInfo> codec_specific_infos_ RTC_GUARDED_BY(encoded_frame_section_);

    rtc::Event decoded_frame_event_;
    Mutex decoded_frame_section_;
    std::optional<VideoFrame> decoded_frame_ RTC_GUARDED_BY(decoded_frame_section_);
    std::optional<uint8_t> decoded_qp_ RTC_GUARDED_BY(decoded_frame_section_);

    uint32_t last_input_frame_timestamp_;
};

} // namespace webrtc
