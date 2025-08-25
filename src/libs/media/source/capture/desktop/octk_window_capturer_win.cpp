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

#include "octk_desktop_capture_options.h"
#include "octk_desktop_capturer.h"
#include "modules/desktop_capture/win/window_capturer_win_gdi.h"

#if defined(OCTK_ENABLE_WIN_WGC)
#include "modules/desktop_capture/blank_detector_desktop_capturer_wrapper.h"
#include "modules/desktop_capture/fallback_desktop_capturer_wrapper.h"
#include "modules/desktop_capture/win/wgc_capturer_win.h"
#include "rtc_base/win/windows_version.h"
#endif  // defined(OCTK_ENABLE_WIN_WGC)

OCTK_BEGIN_NAMESPACE

// static
std::unique_ptr<DesktopCapturer> DesktopCapturer::CreateRawWindowCapturer(
    const DesktopCaptureOptions& options) {
  std::unique_ptr<DesktopCapturer> capturer(
      WindowCapturerWinGdi::CreateRawWindowCapturer(options));
#if defined(OCTK_ENABLE_WIN_WGC)
  if (options.allow_wgc_capturer_fallback() &&
      rtc::rtc_win::GetVersion() >= rtc::rtc_win::Version::VERSION_WIN11) {
    // BlankDectector capturer will send an error when it detects a failed
    // GDI rendering, then Fallback capturer will try to capture it again with
    // WGC.
    capturer = utils::make_unique<BlankDetectorDesktopCapturerWrapper>(
        std::move(capturer), RgbaColor(0, 0, 0, 0),
        /*check_per_capture*/ true);

    capturer = utils::make_unique<FallbackDesktopCapturerWrapper>(
        std::move(capturer),
        WgcCapturerWin::CreateRawWindowCapturer(
            options, /*allow_delayed_capturable_check*/ true));
  }
#endif  // defined(OCTK_ENABLE_WIN_WGC)
  return capturer;
}

OCTK_END_NAMESPACE
