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

#ifndef _OCTK_MAC_DESKTOP_FRAME_PROVIDER_HPP
#define _OCTK_MAC_DESKTOP_FRAME_PROVIDER_HPP

#include <objc/octk_scoped_cftype_ref.hpp>
#include <octk_shared_desktop_frame.hpp>
#include <octk_sequence_checker.hpp>

#include <CoreGraphics/CoreGraphics.h>
#include <IOSurface/IOSurface.h>

#include <map>
#include <memory>

OCTK_BEGIN_NAMESPACE

class DesktopFrameProvider
{
public:
    explicit DesktopFrameProvider(bool allow_iosurface);
    ~DesktopFrameProvider();

    DesktopFrameProvider(const DesktopFrameProvider &) = delete;
    DesktopFrameProvider &operator=(const DesktopFrameProvider &) = delete;

    // The caller takes ownership of the returned desktop frame. Otherwise
    // returns null if `display_id` is invalid or not ready. Note that this
    // function does not remove the frame from the internal container. Caller
    // has to call the Release function.
    std::unique_ptr<DesktopFrame> TakeLatestFrameForDisplay(
        CGDirectDisplayID display_id);

    // OS sends the latest IOSurfaceRef through
    // CGDisplayStreamFrameAvailableHandler callback; we store it here.
    void InvalidateIOSurface(CGDirectDisplayID display_id,
                             ScopedCFTypeRef<IOSurfaceRef> io_surface);

    // Expected to be called before stopping the CGDisplayStreamRef streams.
    void Release();

    bool allow_iosurface() const { return allow_iosurface_; }

private:
    SequenceChecker thread_checker_;
    const bool allow_iosurface_;

    // Most recent IOSurface that contains a capture of matching display.
    std::map<CGDirectDisplayID, std::unique_ptr<SharedDesktopFrame>> io_surfaces_;
};
OCTK_END_NAMESPACE

#endif  // _OCTK_MAC_DESKTOP_FRAME_PROVIDER_HPP
