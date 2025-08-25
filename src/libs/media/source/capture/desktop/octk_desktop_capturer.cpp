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

#include <octk_desktop_capturer.hpp>
#include <octk_desktop_capturer_differ_wrapper.hpp>
#include <octk_cropping_window_capturer.hpp>
#include <octk_desktop_capture_options.hpp>
#include <octk_metrics.hpp>

#include <cstring>
#include <utility>

#if defined(OCTK_ENABLE_WIN_WGC)
#include "modules/desktop_capture/win/wgc_capturer_win.h"
#include "rtc_base/win/windows_version.h"
#endif // defined(OCTK_ENABLE_WIN_WGC)

#if defined(OCTK_USE_PIPEWIRE)
#include "modules/desktop_capture/linux/wayland/base_capturer_pipewire.h"
#endif

OCTK_BEGIN_NAMESPACE
void LogDesktopCapturerFullscreenDetectorUsage()
{
    OCTK_HISTOGRAM_BOOLEAN("WebRTC.Screenshare.DesktopCapturerFullscreenDetector", true);
}

DesktopCapturer::~DesktopCapturer() = default;

DelegatedSourceListController *DesktopCapturer::GetDelegatedSourceListController() { return nullptr; }

void DesktopCapturer::SetSharedMemoryFactory(std::unique_ptr<SharedMemoryFactory> /* shared_memory_factory */)
{
}

void DesktopCapturer::SetExcludedWindow(WindowId /* window */)
{
}

bool DesktopCapturer::GetSourceList(SourceList * /* sources */) { return true; }

bool DesktopCapturer::SelectSource(SourceId /* id */) { return false; }

bool DesktopCapturer::FocusOnSelectedSource() { return false; }

bool DesktopCapturer::IsOccluded(const DesktopVector & /* pos */) { return false; }

// static
std::unique_ptr<DesktopCapturer> DesktopCapturer::CreateWindowCapturer(const DesktopCaptureOptions &options)
{
#if defined(OCTK_ENABLE_WIN_WGC)
    if (options.allow_wgc_window_capturer() && IsWgcSupported(CaptureType::kWindow))
    {
        return WgcCapturerWin::CreateRawWindowCapturer(options);
    }
#endif // defined(OCTK_ENABLE_WIN_WGC)

#if defined(OCTK_OS_WIN)
    if (options.allow_cropping_window_capturer())
    {
        return CroppingWindowCapturer::CreateCapturer(options);
    }
#endif // defined(OCTK_OS_WIN)

    std::unique_ptr<DesktopCapturer> capturer = CreateRawWindowCapturer(options);
    if (capturer && options.detect_updated_region())
    {
        capturer.reset(new DesktopCapturerDifferWrapper(std::move(capturer)));
    }

    return capturer;
}

// static
std::unique_ptr<DesktopCapturer> DesktopCapturer::CreateScreenCapturer(const DesktopCaptureOptions &options)
{
#if defined(OCTK_ENABLE_WIN_WGC)
    if (options.allow_wgc_screen_capturer() && IsWgcSupported(CaptureType::kScreen))
    {
        return WgcCapturerWin::CreateRawScreenCapturer(options);
    }
#endif // defined(OCTK_ENABLE_WIN_WGC)

    std::unique_ptr<DesktopCapturer> capturer = CreateRawScreenCapturer(options);
    if (capturer && options.detect_updated_region())
    {
        capturer.reset(new DesktopCapturerDifferWrapper(std::move(capturer)));
    }

    return capturer;
}

// static
std::unique_ptr<DesktopCapturer>
DesktopCapturer::CreateGenericCapturer([[maybe_unused]] const DesktopCaptureOptions &options)
{
    std::unique_ptr<DesktopCapturer> capturer;

#if defined(OCTK_USE_PIPEWIRE)
    if (options.allow_pipewire() && DesktopCapturer::IsRunningUnderWayland())
    {
        capturer = utils::make_unique<BaseCapturerPipeWire>(options, CaptureType::kAnyScreenContent);
    }

    if (capturer && options.detect_updated_region())
    {
        capturer.reset(new DesktopCapturerDifferWrapper(std::move(capturer)));
    }
#endif // defined(OCTK_USE_PIPEWIRE)

    return capturer;
}

#if defined(OCTK_USE_PIPEWIRE) || defined(OCTK_USE_X11)
bool DesktopCapturer::IsRunningUnderWayland()
{
    const char* xdg_session_type = getenv("XDG_SESSION_TYPE");
    if (!xdg_session_type || strncmp(xdg_session_type, "wayland", 7) != 0)
        return false;

    if (!(getenv("WAYLAND_DISPLAY")))
        return false;

    return true;
}
#endif // defined(OCTK_USE_PIPEWIRE) || defined(OCTK_USE_X11)

OCTK_END_NAMESPACE
