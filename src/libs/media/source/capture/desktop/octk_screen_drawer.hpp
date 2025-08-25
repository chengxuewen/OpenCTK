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

#ifndef _OCTK_DESKTOP_CAPTURE_SCREEN_DRAWER_HPP
#define _OCTK_DESKTOP_CAPTURE_SCREEN_DRAWER_HPP

#include <octk_desktop_capture_types.hpp>
#include <octk_desktop_geometry.hpp>
#include <octk_rgba_color.hpp>

OCTK_BEGIN_NAMESPACE

// A cross-process lock to ensure only one ScreenDrawer can be used at a certain
// time.
class ScreenDrawerLock
{
public:
    virtual ~ScreenDrawerLock();

    static std::unique_ptr<ScreenDrawerLock> Create();

protected:
    ScreenDrawerLock();
};

// A set of basic platform dependent functions to draw various shapes on the
// screen.
class ScreenDrawer
{
public:
    // Creates a ScreenDrawer for the current platform, returns nullptr if no
    // ScreenDrawer implementation available.
    // If the implementation cannot guarantee two ScreenDrawer instances won't
    // impact each other, this function may block current thread until another
    // ScreenDrawer has been destroyed.
    static std::unique_ptr<ScreenDrawer> Create();

    ScreenDrawer();
    virtual ~ScreenDrawer();

    // Returns the region inside which DrawRectangle() function are expected to
    // work, in capturer coordinates (assuming ScreenCapturer::SelectScreen has
    // not been called). This region may exclude regions of the screen reserved by
    // the OS for things like menu bars or app launchers. The DesktopRect is in
    // system coordinate, i.e. the primary monitor always starts from (0, 0).
    virtual DesktopRect DrawableRegion() = 0;

    // Draws a rectangle to cover `rect` with `color`. Note, rect.bottom() and
    // rect.right() two lines are not included. The part of `rect` which is out of
    // DrawableRegion() will be ignored.
    virtual void DrawRectangle(DesktopRect rect, RgbaColor color) = 0;

    // Clears all content on the screen by filling the area with black.
    virtual void Clear() = 0;

    // Blocks current thread until OS finishes previous DrawRectangle() actions.
    // ScreenCapturer should be able to capture the changes after this function
    // finish.
    virtual void WaitForPendingDraws() = 0;

    // Returns true if incomplete shapes previous actions required may be drawn on
    // the screen after a WaitForPendingDraws() call. i.e. Though the complete
    // shapes will eventually be drawn on the screen, due to some OS limitations,
    // these shapes may be partially appeared sometimes.
    virtual bool MayDrawIncompleteShapes() = 0;

    // Returns the id of the drawer window. This function returns kNullWindowId if
    // the implementation does not draw on a window of the system.
    virtual WindowId window_id() const = 0;
};
OCTK_END_NAMESPACE

#endif  // _OCTK_DESKTOP_CAPTURE_SCREEN_DRAWER_HPP
