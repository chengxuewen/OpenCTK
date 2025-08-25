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

#ifndef _OCTK_DESKTOP_CAPTURE_WINDOW_FINDER_HPP
#define _OCTK_DESKTOP_CAPTURE_WINDOW_FINDER_HPP

#include <octk_desktop_capture_types.hpp>
#include <octk_desktop_geometry.hpp>

#include <memory>

#if defined(OCTK_OS_MAC) && !defined(OCTK_OS_IOS)
#   include <mac/octk_desktop_configuration_monitor.hpp>
#endif

OCTK_BEGIN_NAMESPACE

#if defined(OCTK_USE_X11)
class XAtomCache;
#endif

// An interface to return the id of the visible window under a certain point.
class OCTK_MEDIA_API WindowFinder
{
public:
    WindowFinder() = default;
    virtual ~WindowFinder() = default;

    // Returns the id of the visible window under `point`. This function returns
    // kNullWindowId if no window is under `point` and the platform does not have
    // "root window" concept, i.e. the visible area under `point` is the desktop.
    // `point` is always in system coordinate, i.e. the primary monitor always
    // starts from (0, 0).
    virtual WindowId GetWindowUnderPoint(DesktopVector point) = 0;

    struct OCTK_MEDIA_API Options final
    {
        Options();
        ~Options();
        Options(const Options &other);
        Options(Options &&other);

#if defined(OCTK_USE_X11)
        XAtomCache* cache = nullptr;
#endif
#if defined(OCTK_OS_MAC) && !defined(OCTK_OS_IOS)
        std::shared_ptr<DesktopConfigurationMonitor> configuration_monitor;
#endif
    };

    // Creates a platform-independent WindowFinder implementation. This function
    // returns nullptr if `options` does not contain enough information or
    // WindowFinder does not support current platform.
    static std::unique_ptr<WindowFinder> Create(const Options &options);
};
OCTK_END_NAMESPACE

#endif  // _OCTK_DESKTOP_CAPTURE_WINDOW_FINDER_HPP
