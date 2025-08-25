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
#include <utility>

#include "modules/desktop_capture/blank_detector_desktop_capturer_wrapper.h"
#include "octk_desktop_capture_options.h"
#include "octk_desktop_capturer.h"
#include "modules/desktop_capture/fallback_desktop_capturer_wrapper.h"
#include "modules/desktop_capture/rgba_color.h"
#include "modules/desktop_capture/win/screen_capturer_win_directx.h"
#include "modules/desktop_capture/win/screen_capturer_win_gdi.h"

OCTK_BEGIN_NAMESPACE

namespace {

std::unique_ptr<DesktopCapturer> CreateScreenCapturerWinDirectx(
    const DesktopCaptureOptions& options) {
  std::unique_ptr<DesktopCapturer> capturer(
      new ScreenCapturerWinDirectx(options));
  capturer.reset(new BlankDetectorDesktopCapturerWrapper(
      std::move(capturer), RgbaColor(0, 0, 0, 0)));
  return capturer;
}

}  // namespace

// static
std::unique_ptr<DesktopCapturer> DesktopCapturer::CreateRawScreenCapturer(
    const DesktopCaptureOptions& options) {
  // Default capturer if no options are enabled is GDI.
  std::unique_ptr<DesktopCapturer> capturer(new ScreenCapturerWinGdi(options));

  // If DirectX is enabled use it as main capturer with GDI as fallback.
  if (options.allow_directx_capturer()) {
    // `dxgi_duplicator_controller` should be alive in this scope to ensure it
    // won't unload DxgiDuplicatorController.
    auto dxgi_duplicator_controller = DxgiDuplicatorController::Instance();
    if (ScreenCapturerWinDirectx::IsSupported()) {
      capturer.reset(new FallbackDesktopCapturerWrapper(
          CreateScreenCapturerWinDirectx(options), std::move(capturer)));
      return capturer;
    }
  }

  // Use GDI as default capturer without any fallback solution.
  return capturer;
}

OCTK_END_NAMESPACE
