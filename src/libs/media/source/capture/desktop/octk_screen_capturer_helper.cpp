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

#include <octk_screen_capturer_helper.hpp>
#include <octk_checks.hpp>

OCTK_BEGIN_NAMESPACE

void ScreenCapturerHelper::ClearInvalidRegion()
{
    Mutex::Locker scoped_invalid_region_lock(&invalid_region_mutex_);
    invalid_region_.Clear();
}

void ScreenCapturerHelper::InvalidateRegion(const DesktopRegion &invalid_region)
{
    Mutex::Locker scoped_invalid_region_lock(&invalid_region_mutex_);
    invalid_region_.AddRegion(invalid_region);
}

void ScreenCapturerHelper::InvalidateScreen(const DesktopSize &size)
{
    Mutex::Locker scoped_invalid_region_lock(&invalid_region_mutex_);
    invalid_region_.AddRect(DesktopRect::MakeSize(size));
}

void ScreenCapturerHelper::TakeInvalidRegion(DesktopRegion *invalid_region)
{
    invalid_region->Clear();

    {
        Mutex::Locker scoped_invalid_region_lock(&invalid_region_mutex_);
        invalid_region->Swap(&invalid_region_);
    }

    if (log_grid_size_ > 0)
    {
        DesktopRegion expanded_region;
        ExpandToGrid(*invalid_region, log_grid_size_, &expanded_region);
        expanded_region.Swap(invalid_region);

        invalid_region->IntersectWith(DesktopRect::MakeSize(size_most_recent_));
    }
}

void ScreenCapturerHelper::SetLogGridSize(int log_grid_size)
{
    log_grid_size_ = log_grid_size;
}

const DesktopSize &ScreenCapturerHelper::size_most_recent() const
{
    return size_most_recent_;
}

void ScreenCapturerHelper::set_size_most_recent(const DesktopSize &size)
{
    size_most_recent_ = size;
}

// Returns the largest multiple of `n` that is <= `x`.
// `n` must be a power of 2. `nMask` is ~(`n` - 1).
static int DownToMultiple(int x, int nMask)
{
    return (x & nMask);
}

// Returns the smallest multiple of `n` that is >= `x`.
// `n` must be a power of 2. `nMask` is ~(`n` - 1).
static int UpToMultiple(int x, int n, int nMask)
{
    return ((x + n - 1) & nMask);
}

void ScreenCapturerHelper::ExpandToGrid(const DesktopRegion &region,
                                        int log_grid_size,
                                        DesktopRegion *result)
{
    OCTK_DCHECK_GE(log_grid_size, 1);
    int grid_size = 1 << log_grid_size;
    int grid_size_mask = ~(grid_size - 1);

    result->Clear();
    for (DesktopRegion::Iterator it(region); !it.IsAtEnd(); it.Advance())
    {
        int left = DownToMultiple(it.rect().left(), grid_size_mask);
        int right = UpToMultiple(it.rect().right(), grid_size, grid_size_mask);
        int top = DownToMultiple(it.rect().top(), grid_size_mask);
        int bottom = UpToMultiple(it.rect().bottom(), grid_size, grid_size_mask);
        result->AddRect(DesktopRect::MakeLTRB(left, top, right, bottom));
    }
}
OCTK_END_NAMESPACE
