/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2026~Present ChengXueWen.
** Copyright (c) 2014 The WebRTC project authors. All Rights Reserved.
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

#include <test/octk_simulcast_test_fixture_p.hpp>
#include <octk_h264_codecs.hpp>

#include <memory>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

OCTK_BEGIN_NAMESPACE

namespace test
{

namespace
{
std::unique_ptr<SimulcastTestFixture> CreateSpecificSimulcastTestFixture()
{
    std::unique_ptr<VideoEncoderFactory> encoder_factory = std::make_unique<FunctionVideoEncoderFactory>(
        [](const Environment &env, const SdpVideoFormat &format) { return CreateH264Encoder(env); });
    std::unique_ptr<VideoDecoderFactory> decoder_factory = std::make_unique<FunctionVideoDecoderFactory>(
        []() { return H264Decoder::Create(); });
    return CreateSimulcastTestFixture(std::move(encoder_factory), std::move(decoder_factory), SdpVideoFormat::H264());
}
} // namespace

TEST(TestH264Simulcast, TestKeyFrameRequestsOnAllStreams)
{
    GTEST_SKIP() << "Not applicable to H264.";
}

TEST(TestH264Simulcast, TestKeyFrameRequestsOnSpecificStreams)
{
    auto fixture = CreateSpecificSimulcastTestFixture();
    fixture->TestKeyFrameRequestsOnSpecificStreams();
}

TEST(TestH264Simulcast, TestPaddingAllStreams)
{
    auto fixture = CreateSpecificSimulcastTestFixture();
    fixture->TestPaddingAllStreams();
}

TEST(TestH264Simulcast, TestPaddingTwoStreams)
{
    auto fixture = CreateSpecificSimulcastTestFixture();
    fixture->TestPaddingTwoStreams();
}

TEST(TestH264Simulcast, TestPaddingTwoStreamsOneMaxedOut)
{
    auto fixture = CreateSpecificSimulcastTestFixture();
    fixture->TestPaddingTwoStreamsOneMaxedOut();
}

TEST(TestH264Simulcast, TestPaddingOneStream)
{
    auto fixture = CreateSpecificSimulcastTestFixture();
    fixture->TestPaddingOneStream();
}

TEST(TestH264Simulcast, TestPaddingOneStreamTwoMaxedOut)
{
    auto fixture = CreateSpecificSimulcastTestFixture();
    fixture->TestPaddingOneStreamTwoMaxedOut();
}

TEST(TestH264Simulcast, TestSendAllStreams)
{
    auto fixture = CreateSpecificSimulcastTestFixture();
    fixture->TestSendAllStreams();
}

TEST(TestH264Simulcast, TestDisablingStreams)
{
    auto fixture = CreateSpecificSimulcastTestFixture();
    fixture->TestDisablingStreams();
}

TEST(TestH264Simulcast, TestActiveStreams)
{
    auto fixture = CreateSpecificSimulcastTestFixture();
    fixture->TestActiveStreams();
}

TEST(TestH264Simulcast, TestSwitchingToOneStream)
{
    auto fixture = CreateSpecificSimulcastTestFixture();
    fixture->TestSwitchingToOneStream();
}

TEST(TestH264Simulcast, TestSwitchingToOneOddStream)
{
    auto fixture = CreateSpecificSimulcastTestFixture();
    fixture->TestSwitchingToOneOddStream();
}

TEST(TestH264Simulcast, TestStrideEncodeDecode)
{
    auto fixture = CreateSpecificSimulcastTestFixture();
    fixture->TestStrideEncodeDecode();
}

TEST(TestH264Simulcast, TestSpatioTemporalLayers333PatternEncoder)
{
    auto fixture = CreateSpecificSimulcastTestFixture();
    fixture->TestSpatioTemporalLayers333PatternEncoder();
}

} // namespace test

OCTK_END_NAMESPACE
