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

#ifndef _OCTK_FULL_SCREEN_APPLICATION_HANDLER_HPP
#define _OCTK_FULL_SCREEN_APPLICATION_HANDLER_HPP

#include <octk_desktop_capturer.hpp>

#include <memory>

OCTK_BEGIN_NAMESPACE

// Base class for application specific handler to check criteria for switch to
// full-screen mode and find if possible the full-screen window to share.
// Supposed to be created and owned by platform specific
// FullScreenWindowDetector.
class FullScreenApplicationHandler
{
public:
    virtual ~FullScreenApplicationHandler() {}

    FullScreenApplicationHandler(const FullScreenApplicationHandler &) = delete;
    FullScreenApplicationHandler &operator=(const FullScreenApplicationHandler &) = delete;

    explicit FullScreenApplicationHandler(DesktopCapturer::SourceId sourceId);

    // Returns the full-screen window in place of the original window if all the
    // criteria are met, or 0 if no such window found.
    virtual DesktopCapturer::SourceId FindFullScreenWindow(const DesktopCapturer::SourceList &window_list,
                                                           int64_t timestamp) const;

    // Returns source id of original window associated with
    // FullScreenApplicationHandler
    DesktopCapturer::SourceId GetSourceId() const;

private:
    const DesktopCapturer::SourceId source_id_;
};
OCTK_END_NAMESPACE

#endif  // _OCTK_FULL_SCREEN_APPLICATION_HANDLER_HPP
