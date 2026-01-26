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

#ifndef _OCTK_VIDEO_FRAME_TYPE_HPP
#define _OCTK_VIDEO_FRAME_TYPE_HPP

#include <octk_media_global.hpp>
#include <octk_string_view.hpp>
#include <octk_checks.hpp>

OCTK_BEGIN_NAMESPACE

enum class VideoFrameType : int
{
    kEmpty = 0,
    // Wire format for MultiplexEncodedImagePacker seems to depend on numerical values of these constants.
    kDelta = 3,
    kKey = 4
};

namespace utils
{
inline OCTK_CXX14_CONSTEXPR StringView videoFrameTypeToString(VideoFrameType type)
{
    switch (type)
    {
        case VideoFrameType::kEmpty: return "empty";
        case VideoFrameType::kDelta: return "delta";
        case VideoFrameType::kKey: return "key";
    }
    OCTK_CHECK_NOTREACHED();
    return "";
}
} // namespace utils

OCTK_END_NAMESPACE

#endif // _OCTK_VIDEO_FRAME_TYPE_HPP
