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

#include <octk_mouse_cursor.hpp>
#include <octk_desktop_frame.hpp>
#include <octk_checks.hpp>

OCTK_BEGIN_NAMESPACE

MouseCursor::MouseCursor() {}

MouseCursor::MouseCursor(DesktopFrame *image, const DesktopVector &hotspot)
    : image_(image), hotspot_(hotspot)
{
    OCTK_DCHECK(0 <= hotspot_.x() && hotspot_.x() <= image_->size().width());
    OCTK_DCHECK(0 <= hotspot_.y() && hotspot_.y() <= image_->size().height());
}

MouseCursor::~MouseCursor() {}

// static
MouseCursor *MouseCursor::CopyOf(const MouseCursor &cursor)
{
    return cursor.image() ? new MouseCursor(BasicDesktopFrame::CopyOf(*cursor.image()), cursor.hotspot())
                          : new MouseCursor();
}
OCTK_END_NAMESPACE
