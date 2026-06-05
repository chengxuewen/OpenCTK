//
// Created by cxw on 25-8-25.
//
#include <openctk/core/imgui_application.hpp>
#include <openctk/core/logging.hpp>
#include <openctk/core/imgui.hpp>

#include <thread>

int main()
{
    octk::StringView type = "";
    if (0)
    {
        type = octk::constants::kImguiApplicationSDLOpenGL3;
    }
    else if (0)
    {
        type = octk::constants::kImGuiApplicationSDLRenderer3;
    }
    octk::ImGuiApplication::Properties properties;
    properties.title = "test";
    auto imguiApp = octk::ImGuiApplication::Factory::create(type, properties);
    if (!imguiApp)
    {
        OCTK_FATAL("Failed to create ImGuiApplication");
    }
    int count = 0;
    imguiApp->setDrawFunction([&]() { ImGui::Text("This is some useful text. count=%d", count); });
    auto thread = std::thread(
        [&]()
        {
            while (!imguiApp->isFinished())
            {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                count++;
            }
        });
    imguiApp->setQuitFunction([&]() { thread.join(); });
    return imguiApp->exec();
}
