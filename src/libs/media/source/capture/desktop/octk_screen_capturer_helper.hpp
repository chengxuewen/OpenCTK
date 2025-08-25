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

#ifndef _OCTK_SCREEN_CAPTURER_HELPER_HPP
#define _OCTK_SCREEN_CAPTURER_HELPER_HPP

#include <octk_desktop_geometry.hpp>
#include <octk_desktop_region.hpp>
#include <octk_mutex.hpp>

#include <memory>

OCTK_BEGIN_NAMESPACE

// ScreenCapturerHelper is intended to be used by an implementation of the
// ScreenCapturer interface. It maintains a thread-safe invalid region, and
// the size of the most recently captured screen, on behalf of the
// ScreenCapturer that owns it.
class ScreenCapturerHelper
{
public:
    ScreenCapturerHelper() = default;
    ~ScreenCapturerHelper() = default;

    ScreenCapturerHelper(const ScreenCapturerHelper &) = delete;
    ScreenCapturerHelper &operator=(const ScreenCapturerHelper &) = delete;

    // Clear out the invalid region.
    void ClearInvalidRegion();

    // Invalidate the specified region.
    void InvalidateRegion(const DesktopRegion &invalid_region);

    // Invalidate the entire screen, of a given size.
    void InvalidateScreen(const DesktopSize &size);

    // Copies current invalid region to `invalid_region` clears invalid region
    // storage for the next frame.
    void TakeInvalidRegion(DesktopRegion *invalid_region);

    // Access the size of the most recently captured screen.
    const DesktopSize &size_most_recent() const;
    void set_size_most_recent(const DesktopSize &size);

    // Lossy compression can result in color values leaking between pixels in one
    // block. If part of a block changes, then unchanged parts of that block can
    // be changed in the compressed output. So we need to re-render an entire
    // block whenever part of the block changes.
    //
    // If `log_grid_size` is >= 1, then this function makes TakeInvalidRegion()
    // produce an invalid region expanded so that its vertices lie on a grid of
    // size 2 ^ `log_grid_size`. The expanded region is then clipped to the size
    // of the most recently captured screen, as previously set by
    // set_size_most_recent().
    // If `log_grid_size` is <= 0, then the invalid region is not expanded.
    void SetLogGridSize(int log_grid_size);

    // Expands a region so that its vertices all lie on a grid.
    // The grid size must be >= 2, so `log_grid_size` must be >= 1.
    static void ExpandToGrid(const DesktopRegion &region,
                             int log_grid_size,
                             DesktopRegion *result);

private:
    // A region that has been manually invalidated (through InvalidateRegion).
    // These will be returned as dirty_region in the capture data during the next
    // capture.
    DesktopRegion invalid_region_ OCTK_ATTRIBUTE_GUARDED_BY(invalid_region_mutex_);

    // A lock protecting `invalid_region_` across threads.
    Mutex invalid_region_mutex_;

    // The size of the most recently captured screen.
    DesktopSize size_most_recent_;

    // The log (base 2) of the size of the grid to which the invalid region is
    // expanded.
    // If the value is <= 0, then the invalid region is not expanded to a grid.
    int log_grid_size_ = 0;
};
OCTK_END_NAMESPACE

#endif  // _OCTK_SCREEN_CAPTURER_HELPER_HPP
