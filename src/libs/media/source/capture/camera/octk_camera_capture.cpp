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

#include <private/octk_camera_capture_p.hpp>
#include <octk_camera_capture.hpp>
#include <octk_video_rotation.hpp>
#include <octk_string_utils.hpp>
#include <octk_i420_buffer.hpp>
#include <octk_checks.hpp>
#include <octk_yuv.hpp>

#if defined(OCTK_OS_LINUX)
#    include <private/octk_camera_capture_v4l2_p.hpp>
#    include <private/octk_camera_device_info_v4l2_p.hpp>
#endif

OCTK_BEGIN_NAMESPACE

CameraCapture::DeviceInfoPrivate::DeviceInfoPrivate(DeviceInfo *p)
    : mPPtr(p)
{
}

CameraCapture::DeviceInfoPrivate::~DeviceInfoPrivate()
{
    std::lock_guard<std::mutex> lock(mMutex);
    std::free(mLastUsedDeviceName);
}

CameraCapture::DeviceInfo::DeviceInfo(DeviceInfoPrivate *d)
    : mDPtr(d)
{
}

CameraCapture::DeviceInfo::~DeviceInfo()
{
}

int32_t CameraCapture::DeviceInfo::numberOfCapabilities(const char *deviceUniqueIdUTF8)
{
    OCTK_D(DeviceInfo);
    if (!deviceUniqueIdUTF8)
    {
        return -1;
    }

    std::lock_guard<std::mutex> lock(d->mMutex);
    // Is it the same device that is asked for again.
    if (utils::stringEqualsIgnoreCase(deviceUniqueIdUTF8,
                                      StringView(d->mLastUsedDeviceName, d->mLastUsedDeviceNameLength)))
    {
        return static_cast<int32_t>(d->mCapabilities.size());
    }
    return this->createCapabilityMap(deviceUniqueIdUTF8);
}

int32_t CameraCapture::DeviceInfo::getCapability(const char *deviceUniqueIdUTF8, uint32_t deviceCapabilityNumber,
                                                 Capability &capability)
{
    OCTK_D(DeviceInfo);
    OCTK_DCHECK(deviceUniqueIdUTF8);

    std::lock_guard<std::mutex> lock(d->mMutex);

    if (!utils::stringEqualsIgnoreCase(deviceUniqueIdUTF8,
                                       StringView(d->mLastUsedDeviceName, d->mLastUsedDeviceNameLength)))
    {
        if (-1 == this->createCapabilityMap(deviceUniqueIdUTF8))
        {
            return -1;
        }
    }

    // Make sure the number is valid
    if (deviceCapabilityNumber >= (unsigned int)d->mCapabilities.size())
    {
        OCTK_ERROR() << "Invalid deviceCapabilityNumber "
                     << deviceCapabilityNumber << ">= number of capabilities ("
                     << d->mCapabilities.size() << ").";
        return -1;
    }

    capability = d->mCapabilities[deviceCapabilityNumber];
    return 0;
}

int32_t CameraCapture::DeviceInfo::getOrientation(const char *deviceUniqueIdUTF8, VideoRotation &orientation)
{
    orientation = VideoRotation::kAngle0;
    return -1;
}

int32_t CameraCapture::DeviceInfo::getBestMatchedCapability(const char *deviceUniqueIdUTF8, const Capability &requested,
                                                            Capability &resulting)
{
    OCTK_D(DeviceInfo);
    if (!deviceUniqueIdUTF8)
    {
        return -1;
    }

    std::lock_guard<std::mutex> lock(d->mMutex);
    if (!utils::stringEqualsIgnoreCase(deviceUniqueIdUTF8,
                                       StringView(d->mLastUsedDeviceName, d->mLastUsedDeviceNameLength)))
    {
        if (-1 == this->createCapabilityMap(deviceUniqueIdUTF8))
        {
            return -1;
        }
    }

    int32_t bestWidth = 0;
    int32_t bestHeight = 0;
    int32_t bestFrameRate = 0;
    int32_t bestformatIndex = -1;
    VideoType bestVideoType = VideoType::kANY;

    const int32_t numberOfCapabilies = static_cast<int32_t>(d->mCapabilities.size());
    for (int32_t tmp = 0; tmp < numberOfCapabilies; ++tmp)  // Loop through all capabilities
    {
        auto &capability = d->mCapabilities[tmp];

        const int32_t diffWidth = capability.width - requested.width;
        const int32_t diffHeight = capability.height - requested.height;
        const int32_t diffFrameRate = capability.maxFPS - requested.maxFPS;

        const int32_t currentbestDiffWith = bestWidth - requested.width;
        const int32_t currentbestDiffHeight = bestHeight - requested.height;
        const int32_t currentbestDiffFrameRate = bestFrameRate - requested.maxFPS;

        if ((diffHeight >= 0 && diffHeight <= abs(currentbestDiffHeight))  // Height better or equalt that previouse.
                || (currentbestDiffHeight < 0 && diffHeight >= currentbestDiffHeight))
        {
            if (diffHeight == currentbestDiffHeight)  // Found best height. Care about the width)
            {
                if ((diffWidth >= 0 && diffWidth <= abs(currentbestDiffWith))  // Width better or equal
                        || (currentbestDiffWith < 0 && diffWidth >= currentbestDiffWith))
                {
                    if (diffWidth == currentbestDiffWith && diffHeight == currentbestDiffHeight)// Same size as previously
                    {
                        // Also check the best frame rate if the diff is the same as previouse
                        if (((diffFrameRate >= 0 && diffFrameRate <= currentbestDiffFrameRate)  // Frame rate to high but
                             // better match than previouse and we have not selected IUV
                             || (currentbestDiffFrameRate < 0 && diffFrameRate >=
                                 currentbestDiffFrameRate))  // Current frame rate is
                                // lower than requested.
                                // This is better.
                                )
                        {
                            if ((currentbestDiffFrameRate ==
                                 diffFrameRate)  // Same frame rate as previous  or frame rate
                                    // allready good enough
                                    || (currentbestDiffFrameRate >= 0))
                            {
                                if (bestVideoType != requested.videoType &&
                                        requested.videoType != VideoType::kANY &&
                                        (capability.videoType == requested.videoType ||
                                         capability.videoType == VideoType::kI420 ||
                                         capability.videoType == VideoType::kYUY2 ||
                                         capability.videoType == VideoType::kYV12 ||
                                         capability.videoType == VideoType::kNV12))
                                {
                                    bestVideoType = capability.videoType;
                                    bestformatIndex = tmp;
                                }
                                // If width height and frame rate is full filled we can use the
                                // camera for encoding if it is supported.
                                if (capability.height == requested.height &&
                                        capability.width == requested.width &&
                                        capability.maxFPS >= requested.maxFPS)
                                {
                                    bestformatIndex = tmp;
                                }
                            }
                            else  // Better frame rate
                            {
                                bestWidth = capability.width;
                                bestHeight = capability.height;
                                bestFrameRate = capability.maxFPS;
                                bestVideoType = capability.videoType;
                                bestformatIndex = tmp;
                            }
                        }
                    }
                    else  // Better width than previously
                    {
                        bestWidth = capability.width;
                        bestHeight = capability.height;
                        bestFrameRate = capability.maxFPS;
                        bestVideoType = capability.videoType;
                        bestformatIndex = tmp;
                    }
                }     // else width no good
            }
            else  // Better height
            {
                bestWidth = capability.width;
                bestHeight = capability.height;
                bestFrameRate = capability.maxFPS;
                bestVideoType = capability.videoType;
                bestformatIndex = tmp;
            }
        }  // else height not good
    }    // end for

    OCTK_TRACE() << "Best camera format: " << bestWidth << "x"
                 << bestHeight << "@" << bestFrameRate
                 << "fps, color format: "
                 << static_cast<int>(bestVideoType);

    // Copy the capability
    if (bestformatIndex < 0)
    {
        return -1;
    }
    resulting = d->mCapabilities[bestformatIndex];
    return bestformatIndex;
}

CameraCapture::DeviceInfo::SharedPtr CameraCapture::createDeviceInfo()
{
#if defined(OCTK_OS_LINUX)
    return std::make_shared<CameraDeviceInfoV4L2>();
#endif
    return nullptr;
}

CameraCapture::SharedPtr CameraCapture::create(const char *deviceUniqueIdUTF8)
{
#if defined(OCTK_OS_LINUX)
    auto capture = std::make_shared<CameraCaptureV4L2>();
    if (capture->init(deviceUniqueIdUTF8))
    {
        return capture;
    }
#endif
    return nullptr;
}

CameraCapturePrivate::CameraCapturePrivate(CameraCapture *p)
    : mPPtr(p)
{
    mRequestedCapability.maxFPS = 30;
    mRequestedCapability.videoType = VideoType::kI420;
    mRequestedCapability.width = CameraCapture::kDefaultWidth;
    mRequestedCapability.height = CameraCapture::kDefaultHeight;
    memset(mIncomingFrameTimesNanos, 0, sizeof(mIncomingFrameTimesNanos));
}

CameraCapturePrivate::~CameraCapturePrivate()
{
    OCTK_DCHECK_RUN_ON(&mApiChecker);
    if (mDeviceUniqueId)
    {
        delete[] mDeviceUniqueId;
    }
}

void CameraCapturePrivate::updateFrameCount()
{
    OCTK_CHECK_RUNS_SERIALIZED(&mCaptureChecker);
    if (mIncomingFrameTimesNanos[0] / DateTime::kNSecsPerUSec == 0)
    {
        // first no shift
    }
    else
    {
        // shift
        for (int i = (CameraCapture::kFrameRateCountHistorySize - 2); i >= 0; --i)
        {
            mIncomingFrameTimesNanos[i + 1] = mIncomingFrameTimesNanos[i];
        }
    }
    mIncomingFrameTimesNanos[0] = DateTime::TimeNanos();
}

int32_t CameraCapturePrivate::incomingFrame(uint8_t *videoFrame, std::size_t videoFrameLength,
                                            const Capability &frameInfo, int64_t captureTime)
{
    OCTK_CHECK_RUNS_SERIALIZED(&mCaptureChecker);
    std::unique_lock<std::mutex> lock(mApiMutex);
    const auto apply_rotation = mApplyRotation;
    const auto rotateFrame = mVideoRotation;
    lock.unlock();

    const int32_t width = frameInfo.width;
    const int32_t height = frameInfo.height;

    //    TRACE_EVENT1("webrtc", "VC::IncomingFrame", "capture_time", captureTime);

    //    if (_rawDataCallBack)
    //    {
    //        DeliverRawFrame(videoFrame, videoFrameLength, frameInfo, captureTime);
    //        return 0;
    //    }

    // Not encoded, convert to I420.
    if (frameInfo.videoType != VideoType::kMJPG)
    {
        // Allow buffers larger than expected. On linux gstreamer allocates buffers
        // page-aligned and v4l2loopback passes us the buffer size verbatim which
        // for most cases is larger than expected.
        // See https://github.com/umlaeute/v4l2loopback/issues/190.
        if (auto size = utils::videoTypeBufferSize(frameInfo.videoType, width, abs(height)); videoFrameLength < size)
        {
            OCTK_INFO() << "Wrong incoming frame length. Expected " << size
                        << ", Got " << videoFrameLength << ".";
            return -1;
        }
    }

    int stride_y = width;
    int stride_uv = (width + 1) / 2;
    int target_width = width;
    int target_height = abs(height);

    if (apply_rotation)
    {
        // Rotating resolution when for 90/270 degree rotations.
        if (rotateFrame == VideoRotation::kAngle90 || rotateFrame == VideoRotation::kAngle270)
        {
            target_width = std::abs(height);
            target_height = width;
        }
    }

    // Setting absolute height (in case it was negative).
    // In Windows, the image starts bottom left, instead of top left.
    // Setting a negative source height, inverts the image (within LibYuv).
    auto buffer = I420Buffer::create(target_width, target_height, stride_y, stride_uv, stride_uv);
    const auto conversionResult = utils::yuv::convertToI420(videoFrame,
                                                            videoFrameLength,
                                                            buffer.get()->MutableDataY(),
                                                            buffer.get()->strideY(),
                                                            buffer.get()->MutableDataU(),
                                                            buffer.get()->strideU(),
                                                            buffer.get()->MutableDataV(),
                                                            buffer.get()->strideV(),
                                                            0, 0,  // No Cropping
                                                            width, height,
                                                            target_width, target_height,
                                                            rotateFrame,
                                                            frameInfo.videoType);
    if (!conversionResult)
    {
        OCTK_INFO() << "Failed to convert capture frame from type "
                    << static_cast<int>(frameInfo.videoType) << "to I420.";
        return -1;
    }

    VideoFrame captureFrame =
            VideoFrame::Builder()
            .setVideoFrameBuffer(buffer)
            .setRtpTimestamp(0)
            .setTimestampMSecs(DateTime::TimeMillis())
            .setRotation(!mApplyRotation ? mVideoRotation : VideoRotation::kAngle0)
            .build();
    captureFrame.setNtpTimeMSecs(captureTime);

    lock.lock();
    this->deliverCapturedFrame(captureFrame);
    return 0;
}

int32_t CameraCapturePrivate::deliverCapturedFrame(VideoFrame &captureFrame)
{
    OCTK_CHECK_RUNS_SERIALIZED(&mCaptureChecker);

    this->updateFrameCount();  // frame count used for local frame rate callback.

    if (mDataCallBack)
    {
        mDataCallBack->onFrame(captureFrame);
    }
    return 0;
}

CameraCapture::CameraCapture(CameraCapturePrivate *d)
    : mDPtr(d)
{
}

CameraCapture::~CameraCapture()
{
    this->deregisterCaptureDataCallback();
}

void CameraCapture::registerCaptureDataCallback(VideoSinkInterface<VideoFrame> *dataCallback)
{
    OCTK_D(CameraCapture);
    std::lock_guard<std::mutex> lock(d->mApiMutex);
    //    OCTK_DCHECK(!d->_rawDataCallBack);
    d->mDataCallBack = dataCallback;
}

void CameraCapture::deregisterCaptureDataCallback()
{
    OCTK_D(CameraCapture);
    std::lock_guard<std::mutex> lock(d->mApiMutex);
    d->mDataCallBack = NULL;
    //    _rawDataCallBack = NULL;
}

int32_t CameraCapture::startCapture(const Capability &capability)
{
    OCTK_D(CameraCapture);
    OCTK_DCHECK_RUN_ON(&d->mApiChecker);
    d->mRequestedCapability = capability;
    return -1;
}

int32_t CameraCapture::stopCapture()
{
    return -1;
}

bool CameraCapture::isCaptureStarted()
{
    return false;
}

const char *CameraCapture::currentDeviceName() const
{
    OCTK_D(const CameraCapture);
    OCTK_DCHECK_RUN_ON(&d->mApiChecker);
    return d->mDeviceUniqueId;
}

int32_t CameraCapture::captureSettings(Capability &settings)
{
    return -1;
}

int32_t CameraCapture::setCaptureRotation(VideoRotation rotation)
{
    OCTK_D(CameraCapture);
    std::lock_guard<std::mutex> lock(d->mApiMutex);
    d->mVideoRotation = rotation;
    return 0;
}

bool CameraCapture::getApplyRotation()
{
    OCTK_D(CameraCapture);
    std::lock_guard<std::mutex> lock(d->mApiMutex);
    return d->mApplyRotation;
}

bool CameraCapture::setApplyRotation(bool enable)
{
    OCTK_D(CameraCapture);
    std::lock_guard<std::mutex> lock(d->mApiMutex);
    d->mApplyRotation = enable;
    return true;
}

OCTK_END_NAMESPACE
