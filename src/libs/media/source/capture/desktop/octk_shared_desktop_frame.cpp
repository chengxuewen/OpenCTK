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

#include <octk_shared_desktop_frame.hpp>

#include <type_traits>
#include <utility>
#include <memory>

OCTK_BEGIN_NAMESPACE

SharedDesktopFrame::~SharedDesktopFrame() {}

// static
std::unique_ptr<SharedDesktopFrame> SharedDesktopFrame::Wrap(std::unique_ptr<DesktopFrame> desktop_frame)
{
    return std::unique_ptr<SharedDesktopFrame>(
        new SharedDesktopFrame(std::shared_ptr<DesktopFrame>(desktop_frame.release())));
}

SharedDesktopFrame *SharedDesktopFrame::Wrap(DesktopFrame *desktop_frame)
{
    return Wrap(std::unique_ptr<DesktopFrame>(desktop_frame)).release();
}

DesktopFrame *SharedDesktopFrame::GetUnderlyingFrame()
{
    return core_.get();
}

bool SharedDesktopFrame::ShareFrameWith(const SharedDesktopFrame &other) const
{
    return core_.get() == other.core_.get();
}

std::unique_ptr<SharedDesktopFrame> SharedDesktopFrame::Share()
{
    std::unique_ptr<SharedDesktopFrame> result(new SharedDesktopFrame(core_));
    result->CopyFrameInfoFrom(*this);
    return result;
}

bool SharedDesktopFrame::IsShared()
{
    return core_.use_count() > 1;
}

SharedDesktopFrame::SharedDesktopFrame(const std::shared_ptr<DesktopFrame> &frame)
    : DesktopFrame(frame->size(),
                   frame->stride(),
                   frame->data(),
                   frame->shared_memory()), core_(frame)
{
    CopyFrameInfoFrom(*(core_.get()));
}
OCTK_END_NAMESPACE
