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

#include <private/octk_camera_device_info_v4l2_p.hpp>
#include <private/octk_camera_capture_p.hpp>
#include <octk_logging.hpp>
#include <octk_checks.hpp>

#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>

// These defines are here to support building on kernel 3.16 which some downstream projects, e.g. Firefox, use.
#ifndef V4L2_PIX_FMT_ABGR32
#define ABGR32_OVERRIDE 1
#define V4L2_PIX_FMT_ABGR32 v4l2_fourcc('A', 'R', '2', '4')
#endif

#ifndef V4L2_PIX_FMT_ARGB32
#define ARGB32_OVERRIDE 1
#define V4L2_PIX_FMT_ARGB32 v4l2_fourcc('B', 'A', '2', '4')
#endif

#ifndef V4L2_PIX_FMT_RGBA32
#define RGBA32_OVERRIDE 1
#define V4L2_PIX_FMT_RGBA32 v4l2_fourcc('A', 'B', '2', '4')
#endif

OCTK_BEGIN_NAMESPACE

namespace detail
{
    static bool isDeviceNameMatches(const char* name,
                                    const char* deviceUniqueIdUTF8)
    {
        if (strncmp(deviceUniqueIdUTF8, name, strlen(name)) == 0)
        {
            return true;
        }
        return false;
    }
} // namespace detail

class CameraDeviceInfoV4L2Private : public CameraCapture::DeviceInfoPrivate
{
public:
    explicit CameraDeviceInfoV4L2Private(CameraDeviceInfoV4L2 *p);
    ~CameraDeviceInfoV4L2Private();

    int32_t fillCapabilities(int fd);

private:
    OCTK_DECLARE_PUBLIC(CameraDeviceInfoV4L2)
    OCTK_DISABLE_COPY_MOVE(CameraDeviceInfoV4L2Private)
};

CameraDeviceInfoV4L2Private::CameraDeviceInfoV4L2Private(CameraDeviceInfoV4L2 *p)
    : CameraCapture::DeviceInfoPrivate(p)
{
}

CameraDeviceInfoV4L2Private::~CameraDeviceInfoV4L2Private()
{
}

int32_t CameraDeviceInfoV4L2Private::fillCapabilities(int fd)
{
    struct v4l2_format video_fmt;
    memset(&video_fmt, 0, sizeof(struct v4l2_format));

    video_fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    video_fmt.fmt.pix.sizeimage = 0;

    unsigned int videoFormats[] =
    {
        V4L2_PIX_FMT_MJPEG,  V4L2_PIX_FMT_JPEG,   V4L2_PIX_FMT_YUV420,
        V4L2_PIX_FMT_YVU420, V4L2_PIX_FMT_YUYV,   V4L2_PIX_FMT_UYVY,
        V4L2_PIX_FMT_NV12,   V4L2_PIX_FMT_BGR24,  V4L2_PIX_FMT_RGB24,
        V4L2_PIX_FMT_RGB565, V4L2_PIX_FMT_ABGR32, V4L2_PIX_FMT_ARGB32,
        V4L2_PIX_FMT_RGBA32, V4L2_PIX_FMT_BGR32,  V4L2_PIX_FMT_RGB32,
    };
    constexpr int totalFmts = sizeof(videoFormats) / sizeof(unsigned int);

    int sizes = 13;
    unsigned int size[][2] = {{128, 96},   {160, 120},  {176, 144},  {320, 240},
                              {352, 288},  {640, 480},  {704, 576},  {800, 600},
                              {960, 720},  {1280, 720}, {1024, 768}, {1440, 1080},
                              {1920, 1080}};

    for (int fmts = 0; fmts < totalFmts; fmts++)
    {
        for (int i = 0; i < sizes; i++)
        {
            video_fmt.fmt.pix.pixelformat = videoFormats[fmts];
            video_fmt.fmt.pix.width = size[i][0];
            video_fmt.fmt.pix.height = size[i][1];

            if (ioctl(fd, VIDIOC_TRY_FMT, &video_fmt) >= 0)
            {
                if ((video_fmt.fmt.pix.width == size[i][0]) && (video_fmt.fmt.pix.height == size[i][1]))
                {
                    Capability cap;
                    cap.width = video_fmt.fmt.pix.width;
                    cap.height = video_fmt.fmt.pix.height;
                    if (videoFormats[fmts] == V4L2_PIX_FMT_YUYV)
                    {
                        cap.videoType = VideoType::kYUY2;
                    }
                    else if (videoFormats[fmts] == V4L2_PIX_FMT_YUV420)
                    {
                        cap.videoType = VideoType::kI420;
                    }
                    else if (videoFormats[fmts] == V4L2_PIX_FMT_YVU420)
                    {
                        cap.videoType = VideoType::kYV12;
                    }
                    else if (videoFormats[fmts] == V4L2_PIX_FMT_MJPEG ||
                             videoFormats[fmts] == V4L2_PIX_FMT_JPEG)
                    {
                        cap.videoType = VideoType::kMJPG;
                    }
                    else if (videoFormats[fmts] == V4L2_PIX_FMT_UYVY)
                    {
                        cap.videoType = VideoType::kUYVY;
                    }
                    else if (videoFormats[fmts] == V4L2_PIX_FMT_NV12)
                    {
                        cap.videoType = VideoType::kNV12;
                    }
                    else if (videoFormats[fmts] == V4L2_PIX_FMT_BGR24)
                    {
                        // NB that for RGB formats, `VideoType` follows naming conventions
                        // of libyuv[1], where e.g. the format for FOURCC "ARGB" stores
                        // pixels in BGRA order in memory. V4L2[2] on the other hand names
                        // its formats based on the order of the RGB components as stored in
                        // memory. Applies to all RGB formats below.
                        // [1]https://chromium.googlesource.com/libyuv/libyuv/+/refs/heads/main/docs/formats.md#the-argb-fourcc
                        // [2]https://www.kernel.org/doc/html/v6.2/userspace-api/media/v4l/pixfmt-rgb.html#bits-per-component
                        cap.videoType = VideoType::kRGB24;
                    }
                    else if (videoFormats[fmts] == V4L2_PIX_FMT_RGB24)
                    {
                        cap.videoType = VideoType::kBGR24;
                    }
                    else if (videoFormats[fmts] == V4L2_PIX_FMT_RGB565)
                    {
                        cap.videoType = VideoType::kRGB565;
                    }
                    else if (videoFormats[fmts] == V4L2_PIX_FMT_ABGR32)
                    {
                        cap.videoType = VideoType::kARGB;
                    }
                    else if (videoFormats[fmts] == V4L2_PIX_FMT_ARGB32)
                    {
                        cap.videoType = VideoType::kBGRA;
                    }
                    else if (videoFormats[fmts] == V4L2_PIX_FMT_BGR32)
                    {
                        cap.videoType = VideoType::kARGB;
                    }
                    else if (videoFormats[fmts] == V4L2_PIX_FMT_RGB32)
                    {
                        cap.videoType = VideoType::kBGRA;
                    }
                    else if (videoFormats[fmts] == V4L2_PIX_FMT_RGBA32)
                    {
                        cap.videoType = VideoType::kABGR;
                    }
                    else
                    {
                        OCTK_DCHECK_NOTREACHED();
                    }

                    // get fps of current camera mode
                    // V4l2 does not have a stable method of knowing so we just guess.
                    if (cap.width >= 800 && cap.videoType != VideoType::kMJPG)
                    {
                        cap.maxFPS = 15;
                    }
                    else
                    {
                        cap.maxFPS = 30;
                    }

                    mCapabilities.push_back(cap);
                    OCTK_TRACE() << "Camera capability, width:" << cap.width
                                 << " height:" << cap.height
                                 << " type:" << static_cast<int32_t>(cap.videoType)
                                 << " fps:" << cap.maxFPS;
                }
            }
        }
    }

    OCTK_INFO() << "CreateCapabilityMap " << mCapabilities.size();
    return mCapabilities.size();
}

CameraDeviceInfoV4L2::CameraDeviceInfoV4L2()
    : CameraCapture::DeviceInfo(new CameraDeviceInfoV4L2Private(this))
{
}

CameraDeviceInfoV4L2::~CameraDeviceInfoV4L2()
{
}

uint32_t CameraDeviceInfoV4L2::numberOfDevices()
{
    struct v4l2_capability cap;
    uint32_t count = 0;
    char device[20];
    int fd = -1;

    /* detect /dev/video [0-63]VideoCaptureModule entries */
    for (int n = 0; n < 64; n++)
    {
        snprintf(device, sizeof(device), "/dev/video%d", n);
        if ((fd = open(device, O_RDONLY)) != -1)
        {
            // query device capabilities and make sure this is a video capture device
            if (ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0 || !(cap.device_caps & V4L2_CAP_VIDEO_CAPTURE))
            {
                close(fd);
                continue;
            }
            close(fd);
            count++;
        }
    }

    return count;
}

int32_t CameraDeviceInfoV4L2::getDeviceName(uint32_t deviceNumber, char *deviceNameUTF8, uint32_t deviceNameLength,
                                            char *deviceUniqueIdUTF8, uint32_t deviceUniqueIdUTF8Length,
                                            char *productUniqueIdUTF8, uint32_t productUniqueIdUTF8Length)
{
    // Travel through /dev/video [0-63]
    struct v4l2_capability cap;
    bool found = false;
    uint32_t count = 0;
    char device[20];
    int fd = -1;

    for (int n = 0; n < 64; n++)
    {
        snprintf(device, sizeof(device), "/dev/video%d", n);
        if ((fd = open(device, O_RDONLY)) != -1)
        {
            // query device capabilities and make sure this is a video capture device
            if (ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0 || !(cap.device_caps & V4L2_CAP_VIDEO_CAPTURE))
            {
                close(fd);
                continue;
            }
            if (count == deviceNumber)
            {
                // Found the device
                found = true;
                break;
            }
            else
            {
                close(fd);
                count++;
            }
        }
    }

    if (!found)
    {
        return -1;
    }

    // query device capabilities
    if (ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0)
    {
        OCTK_INFO() << "error in querying the device capability for device "
                    << device << ". errno = " << errno;
        close(fd);
        return -1;
    }

    close(fd);

    char cameraName[64];
    memset(deviceNameUTF8, 0, deviceNameLength);
    memcpy(cameraName, cap.card, sizeof(cap.card));

    if (deviceNameLength > strlen(cameraName))
    {
        memcpy(deviceNameUTF8, cameraName, strlen(cameraName));
    }
    else
    {
        OCTK_INFO() << "buffer passed is too small";
        return -1;
    }

    if (cap.bus_info[0] != 0) // may not available in all drivers
    {
        // copy device id
        size_t len = strlen(reinterpret_cast<const char*>(cap.bus_info));
        if (deviceUniqueIdUTF8Length > len)
        {
            memset(deviceUniqueIdUTF8, 0, deviceUniqueIdUTF8Length);
            memcpy(deviceUniqueIdUTF8, cap.bus_info, len);
        }
        else
        {
            OCTK_INFO() << "buffer passed is too small";
            return -1;
        }
    }

    return 0;
}

int32_t CameraDeviceInfoV4L2::init()
{
    return 0;
}

int32_t CameraDeviceInfoV4L2::createCapabilityMap(const char *deviceUniqueIdUTF8)
{
    OCTK_D(CameraDeviceInfoV4L2);

    int fd;
    char device[32];
    bool found = false;

    const int32_t deviceUniqueIdUTF8Length = strlen(deviceUniqueIdUTF8);
    if (deviceUniqueIdUTF8Length >= CameraCapture::kUniqueNameLength)
    {
        OCTK_INFO() << "Device name too long";
        return -1;
    }
    OCTK_INFO() << "CreateCapabilityMap called for device "
                << deviceUniqueIdUTF8;

    /* detect /dev/video [0-63] entries */
    for (int n = 0; n < 64; ++n)
    {
        snprintf(device, sizeof(device), "/dev/video%d", n);
        fd = open(device, O_RDONLY);
        if (fd == -1)
        {
            continue;
        }

        // query device capabilities
        struct v4l2_capability cap;
        if (ioctl(fd, VIDIOC_QUERYCAP, &cap) == 0)
        {
            // skip devices without video capture capability
            if (!(cap.device_caps & V4L2_CAP_VIDEO_CAPTURE))
            {
                continue;
            }

            if (cap.bus_info[0] != 0)
            {
                if (strncmp(reinterpret_cast<const char*>(cap.bus_info),
                            deviceUniqueIdUTF8,
                            strlen(deviceUniqueIdUTF8)) == 0)
                {
                    // match with device id
                    found = true;
                    break;  // fd matches with device unique id supplied
                }
            }
            else
            {
                // match for device name
                if (detail::isDeviceNameMatches(reinterpret_cast<const char*>(cap.card),
                                                deviceUniqueIdUTF8))
                {
                    found = true;
                    break;
                }
            }
        }
        close(fd);  // close since this is not the matching device
    }

    if (!found)
    {
        OCTK_INFO() << "no matching device found";
        return -1;
    }

    // now fd will point to the matching device
    // reset old capability list.
    d->mCapabilities.clear();

    int size = d->fillCapabilities(fd);
    close(fd);

    // Store the new used device name
    d->mLastUsedDeviceNameLength = deviceUniqueIdUTF8Length;
    d->mLastUsedDeviceName = reinterpret_cast<char*>(realloc(d->mLastUsedDeviceName, d->mLastUsedDeviceNameLength + 1));
    memcpy(d->mLastUsedDeviceName, deviceUniqueIdUTF8, d->mLastUsedDeviceNameLength + 1);

    OCTK_INFO() << "CreateCapabilityMap " << d->mCapabilities.size();
    return size;
}

OCTK_END_NAMESPACE
