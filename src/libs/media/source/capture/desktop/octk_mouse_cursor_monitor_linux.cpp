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

#include <memory>

#include "octk_desktop_capture_types.h"
#include "modules/desktop_capture/mouse_cursor_monitor.h"

#if defined(OCTK_USE_X11)
#include "modules/desktop_capture/linux/x11/mouse_cursor_monitor_x11.h"
#endif  // defined(OCTK_USE_X11)

#if defined(OCTK_USE_PIPEWIRE)
#include "modules/desktop_capture/linux/wayland/mouse_cursor_monitor_pipewire.h"
#endif  // defined(OCTK_USE_PIPEWIRE)

OCTK_BEGIN_NAMESPACE

// static
MouseCursorMonitor* MouseCursorMonitor::CreateForWindow(
    const DesktopCaptureOptions& options,
    WindowId window) {
#if defined(OCTK_USE_X11)
  return MouseCursorMonitorX11::CreateForWindow(options, window);
#else
  return nullptr;
#endif  // defined(OCTK_USE_X11)
}

// static
MouseCursorMonitor* MouseCursorMonitor::CreateForScreen(
    const DesktopCaptureOptions& options,
    ScreenId screen) {
#if defined(OCTK_USE_X11)
  return MouseCursorMonitorX11::CreateForScreen(options, screen);
#else
  return nullptr;
#endif  // defined(OCTK_USE_X11)
}

// static
std::unique_ptr<MouseCursorMonitor> MouseCursorMonitor::Create(
    const DesktopCaptureOptions& options) {
#if defined(OCTK_USE_PIPEWIRE)
  if (options.allow_pipewire() && DesktopCapturer::IsRunningUnderWayland() &&
      options.screencast_stream()) {
    return utils::make_unique<MouseCursorMonitorPipeWire>(options);
  }
#endif  // defined(OCTK_USE_PIPEWIRE)

#if defined(OCTK_USE_X11)
  return MouseCursorMonitorX11::Create(options);
#else
  return nullptr;
#endif  // defined(OCTK_USE_X11)
}

OCTK_END_NAMESPACE
