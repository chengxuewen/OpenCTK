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

#include <octk_desktop_geometry.hpp>

#include <algorithm>
#include <cmath>

OCTK_BEGIN_NAMESPACE

bool DesktopRect::Contains(const DesktopVector& point) const
{
    return point.x() >= left() && point.x() < right() && point.y() >= top() && point.y() < bottom();
}

bool DesktopRect::ContainsRect(const DesktopRect& rect) const
{
    return rect.left() >= left() && rect.right() <= right() && rect.top() >= top() && rect.bottom() <= bottom();
}

void DesktopRect::IntersectWith(const DesktopRect& rect)
{
    left_ = std::max(left(), rect.left());
    top_ = std::max(top(), rect.top());
    right_ = std::min(right(), rect.right());
    bottom_ = std::min(bottom(), rect.bottom());
    if (is_empty())
    {
        left_ = 0;
        top_ = 0;
        right_ = 0;
        bottom_ = 0;
    }
}

void DesktopRect::UnionWith(const DesktopRect& rect)
{
    if (is_empty())
    {
        *this = rect;
        return;
    }

    if (rect.is_empty())
    {
        return;
    }

    left_ = std::min(left(), rect.left());
    top_ = std::min(top(), rect.top());
    right_ = std::max(right(), rect.right());
    bottom_ = std::max(bottom(), rect.bottom());
}

void DesktopRect::Translate(int32_t dx, int32_t dy)
{
    left_ += dx;
    top_ += dy;
    right_ += dx;
    bottom_ += dy;
}

void DesktopRect::Extend(int32_t left_offset, int32_t top_offset, int32_t right_offset, int32_t bottom_offset)
{
    left_ -= left_offset;
    top_ -= top_offset;
    right_ += right_offset;
    bottom_ += bottom_offset;
}

void DesktopRect::Scale(double horizontal, double vertical)
{
    right_ += static_cast<int>(std::round(width() * (horizontal - 1)));
    bottom_ += static_cast<int>(std::round(height() * (vertical - 1)));
}

OCTK_END_NAMESPACE
