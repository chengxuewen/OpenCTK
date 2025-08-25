/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present chengxuewen.
** Copyright (c) 2018 The WebRTC project authors.
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

#include <mac/octk_desktop_frame_provider.hpp>
#include <mac/octk_desktop_frame_cgimage.hpp>
#include <mac/octk_desktop_frame_iosurface.hpp>

#include <utility>

OCTK_BEGIN_NAMESPACE

DesktopFrameProvider::DesktopFrameProvider(bool allow_iosurface)
    : allow_iosurface_(allow_iosurface)
{
    thread_checker_.Detach();
}

DesktopFrameProvider::~DesktopFrameProvider()
{
    OCTK_DCHECK(thread_checker_.IsCurrent());

    Release();
}

std::unique_ptr<DesktopFrame> DesktopFrameProvider::TakeLatestFrameForDisplay(
    CGDirectDisplayID display_id)
{
    OCTK_DCHECK(thread_checker_.IsCurrent());

    if (!allow_iosurface_ || !io_surfaces_[display_id])
    {
        // Regenerate a snapshot. If iosurface is on it will be empty until the
        // stream handler is called.
        return DesktopFrameCGImage::CreateForDisplay(display_id);
    }

    return io_surfaces_[display_id]->Share();
}

void DesktopFrameProvider::InvalidateIOSurface(CGDirectDisplayID display_id,
                                               ScopedCFTypeRef <IOSurfaceRef> io_surface)
{
    OCTK_DCHECK(thread_checker_.IsCurrent());

    if (!allow_iosurface_)
    {
        return;
    }

    std::unique_ptr<DesktopFrameIOSurface> desktop_frame_iosurface =
        DesktopFrameIOSurface::Wrap(io_surface);

    io_surfaces_[display_id] = desktop_frame_iosurface ?
                               SharedDesktopFrame::Wrap(std::move(desktop_frame_iosurface)) :
                               nullptr;
}

void DesktopFrameProvider::Release()
{
    OCTK_DCHECK(thread_checker_.IsCurrent());

    if (!allow_iosurface_)
    {
        return;
    }

    io_surfaces_.clear();
}
OCTK_END_NAMESPACE
