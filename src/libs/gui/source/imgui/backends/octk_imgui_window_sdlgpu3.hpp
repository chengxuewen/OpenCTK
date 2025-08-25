//
// Created by cxw on 25-8-25.
//

#ifndef _OCTK_IMGUI_WINDOW_SDLGPU3_HPP
#define _OCTK_IMGUI_WINDOW_SDLGPU3_HPP

#include <octk_imgui_window.hpp>

OCTK_BEGIN_NAMESPACE

class ImGuiWindowSDLGPU3Private;
class ImGuiWindowSDLGPU3 : public ImGuiWindow
{
public:
    ImGuiWindowSDLGPU3();
    ~ImGuiWindowSDLGPU3() override;

    bool init() override;
    bool render() override;
    bool destroy() override;
    std::string typeName() const override;

protected:
    OCTK_DECLARE_PRIVATE(ImGuiWindowSDLGPU3)
    OCTK_DISABLE_COPY_MOVE(ImGuiWindowSDLGPU3)
};

OCTK_END_NAMESPACE

#endif // _OCTK_IMGUI_WINDOW_SDLGPU3_HPP
