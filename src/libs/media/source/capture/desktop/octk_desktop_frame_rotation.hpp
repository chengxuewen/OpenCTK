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

#ifndef _OCTK_DESKTOP_CAPTURE_DESKTOP_FRAME_ROTATION_HPP
#define _OCTK_DESKTOP_CAPTURE_DESKTOP_FRAME_ROTATION_HPP

#include <octk_desktop_geometry.hpp>
#include <octk_desktop_frame.hpp>

OCTK_BEGIN_NAMESPACE

// Represents the rotation of a DesktopFrame.
enum class Rotation
{
    CLOCK_WISE_0,
    CLOCK_WISE_90,
    CLOCK_WISE_180,
    CLOCK_WISE_270,
};

// Rotates input DesktopFrame `source`, copies pixel in an unrotated rectangle
// `source_rect` into the target rectangle of another DesktopFrame `target`.
// Target rectangle here is the rotated `source_rect` plus `target_offset`.
// `rotation` specifies `source` to `target` rotation. `source_rect` is in
// `source` coordinate. `target_offset` is in `target` coordinate.
// This function triggers check failure if `source` does not cover the
// `source_rect`, or `target` does not cover the rotated `rect`.
void RotateDesktopFrame(const DesktopFrame &source,
                        const DesktopRect &source_rect,
                        const Rotation &rotation,
                        const DesktopVector &target_offset,
                        DesktopFrame *target);

// Returns a reverse rotation of `rotation`.
Rotation ReverseRotation(Rotation rotation);

// Returns a rotated DesktopSize of `size`.
DesktopSize RotateSize(DesktopSize size, Rotation rotation);

// Returns a rotated DesktopRect of `rect`. The `size` represents the size of
// the DesktopFrame which `rect` belongs in.
DesktopRect RotateRect(DesktopRect rect, DesktopSize size, Rotation rotation);
OCTK_END_NAMESPACE

#endif  // _OCTK_DESKTOP_CAPTURE_DESKTOP_FRAME_ROTATION_HPP
