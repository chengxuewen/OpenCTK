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

#ifndef _OCTK_CAPTURE_CROPPING_WINDOW_CAPTURER_HPP
#define _OCTK_CAPTURE_CROPPING_WINDOW_CAPTURER_HPP

#include <octk_media_global.hpp>
#include <octk_desktop_capture_options.hpp>
#include <octk_desktop_capture_types.hpp>
#include <octk_desktop_capturer.hpp>
#include <octk_desktop_frame.hpp>
#include <octk_desktop_geometry.hpp>
#include <octk_shared_memory.hpp>

#include <memory>

OCTK_BEGIN_NAMESPACE

// WindowCapturer implementation that uses a screen capturer to capture the
// whole screen and crops the video frame to the window area when the captured
// window is on top.
class OCTK_MEDIA_API CroppingWindowCapturer : public DesktopCapturer, public DesktopCapturer::Callback
{
public:
    static std::unique_ptr<DesktopCapturer> CreateCapturer(
        const DesktopCaptureOptions &options);

    ~CroppingWindowCapturer() override;

    // DesktopCapturer implementation.
    void start(DesktopCapturer::Callback *callback) override;
    void SetSharedMemoryFactory(
        std::unique_ptr<SharedMemoryFactory> shared_memory_factory) override;
    void CaptureFrame() override;
    void SetExcludedWindow(WindowId window) override;
    bool GetSourceList(SourceList *sources) override;
    bool SelectSource(SourceId id) override;
    bool FocusOnSelectedSource() override;
    bool IsOccluded(const DesktopVector &pos) override;

    // DesktopCapturer::Callback implementation, passed to `screen_capturer_` to
    // intercept the capture result.
    void OnCaptureResult(DesktopCapturer::Result result,
                         std::unique_ptr<DesktopFrame> frame) override;

protected:
    explicit CroppingWindowCapturer(const DesktopCaptureOptions &options);

    // The platform implementation should override these methods.

    // Returns true if it is OK to capture the whole screen and crop to the
    // selected window, i.e. the selected window is opaque, rectangular, and not
    // occluded.
    virtual bool ShouldUseScreenCapturer() = 0;

    // Returns the window area relative to the top left of the virtual screen
    // within the bounds of the virtual screen. This function should return the
    // DesktopRect in full desktop coordinates, i.e. the top-left monitor starts
    // from (0, 0).
    virtual DesktopRect GetWindowRectInVirtualScreen() = 0;

    WindowId selected_window() const { return selected_window_; }
    WindowId excluded_window() const { return excluded_window_; }
    DesktopCapturer *window_capturer() const { return window_capturer_.get(); }

private:
    DesktopCaptureOptions options_;
    DesktopCapturer::Callback *callback_;
    std::unique_ptr<DesktopCapturer> window_capturer_;
    std::unique_ptr<DesktopCapturer> screen_capturer_;
    SourceId selected_window_;
    WindowId excluded_window_;
};
OCTK_END_NAMESPACE

#endif  // _OCTK_CAPTURE_CROPPING_WINDOW_CAPTURER_HPP
