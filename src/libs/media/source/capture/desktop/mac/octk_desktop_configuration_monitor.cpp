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

#include <mac/octk_desktop_configuration_monitor.hpp>
#include <mac/octk_desktop_configuration.hpp>
// #include <octk_trace_event.hpp>
#include <octk_logging.hpp>

OCTK_BEGIN_NAMESPACE

DesktopConfigurationMonitor::DesktopConfigurationMonitor()
{
    CGError err = CGDisplayRegisterReconfigurationCallback(
        DesktopConfigurationMonitor::DisplaysReconfiguredCallback, this);
    if (err != kCGErrorSuccess)
    {
        OCTK_ERROR() << "CGDisplayRegisterReconfigurationCallback " << err;
    }
    Mutex::Locker locker(&desktop_configuration_lock_);
    desktop_configuration_ = MacDesktopConfiguration::GetCurrent(MacDesktopConfiguration::TopLeftOrigin);
}

DesktopConfigurationMonitor::~DesktopConfigurationMonitor()
{
    CGError err = CGDisplayRemoveReconfigurationCallback(DesktopConfigurationMonitor::DisplaysReconfiguredCallback,
                                                         this);
    if (err != kCGErrorSuccess)
    {
        OCTK_ERROR() << "CGDisplayRemoveReconfigurationCallback " << err;
    }
}

MacDesktopConfiguration DesktopConfigurationMonitor::desktop_configuration()
{
    Mutex::Locker locker(&desktop_configuration_lock_);
    return desktop_configuration_;
}

// static
// This method may be called on any system thread.
void DesktopConfigurationMonitor::DisplaysReconfiguredCallback(CGDirectDisplayID display,
                                                               CGDisplayChangeSummaryFlags flags,
                                                               void *user_parameter)
{
    DesktopConfigurationMonitor *monitor = reinterpret_cast<DesktopConfigurationMonitor *>(user_parameter);
    monitor->DisplaysReconfigured(display, flags);
}

void DesktopConfigurationMonitor::DisplaysReconfigured(
    CGDirectDisplayID display,
    CGDisplayChangeSummaryFlags flags)
{
    // TRACE_EVENT0("webrtc", "DesktopConfigurationMonitor::DisplaysReconfigured");
    OCTK_INFO() << "DisplaysReconfigured: DisplayID "
                << display << "; ChangeSummaryFlags " << flags;

    if (flags & kCGDisplayBeginConfigurationFlag)
    {
        reconfiguring_displays_.insert(display);
        return;
    }

    reconfiguring_displays_.erase(display);
    if (reconfiguring_displays_.empty())
    {
        Mutex::Locker locker(&desktop_configuration_lock_);
        desktop_configuration_ = MacDesktopConfiguration::GetCurrent(MacDesktopConfiguration::TopLeftOrigin);
    }
}
OCTK_END_NAMESPACE
