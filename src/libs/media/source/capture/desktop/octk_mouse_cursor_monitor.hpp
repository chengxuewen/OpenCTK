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

#ifndef _OCTK_DESKTOP_CAPTURE_MOUSE_CURSOR_MONITOR_HPP
#define _OCTK_DESKTOP_CAPTURE_MOUSE_CURSOR_MONITOR_HPP

#include <octk_desktop_capture_types.hpp>
#include <octk_desktop_geometry.hpp>

#include <memory>

OCTK_BEGIN_NAMESPACE

class DesktopCaptureOptions;

class DesktopFrame;

class MouseCursor;

// Captures mouse shape and position.
class MouseCursorMonitor
{
public:
    // Deprecated: CursorState will not be provided.
    enum CursorState
    {
        // Cursor on top of the window including window decorations.
        INSIDE,

        // Cursor is outside of the window.
        OUTSIDE,
    };

    enum Mode
    {
        // Capture only shape of the mouse cursor, but not position.
        SHAPE_ONLY,

        // Capture both, mouse cursor shape and position.
        SHAPE_AND_POSITION,
    };

    // Callback interface used to pass current mouse cursor position and shape.
    class Callback
    {
    public:
        // Called in response to Capture() when the cursor shape has changed. Must
        // take ownership of `cursor`.
        virtual void OnMouseCursor(MouseCursor *cursor) = 0;

        // Called in response to Capture(). `position` indicates cursor position
        // relative to the `window` specified in the constructor.
        // Deprecated: use the following overload instead.
        virtual void OnMouseCursorPosition(CursorState /* state */,
                                           const DesktopVector & /* position */) {}

        // Called in response to Capture(). `position` indicates cursor absolute
        // position on the system in fullscreen coordinate, i.e. the top-left
        // monitor always starts from (0, 0).
        // The coordinates of the position is controlled by OS, but it's always
        // consistent with DesktopFrame.rect().top_left().
        // TODO(zijiehe): Ensure all implementations return the absolute position.
        // TODO(zijiehe): Current this overload works correctly only when capturing
        // mouse cursor against fullscreen.
        virtual void OnMouseCursorPosition(const DesktopVector & /* position */) {}

    protected:
        virtual ~Callback() {}
    };

    virtual ~MouseCursorMonitor() {}

    // Creates a capturer that notifies of mouse cursor events while the cursor is
    // over the specified window.
    //
    // Deprecated: use Create() function.
    static MouseCursorMonitor *CreateForWindow(const DesktopCaptureOptions &options,
                                               WindowId window);

    // Creates a capturer that monitors the mouse cursor shape and position over
    // the specified screen.
    //
    // Deprecated: use Create() function.
    static OCTK_MEDIA_API MouseCursorMonitor *CreateForScreen(const DesktopCaptureOptions &options,
                                                              ScreenId screen);

    // Creates a capturer that monitors the mouse cursor shape and position across
    // the entire desktop. The capturer ensures that the top-left monitor starts
    // from (0, 0).
    static OCTK_MEDIA_API std::unique_ptr<MouseCursorMonitor> Create(const DesktopCaptureOptions &options);

    // Initializes the monitor with the `callback`, which must remain valid until
    // capturer is destroyed.
    virtual void init(Callback *callback, Mode mode) = 0;

    // Captures current cursor shape and position (depending on the `mode` passed
    // to init()). Calls Callback::OnMouseCursor() if cursor shape has
    // changed since the last call (or when Capture() is called for the first
    // time) and then Callback::OnMouseCursorPosition() if mode is set to
    // SHAPE_AND_POSITION.
    virtual void Capture() = 0;
};
OCTK_END_NAMESPACE

#endif  // _OCTK_DESKTOP_CAPTURE_MOUSE_CURSOR_MONITOR_HPP
