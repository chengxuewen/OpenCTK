#include <octk_video_type.hpp>
#include <octk_checks.hpp>

OCTK_BEGIN_NAMESPACE

namespace utils
{
size_t videoTypeBufferSize(VideoType type, int width, int height)
{
    OCTK_DCHECK_GE(width, 0);
    OCTK_DCHECK_GE(height, 0);
    switch (type)
    {
        case VideoType::kRGB565:
        {
            return width * height * 2;
        }
        case VideoType::kRGB24:
        case VideoType::kBGR24:
        {
            return width * height * 3;
        }
        case VideoType::kARGB:
        case VideoType::kBGRA:
        case VideoType::kABGR:
        case VideoType::kRGBA:
        {
            return width * height * 4;
        }
        case VideoType::kI420:
        case VideoType::kNV21:
        case VideoType::kNV12:
        case VideoType::kYV12:
        {
            int half_width = (width + 1) >> 1;
            int half_height = (height + 1) >> 1;
            return width * height + half_width * half_height * 2;
        }
        case VideoType::kI422:
        case VideoType::kYUY2:
        case VideoType::kUYVY:
        {
            return width * height * 2;
        }
        case VideoType::kI444:
        case VideoType::kI010:
        {
            return width * height * 3;
        }
        case VideoType::kI210:
        {
            return width * height * 4;
        }
        case VideoType::kI400:
        {
            return width * height;
        }
        case VideoType::kMJPG:
        case VideoType::kANY:
        case VideoType::kRAW: break;
    }
    OCTK_DCHECK_NOTREACHED() << "Unexpected pixel format " << (int)type;
    return 0;
}

const char *videoTypeName(VideoType type)
{
    switch (type)
    {
        case VideoType::kRGB565: return "RGB565";
        case VideoType::kRGB24: return "RGB24";
        case VideoType::kBGR24: return "BGR24";
        case VideoType::kARGB: return "ARGB";
        case VideoType::kBGRA: return "BGRA";
        case VideoType::kABGR: return "ABGR";
        case VideoType::kRGBA: return "RGBA";
        case VideoType::kI420: return "I420";
        case VideoType::kNV21: return "NV21";
        case VideoType::kNV12: return "NV12";
        case VideoType::kYV12: return "YV12";
        case VideoType::kI422: return "I422";
        case VideoType::kYUY2: return "YUY2";
        case VideoType::kUYVY: return "UYVY";
        case VideoType::kI444: return "I444";
        case VideoType::kI010: return "I010";
        case VideoType::kI210: return "I210";
        case VideoType::kI400: return "400";
        case VideoType::kMJPG: return "MJPG";
        case VideoType::kRAW: return "RAW";
        case VideoType::kANY: break;
    }
    return "UNKNOWN";
}
} // namespace utils

OCTK_END_NAMESPACE
