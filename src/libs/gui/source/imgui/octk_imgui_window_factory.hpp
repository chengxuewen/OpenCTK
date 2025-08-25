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

#ifndef _OCTK_IMGUI_WINDOW_FACTORY_HPP
#define _OCTK_IMGUI_WINDOW_FACTORY_HPP

#include <octk_imgui_constants.hpp>
#include <octk_imgui_window.hpp>
#include <octk_string_view.hpp>
#include <octk_singleton.hpp>
#include <octk_assert.hpp>

#include <functional>

OCTK_BEGIN_NAMESPACE

using ImGuiWindowCreaterFunction = std::function<ImGuiWindow::UniquePtr()>;

namespace utils
{
template <typename T> ImGuiWindowCreaterFunction makeImGuiWindowCreaterFunction()
{
    return []() -> ImGuiWindow::UniquePtr { return utils::makeUnique<T>(); };
}
} // namespace utils

class ImGuiWindowFactoryPrivate;
class OCTK_GUI_API ImGuiWindowFactory : public Singleton<ImGuiWindowFactory>
{
    OCTK_DECLARE_SINGLETON(ImGuiWindowFactory)
public:
    template <typename T> void registerImGuiWindow(StringView type)
    {
        enum
        {
            Valid = std::is_base_of<ImGuiWindow, T>::value
        };
        OCTK_STATIC_ASSERT_X(Valid, "type must base on ImGuiWindow.");
        this->registerImGuiWindow(type, utils::makeImGuiWindowCreaterFunction<T>());
    }

    std::vector<std::string> imGuiWindowTypes() const;
    ImGuiWindow::UniquePtr createImGuiWindow(StringView type) const;
    void registerImGuiWindow(StringView type, ImGuiWindowCreaterFunction func);

protected:
    ImGuiWindowFactory();
    ~ImGuiWindowFactory() override;

private:
    OCTK_DEFINE_DPTR(ImGuiWindowFactory)
    OCTK_DECLARE_PRIVATE(ImGuiWindowFactory)
    OCTK_DISABLE_COPY_MOVE(ImGuiWindowFactory)
};

OCTK_END_NAMESPACE

#endif // _OCTK_IMGUI_WINDOW_FACTORY_HPP
