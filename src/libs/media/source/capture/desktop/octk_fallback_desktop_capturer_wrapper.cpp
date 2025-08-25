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

#include <octk_fallback_desktop_capturer_wrapper.hpp>
#include <octk_metrics.hpp>
#include <octk_checks.hpp>
// #include "api/sequence_checker.h"
// #include "rtc_base/checks.h"
// #include "system_wrappers/include/metrics.h"

#include <stddef.h>

#include <utility>

OCTK_BEGIN_NAMESPACE

namespace
{

// Implementation to share a SharedMemoryFactory between DesktopCapturer
// instances. This class is designed for synchronized DesktopCapturer
// implementations only.
class SharedMemoryFactoryProxy : public SharedMemoryFactory
{
public:
    // Users should maintain the lifetime of `factory` to ensure it overlives
    // current instance.
    static std::unique_ptr<SharedMemoryFactory> Create(SharedMemoryFactory *factory);
    ~SharedMemoryFactoryProxy() override;

    // Forwards CreateSharedMemory() calls to `factory_`. Users should always call
    // this function in one thread. Users should not call this function after the
    // SharedMemoryFactory which current instance created from has been destroyed.
    std::unique_ptr<SharedMemory> CreateSharedMemory(size_t size) override;

private:
    explicit SharedMemoryFactoryProxy(SharedMemoryFactory *factory);

    SharedMemoryFactory *factory_ = nullptr;
    // SequenceChecker thread_checker_;
};
}  // namespace

SharedMemoryFactoryProxy::SharedMemoryFactoryProxy(SharedMemoryFactory *factory)
{
    OCTK_DCHECK(factory);
    factory_ = factory;
}

// static
std::unique_ptr<SharedMemoryFactory> SharedMemoryFactoryProxy::Create(
    SharedMemoryFactory *factory)
{
    return std::unique_ptr<SharedMemoryFactory>(new SharedMemoryFactoryProxy(factory));
}

SharedMemoryFactoryProxy::~SharedMemoryFactoryProxy() = default;

std::unique_ptr<SharedMemory> SharedMemoryFactoryProxy::CreateSharedMemory(size_t size)
{
    // OCTK_DCHECK(thread_checker_.IsCurrent());
    return factory_->CreateSharedMemory(size);
}

FallbackDesktopCapturerWrapper::FallbackDesktopCapturerWrapper(std::unique_ptr<DesktopCapturer> main_capturer,
                                                               std::unique_ptr<DesktopCapturer> secondary_capturer)
    : main_capturer_(std::move(main_capturer)), secondary_capturer_(std::move(secondary_capturer))
{
    OCTK_DCHECK(main_capturer_);
    OCTK_DCHECK(secondary_capturer_);
}

FallbackDesktopCapturerWrapper::~FallbackDesktopCapturerWrapper() = default;

void FallbackDesktopCapturerWrapper::start(DesktopCapturer::Callback *callback)
{
    callback_ = callback;
    // FallbackDesktopCapturerWrapper catchs the callback of the main capturer,
    // and checks its return value to decide whether the secondary capturer should
    // be involved.
    main_capturer_->start(this);
    // For the secondary capturer, we do not have a backup plan anymore, so
    // FallbackDesktopCapturerWrapper won't check its return value any more. It
    // will directly return to the input `callback`.
    secondary_capturer_->start(callback);
}

void FallbackDesktopCapturerWrapper::SetSharedMemoryFactory(std::unique_ptr<SharedMemoryFactory> shared_memory_factory)
{
    shared_memory_factory_ = std::move(shared_memory_factory);
    if (shared_memory_factory_)
    {
        main_capturer_->SetSharedMemoryFactory(SharedMemoryFactoryProxy::Create(shared_memory_factory_.get()));
        secondary_capturer_->SetSharedMemoryFactory(SharedMemoryFactoryProxy::Create(shared_memory_factory_.get()));
    }
    else
    {
        main_capturer_->SetSharedMemoryFactory(std::unique_ptr<SharedMemoryFactory>());
        secondary_capturer_->SetSharedMemoryFactory(std::unique_ptr<SharedMemoryFactory>());
    }
}

void FallbackDesktopCapturerWrapper::CaptureFrame()
{
    OCTK_DCHECK(callback_);
    if (main_capturer_permanent_error_)
    {
        secondary_capturer_->CaptureFrame();
    }
    else
    {
        main_capturer_->CaptureFrame();
    }
}

void FallbackDesktopCapturerWrapper::SetExcludedWindow(WindowId window)
{
    main_capturer_->SetExcludedWindow(window);
    secondary_capturer_->SetExcludedWindow(window);
}

bool FallbackDesktopCapturerWrapper::GetSourceList(SourceList *sources)
{
    if (main_capturer_permanent_error_)
    {
        return secondary_capturer_->GetSourceList(sources);
    }
    return main_capturer_->GetSourceList(sources);
}

bool FallbackDesktopCapturerWrapper::SelectSource(SourceId id)
{
    if (main_capturer_permanent_error_)
    {
        return secondary_capturer_->SelectSource(id);
    }
    const bool main_capturer_result = main_capturer_->SelectSource(id);
    OCTK_HISTOGRAM_BOOLEAN("WebRTC.DesktopCapture.PrimaryCapturerSelectSourceError", main_capturer_result);
    if (!main_capturer_result)
    {
        main_capturer_permanent_error_ = true;
    }

    return secondary_capturer_->SelectSource(id);
}

bool FallbackDesktopCapturerWrapper::FocusOnSelectedSource()
{
    if (main_capturer_permanent_error_)
    {
        return secondary_capturer_->FocusOnSelectedSource();
    }
    return main_capturer_->FocusOnSelectedSource() || secondary_capturer_->FocusOnSelectedSource();
}

bool FallbackDesktopCapturerWrapper::IsOccluded(const DesktopVector &pos)
{
    // Returns true if either capturer returns true.
    if (main_capturer_permanent_error_)
    {
        return secondary_capturer_->IsOccluded(pos);
    }
    return main_capturer_->IsOccluded(pos) || secondary_capturer_->IsOccluded(pos);
}

void FallbackDesktopCapturerWrapper::OnCaptureResult(
    Result result,
    std::unique_ptr<DesktopFrame> frame)
{
    OCTK_DCHECK(callback_);
    OCTK_HISTOGRAM_BOOLEAN("WebRTC.DesktopCapture.PrimaryCapturerError",
                           result != Result::SUCCESS);
    OCTK_HISTOGRAM_BOOLEAN("WebRTC.DesktopCapture.PrimaryCapturerPermanentError",
                           result == Result::ERROR_PERMANENT);
    if (result == Result::SUCCESS)
    {
        callback_->OnCaptureResult(result, std::move(frame));
        return;
    }

    if (result == Result::ERROR_PERMANENT)
    {
        main_capturer_permanent_error_ = true;
    }
    secondary_capturer_->CaptureFrame();
}
OCTK_END_NAMESPACE
