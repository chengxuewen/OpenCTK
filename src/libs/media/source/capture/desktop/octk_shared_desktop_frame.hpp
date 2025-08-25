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

#ifndef _OCTK_SHARED_DESKTOP_FRAME_HPP
#define _OCTK_SHARED_DESKTOP_FRAME_HPP

#include <octk_desktop_frame.hpp>

#include <memory>

OCTK_BEGIN_NAMESPACE

// SharedDesktopFrame is a DesktopFrame that may have multiple instances all sharing the same buffer.
class OCTK_MEDIA_API SharedDesktopFrame final : public DesktopFrame
{
public:
    ~SharedDesktopFrame() override;

    SharedDesktopFrame(const SharedDesktopFrame &) = delete;
    SharedDesktopFrame &operator=(const SharedDesktopFrame &) = delete;

    static std::unique_ptr<SharedDesktopFrame> Wrap(std::unique_ptr<DesktopFrame> desktop_frame);

    // Deprecated.
    // TODO(sergeyu): remove this method.
    static SharedDesktopFrame *Wrap(DesktopFrame *desktop_frame);

    // Deprecated. Clients do not need to know the underlying DesktopFrame
    // instance.
    // TODO(zijiehe): Remove this method.
    // Returns the underlying instance of DesktopFrame.
    DesktopFrame *GetUnderlyingFrame();

    // Returns whether `this` and `other` share the underlying DesktopFrame.
    bool ShareFrameWith(const SharedDesktopFrame &other) const;

    // Creates a clone of this object.
    std::unique_ptr<SharedDesktopFrame> Share();

    // Checks if the frame is currently shared. If it returns false it's
    // guaranteed that there are no clones of the object.
    bool IsShared();

private:
    // typedef rtc::FinalRefCountedObject <std::unique_ptr<DesktopFrame>> Core;

    SharedDesktopFrame(const std::shared_ptr<DesktopFrame> &frame);

    const std::shared_ptr<DesktopFrame> core_;
};
OCTK_END_NAMESPACE

#endif  // _OCTK_SHARED_DESKTOP_FRAME_HPP
