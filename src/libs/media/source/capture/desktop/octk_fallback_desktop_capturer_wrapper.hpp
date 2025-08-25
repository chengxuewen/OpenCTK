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

#ifndef _OCTK_DESKTOP_CAPTURE_FALLBACK_DESKTOP_CAPTURER_WRAPPER_HPP
#define _OCTK_DESKTOP_CAPTURE_FALLBACK_DESKTOP_CAPTURER_WRAPPER_HPP

#include <octk_desktop_capture_types.hpp>
#include <octk_desktop_capturer.hpp>
#include <octk_desktop_frame.hpp>
#include <octk_desktop_geometry.hpp>
#include <octk_shared_memory.hpp>

#include <memory>

OCTK_BEGIN_NAMESPACE

// A DesktopCapturer wrapper owns two DesktopCapturer implementations. If the
// main DesktopCapturer fails, it uses the secondary one instead. Two capturers
// are expected to return same SourceList, and the meaning of each SourceId is
// identical, otherwise FallbackDesktopCapturerWrapper may return frames from
// different sources. Using asynchronized DesktopCapturer implementations with
// SharedMemoryFactory is not supported, and may result crash or assertion
// failure.
class FallbackDesktopCapturerWrapper final : public DesktopCapturer, public DesktopCapturer::Callback
{
public:
    FallbackDesktopCapturerWrapper(std::unique_ptr<DesktopCapturer> main_capturer,
                                   std::unique_ptr<DesktopCapturer> secondary_capturer);
    ~FallbackDesktopCapturerWrapper() override;

    // DesktopCapturer interface.
    void start(DesktopCapturer::Callback *callback) override;
    void SetSharedMemoryFactory(std::unique_ptr<SharedMemoryFactory> shared_memory_factory) override;
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

    const std::unique_ptr<DesktopCapturer> main_capturer_;
    const std::unique_ptr<DesktopCapturer> secondary_capturer_;
    std::unique_ptr<SharedMemoryFactory> shared_memory_factory_;
    bool main_capturer_permanent_error_ = false;
    DesktopCapturer::Callback *callback_ = nullptr;
};
OCTK_END_NAMESPACE

#endif  // _OCTK_DESKTOP_CAPTURE_FALLBACK_DESKTOP_CAPTURER_WRAPPER_HPP
