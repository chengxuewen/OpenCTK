#include "octk_pipewire_session_p.hpp"


#include <private/octk_camera_capture_pipewire_p.hpp>
#include <private/octk_camera_capture_p.hpp>
#include <octk_platform_thread.hpp>
#include <octk_sanitizer.hpp>
#include <octk_logging.hpp>
#include "octk_pipewire_session_p.hpp"

#include <spa/param/format.h>
#include <spa/param/video/format-utils.h>
#include <spa/pod/builder.h>
#include <spa/utils/result.h>

#include <vector>

OCTK_BEGIN_NAMESPACE

struct
{
    uint32_t spa_format;
    VideoType video_type;
} constexpr kSupportedFormats[] = {
    {SPA_VIDEO_FORMAT_I420, VideoType::kI420},
    {SPA_VIDEO_FORMAT_NV12, VideoType::kNV12},
    {SPA_VIDEO_FORMAT_YUY2, VideoType::kYUY2},
    {SPA_VIDEO_FORMAT_UYVY, VideoType::kUYVY},
    // PipeWire is big-endian for the formats, while libyuv is little-endian
    // This means that BGRA == ARGB, RGBA == ABGR and similar
    // This follows mapping in libcamera PipeWire plugin:
    // https://gitlab.freedesktop.org/pipewire/pipewire/-/blob/master/spa/plugins/libcamera/libcamera-utils.cpp
    {SPA_VIDEO_FORMAT_BGRA, VideoType::kARGB},
    {SPA_VIDEO_FORMAT_RGBA, VideoType::kABGR},
    {SPA_VIDEO_FORMAT_ARGB, VideoType::kBGRA},
    {SPA_VIDEO_FORMAT_RGB, VideoType::kBGR24},
    {SPA_VIDEO_FORMAT_BGR, VideoType::kRGB24},
    {SPA_VIDEO_FORMAT_RGB16, VideoType::kRGB565},
};

static VideoRotation VideorotationFromPipeWireTransform(uint32_t transform)
{
    switch (transform)
    {
        case SPA_META_TRANSFORMATION_90: return VideoRotation::kAngle90;
        case SPA_META_TRANSFORMATION_180: return VideoRotation::kAngle180;
        case SPA_META_TRANSFORMATION_270: return VideoRotation::kAngle270;
        default: return VideoRotation::kAngle0;
    }
}

class CameraCapturePipeWirePrivate : public CameraCapturePrivate
{
public:
    struct Buffer
    {
        void *start;
        size_t length;
    };
    using Capability = CameraCapture::Capability;

    OCTK_STATIC_CONSTANT_NUMBER(kNoOfV4L2Bufffers, 4)

    explicit CameraCapturePipeWirePrivate(CameraCapturePipeWire *p);
    ~CameraCapturePipeWirePrivate() override;

    static VideoType PipeWireRawFormatToVideoType(uint32_t format);
    static uint32_t VideoTypeToPipeWireRawFormat(VideoType type);

    static void OnStreamParamChanged(void *data, uint32_t id, const struct spa_pod *format);
    static void OnStreamStateChanged(void *data,
                                     pw_stream_state old_state,
                                     pw_stream_state state,
                                     const char *error_message);

    static void OnStreamProcess(void *data);

    void OnFormatChanged(const struct spa_pod *format);
    void ProcessBuffers();

    SharedPointer<PipeWireSession> session_ OCTK_ATTRIBUTE_GUARDED_BY(mApiChecker);
    bool initialized_ OCTK_ATTRIBUTE_GUARDED_BY(mApiChecker) = false;
    bool started_ OCTK_ATTRIBUTE_GUARDED_BY(mApiMutex) = false;
    int node_id_ OCTK_ATTRIBUTE_GUARDED_BY(mCaptureChecker);
    CameraCapture::Capability configured_capability_ OCTK_ATTRIBUTE_GUARDED_BY(mCaptureChecker);

    struct pw_stream *stream_ OCTK_ATTRIBUTE_GUARDED_BY(mCaptureChecker) = nullptr;
    struct spa_hook stream_listener_ OCTK_ATTRIBUTE_GUARDED_BY(mCaptureChecker);

private:
    OCTK_DECLARE_PUBLIC(CameraCapturePipeWire)
    OCTK_DISABLE_COPY_MOVE(CameraCapturePipeWirePrivate)
};

CameraCapturePipeWirePrivate::CameraCapturePipeWirePrivate(CameraCapturePipeWire *p)
    : CameraCapturePrivate(p)
{
    session_ = utils::make_shared<PipeWireSession>();
}

CameraCapturePipeWirePrivate::~CameraCapturePipeWirePrivate()
{
}

VideoType CameraCapturePipeWirePrivate::PipeWireRawFormatToVideoType(uint32_t spa_format)
{
    for (const auto &spa_and_pixel_format : kSupportedFormats)
    {
        if (spa_and_pixel_format.spa_format == spa_format)
        {
            return spa_and_pixel_format.video_type;
        }
    }
    OCTK_WARNING() << "Unsupported pixel format: " << spa_format;
    return VideoType::kANY;
}

uint32_t CameraCapturePipeWirePrivate::VideoTypeToPipeWireRawFormat(VideoType type)
{
    for (const auto &spa_and_pixel_format : kSupportedFormats)
    {
        if (spa_and_pixel_format.video_type == type)
        {
            return spa_and_pixel_format.spa_format;
        }
    }
    OCTK_WARNING() << "Unsupported video type: " << static_cast<int>(type);
    return SPA_VIDEO_FORMAT_UNKNOWN;
}

static spa_pod *BuildFormat(spa_pod_builder *builder,
                            VideoType video_type,
                            uint32_t width,
                            uint32_t height,
                            float frame_rate)
{
    spa_pod_frame frame;

    const uint32_t media_subtype = video_type == VideoType::kMJPG ? SPA_MEDIA_SUBTYPE_mjpg : SPA_MEDIA_SUBTYPE_raw;

    spa_pod_builder_push_object(builder, &frame, SPA_TYPE_OBJECT_Format, SPA_PARAM_EnumFormat);
    spa_pod_builder_add(builder,
                        SPA_FORMAT_mediaType,
                        SPA_POD_Id(SPA_MEDIA_TYPE_video),
                        SPA_FORMAT_mediaSubtype,
                        SPA_POD_Id(media_subtype),
                        0);

    if (media_subtype == SPA_MEDIA_SUBTYPE_raw)
    {
        const uint32_t format = (uint32_t)CameraCapturePipeWirePrivate::VideoTypeToPipeWireRawFormat(video_type);
        OCTK_CHECK(format != SPA_VIDEO_FORMAT_UNKNOWN);
        spa_pod_builder_add(builder, SPA_FORMAT_VIDEO_format, SPA_POD_Id(format), 0);
    }

    spa_rectangle resolution = spa_rectangle{width, height};
    spa_pod_builder_add(builder, SPA_FORMAT_VIDEO_size, SPA_POD_Rectangle(&resolution), 0);

    // Framerate can be also set to 0 to be unspecified
    if (frame_rate)
    {
        spa_fraction framerate = spa_fraction{static_cast<uint32_t>(frame_rate), 1};
        spa_pod_builder_add(builder, SPA_FORMAT_VIDEO_framerate, SPA_POD_Fraction(&framerate), 0);
    }
    else
    {
        // Default to some reasonable values
        spa_fraction preferred_frame_rate = spa_fraction{static_cast<uint32_t>(30), 1};
        spa_fraction min_frame_rate = spa_fraction{1, 1};
        spa_fraction max_frame_rate = spa_fraction{30, 1};
        spa_pod_builder_add(builder,
                            SPA_FORMAT_VIDEO_framerate,
                            SPA_POD_CHOICE_RANGE_Fraction(&preferred_frame_rate, &min_frame_rate, &max_frame_rate),
                            0);
    }

    return static_cast<spa_pod *>(spa_pod_builder_pop(builder, &frame));
}

void CameraCapturePipeWirePrivate::OnStreamParamChanged(void *data, uint32_t id, const struct spa_pod *format)
{
    CameraCapturePipeWirePrivate *that = static_cast<CameraCapturePipeWirePrivate *>(data);
    OCTK_DCHECK(that);
    OCTK_CHECK_RUNS_SERIALIZED(&that->mCaptureChecker);

    if (format && id == SPA_PARAM_Format)
    {
        that->OnFormatChanged(format);
    }
}

void CameraCapturePipeWirePrivate::OnStreamStateChanged(void *data,
                                                        pw_stream_state old_state,
                                                        pw_stream_state state,
                                                        const char *error_message)
{
    CameraCapturePipeWirePrivate *that = static_cast<CameraCapturePipeWirePrivate *>(data);
    OCTK_DCHECK(that);

    std::lock_guard<std::mutex> lock(that->mApiMutex);
    switch (state)
    {
        case PW_STREAM_STATE_STREAMING: that->started_ = true; break;
        case PW_STREAM_STATE_ERROR: OCTK_ERROR() << "PipeWire stream state error: " << error_message; [[fallthrough]];
        case PW_STREAM_STATE_PAUSED:
        case PW_STREAM_STATE_UNCONNECTED:
        case PW_STREAM_STATE_CONNECTING: that->started_ = false; break;
    }
    OCTK_TRACE() << "PipeWire stream state change: " << pw_stream_state_as_string(old_state) << " -> "
                 << pw_stream_state_as_string(state);
}

void CameraCapturePipeWirePrivate::OnStreamProcess(void *data)
{
    CameraCapturePipeWirePrivate *that = static_cast<CameraCapturePipeWirePrivate *>(data);
    OCTK_DCHECK(that);
    OCTK_CHECK_RUNS_SERIALIZED(&that->mCaptureChecker);
    that->ProcessBuffers();
}

OCTK_NO_SANITIZE("cfi-icall")
void CameraCapturePipeWirePrivate::OnFormatChanged(const struct spa_pod *format)
{
    OCTK_CHECK_RUNS_SERIALIZED(&mCaptureChecker);

    uint32_t media_type, media_subtype;

    if (spa_format_parse(format, &media_type, &media_subtype) < 0)
    {
        OCTK_ERROR() << "Failed to parse video format.";
        return;
    }

    switch (media_subtype)
    {
        case SPA_MEDIA_SUBTYPE_raw:
        {
            struct spa_video_info_raw f;
            spa_format_video_raw_parse(format, &f);
            configured_capability_.width = f.size.width;
            configured_capability_.height = f.size.height;
            configured_capability_.videoType = PipeWireRawFormatToVideoType(f.format);
            configured_capability_.maxFPS = f.framerate.num / f.framerate.denom;
            break;
        }
        case SPA_MEDIA_SUBTYPE_mjpg:
        {
            struct spa_video_info_mjpg f;
            spa_format_video_mjpg_parse(format, &f);
            configured_capability_.width = f.size.width;
            configured_capability_.height = f.size.height;
            configured_capability_.videoType = VideoType::kMJPG;
            configured_capability_.maxFPS = f.framerate.num / f.framerate.denom;
            break;
        }
        default: configured_capability_.videoType = VideoType::kANY;
    }

    if (configured_capability_.videoType == VideoType::kANY)
    {
        OCTK_ERROR() << "Unsupported video format.";
        return;
    }

    OCTK_TRACE() << "Configured capture format = " << static_cast<int>(configured_capability_.videoType);

    uint8_t buffer[1024] = {};
    auto builder = spa_pod_builder{buffer, sizeof(buffer)};

    // Setup buffers and meta header for new format.
    std::vector<const spa_pod *> params;
    spa_pod_frame frame;
    spa_pod_builder_push_object(&builder, &frame, SPA_TYPE_OBJECT_ParamBuffers, SPA_PARAM_Buffers);

    if (media_subtype == SPA_MEDIA_SUBTYPE_raw)
    {
        // Enforce stride without padding.
        size_t stride;
        switch (configured_capability_.videoType)
        {
            case VideoType::kI420:
            case VideoType::kNV12: stride = configured_capability_.width; break;
            case VideoType::kYUY2:
            case VideoType::kUYVY:
            case VideoType::kRGB565: stride = configured_capability_.width * 2; break;
            case VideoType::kRGB24:
            case VideoType::kBGR24: stride = configured_capability_.width * 3; break;
            case VideoType::kARGB:
            case VideoType::kABGR:
            case VideoType::kBGRA: stride = configured_capability_.width * 4; break;
            default: OCTK_ERROR() << "Unsupported video format."; return;
        }
        spa_pod_builder_add(&builder, SPA_PARAM_BUFFERS_stride, SPA_POD_Int(stride), 0);
    }

    const int buffer_types = (1 << SPA_DATA_DmaBuf) | (1 << SPA_DATA_MemFd) | (1 << SPA_DATA_MemPtr);
    spa_pod_builder_add(&builder,
                        SPA_PARAM_BUFFERS_buffers,
                        SPA_POD_CHOICE_RANGE_Int(8, 1, 32),
                        SPA_PARAM_BUFFERS_dataType,
                        SPA_POD_CHOICE_FLAGS_Int(buffer_types),
                        0);
    params.push_back(static_cast<spa_pod *>(spa_pod_builder_pop(&builder, &frame)));

    params.push_back(
        reinterpret_cast<spa_pod *>(spa_pod_builder_add_object(&builder,
                                                               SPA_TYPE_OBJECT_ParamMeta,
                                                               SPA_PARAM_Meta,
                                                               SPA_PARAM_META_type,
                                                               SPA_POD_Id(SPA_META_Header),
                                                               SPA_PARAM_META_size,
                                                               SPA_POD_Int(sizeof(struct spa_meta_header)))));
    params.push_back(
        reinterpret_cast<spa_pod *>(spa_pod_builder_add_object(&builder,
                                                               SPA_TYPE_OBJECT_ParamMeta,
                                                               SPA_PARAM_Meta,
                                                               SPA_PARAM_META_type,
                                                               SPA_POD_Id(SPA_META_VideoTransform),
                                                               SPA_PARAM_META_size,
                                                               SPA_POD_Int(sizeof(struct spa_meta_videotransform)))));
    pw_stream_update_params(stream_, params.data(), params.size());
}

OCTK_NO_SANITIZE("cfi-icall")
void CameraCapturePipeWirePrivate::ProcessBuffers()
{
    OCTK_P(CameraCapturePipeWire);
    OCTK_CHECK_RUNS_SERIALIZED(&mCaptureChecker);

    while (pw_buffer *buffer = pw_stream_dequeue_buffer(stream_))
    {
        spa_buffer *spaBuffer = buffer->buffer;
        struct spa_meta_header *h;
        h = static_cast<struct spa_meta_header *>(spa_buffer_find_meta_data(spaBuffer, SPA_META_Header, sizeof(*h)));

        struct spa_meta_videotransform *videotransform;
        videotransform = static_cast<struct spa_meta_videotransform *>(
            spa_buffer_find_meta_data(spaBuffer, SPA_META_VideoTransform, sizeof(*videotransform)));
        if (videotransform)
        {
            VideoRotation rotation = VideorotationFromPipeWireTransform(videotransform->transform);
            p->setCaptureRotation(rotation);
            p->setApplyRotation(rotation != VideoRotation::kAngle0);
        }

        if (h->flags & SPA_META_HEADER_FLAG_CORRUPTED)
        {
            OCTK_INFO() << "Dropping corruped frame.";
            pw_stream_queue_buffer(stream_, buffer);
            continue;
        }

        if (spaBuffer->datas[0].type == SPA_DATA_DmaBuf || spaBuffer->datas[0].type == SPA_DATA_MemFd)
        {
            ScopedBuf frame;
            frame.initialize(static_cast<uint8_t *>(mmap(nullptr,
                                                         spaBuffer->datas[0].maxsize,
                                                         PROT_READ,
                                                         MAP_SHARED,
                                                         spaBuffer->datas[0].fd,
                                                         spaBuffer->datas[0].mapoffset)),
                             spaBuffer->datas[0].maxsize,
                             spaBuffer->datas[0].fd,
                             spaBuffer->datas[0].type == SPA_DATA_DmaBuf);

            if (!frame)
            {
                OCTK_ERROR() << "Failed to mmap the memory: " << std::strerror(errno);
                return;
            }

            this->incomingFrame(SPA_MEMBER(frame.get(), spaBuffer->datas[0].mapoffset, uint8_t),
                                spaBuffer->datas[0].chunk->size,
                                configured_capability_);
        }
        else
        { // SPA_DATA_MemPtr
            this->incomingFrame(static_cast<uint8_t *>(spaBuffer->datas[0].data),
                                spaBuffer->datas[0].chunk->size,
                                configured_capability_);
        }

        pw_stream_queue_buffer(stream_, buffer);
    }
}

CameraCapturePipeWire::CameraCapturePipeWire()
    : CameraCapture(new CameraCapturePipeWirePrivate(this))
{
}

CameraCapturePipeWire::~CameraCapturePipeWire()
{
    OCTK_D(CameraCapturePipeWire);
    OCTK_DCHECK_RUN_ON(&d->mApiChecker);
    this->stopCapture();
}

OCTK_NO_SANITIZE("cfi-icall")
int32_t CameraCapturePipeWire::startCapture(const Capability &capability)
{
    OCTK_D(CameraCapturePipeWire);
    OCTK_DCHECK_RUN_ON(&d->mApiChecker);

    if (d->initialized_)
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

    uint8_t buffer[1024] = {};

    // We don't want members above to be guarded by capture_checker_ as
    // it's meant to be for members that are accessed on the API thread
    // only when we are not capturing. The code above can be called many
    // times while sharing instance of VideoCapturePipeWire between
    // websites and therefore it would not follow the requirements of this
    // checker.
    OCTK_CHECK_RUNS_SERIALIZED(&d->mCaptureChecker);
    PipeWireThreadLoopLock thread_loop_lock(d->session_->pw_main_loop_);

    OCTK_TRACE() << "Creating new PipeWire stream for node " << d->node_id_;

    pw_properties *reuse_props = pw_properties_new_string("pipewire.client.reuse=1");
    d->stream_ = pw_stream_new(d->session_->pw_core_, "camera-stream", reuse_props);

    if (!d->stream_)
    {
        OCTK_ERROR() << "Failed to create camera stream!";
        return -1;
    }

    static const pw_stream_events stream_events{
        .version = PW_VERSION_STREAM_EVENTS,
        .state_changed = &CameraCapturePipeWirePrivate::OnStreamStateChanged,
        .param_changed = &CameraCapturePipeWirePrivate::OnStreamParamChanged,
        .process = &CameraCapturePipeWirePrivate::OnStreamProcess,
    };

    pw_stream_add_listener(d->stream_, &d->stream_listener_, &stream_events, d);

    spa_pod_builder builder = spa_pod_builder{buffer, sizeof(buffer)};
    std::vector<const spa_pod *> params;
    uint32_t width = capability.width;
    uint32_t height = capability.height;
    uint32_t frame_rate = capability.maxFPS;
    VideoType video_type = capability.videoType;

    params.push_back(BuildFormat(&builder, video_type, width, height, frame_rate));

    int res = pw_stream_connect(
        d->stream_,
        PW_DIRECTION_INPUT,
        d->node_id_,
        static_cast<enum pw_stream_flags>(PW_STREAM_FLAG_AUTOCONNECT | PW_STREAM_FLAG_DONT_RECONNECT),
        params.data(),
        params.size());
    if (res != 0)
    {
        OCTK_ERROR() << "Could not connect to camera stream: " << spa_strerror(res);
        return -1;
    }

    d->mRequestedCapability = capability;
    d->initialized_ = true;
    return 0;
}

int32_t CameraCapturePipeWire::stopCapture()
{
    OCTK_D(CameraCapturePipeWire);
    OCTK_DCHECK_RUN_ON(&d->mApiChecker);

    PipeWireThreadLoopLock thread_loop_lock(d->session_->pw_main_loop_);
    // PipeWireSession is guarded by API checker so just make sure we do
    // race detection when the PipeWire loop is locked/stopped to not run
    // any callback at this point.
    OCTK_CHECK_RUNS_SERIALIZED(&d->mCaptureChecker);
    if (d->stream_)
    {
        pw_stream_destroy(d->stream_);
        d->stream_ = nullptr;
    }

    d->mRequestedCapability = Capability();
    return 0;
}

bool CameraCapturePipeWire::isCaptureStarted()
{
    OCTK_D(CameraCapturePipeWire);
    OCTK_DCHECK_RUN_ON(&d->mApiChecker);
    std::lock_guard<std::mutex> lock(d->mApiMutex);

    return d->started_;
}

int32_t CameraCapturePipeWire::captureSettings(Capability &settings)
{
    OCTK_D(CameraCapturePipeWire);
    OCTK_DCHECK_RUN_ON(&d->mApiChecker);

    settings = d->mRequestedCapability;
    return 0;
}

bool CameraCapturePipeWire::init(const char *deviceUniqueIdUTF8)
{
    OCTK_D(CameraCapturePipeWire);
    OCTK_CHECK_RUNS_SERIALIZED(&d->mCaptureChecker);
    OCTK_DCHECK_RUN_ON(&d->mApiChecker);

    std::promise<PipeWireSession::Status> promise;
    d->session_->Init([&promise](PipeWireSession::Status status) { promise.set_value(status); });
    const auto status = promise.get_future().get();
    OCTK_DEBUG("CameraCapturePipeWire::init() session_ status:{}", fmt::as_int(status));

    auto node = std::find_if(d->session_->nodes_.begin(),
                             d->session_->nodes_.end(),
                             [deviceUniqueIdUTF8](const PipeWireNode::PipeWireNodePtr &node)
                             { return node->unique_id() == deviceUniqueIdUTF8; });
    if (node == d->session_->nodes_.end())
    {
        return false;
    }
    d->node_id_ = (*node)->id();

    const int len = strlen(deviceUniqueIdUTF8);
    d->mDeviceUniqueId = new (std::nothrow) char[len + 1];
    memcpy(d->mDeviceUniqueId, deviceUniqueIdUTF8, len + 1);
    return true;
}

OCTK_END_NAMESPACE