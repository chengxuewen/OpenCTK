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
#include <octk_desktop_capturer.hpp>
#include <octk_desktop_frame.hpp>
#include <mac/octk_desktop_configuration.hpp>
#include <mac/octk_desktop_configuration_monitor.hpp>
#include <mac/octk_desktop_frame_cgimage.hpp>
#include <mac/octk_window_list_utils.hpp>
#include <mac/octk_window_finder_mac.hpp>
#include <octk_logging.hpp>
#include <octk_checks.hpp>
// #include "rtc_base/trace_event.h"
// #include "api/scoped_refptr.h"

#include <ApplicationServices/ApplicationServices.h>
#include <CoreFoundation/CoreFoundation.h>
#include <Cocoa/Cocoa.h>

#include <utility>

OCTK_BEGIN_NAMESPACE

namespace
{

// Returns true if the window exists.
bool IsWindowValid(CGWindowID id)
{
    CFArrayRef window_id_array = CFArrayCreate(nullptr, reinterpret_cast<const void **>(&id), 1, nullptr);
    CFArrayRef window_array = CGWindowListCreateDescriptionFromArray(window_id_array);
    bool valid = window_array && CFArrayGetCount(window_array);
    CFRelease(window_id_array);
    CFRelease(window_array);

    return valid;
}

class WindowCapturerMac : public DesktopCapturer
{
public:
    explicit WindowCapturerMac(std::shared_ptr<FullScreenWindowDetector> full_screen_window_detector,
                               std::shared_ptr<DesktopConfigurationMonitor> configuration_monitor);
    ~WindowCapturerMac() override;

    WindowCapturerMac(const WindowCapturerMac &) = delete;
    WindowCapturerMac &operator=(const WindowCapturerMac &) = delete;

    // DesktopCapturer interface.
    void start(Callback *callback) override;
    void CaptureFrame() override;
    bool GetSourceList(SourceList *sources) override;
    bool SelectSource(SourceId id) override;
    bool FocusOnSelectedSource() override;
    bool IsOccluded(const DesktopVector &pos) override;

private:
    Callback *callback_ = nullptr;

    // The window being captured.
    CGWindowID window_id_ = 0;

    std::shared_ptr<FullScreenWindowDetector> full_screen_window_detector_;

    const std::shared_ptr<DesktopConfigurationMonitor> configuration_monitor_;

    WindowFinderMac window_finder_;

    // Used to make sure that we only log the usage of fullscreen detection once.
    bool fullscreen_usage_logged_ = false;
};

WindowCapturerMac::WindowCapturerMac(std::shared_ptr<FullScreenWindowDetector> full_screen_window_detector,
                                     std::shared_ptr<DesktopConfigurationMonitor> configuration_monitor)
    : full_screen_window_detector_(std::move(full_screen_window_detector)), configuration_monitor_(
    std::move(configuration_monitor)), window_finder_(configuration_monitor_) {}

WindowCapturerMac::~WindowCapturerMac() {}

bool WindowCapturerMac::GetSourceList(SourceList *sources)
{
    return GetWindowList(sources, true, true);
}

bool WindowCapturerMac::SelectSource(SourceId id)
{
    if (!IsWindowValid(id))
    {
        return false;
    }
    window_id_ = id;
    return true;
}

bool WindowCapturerMac::FocusOnSelectedSource()
{
    if (!window_id_)
    {
        return false;
    }

    CGWindowID ids[1];
    ids[0] = window_id_;
    CFArrayRef window_id_array = CFArrayCreate(nullptr, reinterpret_cast<const void **>(&ids), 1, nullptr);

    CFArrayRef window_array = CGWindowListCreateDescriptionFromArray(window_id_array);
    if (!window_array || 0 == CFArrayGetCount(window_array))
    {
        // Could not find the window. It might have been closed.
        OCTK_INFO() << "Window not found";
        CFRelease(window_id_array);
        return false;
    }

    CFDictionaryRef window = reinterpret_cast<CFDictionaryRef>(CFArrayGetValueAtIndex(window_array, 0));
    CFNumberRef pid_ref = reinterpret_cast<CFNumberRef>(CFDictionaryGetValue(window, kCGWindowOwnerPID));

    int pid;
    CFNumberGetValue(pid_ref, kCFNumberIntType, &pid);

    // TODO(jiayl): this will bring the process main window to the front. We
    // should find a way to bring only the window to the front.
    bool result = [[NSRunningApplication runningApplicationWithProcessIdentifier:pid] activateWithOptions:0];

    CFRelease(window_id_array);
    CFRelease(window_array);
    return result;
}

bool WindowCapturerMac::IsOccluded(const DesktopVector &pos)
{
    DesktopVector sys_pos = pos;
    if (configuration_monitor_)
    {
        auto configuration = configuration_monitor_->desktop_configuration();
        sys_pos = pos.add(configuration.bounds.top_left());
    }
    return window_finder_.GetWindowUnderPoint(sys_pos) != window_id_;
}

void WindowCapturerMac::start(Callback *callback)
{
    OCTK_DCHECK(!callback_);
    OCTK_DCHECK(callback);

    callback_ = callback;
}

void WindowCapturerMac::CaptureFrame()
{
    // TRACE_EVENT0("webrtc", "WindowCapturerMac::CaptureFrame");

    if (!IsWindowValid(window_id_))
    {
        OCTK_ERROR() << "The window is not valid any longer.";
        callback_->OnCaptureResult(Result::ERROR_PERMANENT, nullptr);
        return;
    }

    CGWindowID on_screen_window = window_id_;
    if (full_screen_window_detector_)
    {
        full_screen_window_detector_->UpdateWindowListIfNeeded(window_id_, [](DesktopCapturer::SourceList *sources) {
            // Not using webrtc::GetWindowList(sources, true, false)
            // as it doesn't allow to have in the result window with
            // empty title along with titled window owned by the same pid.
            return GetWindowList(
                [sources](CFDictionaryRef window) {
                    WindowId window_id = GetWindowId(window);
                    if (window_id != kNullWindowId)
                    {
                        sources->push_back(DesktopCapturer::Source{window_id, GetWindowTitle(window)});
                    }
                    return true;
                },
                true,
                false);
        });

        CGWindowID full_screen_window = full_screen_window_detector_->FindFullScreenWindow(window_id_);

        if (full_screen_window != kCGNullWindowID)
        {
            // If this is the first time this happens, report to UMA that the feature is active.
            if (!fullscreen_usage_logged_)
            {
                LogDesktopCapturerFullscreenDetectorUsage();
                fullscreen_usage_logged_ = true;
            }
            on_screen_window = full_screen_window;
        }
    }

    std::unique_ptr<DesktopFrame> frame = DesktopFrameCGImage::CreateForWindow(on_screen_window);
    if (!frame)
    {
        OCTK_WARNING() << "Temporarily failed to capture window.";
        callback_->OnCaptureResult(Result::ERROR_TEMPORARY, nullptr);
        return;
    }

    frame->mutable_updated_region()->SetRect(DesktopRect::MakeSize(frame->size()));
    frame->set_top_left(GetWindowBounds(on_screen_window).top_left());

    float scale_factor = GetWindowScaleFactor(window_id_, frame->size());
    frame->set_dpi(DesktopVector(kStandardDPI * scale_factor, kStandardDPI * scale_factor));

    callback_->OnCaptureResult(Result::SUCCESS, std::move(frame));
}
}  // namespace

// static
std::unique_ptr<DesktopCapturer> DesktopCapturer::CreateRawWindowCapturer(const DesktopCaptureOptions &options)
{
    return std::unique_ptr<DesktopCapturer>(
        new WindowCapturerMac(options.full_screen_window_detector(), options.configuration_monitor()));
}
OCTK_END_NAMESPACE
