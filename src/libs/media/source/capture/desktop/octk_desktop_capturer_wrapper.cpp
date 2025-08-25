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

#include <octk_desktop_capturer_wrapper.hpp>
#include <octk_checks.hpp>

#include <utility>

OCTK_BEGIN_NAMESPACE

DesktopCapturerWrapper::DesktopCapturerWrapper(std::unique_ptr<DesktopCapturer> base_capturer)
    : base_capturer_(std::move(base_capturer))
{
    OCTK_DCHECK(base_capturer_);
}

DesktopCapturerWrapper::~DesktopCapturerWrapper() = default;

void DesktopCapturerWrapper::start(Callback *callback)
{
    base_capturer_->start(callback);
}

void DesktopCapturerWrapper::SetSharedMemoryFactory(std::unique_ptr<SharedMemoryFactory> shared_memory_factory)
{
    base_capturer_->SetSharedMemoryFactory(std::move(shared_memory_factory));
}

void DesktopCapturerWrapper::CaptureFrame()
{
    base_capturer_->CaptureFrame();
}

void DesktopCapturerWrapper::SetExcludedWindow(WindowId window)
{
    base_capturer_->SetExcludedWindow(window);
}

bool DesktopCapturerWrapper::GetSourceList(SourceList *sources)
{
    return base_capturer_->GetSourceList(sources);
}

bool DesktopCapturerWrapper::SelectSource(SourceId id)
{
    return base_capturer_->SelectSource(id);
}

bool DesktopCapturerWrapper::FocusOnSelectedSource()
{
    return base_capturer_->FocusOnSelectedSource();
}

bool DesktopCapturerWrapper::IsOccluded(const DesktopVector &pos)
{
    return base_capturer_->IsOccluded(pos);
}
OCTK_END_NAMESPACE
