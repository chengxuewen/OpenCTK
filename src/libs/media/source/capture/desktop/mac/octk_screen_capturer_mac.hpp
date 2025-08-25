/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present chengxuewen.
** Copyright (c) 2018 The WebRTC project authors.
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

#ifndef _OCTK_DESKTOP_CAPTURE_MAC_SCREEN_CAPTURER_MAC_HPP
#define _OCTK_DESKTOP_CAPTURE_MAC_SCREEN_CAPTURER_MAC_HPP

// #include "api/sequence_checker.h"
#include <octk_desktop_capture_options.hpp>
#include <octk_desktop_capturer.hpp>
#include <octk_desktop_frame.hpp>
#include <octk_desktop_geometry.hpp>
#include <octk_desktop_region.hpp>
#include <mac/octk_desktop_configuration.hpp>
#include <mac/octk_desktop_configuration_monitor.hpp>
#include <mac/octk_desktop_frame_provider.hpp>
#include <octk_screen_capture_frame_queue.hpp>
#include <octk_screen_capturer_helper.hpp>
#include <octk_shared_desktop_frame.hpp>

#include <CoreGraphics/CoreGraphics.h>

#include <memory>
#include <vector>

OCTK_BEGIN_NAMESPACE

class DisplayStreamManager;

// A class to perform video frame capturing for mac.
class ScreenCapturerMac final : public DesktopCapturer
{
public:
    ScreenCapturerMac(std::shared_ptr <DesktopConfigurationMonitor> desktop_config_monitor,
                      bool detect_updated_region,
                      bool allow_iosurface);
    ~ScreenCapturerMac() override;

    ScreenCapturerMac(const ScreenCapturerMac &) = delete;
    ScreenCapturerMac &operator=(const ScreenCapturerMac &) = delete;

    // TODO(julien.isorce): Remove init() or make it private.
    bool init();

    // DesktopCapturer interface.
    void start(Callback *callback) override;
    void CaptureFrame() override;
    void SetExcludedWindow(WindowId window) override;
    bool GetSourceList(SourceList *screens) override;
    bool SelectSource(SourceId id) override;

private:
    // Returns false if the selected screen is no longer valid.
    bool CgBlit(const DesktopFrame &frame, const DesktopRegion &region);

    // Called when the screen configuration is changed.
    void ScreenConfigurationChanged();

    bool RegisterRefreshAndMoveHandlers();
    void UnregisterRefreshAndMoveHandlers();

    void ScreenRefresh(CGDirectDisplayID display_id,
                       CGRectCount count,
                       const CGRect *rect_array,
                       DesktopVector display_origin,
                       IOSurfaceRef io_surface);
    void ReleaseBuffers();

    std::unique_ptr<DesktopFrame> CreateFrame();

    const bool detect_updated_region_;

    Callback *callback_ = nullptr;

    // Queue of the frames buffers.
    ScreenCaptureFrameQueue <SharedDesktopFrame> queue_;

    // Current display configuration.
    MacDesktopConfiguration desktop_config_;

    // Currently selected display, or 0 if the full desktop is selected. On OS X
    // 10.6 and before, this is always 0.
    CGDirectDisplayID current_display_ = 0;

    // The physical pixel bounds of the current screen.
    DesktopRect screen_pixel_bounds_;

    // The dip to physical pixel scale of the current screen.
    float dip_to_pixel_scale_ = 1.0f;

    // A thread-safe list of invalid rectangles, and the size of the most
    // recently captured screen.
    ScreenCapturerHelper helper_;

    // Contains an invalid region from the previous capture.
    DesktopRegion last_invalid_region_;

    // Monitoring display reconfiguration.
    std::shared_ptr <DesktopConfigurationMonitor> desktop_config_monitor_;

    CGWindowID excluded_window_ = 0;

    // List of streams, one per screen.
    std::vector<CGDisplayStreamRef> display_streams_;

    // Container holding latest state of the snapshot per displays.
    DesktopFrameProvider desktop_frame_provider_;

    // start, CaptureFrame and destructor have to called in the same thread.
    SequenceChecker thread_checker_;
};
OCTK_END_NAMESPACE

#endif  // _OCTK_DESKTOP_CAPTURE_MAC_SCREEN_CAPTURER_MAC_HPP
