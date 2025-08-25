/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present chengxuewen.
** Copyright (c) 2013 The WebRTC project authors.
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

#ifndef _OCTK_MAC_DESKTOP_CONFIGURATION_HPP
#define _OCTK_MAC_DESKTOP_CONFIGURATION_HPP

#include <octk_desktop_geometry.hpp>

#include <ApplicationServices/ApplicationServices.h>

#include <vector>

OCTK_BEGIN_NAMESPACE

// Describes the configuration of a specific display.
struct MacDisplayConfiguration
{
    MacDisplayConfiguration();
    MacDisplayConfiguration(const MacDisplayConfiguration &other);
    MacDisplayConfiguration(MacDisplayConfiguration &&other);
    ~MacDisplayConfiguration();

    MacDisplayConfiguration &operator=(const MacDisplayConfiguration &other);
    MacDisplayConfiguration &operator=(MacDisplayConfiguration &&other);

    // Cocoa identifier for this display.
    CGDirectDisplayID id = 0;

    // Bounds of this display in Density-Independent Pixels (DIPs).
    DesktopRect bounds;

    // Bounds of this display in physical pixels.
    DesktopRect pixel_bounds;

    // Scale factor from DIPs to physical pixels.
    float dip_to_pixel_scale = 1.0f;

    // Display type, built-in or external.
    bool is_builtin;
};

typedef std::vector<MacDisplayConfiguration> MacDisplayConfigurations;

// Describes the configuration of the whole desktop.
struct OCTK_MEDIA_API MacDesktopConfiguration
{
    // Used to request bottom-up or top-down coordinates.
    enum Origin { BottomLeftOrigin, TopLeftOrigin };

    MacDesktopConfiguration();
    MacDesktopConfiguration(const MacDesktopConfiguration &other);
    MacDesktopConfiguration(MacDesktopConfiguration &&other);
    ~MacDesktopConfiguration();

    MacDesktopConfiguration &operator=(const MacDesktopConfiguration &other);
    MacDesktopConfiguration &operator=(MacDesktopConfiguration &&other);

    // Returns the desktop & display configurations.
    // If BottomLeftOrigin is used, the output is in Cocoa-style "bottom-up"
    // (the origin is the bottom-left of the primary monitor, and coordinates
    // increase as you move up the screen). Otherwise, the configuration will be
    // converted to follow top-left coordinate system as Windows and X11.
    static MacDesktopConfiguration GetCurrent(Origin origin);

    // Returns true if the given desktop configuration equals this one.
    bool Equals(const MacDesktopConfiguration &other);

    // If `id` corresponds to the built-in display, return its configuration,
    // otherwise return the configuration for the display with the specified id,
    // or nullptr if no such display exists.
    const MacDisplayConfiguration *FindDisplayConfigurationById(
        CGDirectDisplayID id);

    // Bounds of the desktop excluding monitors with DPI settings different from
    // the main monitor. In Density-Independent Pixels (DIPs).
    DesktopRect bounds;

    // Same as bounds, but expressed in physical pixels.
    DesktopRect pixel_bounds;

    // Scale factor from DIPs to physical pixels.
    float dip_to_pixel_scale = 1.0f;

    // Configurations of the displays making up the desktop area.
    MacDisplayConfigurations displays;
};
OCTK_END_NAMESPACE

#endif  // _OCTK_MAC_DESKTOP_CONFIGURATION_HPP
