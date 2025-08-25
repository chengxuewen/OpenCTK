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

#ifndef _OCTK_VIDEO_RESOLUTION_HPP
#define _OCTK_VIDEO_RESOLUTION_HPP

#include <octk_media_global.hpp>

#include <utility>

OCTK_BEGIN_NAMESPACE

// A struct representing a video resolution in pixels.
struct OCTK_MEDIA_API Resolution final
{
    int width = 0;
    int height = 0;

    Resolution() = default;
    Resolution(int w, int h) : width(w), height(h) {}

    // Helper methods.
    int PixelCount() const { return width * height; }
    std::pair<int, int> ToPair() const { return std::make_pair(width, height); }
};

inline bool operator==(const Resolution &lhs, const Resolution &rhs)
{
    return lhs.width == rhs.width && lhs.height == rhs.height;
}

inline bool operator!=(const Resolution &lhs, const Resolution &rhs)
{
    return !(lhs == rhs);
}
OCTK_END_NAMESPACE

#endif  // _OCTK_VIDEO_RESOLUTION_HPP
