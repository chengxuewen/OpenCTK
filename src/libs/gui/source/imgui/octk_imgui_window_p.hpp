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

#ifndef _OCTK_IMGUI_WINDOW_P_HPP
#define _OCTK_IMGUI_WINDOW_P_HPP

#include <octk_imgui_constants.hpp>
#include <private/octk_sdl_p.hpp>
#include <octk_imgui_window.hpp>
#include <octk_once_flag.hpp>
#include <octk_spinlock.hpp>

#include <imgui.h>

OCTK_BEGIN_NAMESPACE

class ImGuiWindowPrivate
{
protected:
    OCTK_DEFINE_PPTR(ImGuiWindow)
    OCTK_DECLARE_PUBLIC(ImGuiWindow)
    OCTK_DISABLE_COPY_MOVE(ImGuiWindowPrivate)
public:
    using DrawFunction = ImGuiWindow::DrawFunction;

    explicit ImGuiWindowPrivate(ImGuiWindow *p);
    virtual ~ImGuiWindowPrivate();

    std::atomic_bool mLooping{false};
    const ImVec4 mClearColor{0.45f, 0.55f, 0.60f, 1.00f};

    std::string mLastError;
    DrawFunction mDrawFunction;
    mutable SpinLock mDrawFunctionSpinLock;
};

OCTK_END_NAMESPACE

#endif // _OCTK_IMGUI_WINDOW_P_HPP
