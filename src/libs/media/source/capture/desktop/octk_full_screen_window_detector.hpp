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

#ifndef _OCTK_FULL_SCREEN_WINDOW_DETECTOR_HPP
#define _OCTK_FULL_SCREEN_WINDOW_DETECTOR_HPP

#include <octk_function_view.hpp>
#include <octk_desktop_capturer.hpp>
#include <octk_full_screen_application_handler.hpp>

#include <memory>

OCTK_BEGIN_NAMESPACE

// This is a way to handle switch to full-screen mode for application in some
// specific cases:
// - Chrome on MacOS creates a new window in full-screen mode to
//   show a tab full-screen and minimizes the old window.
// - PowerPoint creates new windows in full-screen mode when user goes to
//   presentation mode (Slide Show Window, Presentation Window).
//
// To continue capturing in these cases, we try to find the new full-screen
// window using criteria provided by application specific
// FullScreenApplicationHandler.

class FullScreenWindowDetector
{
public:
    using ApplicationHandlerFactory =
        std::function<std::unique_ptr<FullScreenApplicationHandler>(DesktopCapturer::SourceId sourceId)>;

    FullScreenWindowDetector(ApplicationHandlerFactory application_handler_factory);
    FullScreenWindowDetector(const FullScreenWindowDetector &) = delete;
    FullScreenWindowDetector &operator=(const FullScreenWindowDetector &) = delete;
    virtual ~FullScreenWindowDetector() {}

    // Returns the full-screen window in place of the original window if all the
    // criteria provided by FullScreenApplicationHandler are met, or 0 if no such
    // window found.
    DesktopCapturer::SourceId FindFullScreenWindow(DesktopCapturer::SourceId original_source_id);

    // The caller should call this function periodically, implementation will
    // update internal state no often than twice per second
    void UpdateWindowListIfNeeded(DesktopCapturer::SourceId original_source_id,
                                  FunctionView<bool(DesktopCapturer::SourceList *)> get_sources);

    static std::unique_ptr<FullScreenWindowDetector> CreateFullScreenWindowDetector();

protected:
    std::unique_ptr<FullScreenApplicationHandler> app_handler_;

private:
    void CreateApplicationHandlerIfNeeded(DesktopCapturer::SourceId source_id);

    ApplicationHandlerFactory application_handler_factory_;

    int64_t last_update_time_ms_;
    DesktopCapturer::SourceId previous_source_id_;

    // Save the source id when we fail to create an instance of
    // CreateApplicationHandlerIfNeeded to avoid redundant attempt to do it again.
    DesktopCapturer::SourceId no_handler_source_id_;

    DesktopCapturer::SourceList window_list_;
};
OCTK_END_NAMESPACE

#endif  // _OCTK_FULL_SCREEN_WINDOW_DETECTOR_HPP
