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

#include <windows.h>

#include <memory>

#include "modules/desktop_capture/screen_drawer.h"
#include "system_wrappers/include/sleep.h"

OCTK_BEGIN_NAMESPACE

namespace {

static constexpr TCHAR kMutexName[] =
    TEXT("Local\\ScreenDrawerWin-da834f82-8044-11e6-ac81-73dcdd1c1869");

class ScreenDrawerLockWin : public ScreenDrawerLock {
 public:
  ScreenDrawerLockWin();
  ~ScreenDrawerLockWin() override;

 private:
  HANDLE mMutex;
};

ScreenDrawerLockWin::ScreenDrawerLockWin() {
  while (true) {
    mMutex = CreateMutex(NULL, FALSE, kMutexName);
    if (GetLastError() != ERROR_ALREADY_EXISTS && mMutex != NULL) {
      break;
    } else {
      if (mMutex) {
        CloseHandle(mMutex);
      }
      SleepMs(1000);
    }
  }
}

ScreenDrawerLockWin::~ScreenDrawerLockWin() {
  CloseHandle(mMutex);
}

DesktopRect GetScreenRect() {
  HDC hdc = GetDC(NULL);
  DesktopRect rect = DesktopRect::MakeWH(GetDeviceCaps(hdc, HORZRES),
                                         GetDeviceCaps(hdc, VERTRES));
  ReleaseDC(NULL, hdc);
  return rect;
}

HWND CreateDrawerWindow(DesktopRect rect) {
  HWND hwnd = CreateWindowA(
      "STATIC", "DrawerWindow", WS_POPUPWINDOW | WS_VISIBLE, rect.left(),
      rect.top(), rect.width(), rect.height(), NULL, NULL, NULL, NULL);
  SetForegroundWindow(hwnd);
  return hwnd;
}

COLORREF ColorToRef(RgbaColor color) {
  // Windows device context does not support alpha.
  return RGB(color.red, color.green, color.blue);
}

// A ScreenDrawer implementation for Windows.
class ScreenDrawerWin : public ScreenDrawer {
 public:
  ScreenDrawerWin();
  ~ScreenDrawerWin() override;

  // ScreenDrawer interface.
  DesktopRect DrawableRegion() override;
  void DrawRectangle(DesktopRect rect, RgbaColor color) override;
  void Clear() override;
  void WaitForPendingDraws() override;
  bool MayDrawIncompleteShapes() override;
  WindowId window_id() const override;

 private:
  // Bring the window to the front, this can help to avoid the impact from other
  // windows or shadow effects.
  void BringToFront();

  // draw a line with `color`.
  void DrawLine(DesktopVector start, DesktopVector end, RgbaColor color);

  // draw a dot with `color`.
  void DrawDot(DesktopVector vect, RgbaColor color);

  const DesktopRect rect_;
  HWND window_;
  HDC hdc_;
};

ScreenDrawerWin::ScreenDrawerWin()
    : ScreenDrawer(),
      rect_(GetScreenRect()),
      window_(CreateDrawerWindow(rect_)),
      hdc_(GetWindowDC(window_)) {
  // We do not need to handle any messages for the `window_`, so disable Windows
  // from processing windows ghosting feature.
  DisableProcessWindowsGhosting();

  // Always use stock pen (DC_PEN) and brush (DC_BRUSH).
  SelectObject(hdc_, GetStockObject(DC_PEN));
  SelectObject(hdc_, GetStockObject(DC_BRUSH));
  BringToFront();
}

ScreenDrawerWin::~ScreenDrawerWin() {
  ReleaseDC(NULL, hdc_);
  DestroyWindow(window_);
  // Unfortunately there is no EnableProcessWindowsGhosting() API.
}

DesktopRect ScreenDrawerWin::DrawableRegion() {
  return rect_;
}

void ScreenDrawerWin::DrawRectangle(DesktopRect rect, RgbaColor color) {
  if (rect.width() == 1 && rect.height() == 1) {
    // Rectangle function cannot draw a 1 pixel rectangle.
    DrawDot(rect.top_left(), color);
    return;
  }

  if (rect.width() == 1 || rect.height() == 1) {
    // Rectangle function cannot draw a 1 pixel rectangle.
    DrawLine(rect.top_left(), DesktopVector(rect.right(), rect.bottom()),
             color);
    return;
  }

  SetDCBrushColor(hdc_, ColorToRef(color));
  SetDCPenColor(hdc_, ColorToRef(color));
  Rectangle(hdc_, rect.left(), rect.top(), rect.right(), rect.bottom());
}

void ScreenDrawerWin::Clear() {
  DrawRectangle(rect_, RgbaColor(0, 0, 0));
}

// TODO(zijiehe): Find the right signal to indicate the finish of all pending
// paintings.
void ScreenDrawerWin::WaitForPendingDraws() {
  BringToFront();
  SleepMs(50);
}

bool ScreenDrawerWin::MayDrawIncompleteShapes() {
  return true;
}

WindowId ScreenDrawerWin::window_id() const {
  return reinterpret_cast<WindowId>(window_);
}

void ScreenDrawerWin::DrawLine(DesktopVector start,
                               DesktopVector end,
                               RgbaColor color) {
  POINT points[2];
  points[0].x = start.x();
  points[0].y = start.y();
  points[1].x = end.x();
  points[1].y = end.y();
  SetDCPenColor(hdc_, ColorToRef(color));
  Polyline(hdc_, points, 2);
}

void ScreenDrawerWin::DrawDot(DesktopVector vect, RgbaColor color) {
  SetPixel(hdc_, vect.x(), vect.y(), ColorToRef(color));
}

void ScreenDrawerWin::BringToFront() {
  if (SetWindowPos(window_, HWND_TOPMOST, 0, 0, 0, 0,
                   SWP_NOMOVE | SWP_NOSIZE) != FALSE) {
    return;
  }

  long ex_style = GetWindowLong(window_, GWL_EXSTYLE);
  ex_style |= WS_EX_TOPMOST;
  if (SetWindowLong(window_, GWL_EXSTYLE, ex_style) != 0) {
    return;
  }

  BringWindowToTop(window_);
}

}  // namespace

// static
std::unique_ptr<ScreenDrawerLock> ScreenDrawerLock::Create() {
  return std::unique_ptr<ScreenDrawerLock>(new ScreenDrawerLockWin());
}

// static
std::unique_ptr<ScreenDrawer> ScreenDrawer::Create() {
  return std::unique_ptr<ScreenDrawer>(new ScreenDrawerWin());
}

OCTK_END_NAMESPACE
