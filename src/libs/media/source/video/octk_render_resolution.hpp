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

#ifndef _OCTK_VIDEO_RENDER_RESOLUTION_HPP
#define _OCTK_VIDEO_RENDER_RESOLUTION_HPP

#include <octk_media_global.hpp>

OCTK_BEGIN_NAMESPACE

// TODO(bugs.webrtc.org/12114) : remove in favor of Resolution.
class RenderResolution
{
public:
    constexpr RenderResolution() = default;
    constexpr RenderResolution(int width, int height) : width_(width), height_(height) {}
    RenderResolution(const RenderResolution &) = default;
    RenderResolution &operator=(const RenderResolution &) = default;

    friend bool operator==(const RenderResolution &lhs, const RenderResolution &rhs)
    {
        return lhs.width_ == rhs.width_ && lhs.height_ == rhs.height_;
    }
    friend bool operator!=(const RenderResolution &lhs, const RenderResolution &rhs)
    {
        return !(lhs == rhs);
    }

    constexpr bool Valid() const { return width_ > 0 && height_ > 0; }

    constexpr int Width() const { return width_; }
    constexpr int Height() const { return height_; }

private:
    int width_ = 0;
    int height_ = 0;
};

OCTK_END_NAMESPACE

#endif  // _OCTK_VIDEO_RENDER_RESOLUTION_HPP
