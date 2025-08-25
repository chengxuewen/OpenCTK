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

#include <octk_desktop_capture_options.hpp>

#if defined(OCTK_OS_MAC) && !defined(OCTK_OS_IOS)
#   include <mac/octk_full_screen_mac_application_handler.hpp>
#elif defined(OCTK_OS_WIN)
#   include "modules/desktop_capture/win/full_screen_win_application_handler.h"
#endif
#if defined(OCTK_USE_PIPEWIRE)
#   include "modules/desktop_capture/linux/wayland/shared_screencast_stream.h"
#endif

OCTK_BEGIN_NAMESPACE

DesktopCaptureOptions::DesktopCaptureOptions() {}
DesktopCaptureOptions::DesktopCaptureOptions(const DesktopCaptureOptions &options) = default;
DesktopCaptureOptions::DesktopCaptureOptions(DesktopCaptureOptions &&options) = default;
DesktopCaptureOptions::~DesktopCaptureOptions() {}

DesktopCaptureOptions &DesktopCaptureOptions::operator=(const DesktopCaptureOptions &options) = default;
DesktopCaptureOptions &DesktopCaptureOptions::operator=(DesktopCaptureOptions &&options) = default;

// static
DesktopCaptureOptions DesktopCaptureOptions::CreateDefault()
{
    DesktopCaptureOptions result;
#if defined(OCTK_USE_X11)
    result.set_x_display(SharedXDisplay::CreateDefault());
#endif
#if defined(OCTK_USE_PIPEWIRE)
    result.set_screencast_stream(SharedScreenCastStream::CreateDefault());
#endif
#if defined(OCTK_OS_MAC) && !defined(OCTK_OS_IOS)
    result.set_configuration_monitor(std::make_shared<DesktopConfigurationMonitor>());
    result.set_full_screen_window_detector(
        std::make_shared<FullScreenWindowDetector>(CreateFullScreenMacApplicationHandler));
#elif defined(OCTK_OS_WIN)
    result.set_full_screen_window_detector(
        std::make_shared<FullScreenWindowDetector>(
            CreateFullScreenWinApplicationHandler));
#endif
    return result;
}
OCTK_END_NAMESPACE
