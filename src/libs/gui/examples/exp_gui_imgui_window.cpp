//
// Created by cxw on 25-8-25.
//
#include <octk_imgui_window_factory.hpp>
#include <octk_imgui.hpp>

#include <thread>

int main()
{
    auto window = octk::ImGuiWindowFactory::instance()->createImGuiWindow(octk::constants::kImGuiWindowSDLGPU3);
    window->setDrawFunction(
        []()
        {
            octk::ImGui::drawText("This is some useful text.");
            octk::ImGui::drawDemo();
        });
    return window->exec();
}
