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

#ifndef _OCTK_DESKTOP_CAPTURER_DIFFER_WRAPPER_HPP
#define _OCTK_DESKTOP_CAPTURER_DIFFER_WRAPPER_HPP

#include <octk_desktop_capture_types.hpp>
#include <octk_shared_desktop_frame.hpp>
#include <octk_desktop_capturer.hpp>
#include <octk_desktop_geometry.hpp>
#include <octk_desktop_frame.hpp>

#include <memory>

#if defined(OCTK_USE_GIO)
#   include "octk_desktop_capture_metadata.h"
#endif  // defined(OCTK_USE_GIO)

OCTK_BEGIN_NAMESPACE

// DesktopCapturer wrapper that calculates updated_region() by comparing frames
// content. This class always expects the underlying DesktopCapturer
// implementation returns a superset of updated regions in DestkopFrame. If a
// DesktopCapturer implementation does not know the updated region, it should
// set updated_region() to full frame.
//
// This class marks entire frame as updated if the frame size or frame stride
// has been changed.
class OCTK_MEDIA_API DesktopCapturerDifferWrapper : public DesktopCapturer, public DesktopCapturer::Callback
{
public:
    // Creates a DesktopCapturerDifferWrapper with a DesktopCapturer
    // implementation, and takes its ownership.
    explicit DesktopCapturerDifferWrapper(std::unique_ptr<DesktopCapturer> base_capturer);
    ~DesktopCapturerDifferWrapper() override;

    // DesktopCapturer interface.
    void start(DesktopCapturer::Callback *callback) override;
    void SetSharedMemoryFactory(std::unique_ptr<SharedMemoryFactory> shared_memory_factory) override;
    void CaptureFrame() override;
    void SetExcludedWindow(WindowId window) override;
    bool GetSourceList(SourceList *screens) override;
    bool SelectSource(SourceId id) override;
    bool FocusOnSelectedSource() override;
    bool IsOccluded(const DesktopVector &pos) override;
#if defined(OCTK_USE_GIO)
    DesktopCaptureMetadata GetMetadata() override;
#endif  // defined(OCTK_USE_GIO)
private:
    // DesktopCapturer::Callback interface.
    void OnCaptureResult(Result result, std::unique_ptr<DesktopFrame> frame) override;

    const std::unique_ptr<DesktopCapturer> base_capturer_;
    DesktopCapturer::Callback *callback_;
    std::unique_ptr<SharedDesktopFrame> last_frame_;
};
OCTK_END_NAMESPACE

#endif  // _OCTK_DESKTOP_CAPTURER_DIFFER_WRAPPER_HPP
