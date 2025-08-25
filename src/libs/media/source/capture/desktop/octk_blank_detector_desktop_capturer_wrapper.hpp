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

#ifndef _OCTK_BLANK_DETECTOR_DESKTOP_CAPTURER_WRAPPER_HPP
#define _OCTK_BLANK_DETECTOR_DESKTOP_CAPTURER_WRAPPER_HPP

#include <octk_desktop_capture_types.hpp>
#include <octk_desktop_capturer.hpp>
#include <octk_desktop_frame.hpp>
#include <octk_rgba_color.hpp>
#include <octk_shared_memory.hpp>

#include <memory>

OCTK_BEGIN_NAMESPACE

// A DesktopCapturer wrapper detects the return value of its owned
// DesktopCapturer implementation. If sampled pixels returned by the
// DesktopCapturer implementation all equal to the blank pixel, this wrapper
// returns ERROR_TEMPORARY. If the DesktopCapturer implementation fails for too
// many times, this wrapper returns ERROR_PERMANENT.
class BlankDetectorDesktopCapturerWrapper final
    : public DesktopCapturer, public DesktopCapturer::Callback
{
public:
    // Creates BlankDetectorDesktopCapturerWrapper. BlankDesktopCapturerWrapper
    // takes ownership of `capturer`. The `blank_pixel` is the unmodified color
    // returned by the `capturer`.
    BlankDetectorDesktopCapturerWrapper(std::unique_ptr<DesktopCapturer> capturer,
                                        RgbaColor blank_pixel,
                                        bool check_per_capture = false);
    ~BlankDetectorDesktopCapturerWrapper() override;

    // DesktopCapturer interface.
    void start(DesktopCapturer::Callback *callback) override;
    void SetSharedMemoryFactory(
        std::unique_ptr<SharedMemoryFactory> shared_memory_factory) override;
    void CaptureFrame() override;
    void SetExcludedWindow(WindowId window) override;
    bool GetSourceList(SourceList *sources) override;
    bool SelectSource(SourceId id) override;
    bool FocusOnSelectedSource() override;
    bool IsOccluded(const DesktopVector &pos) override;

private:
    // DesktopCapturer::Callback interface.
    void OnCaptureResult(Result result,
                         std::unique_ptr<DesktopFrame> frame) override;

    bool IsBlankFrame(const DesktopFrame &frame) const;

    // Detects whether pixel at (x, y) equals to `blank_pixel_`.
    bool IsBlankPixel(const DesktopFrame &frame, int x, int y) const;

    const std::unique_ptr<DesktopCapturer> capturer_;
    const RgbaColor blank_pixel_;

    // Whether a non-blank frame has been received.
    bool non_blank_frame_received_ = false;

    // Whether the last frame is blank.
    bool last_frame_is_blank_ = false;

    // Whether current frame is the first frame.
    bool is_first_frame_ = true;

    // Blank inspection is made per capture instead of once for all
    // screens or windows.
    bool check_per_capture_ = false;

    DesktopCapturer::Callback *callback_ = nullptr;
};
OCTK_END_NAMESPACE

#endif  // _OCTK_BLANK_DETECTOR_DESKTOP_CAPTURER_WRAPPER_HPP
