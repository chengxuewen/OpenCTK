//
// Created by cxw on 25-8-25.
//

#ifndef _OCTK_VIDEO_CAPTURE_CAPABILITY_HPP
#define _OCTK_VIDEO_CAPTURE_CAPABILITY_HPP

#include <octk_video_frame.hpp>
#include <octk_video_type.hpp>

OCTK_BEGIN_NAMESPACE

namespace constants
{
OCTK_STATIC_CONSTANT_NUMBER(kVideoCaptureUniqueNameLength, 1024) // Max unique capture device name lenght
OCTK_STATIC_CONSTANT_NUMBER(kVideoCaptureDeviceNameLength, 256)  // Max capture device name lenght
OCTK_STATIC_CONSTANT_NUMBER(kVideoCaptureProductIdLength, 128)   // Max product id length
} // namespace constants

struct VideoCaptureCapability
{
    int32_t width;
    int32_t height;
    int32_t maxFPS;
    bool interlaced;
    VideoType videoType;

    VideoCaptureCapability()
    {
        width = 0;
        height = 0;
        maxFPS = 0;
        interlaced = false;
        videoType = VideoType::kUnknown;
    }
    bool operator!=(const VideoCaptureCapability &other) const
    {
        if (width != other.width)
        {
            return true;
        }
        if (height != other.height)
        {
            return true;
        }
        if (maxFPS != other.maxFPS)
        {
            return true;
        }
        if (videoType != other.videoType)
        {
            return true;
        }
        if (interlaced != other.interlaced)
        {
            return true;
        }
        return false;
    }
    bool operator==(const VideoCaptureCapability &other) const { return !operator!=(other); }
};

OCTK_END_NAMESPACE

#endif // _OCTK_VIDEO_CAPTURE_CAPABILITY_HPP
