#include <private/octk_camera_capture_v4l2_p.hpp>
#include <private/octk_camera_capture_p.hpp>
#include <octk_platform_thread.hpp>

#include <linux/videodev2.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>

#include <string>
#include <thread>
#include <new>

// These defines are here to support building on kernel 3.16 which some
// downstream projects, e.g. Firefox, use.
// TODO(apehrson): Remove them and their undefs when no longer needed.
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
    inline std::string getFourccName(uint32_t fourcc)
    {
        std::string name;
        name.push_back(static_cast<char>(fourcc & 0xFF));
        name.push_back(static_cast<char>((fourcc >> 8) & 0xFF));
        name.push_back(static_cast<char>((fourcc >> 16) & 0xFF));
        name.push_back(static_cast<char>((fourcc >> 24) & 0xFF));
        return name;
    }
} // namespace detail

class CameraCaptureV4L2Private : public CameraCapturePrivate
{
public:
    struct Buffer
    {
        void* start;
        size_t length;
    };
    using Capability = CameraCapture::Capability;

    OCTK_STATIC_CONSTANT_NUMBER(kNoOfV4L2Bufffers, 4)

    explicit CameraCaptureV4L2Private(CameraCaptureV4L2 *p);
    virtual ~CameraCaptureV4L2Private();

    bool deAllocateVideoBuffers();
    bool allocateVideoBuffers();
    bool captureProcess();

    PlatformThread::SharedPtr mCaptureThread OCTK_ATTRIBUTE_GUARDED_BY(mApiChecker);

    std::mutex mCaptureMutex OCTK_ATTRIBUTE_ACQUIRED_BEFORE(mApiMutex);
    bool mQuit OCTK_ATTRIBUTE_GUARDED_BY(mCaptureMutex);
    int32_t mDeviceId OCTK_ATTRIBUTE_GUARDED_BY(mApiChecker) = -1;
    int32_t mDeviceFd OCTK_ATTRIBUTE_GUARDED_BY(mCaptureChecker) = -1;

    int32_t mBuffersAllocatedByDevice OCTK_ATTRIBUTE_GUARDED_BY(mCaptureMutex) = -1;
    Capability mConfiguredCapability OCTK_ATTRIBUTE_GUARDED_BY(mCaptureChecker);
    bool mStreaming OCTK_ATTRIBUTE_GUARDED_BY(mCaptureChecker) = false;
    bool mCaptureStarted OCTK_ATTRIBUTE_GUARDED_BY(mApiChecker) = false;
    Buffer *mPoolBuffer OCTK_ATTRIBUTE_GUARDED_BY(mCaptureMutex) = nullptr;

private:
    OCTK_DECLARE_PUBLIC(CameraCaptureV4L2)
    OCTK_DISABLE_COPY_MOVE(CameraCaptureV4L2Private)
};

CameraCaptureV4L2Private::CameraCaptureV4L2Private(CameraCaptureV4L2 *p)
    : CameraCapturePrivate(p)
{
}

CameraCaptureV4L2Private::~CameraCaptureV4L2Private()
{
}

bool CameraCaptureV4L2Private::deAllocateVideoBuffers()
{
    OCTK_CHECK_RUNS_SERIALIZED(&mCaptureChecker);
    // unmap buffers
    for (int i = 0; i < mBuffersAllocatedByDevice; i++)
    {
        munmap(mPoolBuffer[i].start, mPoolBuffer[i].length);
    }

    delete[] mPoolBuffer;

    // turn off stream
    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(mDeviceFd, VIDIOC_STREAMOFF, &type) < 0)
    {
        OCTK_INFO() << "VIDIOC_STREAMOFF error. errno: " << errno;
    }

    return true;
}

bool CameraCaptureV4L2Private::allocateVideoBuffers()
{
    OCTK_CHECK_RUNS_SERIALIZED(&mCaptureChecker);
    struct v4l2_requestbuffers rbuffer;
    memset(&rbuffer, 0, sizeof(v4l2_requestbuffers));

    rbuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    rbuffer.memory = V4L2_MEMORY_MMAP;
    rbuffer.count = kNoOfV4L2Bufffers;

    if (ioctl(mDeviceFd, VIDIOC_REQBUFS, &rbuffer) < 0)
    {
        OCTK_INFO() << "Could not get buffers from device. errno = " << errno;
        return false;
    }

    if (rbuffer.count > kNoOfV4L2Bufffers)
    {
        rbuffer.count = kNoOfV4L2Bufffers;
    }

    mBuffersAllocatedByDevice = rbuffer.count;

    // Map the buffers
    mPoolBuffer = new Buffer[rbuffer.count];

    for (unsigned int i = 0; i < rbuffer.count; i++)
    {
        struct v4l2_buffer buffer;
        memset(&buffer, 0, sizeof(v4l2_buffer));
        buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buffer.memory = V4L2_MEMORY_MMAP;
        buffer.index = i;

        if (ioctl(mDeviceFd, VIDIOC_QUERYBUF, &buffer) < 0)
        {
            return false;
        }

        mPoolBuffer[i].start = mmap(NULL, buffer.length, PROT_READ | PROT_WRITE, MAP_SHARED, mDeviceFd, buffer.m.offset);

        if (MAP_FAILED == mPoolBuffer[i].start)
        {
            for (unsigned int j = 0; j < i; j++)
            {
                munmap(mPoolBuffer[j].start, mPoolBuffer[j].length);
            }
            return false;
        }

        mPoolBuffer[i].length = buffer.length;

        if (ioctl(mDeviceFd, VIDIOC_QBUF, &buffer) < 0)
        {
            return false;
        }
    }
    return true;
}

bool CameraCaptureV4L2Private::captureProcess()
{
//    OCTK_TRACE() << "CameraCaptureV4L2Private::captureProcess():enter";
    OCTK_CHECK_RUNS_SERIALIZED(&mCaptureChecker);

    int retVal = 0;
    fd_set rSet;
    struct timeval timeout;

    FD_ZERO(&rSet);
    FD_SET(mDeviceFd, &rSet);
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    // mDeviceFd written only in StartCapture, when this thread isn't running.
//    OCTK_TRACE() << "CameraCaptureV4L2Private::captureProcess():select start";
    retVal = select(mDeviceFd + 1, &rSet, NULL, NULL, &timeout);
//    OCTK_TRACE() << "CameraCaptureV4L2Private::captureProcess():select finish";

    {
        std::lock_guard<std::mutex> lock(mCaptureMutex);
//        OCTK_TRACE() << "CameraCaptureV4L2Private::captureProcess():mCaptureMutex";

        if (mQuit)
        {
            return false;
        }

        if (retVal < 0 && errno != EINTR) // continue if interrupted
        {
            // select failed
//            OCTK_TRACE() << "CameraCaptureV4L2Private::captureProcess():select failed";
            return false;
        }
        else if (retVal == 0)
        {
            // select timed out
//            OCTK_TRACE() << "CameraCaptureV4L2Private::captureProcess():select timed out";
            return true;
        }
        else if (!FD_ISSET(mDeviceFd, &rSet))
        {
            // not event on camera handle
//            OCTK_TRACE() << "CameraCaptureV4L2Private::captureProcess():select not event";
            return true;
        }

        if (mStreaming)
        {
//            OCTK_TRACE() << "CameraCaptureV4L2Private::captureProcess():ioctl buffer----------";
            struct v4l2_buffer buf;
            memset(&buf, 0, sizeof(struct v4l2_buffer));
            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_MMAP;
            // dequeue a buffer - repeat until dequeued properly!
            while (ioctl(mDeviceFd, VIDIOC_DQBUF, &buf) < 0)
            {
                if (errno != EINTR)
                {
                    OCTK_INFO() << "could not sync on a buffer on device "
                                << strerror(errno);
                    return true;
                }
            }

            // convert to to I420 if needed
//            OCTK_TRACE() << "CameraCaptureV4L2Private::captureProcess():incomingFrame";
            this->incomingFrame(reinterpret_cast<uint8_t*>(mPoolBuffer[buf.index].start), buf.bytesused, mConfiguredCapability);
            // enqueue the buffer again
            if (ioctl(mDeviceFd, VIDIOC_QBUF, &buf) == -1)
            {
                OCTK_INFO() << "Failed to enqueue capture buffer";
            }
        }
    }
    usleep(0);
    return true;
}

CameraCaptureV4L2::CameraCaptureV4L2()
    : CameraCapture(new CameraCaptureV4L2Private(this))
{
}

CameraCaptureV4L2::~CameraCaptureV4L2()
{
    OCTK_D(CameraCaptureV4L2);
    OCTK_DCHECK_RUN_ON(&d->mApiChecker);
    OCTK_CHECK_RUNS_SERIALIZED(&d->mCaptureChecker);

    this->stopCapture();
    if (-1 != d->mDeviceFd)
    {
        close(d->mDeviceFd);
    }
}

int32_t CameraCaptureV4L2::startCapture(const Capability &capability)
{
    OCTK_D(CameraCaptureV4L2);
    OCTK_DCHECK_RUN_ON(&d->mApiChecker);;

    if (d->mCaptureStarted)
    {
        if (capability == d->mRequestedCapability)
        {
            return 0;
        }
        else
        {
            this->stopCapture();
        }
    }

    // We don't want members above to be guarded by capture_checker_ as
    // it's meant to be for members that are accessed on the API thread
    // only when we are not capturing. The code above can be called many
    // times while sharing instance of VideoCaptureV4L2 between websites
    // and therefore it would not follow the requirements of this checker.
    OCTK_CHECK_RUNS_SERIALIZED(&d->mCaptureChecker);

    // Set a baseline of configured parameters. It is updated here during
    // configuration, then read from the capture thread.
    d->mConfiguredCapability = capability;

    std::lock_guard<std::mutex> lock(d->mCaptureMutex);
    // first open /dev/video device
    char device[20];
    snprintf(device, sizeof(device), "/dev/video%d", d->mDeviceId);

    if ((d->mDeviceFd = open(device, O_RDWR | O_NONBLOCK, 0)) < 0)
    {
        OCTK_INFO() << "error in opening " << device << " errono = " << errno;
        return -1;
    }

    // Supported video formats in preferred order.
    // If the requested resolution is larger than VGA, we prefer MJPEG. Go for
    // I420 otherwise.
    unsigned int hdFmts[] =
    {
        V4L2_PIX_FMT_MJPEG,  V4L2_PIX_FMT_YUV420, V4L2_PIX_FMT_YVU420,
        V4L2_PIX_FMT_YUYV,   V4L2_PIX_FMT_UYVY,   V4L2_PIX_FMT_NV12,
        V4L2_PIX_FMT_ABGR32, V4L2_PIX_FMT_ARGB32, V4L2_PIX_FMT_RGBA32,
        V4L2_PIX_FMT_BGR32,  V4L2_PIX_FMT_RGB32,  V4L2_PIX_FMT_BGR24,
        V4L2_PIX_FMT_RGB24,  V4L2_PIX_FMT_RGB565, V4L2_PIX_FMT_JPEG,
    };
    unsigned int sdFmts[] =
    {
        V4L2_PIX_FMT_YUV420, V4L2_PIX_FMT_YVU420, V4L2_PIX_FMT_YUYV,
        V4L2_PIX_FMT_UYVY,   V4L2_PIX_FMT_NV12,   V4L2_PIX_FMT_ABGR32,
        V4L2_PIX_FMT_ARGB32, V4L2_PIX_FMT_RGBA32, V4L2_PIX_FMT_BGR32,
        V4L2_PIX_FMT_RGB32,  V4L2_PIX_FMT_BGR24,  V4L2_PIX_FMT_RGB24,
        V4L2_PIX_FMT_RGB565, V4L2_PIX_FMT_MJPEG,  V4L2_PIX_FMT_JPEG,
    };
    const bool isHd = capability.width > 640 || capability.height > 480;
    unsigned int* fmts = isHd ? hdFmts : sdFmts;
    static_assert(sizeof(hdFmts) == sizeof(sdFmts));
    constexpr int nFormats = sizeof(hdFmts) / sizeof(unsigned int);

    // Enumerate image formats.
    struct v4l2_fmtdesc fmt;
    int fmtsIdx = nFormats;
    memset(&fmt, 0, sizeof(fmt));
    fmt.index = 0;
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    OCTK_INFO() << "Video Capture enumerats supported image formats:";
    while (ioctl(d->mDeviceFd, VIDIOC_ENUM_FMT, &fmt) == 0)
    {
        OCTK_INFO() << "  { pixelformat = "
                    << detail::getFourccName(fmt.pixelformat)
                    << ", description = '" << fmt.description << "' }";
        // Match the preferred order.
        for (int i = 0; i < nFormats; i++)
        {
            if (fmt.pixelformat == fmts[i] && i < fmtsIdx)
            {
                fmtsIdx = i;
            }
        }
        // Keep enumerating.
        fmt.index++;
    }

    if (fmtsIdx == nFormats)
    {
        OCTK_INFO() << "no supporting video formats found";
        return -1;
    }
    else
    {
        OCTK_INFO() << "We prefer format "
                    << detail::getFourccName(fmts[fmtsIdx]);
    }

    struct v4l2_format video_fmt;
    memset(&video_fmt, 0, sizeof(struct v4l2_format));
    video_fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    video_fmt.fmt.pix.sizeimage = 0;
    video_fmt.fmt.pix.width = capability.width;
    video_fmt.fmt.pix.height = capability.height;
    video_fmt.fmt.pix.pixelformat = fmts[fmtsIdx];

    if (video_fmt.fmt.pix.pixelformat == V4L2_PIX_FMT_YUYV)
    {
        d->mConfiguredCapability.videoType = VideoType::kYUY2;
    }
    else if (video_fmt.fmt.pix.pixelformat == V4L2_PIX_FMT_YUV420)
    {
        d->mConfiguredCapability.videoType = VideoType::kI420;
    }
    else if (video_fmt.fmt.pix.pixelformat == V4L2_PIX_FMT_YVU420)
    {
        d->mConfiguredCapability.videoType = VideoType::kYV12;
    }
    else if (video_fmt.fmt.pix.pixelformat == V4L2_PIX_FMT_UYVY)
    {
        d->mConfiguredCapability.videoType = VideoType::kUYVY;
    }
    else if (video_fmt.fmt.pix.pixelformat == V4L2_PIX_FMT_NV12)
    {
        d->mConfiguredCapability.videoType = VideoType::kNV12;
    }
    else if (video_fmt.fmt.pix.pixelformat == V4L2_PIX_FMT_BGR24)
    {
        d->mConfiguredCapability.videoType = VideoType::kRGB24;
    }
    else if (video_fmt.fmt.pix.pixelformat == V4L2_PIX_FMT_RGB24)
    {
        d->mConfiguredCapability.videoType = VideoType::kBGR24;
    }
    else if (video_fmt.fmt.pix.pixelformat == V4L2_PIX_FMT_RGB565)
    {
        d->mConfiguredCapability.videoType = VideoType::kRGB565;
    }
    else if (video_fmt.fmt.pix.pixelformat == V4L2_PIX_FMT_ABGR32 ||
             video_fmt.fmt.pix.pixelformat == V4L2_PIX_FMT_BGR32)
    {
        d->mConfiguredCapability.videoType = VideoType::kARGB;
    }
    else if (video_fmt.fmt.pix.pixelformat == V4L2_PIX_FMT_ARGB32 ||
             video_fmt.fmt.pix.pixelformat == V4L2_PIX_FMT_RGB32)
    {
        d->mConfiguredCapability.videoType = VideoType::kBGRA;
    }
    else if (video_fmt.fmt.pix.pixelformat == V4L2_PIX_FMT_RGBA32)
    {
        d->mConfiguredCapability.videoType = VideoType::kABGR;
    }
    else if (video_fmt.fmt.pix.pixelformat == V4L2_PIX_FMT_MJPEG ||
             video_fmt.fmt.pix.pixelformat == V4L2_PIX_FMT_JPEG)
    {
        d->mConfiguredCapability.videoType = VideoType::kMJPG;
    }
    else
    {
        OCTK_DCHECK_NOTREACHED();
    }

    // set format and frame size now
    if (ioctl(d->mDeviceFd, VIDIOC_S_FMT, &video_fmt) < 0)
    {
        OCTK_INFO() << "error in VIDIOC_S_FMT, errno = " << errno;
        return -1;
    }

    // initialize current width and height
    d->mConfiguredCapability.width = video_fmt.fmt.pix.width;
    d->mConfiguredCapability.height = video_fmt.fmt.pix.height;

    // Trying to set frame rate, before check driver capability.
    bool driver_framerate_support = true;
    struct v4l2_streamparm streamparms;
    memset(&streamparms, 0, sizeof(streamparms));
    streamparms.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(d->mDeviceFd, VIDIOC_G_PARM, &streamparms) < 0)
    {
        OCTK_INFO() << "error in VIDIOC_G_PARM errno = " << errno;
        driver_framerate_support = false;
        // continue
    }
    else
    {
        // check the capability flag is set to V4L2_CAP_TIMEPERFRAME.
        if (streamparms.parm.capture.capability & V4L2_CAP_TIMEPERFRAME)
        {
            // driver supports the feature. Set required framerate.
            memset(&streamparms, 0, sizeof(streamparms));
            streamparms.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            streamparms.parm.capture.timeperframe.numerator = 1;
            streamparms.parm.capture.timeperframe.denominator = capability.maxFPS;
            if (ioctl(d->mDeviceFd, VIDIOC_S_PARM, &streamparms) < 0)
            {
                OCTK_INFO() << "Failed to set the framerate. errno=" << errno;
                driver_framerate_support = false;
            }
        }
    }
    // If driver doesn't support framerate control, need to hardcode.
    // Hardcoding the value based on the frame size.
    if (!driver_framerate_support)
    {
        if (d->mConfiguredCapability.width >= 800 && d->mConfiguredCapability.videoType != VideoType::kMJPG)
        {
            d->mConfiguredCapability.maxFPS = 15;
        }
        else
        {
            d->mConfiguredCapability.maxFPS = 30;
        }
    }

    if (!d->allocateVideoBuffers())
    {
        OCTK_INFO() << "failed to allocate video capture buffers";
        return -1;
    }

    // Needed to start UVC camera - from the uvcview application
    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(d->mDeviceFd, VIDIOC_STREAMON, &type) == -1)
    {
        OCTK_INFO() << "Failed to turn on stream";
        return -1;
    }

    d->mRequestedCapability = capability;
    d->mCaptureStarted = true;
    d->mStreaming = true;

    // start capture thread;
    if (!d->mCaptureThread.get())
    {
        d->mQuit = false;
        d->mCaptureThread = PlatformThread::create([=]
        {
            while (d->captureProcess())
            {
            }
        });
        d->mCaptureThread->start(PlatformThread::Priority::kHighest);
    }
    return 0;
}

int32_t CameraCaptureV4L2::stopCapture()
{
    OCTK_D(CameraCaptureV4L2);
    OCTK_DCHECK_RUN_ON(&d->mApiChecker);;

    if (d->mCaptureThread.get())
    {
        {
            std::lock_guard<std::mutex> lock(d->mCaptureMutex);
            d->mQuit = true;
        }
        // Make sure the capture thread stops using the mutex.
        d->mCaptureThread->wait();
    }

    d->mCaptureStarted = false;

    OCTK_CHECK_RUNS_SERIALIZED(&d->mCaptureChecker);
    std::lock_guard<std::mutex> lock(d->mCaptureMutex);
    if (d->mStreaming)
    {
        d->mStreaming = false;

        d->deAllocateVideoBuffers();
        close(d->mDeviceFd);
        d->mDeviceFd = -1;

        d->mRequestedCapability = d->mConfiguredCapability = Capability();
    }

    return 0;
}

bool CameraCaptureV4L2::isCaptureStarted()
{
    OCTK_D(CameraCaptureV4L2);
    OCTK_DCHECK_RUN_ON(&d->mApiChecker);;
    return d->mCaptureStarted;
}

int32_t CameraCaptureV4L2::captureSettings(Capability &settings)
{
    OCTK_D(CameraCaptureV4L2);
    OCTK_DCHECK_RUN_ON(&d->mApiChecker);;
    settings = d->mRequestedCapability;

    return 0;
}

bool CameraCaptureV4L2::init(const char *deviceUniqueIdUTF8)
{
    OCTK_D(CameraCaptureV4L2);
    OCTK_DCHECK_RUN_ON(&d->mApiChecker);;

    int len = strlen((const char*)deviceUniqueIdUTF8);
    d->mDeviceUniqueId = new (std::nothrow) char[len + 1];
    if (d->mDeviceUniqueId)
    {
        memcpy(d->mDeviceUniqueId, deviceUniqueIdUTF8, len + 1);
    }

    int fd;
    char device[32];
    bool found = false;

    /* detect /dev/video [0-63] entries */
    int n;
    for (n = 0; n < 64; n++)
    {
        snprintf(device, sizeof(device), "/dev/video%d", n);
        if ((fd = open(device, O_RDONLY)) != -1)
        {
            // query device capabilities
            struct v4l2_capability cap;
            if (ioctl(fd, VIDIOC_QUERYCAP, &cap) == 0)
            {
                if (cap.bus_info[0] != 0)
                {
                    if (strncmp((const char*)cap.bus_info,
                                (const char*)deviceUniqueIdUTF8,
                                strlen((const char*)deviceUniqueIdUTF8)) == 0)
                    {
                        // match with device id
                        close(fd);
                        found = true;
                        break;  // fd matches with device unique id supplied
                    }
                }
            }
            close(fd);  // close since this is not the matching device
        }
    }
    if (!found)
    {
        OCTK_INFO() << "no matching device found";
        return false;
    }
    d->mDeviceId = n;  // store the device id
    return true;
}

OCTK_END_NAMESPACE

#ifdef ABGR32_OVERRIDE
#undef ABGR32_OVERRIDE
#undef V4L2_PIX_FMT_ABGR32
#endif

#ifdef ARGB32_OVERRIDE
#undef ARGB32_OVERRIDE
#undef V4L2_PIX_FMT_ARGB32
#endif

#ifdef RGBA32_OVERRIDE
#undef RGBA32_OVERRIDE
#undef V4L2_PIX_FMT_RGBA32
#endif

