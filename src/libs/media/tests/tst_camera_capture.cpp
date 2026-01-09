/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2026~Present ChengXueWen.
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

#include <octk_camera_capture.hpp>
#include <octk_date_time.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstdint>
#include <thread>

OCTK_BEGIN_NAMESPACE

namespace
{
    static const int kTimeOut = 5000;
#ifdef OCTK_OS_MAC
    static const int kTestHeight = 288;
    static const int kTestWidth = 352;
    static const int kTestFramerate = 30;
#endif
} // namespace

class CameraCaptureCallback : public VideoSinkInterface<VideoFrame>
{
public:
    CameraCaptureCallback()
        : last_render_time_ms_(0),
          incoming_frames_(0),
          timing_warnings_(0),
          rotate_frame_(VideoRotation::kAngle0)
    {
    }

    ~CameraCaptureCallback() override
    {
        if (timing_warnings_ > 0)
        {
            printf("No of timing warnings %d\n", timing_warnings_);
        }
    }

    void onFrame(const VideoFrame& videoFrame) override
    {
        std::lock_guard<std::mutex> lock(mCaptureMutex);
        int height = videoFrame.height();
        int width = videoFrame.width();
#if defined(OCTK_OS_ANDROID)
        // Android camera frames may be rotated depending on test device
        // orientation.
        EXPECT_TRUE(height == capability_.height || height == capability_.width);
        EXPECT_TRUE(width == capability_.width || width == capability_.height);
#else
        EXPECT_EQ(height, capability_.height);
        EXPECT_EQ(width, capability_.width);
        EXPECT_EQ(rotate_frame_, videoFrame.rotation());
#endif
        // RenderTimstamp should be the time now.
        EXPECT_TRUE(videoFrame.renderTimeMSecs() >= DateTime::TimeMillis() - 30 &&
                    videoFrame.renderTimeMSecs() <= DateTime::TimeMillis());

        if ((videoFrame.renderTimeMSecs() >
             last_render_time_ms_ + (1000 * 1.1) / capability_.maxFPS &&
             last_render_time_ms_ > 0) ||
                (videoFrame.renderTimeMSecs() <
                 last_render_time_ms_ + (1000 * 0.9) / capability_.maxFPS &&
                 last_render_time_ms_ > 0))
        {
            timing_warnings_++;
        }

        incoming_frames_++;
        last_render_time_ms_ = videoFrame.renderTimeMSecs();
        last_frame_ = videoFrame.videoFrameBuffer();
    }

    void setExpectedCapability(CameraCapture::Capability capability)
    {
        std::lock_guard<std::mutex> lock(mCaptureMutex);
        capability_ = capability;
        incoming_frames_ = 0;
        last_render_time_ms_ = 0;
    }
    int incomingFrames()
    {
        std::lock_guard<std::mutex> lock(mCaptureMutex);
        return incoming_frames_;
    }

    int timingWarnings()
    {
        std::lock_guard<std::mutex> lock(mCaptureMutex);
        return timing_warnings_;
    }
    CameraCapture::Capability capability()
    {
        std::lock_guard<std::mutex> lock(mCaptureMutex);
        return capability_;
    }

    //    bool compareLastFrame(const VideoFrame& frame)
    //    {
    //        std::lock_guard<std::mutex> lock(mCaptureMutex);
    //        return webrtc::test::FrameBufsEqual(last_frame_, frame.video_frame_buffer());
    //    }

    void setExpectedCaptureRotation(VideoRotation rotation)
    {
        std::lock_guard<std::mutex> lock(mCaptureMutex);
        rotate_frame_ = rotation;
    }

private:
    std::mutex mCaptureMutex;
    CameraCapture::Capability capability_;
    int64_t last_render_time_ms_;
    int incoming_frames_;
    int timing_warnings_;
    std::shared_ptr<VideoFrameBuffer> last_frame_;
    VideoRotation rotate_frame_;
};

class CameraCaptureTest : public ::testing::Test
{
public:
    CameraCaptureTest() : mNumberOfDevices(0) {}

    void SetUp() override
    {
        mDeviceInfo = CameraCapture::createDeviceInfo();
        ASSERT_TRUE(mDeviceInfo.get());
        mNumberOfDevices = mDeviceInfo->numberOfDevices();
        ASSERT_GT(mNumberOfDevices, 0u);
    }

    CameraCapture::SharedPtr openCameraCapture(unsigned int device,
                                               VideoSinkInterface<VideoFrame>* callback)
    {
        char device_name[256];
        char unique_name[256];

        EXPECT_EQ(0, mDeviceInfo->getDeviceName(device, device_name, 256,
                                                unique_name, 256));

        auto module = CameraCapture::create(unique_name);
        if (module.get() == NULL)
        {
            return nullptr;
        }

        EXPECT_FALSE(module->isCaptureStarted());

        module->registerCaptureDataCallback(callback);
        return module;
    }

    void startCapture(CameraCapture::SharedPtr &capture,
                      CameraCapture::Capability capability)
    {
        ASSERT_EQ(0, capture->startCapture(capability));
        EXPECT_TRUE(capture->isCaptureStarted());

        CameraCapture::Capability resulting_capability;
        EXPECT_EQ(0, capture->captureSettings(resulting_capability));
        EXPECT_EQ(capability.width, resulting_capability.width);
        EXPECT_EQ(capability.height, resulting_capability.height);
    }

    CameraCapture::DeviceInfo::SharedPtr mDeviceInfo;
    unsigned int mNumberOfDevices;
};

#ifdef OCTK_OS_MAC
// Currently fails on Mac 64-bit
#define MAYBE_Capabilities DISABLED_Capabilities
#else
#define MAYBE_Capabilities Capabilities
#endif
TEST_F(CameraCaptureTest, MAYBE_CreateDelete)
{
    for (int i = 0; i < 5; ++i)
    {
        int64_t start_time = DateTime::TimeMillis();
        CameraCaptureCallback capture_observer;
        auto module = this->openCameraCapture(0, &capture_observer);
        ASSERT_TRUE(module.get() != NULL);

        CameraCapture::Capability capability;
#ifndef OCTK_OS_MAC
        mDeviceInfo->getCapability(module->currentDeviceName(), 0, capability);
#else
        capability.width = kTestWidth;
        capability.height = kTestHeight;
        capability.maxFPS = kTestFramerate;
        capability.videoType = VideoType::kANY;
#endif
        capture_observer.setExpectedCapability(capability);
        ASSERT_NO_FATAL_FAILURE(this->startCapture(module, capability));

        // Less than 4s to start the camera.
        EXPECT_LE(DateTime::TimeMillis() - start_time, 4000);

        // Make sure 5 frames are captured.
        std::this_thread::sleep_for(std::chrono::milliseconds(kTimeOut));
        EXPECT_GE(capture_observer.incomingFrames(), 5);

        int64_t stop_time = DateTime::TimeMillis();
        EXPECT_EQ(0, module->stopCapture());
        EXPECT_FALSE(module->isCaptureStarted());

        // Less than 3s to stop the camera.
        EXPECT_LE(DateTime::TimeMillis() - stop_time, 3000);
    }
}

#ifdef OCTK_OS_MAC
// Currently fails on Mac 64-bit
#define MAYBE_TestTwoCameras DISABLED_TestTwoCameras
#else
#define MAYBE_TestTwoCameras TestTwoCameras
#endif
TEST_F(CameraCaptureTest, MAYBE_TestTwoCameras)
{
    if (mNumberOfDevices < 2)
    {
        printf("There are not two cameras available. Aborting test. \n");
        return;
    }

    CameraCaptureCallback capture_observer1;
    auto module1 = this->openCameraCapture(0, &capture_observer1);
    ASSERT_TRUE(module1.get() != NULL);
    CameraCapture::Capability capability1;
#ifndef OCTK_OS_MAC
    mDeviceInfo->getCapability(module1->currentDeviceName(), 0, capability1);
#else
    capability1.width = kTestWidth;
    capability1.height = kTestHeight;
    capability1.maxFPS = kTestFramerate;
    capability1.videoType = VideoType::kANY;
#endif
    capture_observer1.setExpectedCapability(capability1);

    CameraCaptureCallback capture_observer2;
    auto module2 = this->openCameraCapture(1, &capture_observer2);
    ASSERT_TRUE(module2.get() != NULL);
    CameraCapture::Capability capability2;
#ifndef OCTK_OS_MAC
    mDeviceInfo->getCapability(module2->currentDeviceName(), 0, capability2);
#else
    capability2.width = kTestWidth;
    capability2.height = kTestHeight;
    capability2.maxFPS = kTestFramerate;
    capability1.videoType = VideoType::kANY;
#endif
    capture_observer2.setExpectedCapability(capability2);

    ASSERT_NO_FATAL_FAILURE(this->startCapture(module1, capability1));
    ASSERT_NO_FATAL_FAILURE(this->startCapture(module2, capability2));
    std::this_thread::sleep_for(std::chrono::milliseconds(kTimeOut));
    EXPECT_GE(capture_observer1.incomingFrames(), 5);
    EXPECT_GE(capture_observer2.incomingFrames(), 5);
    EXPECT_EQ(0, module2->stopCapture());
    EXPECT_EQ(0, module1->stopCapture());
}

#ifdef OCTK_OS_MAC
// No VideoCaptureImpl on Mac.
#define MAYBE_ConcurrentAccess DISABLED_ConcurrentAccess
#else
#define MAYBE_ConcurrentAccess ConcurrentAccess
#endif
TEST_F(CameraCaptureTest, MAYBE_ConcurrentAccess)
{
    CameraCapture::Capability capability;

    CameraCaptureCallback capture_observer1;
    auto module1 = this->openCameraCapture(0, &capture_observer1);
    ASSERT_TRUE(module1.get() != NULL);
    mDeviceInfo->getCapability(module1->currentDeviceName(), 0, capability);
    capture_observer1.setExpectedCapability(capability);

    CameraCaptureCallback capture_observer2;
    auto module2 = this->openCameraCapture(0, &capture_observer2);
    ASSERT_TRUE(module2.get() != NULL);
    mDeviceInfo->getCapability(module2->currentDeviceName(), 0, capability);
    capture_observer2.setExpectedCapability(capability);

    // Starting module1 should work.
    ASSERT_NO_FATAL_FAILURE(this->startCapture(module1, capability));
    std::this_thread::sleep_for(std::chrono::milliseconds(kTimeOut));
    EXPECT_GE(capture_observer1.incomingFrames(), 5);
    // When module1 is stopped, starting module2 for the same device should work.
    EXPECT_EQ(0, module1->stopCapture());

    ASSERT_NO_FATAL_FAILURE(this->startCapture(module2, capability));
    std::this_thread::sleep_for(std::chrono::milliseconds(kTimeOut));
    EXPECT_GE(capture_observer1.incomingFrames(), 5);
    EXPECT_EQ(0, module2->stopCapture());
}

OCTK_END_NAMESPACE
