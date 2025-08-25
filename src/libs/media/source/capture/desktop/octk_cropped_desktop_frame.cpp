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

#include <octk_cropped_desktop_frame.hpp>
#include <octk_desktop_region.hpp>
#include <octk_checks.hpp>

#include <utility>
#include <memory>

OCTK_BEGIN_NAMESPACE

// A DesktopFrame that is a sub-rect of another DesktopFrame.
class CroppedDesktopFrame : public DesktopFrame
{
public:
    CroppedDesktopFrame(std::unique_ptr<DesktopFrame> frame,
                        const DesktopRect &rect);

    CroppedDesktopFrame(const CroppedDesktopFrame &) = delete;
    CroppedDesktopFrame &operator=(const CroppedDesktopFrame &) = delete;

private:
    const std::unique_ptr<DesktopFrame> frame_;
};

std::unique_ptr<DesktopFrame> CreateCroppedDesktopFrame(
    std::unique_ptr<DesktopFrame> frame,
    const DesktopRect &rect)
{
    OCTK_DCHECK(frame);

    DesktopRect intersection = DesktopRect::MakeSize(frame->size());
    intersection.IntersectWith(rect);
    if (intersection.is_empty())
    {
        return nullptr;
    }

    if (frame->size().equals(rect.size()))
    {
        return frame;
    }

    return std::unique_ptr<DesktopFrame>(new CroppedDesktopFrame(std::move(frame), intersection));
}

CroppedDesktopFrame::CroppedDesktopFrame(std::unique_ptr<DesktopFrame> frame,
                                         const DesktopRect &rect)
    : DesktopFrame(rect.size(),
                   frame->stride(),
                   frame->GetFrameDataAtPos(rect.top_left()),
                   frame->shared_memory()), frame_(std::move(frame))
{
    MoveFrameInfoFrom(frame_.get());
    set_top_left(frame_->top_left().add(rect.top_left()));
    mutable_updated_region()->IntersectWith(rect);
    mutable_updated_region()->Translate(-rect.left(), -rect.top());
}
OCTK_END_NAMESPACE
