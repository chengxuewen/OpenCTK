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

#ifndef _OCTK_DESKTOP_CAPTURE_TYPES_HPP
#define _OCTK_DESKTOP_CAPTURE_TYPES_HPP

#include <octk_media_global.hpp>

#include <cstdint>

OCTK_BEGIN_NAMESPACE

enum class CaptureType
{
    kWindow,
    kScreen,
    kAnyScreenContent
};

// Type used to identify windows on the desktop. Values are platform-specific:
//   - On Windows: HWND cast to intptr_t.
//   - On Linux (with X11): X11 Window (unsigned long) type cast to intptr_t.
//   - On OSX: integer window number.
typedef intptr_t WindowId;

const WindowId kNullWindowId = 0;

const int64_t kInvalidDisplayId = -1;

// Type used to identify screens on the desktop. Values are platform-specific:
//   - On Windows: integer display device index.
//   - On OSX: CGDirectDisplayID cast to intptr_t.
//   - On Linux (with X11): TBD.
//   - On ChromeOS: display::Display::id() is an int64_t.
// On Windows, ScreenId is implementation dependent: sending a ScreenId from one
// implementation to another usually won't work correctly.
#if defined(CHROMEOS)
typedef int64_t ScreenId;
#else
typedef intptr_t ScreenId;
#endif

// The screen id corresponds to all screen combined together.
const ScreenId kFullDesktopScreenId = -1;

const ScreenId kInvalidScreenId = -2;

// Integers to attach to each DesktopFrame to differentiate the generator of
// the frame. The entries in this namespace should remain in sync with the
// SequentialDesktopCapturerId enum, which is logged via UMA.
// `kScreenCapturerWinGdi` and `kScreenCapturerWinDirectx` values are preserved
// to maintain compatibility
namespace DesktopCapturerId
{
constexpr uint32_t CreateFourCC(char a, char b, char c, char d)
{
    return ((static_cast<uint32_t>(a)) | (static_cast<uint32_t>(b) << 8) | (static_cast<uint32_t>(c) << 16) |
            (static_cast<uint32_t>(d) << 24));
}

constexpr uint32_t kUnknown = 0;
constexpr uint32_t kWgcCapturerWin = 1;
constexpr uint32_t kScreenCapturerWinMagnifier = 2;
constexpr uint32_t kWindowCapturerWinGdi = 3;
constexpr uint32_t kScreenCapturerWinGdi = CreateFourCC('G', 'D', 'I', ' ');
constexpr uint32_t kScreenCapturerWinDirectx = CreateFourCC('D', 'X', 'G', 'I');
constexpr uint32_t kX11CapturerLinux = CreateFourCC('X', '1', '1', ' ');
constexpr uint32_t kWaylandCapturerLinux = CreateFourCC('W', 'L', ' ', ' ');
} // namespace DesktopCapturerId

OCTK_END_NAMESPACE

#endif // _OCTK_DESKTOP_CAPTURE_TYPES_HPP
