/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present chengxuewen.
** Copyright (c) 2014 The WebRTC project authors.
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


#ifndef _OCTK_MAC_WINDOW_LIST_UTILS_HPP
#define _OCTK_MAC_WINDOW_LIST_UTILS_HPP

#include <octk_function_view.hpp>
#include <octk_desktop_capturer.hpp>
#include <octk_desktop_geometry.hpp>
#include <octk_desktop_capture_types.hpp>
#include <mac/octk_desktop_configuration.hpp>

#include <ApplicationServices/ApplicationServices.h>

#include <string>

OCTK_BEGIN_NAMESPACE

// Iterates all on-screen windows in decreasing z-order and sends them
// one-by-one to `on_window` function. If `on_window` returns false, this
// function returns immediately. GetWindowList() returns false if native APIs
// failed. Menus, dock (if `only_zero_layer`), minimized windows (if
// `ignore_minimized` is true) and any windows which do not have a valid window
// id or title will be ignored.
bool OCTK_MEDIA_API GetWindowList(FunctionView<bool(CFDictionaryRef)> on_window,
                                  bool ignore_minimized,
                                  bool only_zero_layer);

// Another helper function to get the on-screen windows.
bool OCTK_MEDIA_API GetWindowList(DesktopCapturer::SourceList *windows,
                                  bool ignore_minimized,
                                  bool only_zero_layer);

// Returns true if the window is occupying a full screen.
bool IsWindowFullScreen(const MacDesktopConfiguration &desktop_config,
                        CFDictionaryRef window);

// Returns true if the window is occupying a full screen.
bool IsWindowFullScreen(const MacDesktopConfiguration &desktop_config,
                        CGWindowID id);

// Returns true if the `window` is on screen. This function returns false if
// native APIs fail.
bool IsWindowOnScreen(CFDictionaryRef window);

// Returns true if the window is on screen. This function returns false if
// native APIs fail or `id` cannot be found.
bool IsWindowOnScreen(CGWindowID id);

// Returns utf-8 encoded title of `window`. If `window` is not a window or no
// valid title can be retrieved, this function returns an empty string.
std::string GetWindowTitle(CFDictionaryRef window);

// Returns utf-8 encoded title of window `id`. If `id` cannot be found or no
// valid title can be retrieved, this function returns an empty string.
std::string GetWindowTitle(CGWindowID id);

// Returns utf-8 encoded owner name of `window`. If `window` is not a window or
// if no valid owner name can be retrieved, returns an empty string.
std::string GetWindowOwnerName(CFDictionaryRef window);

// Returns utf-8 encoded owner name of the given window `id`. If `id` cannot be
// found or if no valid owner name can be retrieved, returns an empty string.
std::string GetWindowOwnerName(CGWindowID id);

// Returns id of `window`. If `window` is not a window or the window id cannot
// be retrieved, this function returns kNullWindowId.
WindowId GetWindowId(CFDictionaryRef window);

// Returns the pid of the process owning `window`. Return 0 if `window` is not
// a window or no valid owner can be retrieved.
int GetWindowOwnerPid(CFDictionaryRef window);

// Returns the pid of the process owning the window `id`. Return 0 if `id`
// cannot be found or no valid owner can be retrieved.
int GetWindowOwnerPid(CGWindowID id);

// Returns the DIP to physical pixel scale at `position`. `position` is in
// *unscaled* system coordinate, i.e. it's device-independent and the primary
// monitor starts from (0, 0). If `position` is out of the system display, this
// function returns 1.
float GetScaleFactorAtPosition(const MacDesktopConfiguration &desktop_config,
                               DesktopVector position);

// Returns the DIP to physical pixel scale factor of the window with `id`.
// The bounds of the window with `id` is in DIP coordinates and `size` is the
// CGImage size of the window with `id` in physical coordinates. Comparing them
// can give the current scale factor.
// If the window overlaps multiple monitors, OS will decide on which monitor the
// window is displayed and use its scale factor to the window. So this method
// still works.
float GetWindowScaleFactor(CGWindowID id, DesktopSize size);

// Returns the bounds of `window`. If `window` is not a window or the bounds
// cannot be retrieved, this function returns an empty DesktopRect. The returned
// DesktopRect is in system coordinate, i.e. the primary monitor always starts
// from (0, 0).
// Deprecated: This function should be avoided in favor of the overload with
// MacDesktopConfiguration.
DesktopRect GetWindowBounds(CFDictionaryRef window);

// Returns the bounds of window with `id`. If `id` does not represent a window
// or the bounds cannot be retrieved, this function returns an empty
// DesktopRect. The returned DesktopRect is in system coordinates.
// Deprecated: This function should be avoided in favor of the overload with
// MacDesktopConfiguration.
DesktopRect GetWindowBounds(CGWindowID id);
OCTK_END_NAMESPACE

#endif  // _OCTK_MAC_WINDOW_LIST_UTILS_HPP
