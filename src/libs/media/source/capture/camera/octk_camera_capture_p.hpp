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

#pragma once

#include <octk_context_checker.hpp>
#include <octk_camera_capture.hpp>
#include <octk_race_checker.hpp>
#include <octk_date_time.hpp>

OCTK_BEGIN_NAMESPACE

class CameraCapture::DeviceInfoPrivate
{
public:
    using Capability = CameraCapture::Capability;
    using Capabilities = CameraCapture::Capabilities;

    explicit DeviceInfoPrivate(DeviceInfo *p);
    virtual ~DeviceInfoPrivate();

    std::mutex mMutex;
    Capabilities mCapabilities OCTK_ATTRIBUTE_GUARDED_BY(mMutex);
    char* mLastUsedDeviceName OCTK_ATTRIBUTE_GUARDED_BY(mMutex) = nullptr;
    uint32_t mLastUsedDeviceNameLength OCTK_ATTRIBUTE_GUARDED_BY(mMutex) = 0;

protected:
    OCTK_DEFINE_PPTR(DeviceInfo)
    OCTK_DECLARE_PUBLIC(DeviceInfo)
    OCTK_DISABLE_COPY_MOVE(DeviceInfoPrivate)
};

class CameraCapturePrivate
{
public:
    using Capability = CameraCapture::Capability;

    explicit CameraCapturePrivate(CameraCapture *p);
    virtual ~CameraCapturePrivate();

    void updateFrameCount();
    int32_t incomingFrame(uint8_t *videoFrame,
                          size_t videoFrameLength,
                          const Capability& frameInfo,
                          int64_t captureTime = 0);
    int32_t deliverCapturedFrame(VideoFrame &captureFrame);

protected:
    // Calls to the public API must happen on a single thread.
    ContextChecker mApiChecker;
    // RaceChecker for members that can be accessed on the API thread while
    // capture is not happening, and on a callback thread otherwise.
    RaceChecker mCaptureChecker;
    // current Device unique name;
    char* mDeviceUniqueId OCTK_ATTRIBUTE_GUARDED_BY(mApiChecker) = nullptr;
    std::mutex mApiMutex;
    // Should be set by platform dependent code in StartCapture.
    Capability mRequestedCapability OCTK_ATTRIBUTE_GUARDED_BY(mApiChecker);

private:
    // last time the module process function was called.
    int64_t mLastProcessTimeNanos OCTK_ATTRIBUTE_GUARDED_BY(mCaptureChecker) = DateTime::TimeNanos();
    // last time the frame rate callback function was called.
    int64_t mLastFrameRateCallbackTimeNanos OCTK_ATTRIBUTE_GUARDED_BY(mCaptureChecker) = DateTime::TimeNanos();

    VideoSinkInterface<VideoFrame>* mDataCallBack OCTK_ATTRIBUTE_GUARDED_BY(mApiMutex) = nullptr;
    //    RawVideoSinkInterface* _rawDataCallBack OCTK_ATTRIBUTE_GUARDED_BY(mApiMutex);

    int64_t mLastProcessFrameTimeNanos OCTK_ATTRIBUTE_GUARDED_BY(mCaptureChecker) = DateTime::TimeNanos();
    // timestamp for local captured frames
    int64_t mIncomingFrameTimesNanos[CameraCapture::kFrameRateCountHistorySize] OCTK_ATTRIBUTE_GUARDED_BY(mCaptureChecker);
    // Set if the frame should be rotated by the capture module.
    VideoRotation mVideoRotation OCTK_ATTRIBUTE_GUARDED_BY(mApiMutex) = VideoRotation::kAngle0;
    // Indicate whether rotation should be applied before delivered externally.
    bool mApplyRotation OCTK_ATTRIBUTE_GUARDED_BY(mApiMutex) = false;

protected:
    OCTK_DEFINE_PPTR(CameraCapture)
    OCTK_DECLARE_PUBLIC(CameraCapture)
    OCTK_DISABLE_COPY_MOVE(CameraCapturePrivate)
};

OCTK_END_NAMESPACE
