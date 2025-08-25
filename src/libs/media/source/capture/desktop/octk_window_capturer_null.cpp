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

#include "octk_desktop_capturer.h"
#include "octk_desktop_frame.h"
#include "rtc_base/checks.h"

OCTK_BEGIN_NAMESPACE

namespace {

class WindowCapturerNull : public DesktopCapturer {
 public:
  WindowCapturerNull();
  ~WindowCapturerNull() override;

  WindowCapturerNull(const WindowCapturerNull&) = delete;
  WindowCapturerNull& operator=(const WindowCapturerNull&) = delete;

  // DesktopCapturer interface.
  void start(Callback* callback) override;
  void CaptureFrame() override;
  bool GetSourceList(SourceList* sources) override;
  bool SelectSource(SourceId id) override;

 private:
  Callback* callback_ = nullptr;
};

WindowCapturerNull::WindowCapturerNull() {}
WindowCapturerNull::~WindowCapturerNull() {}

bool WindowCapturerNull::GetSourceList(SourceList* sources) {
  // Not implemented yet.
  return false;
}

bool WindowCapturerNull::SelectSource(SourceId id) {
  // Not implemented yet.
  return false;
}

void WindowCapturerNull::start(Callback* callback) {
  OCTK_DCHECK(!callback_);
  OCTK_DCHECK(callback);

  callback_ = callback;
}

void WindowCapturerNull::CaptureFrame() {
  // Not implemented yet.
  callback_->OnCaptureResult(Result::ERROR_TEMPORARY, nullptr);
}

}  // namespace

// static
std::unique_ptr<DesktopCapturer> DesktopCapturer::CreateRawWindowCapturer(
    const DesktopCaptureOptions& options) {
  return std::unique_ptr<DesktopCapturer>(new WindowCapturerNull());
}

OCTK_END_NAMESPACE
