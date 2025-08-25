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

#include <mac/octk_desktop_frame_iosurface.hpp>
#include <octk_logging.hpp>
#include <octk_checks.hpp>

OCTK_BEGIN_NAMESPACE

// static
std::unique_ptr<DesktopFrameIOSurface> DesktopFrameIOSurface::Wrap(ScopedCFTypeRef <IOSurfaceRef> io_surface)
{
    if (!io_surface)
    {
        return nullptr;
    }

    IOSurfaceIncrementUseCount(io_surface.get());
    IOReturn status = IOSurfaceLock(io_surface.get(), kIOSurfaceLockReadOnly, nullptr);
    if (status != kIOReturnSuccess)
    {
        OCTK_ERROR() << "Failed to lock the IOSurface with status " << status;
        IOSurfaceDecrementUseCount(io_surface.get());
        return nullptr;
    }

    // Verify that the image has 32-bit depth.
    int bytes_per_pixel = IOSurfaceGetBytesPerElement(io_surface.get());
    if (bytes_per_pixel != DesktopFrame::kBytesPerPixel)
    {
        OCTK_ERROR() << "CGDisplayStream handler returned IOSurface with " << (8 * bytes_per_pixel)
                           << " bits per pixel. Only 32-bit depth is supported.";
        IOSurfaceUnlock(io_surface.get(), kIOSurfaceLockReadOnly, nullptr);
        IOSurfaceDecrementUseCount(io_surface.get());
        return nullptr;
    }

    return std::unique_ptr<DesktopFrameIOSurface>(new DesktopFrameIOSurface(io_surface));
}

DesktopFrameIOSurface::DesktopFrameIOSurface(ScopedCFTypeRef <IOSurfaceRef> io_surface)
    : DesktopFrame(
    DesktopSize(IOSurfaceGetWidth(io_surface.get()), IOSurfaceGetHeight(io_surface.get())),
    IOSurfaceGetBytesPerRow(io_surface.get()),
    static_cast<uint8_t *>(IOSurfaceGetBaseAddress(io_surface.get())),
    nullptr), io_surface_(io_surface)
{
    OCTK_DCHECK(io_surface_);
}

DesktopFrameIOSurface::~DesktopFrameIOSurface()
{
    IOSurfaceUnlock(io_surface_.get(), kIOSurfaceLockReadOnly, nullptr);
    IOSurfaceDecrementUseCount(io_surface_.get());
}
OCTK_END_NAMESPACE
