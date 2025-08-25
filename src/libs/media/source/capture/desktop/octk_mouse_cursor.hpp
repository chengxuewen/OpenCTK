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

#ifndef _OCTK_DESKTOP_CAPTURE_MOUSE_CURSOR_HPP
#define _OCTK_DESKTOP_CAPTURE_MOUSE_CURSOR_HPP

#include <octk_desktop_frame.hpp>
#include <octk_desktop_geometry.hpp>

#include <memory>

OCTK_BEGIN_NAMESPACE

class OCTK_MEDIA_API MouseCursor
{
public:
    MouseCursor();

    // Takes ownership of `image`. `hotspot` must be within `image` boundaries.
    MouseCursor(DesktopFrame *image, const DesktopVector &hotspot);

    ~MouseCursor();

    MouseCursor(const MouseCursor &) = delete;
    MouseCursor &operator=(const MouseCursor &) = delete;

    static MouseCursor *CopyOf(const MouseCursor &cursor);

    void set_image(DesktopFrame *image) { image_.reset(image); }
    const DesktopFrame *image() const { return image_.get(); }

    void set_hotspot(const DesktopVector &hotspot) { hotspot_ = hotspot; }
    const DesktopVector &hotspot() const { return hotspot_; }

private:
    std::unique_ptr<DesktopFrame> image_;
    DesktopVector hotspot_;
};
OCTK_END_NAMESPACE

#endif  // _OCTK_DESKTOP_CAPTURE_MOUSE_CURSOR_HPP
