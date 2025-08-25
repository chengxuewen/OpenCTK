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

#ifndef _OCTK_DESKTOP_CAPTURE_RGBA_COLOR_HPP
#define _OCTK_DESKTOP_CAPTURE_RGBA_COLOR_HPP

#include <octk_desktop_frame.hpp>

#include <stdint.h>

OCTK_BEGIN_NAMESPACE

// A four-byte structure to store a color in BGRA format. This structure also
// provides functions to be created from uint8_t array, say,
// DesktopFrame::data(). It always uses BGRA order for internal storage to match
// DesktopFrame::data().
struct RgbaColor final
{
    // Creates a color with BGRA channels.
    RgbaColor(uint8_t blue, uint8_t green, uint8_t red, uint8_t alpha);

    // Creates a color with BGR channels, and set alpha channel to 255 (opaque).
    RgbaColor(uint8_t blue, uint8_t green, uint8_t red);

    // Creates a color from four-byte in BGRA order, i.e. DesktopFrame::data().
    explicit RgbaColor(const uint8_t *bgra);

    // Creates a color from BGRA channels in a uint format. Consumers should make
    // sure the memory order of the uint32_t is always BGRA from left to right, no
    // matter the system endian. This function creates an equivalent RgbaColor
    // instance from the ToUInt32() result of another RgbaColor instance.
    explicit RgbaColor(uint32_t bgra);

    // Returns true if `this` and `right` is the same color.
    bool operator==(const RgbaColor &right) const;

    // Returns true if `this` and `right` are different colors.
    bool operator!=(const RgbaColor &right) const;

    uint32_t ToUInt32() const;

    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t alpha;
};

static_assert(DesktopFrame::kBytesPerPixel == sizeof(RgbaColor),
              "A pixel in DesktopFrame should be safe to be represented by a RgbaColor");
OCTK_END_NAMESPACE

#endif  // _OCTK_DESKTOP_CAPTURE_RGBA_COLOR_HPP
