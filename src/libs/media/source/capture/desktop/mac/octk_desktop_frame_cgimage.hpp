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

#ifndef _OCTK_MAC_DESKTOP_FRAME_CGIMAGE_HPP
#define _OCTK_MAC_DESKTOP_FRAME_CGIMAGE_HPP

#include <octk_desktop_frame.hpp>
#include <objc/octk_scoped_cftype_ref.hpp>

#include <CoreGraphics/CoreGraphics.h>

#include <memory>

OCTK_BEGIN_NAMESPACE

class OCTK_MEDIA_API DesktopFrameCGImage final : public DesktopFrame
{
public:
    // Create an image containing a snapshot of the display at the time this is
    // being called.
    static std::unique_ptr<DesktopFrameCGImage> CreateForDisplay(CGDirectDisplayID display_id);

    // Create an image containing a snaphot of the given window at the time this
    // is being called. This also works when the window is overlapped or in
    // another workspace.
    static std::unique_ptr<DesktopFrameCGImage> CreateForWindow(CGWindowID window_id);

    static std::unique_ptr<DesktopFrameCGImage> CreateFromCGImage(ScopedCFTypeRef<CGImageRef> cg_image);

    ~DesktopFrameCGImage() override;

    DesktopFrameCGImage(const DesktopFrameCGImage &) = delete;
    DesktopFrameCGImage &operator=(const DesktopFrameCGImage &) = delete;

private:
    // This constructor expects `cg_image` to hold a non-null CGImageRef.
    DesktopFrameCGImage(DesktopSize size,
                        int stride,
                        uint8_t *data,
                        ScopedCFTypeRef<CGImageRef> cg_image,
                        ScopedCFTypeRef<CFDataRef> cg_data);

    const ScopedCFTypeRef<CGImageRef> cg_image_;
    const ScopedCFTypeRef<CFDataRef> cg_data_;
};
OCTK_END_NAMESPACE

#endif  // _OCTK_MAC_DESKTOP_FRAME_CGIMAGE_HPP
