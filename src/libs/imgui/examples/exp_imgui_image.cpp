#include <octk_imgui_application.hpp>
#include <octk_imgui_theme.hpp>
#include <octk_logging.hpp>

#include <SDL3/SDL.h>

#include <iostream>
#include <thread>

int main()
{
    // octk::StringView type = octk::constants::kImguiApplicationSDLOpenGL3;
    octk::StringView type = octk::constants::kImGuiApplicationSDLRenderer3;
    octk::ImGuiApplication::Properties properties;
    properties.title = "exp_imgui_theme.cpp";
    auto imguiApp = octk::ImGuiApplication::Factory::create(type, properties);
    octk::ImGuiImage::SharedPtr image;
    auto expected = imguiApp->loadImage("test.bmp");
    if (expected.has_value())
    {
        image = expected.value();
        auto aspectRatio = image->aspectRatio();
        std::cout << "aspectRatio: " << aspectRatio << std::endl;
    }
    else
    {
        std::cerr << "Failed to load image, " << expected.error() << std::endl;
    }
    imguiApp->setInitFunction(
        [&]() {

        });
    imguiApp->setDrawFunction(
        [&]()
        {
            if (image)
            {
                ImGui::Begin("ImGuiImage");
                image->checkUpdateTexture();
                ImGui::Image(image->textureId(), image->scaledSize(500, 500));
                ImGui::End();
            }
        });
    return imguiApp->exec();
}
