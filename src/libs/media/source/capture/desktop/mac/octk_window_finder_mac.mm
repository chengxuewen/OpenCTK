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

#include <mac/octk_desktop_configuration_monitor.hpp>
#include <mac/octk_desktop_configuration.hpp>
#include <mac/octk_window_list_utils.hpp>
#include <mac/octk_window_finder_mac.hpp>
#include <octk_memory.hpp>

#include <CoreFoundation/CoreFoundation.h>

#include <memory>
#include <utility>

OCTK_BEGIN_NAMESPACE

WindowFinderMac::WindowFinderMac(std::shared_ptr<DesktopConfigurationMonitor> configuration_monitor)
    : configuration_monitor_(std::move(configuration_monitor)) {}
WindowFinderMac::~WindowFinderMac() = default;

WindowId WindowFinderMac::GetWindowUnderPoint(DesktopVector point)
{
    WindowId id = kNullWindowId;
    GetWindowList(
        [&id, point](CFDictionaryRef window) {
            DesktopRect bounds;
            bounds = GetWindowBounds(window);
            if (bounds.Contains(point))
            {
                id = GetWindowId(window);
                return false;
            }
            return true;
        },
        true,
        true);
    return id;
}

// static
std::unique_ptr<WindowFinder> WindowFinder::Create(const WindowFinder::Options &options)
{
    return utils::make_unique<WindowFinderMac>(options.configuration_monitor);
}
OCTK_END_NAMESPACE
