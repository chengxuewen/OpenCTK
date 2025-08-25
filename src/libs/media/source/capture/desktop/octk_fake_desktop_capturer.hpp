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

#ifndef _OCTK_DESKTOP_CAPTURE_FAKE_DESKTOP_CAPTURER_HPP
#define _OCTK_DESKTOP_CAPTURE_FAKE_DESKTOP_CAPTURER_HPP

#include <octk_desktop_frame_generator.hpp>
#include <octk_desktop_capturer.hpp>
#include <octk_shared_memory.hpp>

#include <memory>

OCTK_BEGIN_NAMESPACE

// A fake implementation of DesktopCapturer or its derived interfaces to
// generate DesktopFrame for testing purpose.
//
// Consumers can provide a FrameGenerator instance to generate instances of
// DesktopFrame to return for each Capture() function call.
// If no FrameGenerator provided, FakeDesktopCapturer will always return a
// nullptr DesktopFrame.
//
// Double buffering is guaranteed by the FrameGenerator. FrameGenerator
// implements in desktop_frame_generator.h guarantee double buffering, they
// creates a new instance of DesktopFrame each time.
class OCTK_MEDIA_API FakeDesktopCapturer : public DesktopCapturer
{
public:
    FakeDesktopCapturer();
    ~FakeDesktopCapturer() override;

    // Decides the result which will be returned in next Capture() callback.
    void set_result(DesktopCapturer::Result result);

    // Uses the `generator` provided as DesktopFrameGenerator, FakeDesktopCapturer
    // does not take the ownership of `generator`.
    void set_frame_generator(DesktopFrameGenerator *generator);

    // Count of DesktopFrame(s) have been returned by this instance. This field
    // would never be negative.
    int num_frames_captured() const;

    // Count of CaptureFrame() calls have been made. This field would never be
    // negative.
    int num_capture_attempts() const;

    // DesktopCapturer interface
    void start(DesktopCapturer::Callback *callback) override;
    void CaptureFrame() override;
    void SetSharedMemoryFactory(std::unique_ptr<SharedMemoryFactory> shared_memory_factory) override;
    bool GetSourceList(DesktopCapturer::SourceList *sources) override;
    bool SelectSource(DesktopCapturer::SourceId id) override;

private:
    static constexpr DesktopCapturer::SourceId kWindowId = 1378277495;
    static constexpr DesktopCapturer::SourceId kScreenId = 1378277496;

    DesktopCapturer::Callback *callback_ = nullptr;
    std::unique_ptr<SharedMemoryFactory> shared_memory_factory_;
    DesktopCapturer::Result result_ = Result::SUCCESS;
    DesktopFrameGenerator *generator_ = nullptr;
    int num_frames_captured_ = 0;
    int num_capture_attempts_ = 0;
};
OCTK_END_NAMESPACE

#endif  // _OCTK_DESKTOP_CAPTURE_FAKE_DESKTOP_CAPTURER_HPP
