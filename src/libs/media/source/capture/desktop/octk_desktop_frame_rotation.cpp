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

#include <octk_desktop_frame_rotation.hpp>
#include <octk_checks.hpp>

#include <libyuv.h>

OCTK_BEGIN_NAMESPACE

namespace
{
libyuv::RotationMode ToLibyuvRotationMode(Rotation rotation)
{
    switch (rotation)
    {
        case Rotation::CLOCK_WISE_0:
            return libyuv::kRotate0;
        case Rotation::CLOCK_WISE_90:
            return libyuv::kRotate90;
        case Rotation::CLOCK_WISE_180:
            return libyuv::kRotate180;
        case Rotation::CLOCK_WISE_270:
            return libyuv::kRotate270;
    }
    OCTK_DCHECK_NOTREACHED();
    return libyuv::kRotate0;
}

DesktopRect RotateAndOffsetRect(DesktopRect rect,
                                DesktopSize size,
                                Rotation rotation,
                                DesktopVector offset)
{
    DesktopRect result = RotateRect(rect, size, rotation);
    result.Translate(offset);
    return result;
}
}  // namespace

Rotation ReverseRotation(Rotation rotation)
{
    switch (rotation)
    {
        case Rotation::CLOCK_WISE_0:
            return rotation;
        case Rotation::CLOCK_WISE_90:
            return Rotation::CLOCK_WISE_270;
        case Rotation::CLOCK_WISE_180:
            return Rotation::CLOCK_WISE_180;
        case Rotation::CLOCK_WISE_270:
            return Rotation::CLOCK_WISE_90;
    }
    OCTK_DCHECK_NOTREACHED();
    return Rotation::CLOCK_WISE_0;
}

DesktopSize RotateSize(DesktopSize size, Rotation rotation)
{
    switch (rotation)
    {
        case Rotation::CLOCK_WISE_0:
        case Rotation::CLOCK_WISE_180:
            return size;
        case Rotation::CLOCK_WISE_90:
        case Rotation::CLOCK_WISE_270:
            return DesktopSize(size.height(), size.width());
    }
    OCTK_DCHECK_NOTREACHED();
    return DesktopSize();
}

DesktopRect RotateRect(DesktopRect rect, DesktopSize size, Rotation rotation)
{
    switch (rotation)
    {
        case Rotation::CLOCK_WISE_0:
            return rect;
        case Rotation::CLOCK_WISE_90:
            return DesktopRect::MakeXYWH(size.height() - rect.bottom(), rect.left(),
                                         rect.height(), rect.width());
        case Rotation::CLOCK_WISE_180:
            return DesktopRect::MakeXYWH(size.width() - rect.right(),
                                         size.height() - rect.bottom(), rect.width(),
                                         rect.height());
        case Rotation::CLOCK_WISE_270:
            return DesktopRect::MakeXYWH(rect.top(), size.width() - rect.right(),
                                         rect.height(), rect.width());
    }
    OCTK_DCHECK_NOTREACHED();
    return DesktopRect();
}

void RotateDesktopFrame(const DesktopFrame &source,
                        const DesktopRect &source_rect,
                        const Rotation &rotation,
                        const DesktopVector &target_offset,
                        DesktopFrame *target)
{
    OCTK_DCHECK(target);
    OCTK_DCHECK(DesktopRect::MakeSize(source.size()).ContainsRect(source_rect));
    // The rectangle in `target`.
    const DesktopRect target_rect = RotateAndOffsetRect(source_rect, source.size(), rotation, target_offset);
    OCTK_DCHECK(DesktopRect::MakeSize(target->size()).ContainsRect(target_rect));

    if (target_rect.is_empty())
    {
        return;
    }

    int result = libyuv::ARGBRotate(source.GetFrameDataAtPos(source_rect.top_left()), source.stride(),
                                    target->GetFrameDataAtPos(target_rect.top_left()), target->stride(),
                                    source_rect.width(), source_rect.height(),
                                    ToLibyuvRotationMode(rotation));
    OCTK_DCHECK_EQ(result, 0);
}
OCTK_END_NAMESPACE
