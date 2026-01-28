/*
 *  Copyright (c) 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <octk_create_frame_generator_capturer.hpp>
//#include <test/octk_simulated_time_controller_p.hpp>
#include <octk_frame_generator_capturer.hpp>
#include <octk_create_frame_generator.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#if 0
OCTK_BEGIN_NAMESPACE

namespace
{
using ::testing::Eq;
using ::testing::Property;

constexpr int kWidth = 640;
constexpr int kHeight = 360;

class MockVideoSinkInterfaceVideoFrame : public VideoSinkInterface<VideoFrame>
{
public:
    MOCK_METHOD(void, onFrame, (const VideoFrame &frame), (override));
    MOCK_METHOD(void, onDiscardedFrame, (), (override));
};
} // namespace

TEST(FrameGeneratorCapturerTest, CreateFromConfig)
{
    GlobalSimulatedTimeController time(Timestamp::Seconds(1000));
    FrameGeneratorCapturerConfig config;
    config.squares_video->width = 300;
    config.squares_video->height = 200;
    config.squares_video->framerate = 20;
    auto capturer = utils::CreateFrameGeneratorCapturer(time.GetClock(), *time.GetTaskQueueFactory(), config);
    testing::StrictMock<MockVideoSinkInterfaceVideoFrame> mock_sink;
    capturer->addOrUpdateSink(&mock_sink, VideoSinkWants());
    capturer->start();
    EXPECT_CALL(mock_sink, onFrame(Property(&VideoFrame::width, Eq(300)))).Times(21);
    time.AdvanceTime(TimeDelta::Seconds(1));
}

TEST(FrameGeneratorCapturerTest, OnOutputFormatRequest)
{
    GlobalSimulatedTimeController time(Timestamp::Seconds(1000));
    FrameGeneratorCapturerConfig config;
    config.squares_video->width = kWidth;
    config.squares_video->height = kHeight;
    config.squares_video->framerate = 20;
    auto capturer = utils::CreateFrameGeneratorCapturer(time.GetClock(), *time.GetTaskQueueFactory(), config);
    testing::StrictMock<MockVideoSinkInterfaceVideoFrame> mock_sink;
    capturer->addOrUpdateSink(&mock_sink, VideoSinkWants());
    capturer->onOutputFormatRequest(kWidth / 2, kHeight / 2, /*max_fps=*/10);
    capturer->start();
    EXPECT_CALL(mock_sink, onFrame(Property(&VideoFrame::width, Eq(kWidth / 2)))).Times(11);
    time.AdvanceTime(TimeDelta::Seconds(1));
}

TEST(FrameGeneratorCapturerTest, ChangeResolution)
{
    GlobalSimulatedTimeController time(Timestamp::Seconds(1000));
    FrameGeneratorCapturerConfig config;
    config.squares_video->width = kWidth;
    config.squares_video->height = kHeight;
    config.squares_video->framerate = 20;
    auto capturer = utils::CreateFrameGeneratorCapturer(time.GetClock(), *time.GetTaskQueueFactory(), config);
    EXPECT_TRUE(capturer->getResolution());
    EXPECT_EQ(kWidth, capturer->getResolution()->width);
    EXPECT_EQ(kHeight, capturer->getResolution()->height);
    capturer->start();
    time.AdvanceTime(TimeDelta::Seconds(1));
    ASSERT_TRUE(capturer->getResolution());
    EXPECT_EQ(kWidth, capturer->getResolution()->width);
    EXPECT_EQ(kHeight, capturer->getResolution()->height);

    capturer->changeResolution(kWidth / 2, kHeight / 2);
    time.AdvanceTime(TimeDelta::Seconds(1));
    ASSERT_TRUE(capturer->getResolution());
    EXPECT_EQ(kWidth / 2, capturer->getResolution()->width);
    EXPECT_EQ(kHeight / 2, capturer->getResolution()->height);
}

OCTK_END_NAMESPACE
#endif
