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

#include <octk_blank_detector_desktop_capturer_wrapper.hpp>
#include <octk_desktop_geometry.hpp>
#include <octk_desktop_region.hpp>
#include <octk_metrics.hpp>
#include <octk_checks.hpp>

#include <utility>

#include <stdint.h>

OCTK_BEGIN_NAMESPACE

BlankDetectorDesktopCapturerWrapper::BlankDetectorDesktopCapturerWrapper(std::unique_ptr<DesktopCapturer> capturer,
                                                                         RgbaColor blank_pixel,
                                                                         bool check_per_capture)
    : capturer_(std::move(capturer)), blank_pixel_(blank_pixel), check_per_capture_(check_per_capture)
{
    OCTK_DCHECK(capturer_);
}

BlankDetectorDesktopCapturerWrapper::~BlankDetectorDesktopCapturerWrapper() =
default;

void BlankDetectorDesktopCapturerWrapper::start(
    DesktopCapturer::Callback *callback)
{
    callback_ = callback;
    capturer_->start(this);
}

void BlankDetectorDesktopCapturerWrapper::SetSharedMemoryFactory(
    std::unique_ptr<SharedMemoryFactory> shared_memory_factory)
{
    capturer_->SetSharedMemoryFactory(std::move(shared_memory_factory));
}

void BlankDetectorDesktopCapturerWrapper::CaptureFrame()
{
    OCTK_DCHECK(callback_);
    capturer_->CaptureFrame();
}

void BlankDetectorDesktopCapturerWrapper::SetExcludedWindow(WindowId window)
{
    capturer_->SetExcludedWindow(window);
}

bool BlankDetectorDesktopCapturerWrapper::GetSourceList(SourceList *sources)
{
    return capturer_->GetSourceList(sources);
}

bool BlankDetectorDesktopCapturerWrapper::SelectSource(SourceId id)
{
    if (check_per_capture_)
    {
        // If we start capturing a new source, we must reset these members
        // so we don't short circuit the blank detection logic.
        is_first_frame_ = true;
        non_blank_frame_received_ = false;
    }

    return capturer_->SelectSource(id);
}

bool BlankDetectorDesktopCapturerWrapper::FocusOnSelectedSource()
{
    return capturer_->FocusOnSelectedSource();
}

bool BlankDetectorDesktopCapturerWrapper::IsOccluded(const DesktopVector &pos)
{
    return capturer_->IsOccluded(pos);
}

void BlankDetectorDesktopCapturerWrapper::OnCaptureResult(
    Result result,
    std::unique_ptr<DesktopFrame> frame)
{
    OCTK_DCHECK(callback_);
    if (result != Result::SUCCESS || non_blank_frame_received_)
    {
        callback_->OnCaptureResult(result, std::move(frame));
        return;
    }

    if (!frame)
    {
        // Capturer can call the blank detector with empty frame. Blank
        // detector regards it as a blank frame.
        callback_->OnCaptureResult(Result::ERROR_TEMPORARY,
                                   std::unique_ptr<DesktopFrame>());
        return;
    }

    // If nothing has been changed in current frame, we do not need to check it
    // again.
    if (!frame->updated_region().is_empty() || is_first_frame_)
    {
        last_frame_is_blank_ = IsBlankFrame(*frame);
        is_first_frame_ = false;
    }
    OCTK_HISTOGRAM_BOOLEAN("WebRTC.DesktopCapture.BlankFrameDetected",
                           last_frame_is_blank_);
    if (!last_frame_is_blank_)
    {
        non_blank_frame_received_ = true;
        callback_->OnCaptureResult(Result::SUCCESS, std::move(frame));
        return;
    }

    callback_->OnCaptureResult(Result::ERROR_TEMPORARY,
                               std::unique_ptr<DesktopFrame>());
}

bool BlankDetectorDesktopCapturerWrapper::IsBlankFrame(
    const DesktopFrame &frame) const
{
    // We will check 7489 pixels for a frame with 1024 x 768 resolution.
    for (int i = 0; i < frame.size().width() * frame.size().height(); i += 105)
    {
        const int x = i % frame.size().width();
        const int y = i / frame.size().width();
        if (!IsBlankPixel(frame, x, y))
        {
            return false;
        }
    }

    // We are verifying the pixel in the center as well.
    return IsBlankPixel(frame, frame.size().width() / 2,
                        frame.size().height() / 2);
}

bool BlankDetectorDesktopCapturerWrapper::IsBlankPixel(
    const DesktopFrame &frame,
    int x,
    int y) const
{
    uint8_t *pixel_data = frame.GetFrameDataAtPos(DesktopVector(x, y));
    return RgbaColor(pixel_data) == blank_pixel_;
}
OCTK_END_NAMESPACE
