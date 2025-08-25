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

#ifndef _OCTK_IMGUI_WINDOW_HPP
#define _OCTK_IMGUI_WINDOW_HPP

#include <octk_gui_global.hpp>
#include <octk_memory.hpp>

#include <functional>

OCTK_BEGIN_NAMESPACE

class ImGuiWindowPrivate;
class OCTK_GUI_API ImGuiWindow
{
public:
    using DrawFunction = std::function<void()>;

    using SharedPtr = std::shared_ptr<ImGuiWindow>;
    using UniquePtr = std::unique_ptr<ImGuiWindow>;

    ImGuiWindow();
    ImGuiWindow(ImGuiWindowPrivate *d);
    virtual ~ImGuiWindow();

    std::string lastError() const;
    void setDrawFunction(DrawFunction func);

    bool exec();
    void stopExec();

    virtual bool init() = 0;
    virtual bool render() = 0;
    virtual bool destroy() = 0;
    virtual std::string typeName() const = 0;

protected:
    void setError(const std::string &error);

protected:
    OCTK_DEFINE_DPTR(ImGuiWindow)
    OCTK_DECLARE_PRIVATE(ImGuiWindow)
    OCTK_DISABLE_COPY_MOVE(ImGuiWindow)
};

OCTK_END_NAMESPACE

#endif // _OCTK_IMGUI_WINDOW_HPP
