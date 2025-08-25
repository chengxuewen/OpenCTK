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

#include <octk_fake_desktop_capturer.hpp>
#include <octk_desktop_capture_types.hpp>
#include <octk_desktop_frame.hpp>

#include <utility>

OCTK_BEGIN_NAMESPACE

FakeDesktopCapturer::FakeDesktopCapturer() = default;
FakeDesktopCapturer::~FakeDesktopCapturer() = default;

void FakeDesktopCapturer::set_result(DesktopCapturer::Result result)
{
    result_ = result;
}

int FakeDesktopCapturer::num_frames_captured() const
{
    return num_frames_captured_;
}

int FakeDesktopCapturer::num_capture_attempts() const
{
    return num_capture_attempts_;
}

// Uses the `generator` provided as DesktopFrameGenerator, FakeDesktopCapturer
// does
// not take the ownership of `generator`.
void FakeDesktopCapturer::set_frame_generator(DesktopFrameGenerator *generator)
{
    generator_ = generator;
}

void FakeDesktopCapturer::start(DesktopCapturer::Callback *callback)
{
    callback_ = callback;
}

void FakeDesktopCapturer::CaptureFrame()
{
    num_capture_attempts_++;
    if (generator_)
    {
        if (result_ != DesktopCapturer::Result::SUCCESS)
        {
            callback_->OnCaptureResult(result_, nullptr);
            return;
        }

        std::unique_ptr<DesktopFrame> frame(generator_->GetnextFrame(shared_memory_factory_.get()));
        if (frame)
        {
            num_frames_captured_++;
            callback_->OnCaptureResult(result_, std::move(frame));
        }
        else
        {
            callback_->OnCaptureResult(DesktopCapturer::Result::ERROR_TEMPORARY, nullptr);
        }
        return;
    }
    callback_->OnCaptureResult(DesktopCapturer::Result::ERROR_PERMANENT, nullptr);
}

void FakeDesktopCapturer::SetSharedMemoryFactory(std::unique_ptr<SharedMemoryFactory> shared_memory_factory)
{
    shared_memory_factory_ = std::move(shared_memory_factory);
}

bool FakeDesktopCapturer::GetSourceList(DesktopCapturer::SourceList *sources)
{
    sources->push_back({kWindowId, "A-Fake-DesktopCapturer-Window"});
    sources->push_back({kScreenId});
    return true;
}

bool FakeDesktopCapturer::SelectSource(DesktopCapturer::SourceId id)
{
    return id == kWindowId || id == kScreenId || id == kFullDesktopScreenId;
}
OCTK_END_NAMESPACE
