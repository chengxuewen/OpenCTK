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

#ifndef _OCTK_MAC_DESKTOP_FRAME_IOSURFACE_HPP
#define _OCTK_MAC_DESKTOP_FRAME_IOSURFACE_HPP

#include <octk_desktop_frame.hpp>
#include <objc/octk_scoped_cftype_ref.hpp>

#include <CoreGraphics/CoreGraphics.h>
#include <IOSurface/IOSurface.h>

#include <memory>

OCTK_BEGIN_NAMESPACE

class DesktopFrameIOSurface final : public DesktopFrame
{
public:
    // Lock an IOSurfaceRef containing a snapshot of a display. Return NULL if
    // failed to lock.
    static std::unique_ptr<DesktopFrameIOSurface> Wrap(ScopedCFTypeRef <IOSurfaceRef> io_surface);

    ~DesktopFrameIOSurface() override;

    DesktopFrameIOSurface(const DesktopFrameIOSurface &) = delete;
    DesktopFrameIOSurface &operator=(const DesktopFrameIOSurface &) = delete;

private:
    // This constructor expects `io_surface` to hold a non-null IOSurfaceRef.
    explicit DesktopFrameIOSurface(ScopedCFTypeRef <IOSurfaceRef> io_surface);

    const ScopedCFTypeRef <IOSurfaceRef> io_surface_;
};
OCTK_END_NAMESPACE

#endif  // _OCTK_MAC_DESKTOP_FRAME_IOSURFACE_HPP
