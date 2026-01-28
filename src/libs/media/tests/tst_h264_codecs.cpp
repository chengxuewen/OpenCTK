/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2026~Present ChengXueWen.
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

#include <test/octk_video_codec_test_p.hpp>
#include <octk_color_space.hpp>
#include <octk_encoded_image.hpp>
#include <octk_video_frame.hpp>
#include <octk_video_codec.hpp>
#include <octk_video_encoder.hpp>
#include <octk_video_decoder.hpp>
#include <octk_h264_codecs.hpp>
#include <octk_yuv.hpp>

#include <stdint.h>
#include <memory>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

OCTK_BEGIN_NAMESPACE

class TestH264Impl : public VideoCodecUnitTest
{
protected:
    std::unique_ptr<VideoEncoder> CreateEncoder() override { return CreateH264Encoder(env_); }

    std::unique_ptr<VideoDecoder> CreateDecoder() override { return H264Decoder::Create(); }

    void ModifyCodecSettings(VideoCodec *codec_settings) override
    {
        test::CodecSettings(kVideoCodecH264, codec_settings);
    }
};

#if OCTK_FEATURE_MEDIA_USE_H264
#    define MAYBE_EncodeDecode             EncodeDecode
#    define MAYBE_DecodedQpEqualsEncodedQp DecodedQpEqualsEncodedQp
#else
#    define MAYBE_EncodeDecode             DISABLED_EncodeDecode
#    define MAYBE_DecodedQpEqualsEncodedQp DISABLED_DecodedQpEqualsEncodedQp
#endif

TEST_F(TestH264Impl, MAYBE_EncodeDecode)
{
    VideoFrame input_frame = NextInputFrame();
    EXPECT_EQ(WEBRTC_VIDEO_CODEC_OK, encoder_->encode(input_frame, nullptr));
    EncodedImage encoded_frame;
    CodecSpecificInfo codec_specific_info;
    ASSERT_TRUE(WaitForEncodedFrame(&encoded_frame, &codec_specific_info));
    // First frame should be a key frame.
    encoded_frame._frameType = VideoFrameType::kKey;
    EXPECT_EQ(WEBRTC_VIDEO_CODEC_OK, decoder_->Decode(encoded_frame, 0));
    std::unique_ptr<VideoFrame> decoded_frame;
    Optional<uint8_t> decoded_qp;
    ASSERT_TRUE(WaitForDecodedFrame(&decoded_frame, &decoded_qp));
    ASSERT_TRUE(decoded_frame);
    EXPECT_GT(utils::I420PSNR(&input_frame, decoded_frame.get()), 36);

    const ColorSpace color_space = *decoded_frame->colorSpace();
    EXPECT_EQ(ColorSpace::PrimaryID::kUnspecified, color_space.primaries());
    EXPECT_EQ(ColorSpace::TransferID::kUnspecified, color_space.transfer());
    EXPECT_EQ(ColorSpace::MatrixID::kUnspecified, color_space.matrix());
    EXPECT_EQ(ColorSpace::RangeID::kInvalid, color_space.range());
    EXPECT_EQ(ColorSpace::ChromaSiting::kUnspecified, color_space.chroma_siting_horizontal());
    EXPECT_EQ(ColorSpace::ChromaSiting::kUnspecified, color_space.chroma_siting_vertical());
}

TEST_F(TestH264Impl, MAYBE_DecodedQpEqualsEncodedQp)
{
    EXPECT_EQ(WEBRTC_VIDEO_CODEC_OK, encoder_->encode(NextInputFrame(), nullptr));
    EncodedImage encoded_frame;
    CodecSpecificInfo codec_specific_info;
    ASSERT_TRUE(WaitForEncodedFrame(&encoded_frame, &codec_specific_info));
    // First frame should be a key frame.
    encoded_frame._frameType = VideoFrameType::kKey;
    EXPECT_EQ(WEBRTC_VIDEO_CODEC_OK, decoder_->Decode(encoded_frame, 0));
    std::unique_ptr<VideoFrame> decoded_frame;
    Optional<uint8_t> decoded_qp;
    ASSERT_TRUE(WaitForDecodedFrame(&decoded_frame, &decoded_qp));
    ASSERT_TRUE(decoded_frame);
    ASSERT_TRUE(decoded_qp);
    EXPECT_EQ(encoded_frame.qp_, *decoded_qp);
}

OCTK_END_NAMESPACE
