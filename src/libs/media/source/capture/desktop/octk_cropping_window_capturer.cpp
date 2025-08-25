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

#include <octk_cropping_window_capturer.hpp>
#include <octk_cropped_desktop_frame.hpp>
#include <octk_logging.hpp>

#include <cstddef>
#include <utility>

OCTK_BEGIN_NAMESPACE

CroppingWindowCapturer::CroppingWindowCapturer(const DesktopCaptureOptions &options)
    : options_(options), callback_(NULL), window_capturer_(DesktopCapturer::CreateRawWindowCapturer(options))
    , selected_window_(kNullWindowId), excluded_window_(kNullWindowId) {}

CroppingWindowCapturer::~CroppingWindowCapturer() {}

void CroppingWindowCapturer::start(DesktopCapturer::Callback *callback)
{
    callback_ = callback;
    window_capturer_->start(callback);
}

void CroppingWindowCapturer::SetSharedMemoryFactory(
    std::unique_ptr<SharedMemoryFactory> shared_memory_factory)
{
    window_capturer_->SetSharedMemoryFactory(std::move(shared_memory_factory));
}

void CroppingWindowCapturer::CaptureFrame()
{
    if (ShouldUseScreenCapturer())
    {
        if (!screen_capturer_.get())
        {
            screen_capturer_ = DesktopCapturer::CreateRawScreenCapturer(options_);
            if (excluded_window_)
            {
                screen_capturer_->SetExcludedWindow(excluded_window_);
            }
            screen_capturer_->start(this);
        }
        screen_capturer_->CaptureFrame();
    }
    else
    {
        window_capturer_->CaptureFrame();
    }
}

void CroppingWindowCapturer::SetExcludedWindow(WindowId window)
{
    excluded_window_ = window;
    if (screen_capturer_.get())
    {
        screen_capturer_->SetExcludedWindow(window);
    }
}

bool CroppingWindowCapturer::GetSourceList(SourceList *sources)
{
    return window_capturer_->GetSourceList(sources);
}

bool CroppingWindowCapturer::SelectSource(SourceId id)
{
    if (window_capturer_->SelectSource(id))
    {
        selected_window_ = id;
        return true;
    }
    return false;
}

bool CroppingWindowCapturer::FocusOnSelectedSource()
{
    return window_capturer_->FocusOnSelectedSource();
}

void CroppingWindowCapturer::OnCaptureResult(
    DesktopCapturer::Result result,
    std::unique_ptr<DesktopFrame> screen_frame)
{
    if (!ShouldUseScreenCapturer())
    {
        OCTK_INFO() << "Window no longer on top when ScreenCapturer finishes";
        window_capturer_->CaptureFrame();
        return;
    }

    if (result != Result::SUCCESS)
    {
        OCTK_WARNING() << "ScreenCapturer failed to capture a frame";
        callback_->OnCaptureResult(result, nullptr);
        return;
    }

    DesktopRect window_rect = GetWindowRectInVirtualScreen();
    if (window_rect.is_empty())
    {
        OCTK_WARNING() << "Window rect is empty";
        callback_->OnCaptureResult(Result::ERROR_TEMPORARY, nullptr);
        return;
    }

    std::unique_ptr<DesktopFrame> cropped_frame =
        CreateCroppedDesktopFrame(std::move(screen_frame), window_rect);

    if (!cropped_frame)
    {
        OCTK_WARNING() << "Window is outside of the captured display";
        callback_->OnCaptureResult(Result::ERROR_TEMPORARY, nullptr);
        return;
    }

    callback_->OnCaptureResult(Result::SUCCESS, std::move(cropped_frame));
}

bool CroppingWindowCapturer::IsOccluded(const DesktopVector &pos)
{
    // Returns true if either capturer returns true.
    if (window_capturer_->IsOccluded(pos))
    {
        return true;
    }
    if (screen_capturer_ != nullptr && screen_capturer_->IsOccluded(pos))
    {
        return true;
    }
    return false;
}

#if !defined(OCTK_OS_WIN)
// CroppingWindowCapturer is implemented only for windows. On other platforms
// the regular window capturer is used.
// static
std::unique_ptr<DesktopCapturer> CroppingWindowCapturer::CreateCapturer(const DesktopCaptureOptions &options)
{
    return DesktopCapturer::CreateWindowCapturer(options);
}
#endif
OCTK_END_NAMESPACE
