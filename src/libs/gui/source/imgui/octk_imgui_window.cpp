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

#include <private/octk_imgui_window_p.hpp>

OCTK_BEGIN_NAMESPACE

ImGuiWindowPrivate::ImGuiWindowPrivate(ImGuiWindow *p)
    : mPPtr(p)
{
}

ImGuiWindowPrivate::~ImGuiWindowPrivate() { }

ImGuiWindow::ImGuiWindow()
    : ImGuiWindow(new ImGuiWindowPrivate(this))
{
}
ImGuiWindow::ImGuiWindow(ImGuiWindowPrivate *d)
    : mDPtr(d)
{
}

ImGuiWindow::~ImGuiWindow() { }

std::string ImGuiWindow::lastError() const
{
    OCTK_D(const ImGuiWindow);
    return d->mLastError;
}

void ImGuiWindow::setError(const std::string &error)
{
    OCTK_D(ImGuiWindow);
    if (error != d->mLastError)
    {
        d->mLastError = error;
    }
}

void ImGuiWindow::setDrawFunction(DrawFunction func)
{
    OCTK_D(ImGuiWindow);
    SpinLock::Locker locker(d->mDrawFunctionSpinLock);
    d->mDrawFunction = func;
}

bool ImGuiWindow::exec()
{
    OCTK_D(ImGuiWindow);
    d->mLooping.store(true);
    while (d->mLooping.load())
    {
        if (!this->render())
        {
            return false;
        }
    }
    return true;
}

void ImGuiWindow::stopExec()
{
    OCTK_D(ImGuiWindow);
    d->mLooping.store(false);
}

OCTK_END_NAMESPACE