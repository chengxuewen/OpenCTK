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

#ifndef _OCTK_MAC_DESKTOP_CONFIGURATION_MONITOR_HPP
#define _OCTK_MAC_DESKTOP_CONFIGURATION_MONITOR_HPP

#include <mac/octk_desktop_configuration.hpp>
#include <octk_mutex.hpp>

#include <ApplicationServices/ApplicationServices.h>

#include <memory>
#include <set>

OCTK_BEGIN_NAMESPACE

// The class provides functions to synchronize capturing and display
// reconfiguring across threads, and the up-to-date MacDesktopConfiguration.
class DesktopConfigurationMonitor final
{
public:
    DesktopConfigurationMonitor();
    ~DesktopConfigurationMonitor();

    DesktopConfigurationMonitor(const DesktopConfigurationMonitor &) = delete;
    DesktopConfigurationMonitor &operator=(const DesktopConfigurationMonitor &) = delete;

    // Returns the current desktop configuration.
    MacDesktopConfiguration desktop_configuration();

private:
    static void DisplaysReconfiguredCallback(CGDirectDisplayID display,
                                             CGDisplayChangeSummaryFlags flags,
                                             void *user_parameter);
    void DisplaysReconfigured(CGDirectDisplayID display,
                              CGDisplayChangeSummaryFlags flags);

    Mutex desktop_configuration_lock_;
    MacDesktopConfiguration desktop_configuration_ OCTK_ATTRIBUTE_GUARDED_BY(&desktop_configuration_lock_);
    std::set<CGDirectDisplayID> reconfiguring_displays_;
};
OCTK_END_NAMESPACE

#endif  // _OCTK_MAC_DESKTOP_CONFIGURATION_MONITOR_HPP
