#include <octk_imgui_application.hpp>
#include <octk_imgui_theme.hpp>
#include <octk_logging.hpp>

#include <iostream>
#include <thread>

namespace constants
{
OCTK_STATIC_CONSTANT_STRING(kDockWindow, "DockWindow");
OCTK_STATIC_CONSTANT_STRING(kDockSpace, "DockSpace");

OCTK_STATIC_CONSTANT_STRING(kClientWindow, "Client");
OCTK_STATIC_CONSTANT_STRING(kLocalMediaWindow, "LocalMedia");
OCTK_STATIC_CONSTANT_STRING(kRemoteMediaWindow, "RemoteMedia");
OCTK_STATIC_CONSTANT_STRING(kStatsReportWindow, "StatsReport");
OCTK_STATIC_CONSTANT_STRING(kOutputLogInfoWindow, "OutputLogInfo");
OCTK_STATIC_CONSTANT_STRING(kOutputStatusWindow, "OutputStatus");
}; // namespace constants

struct WindowData
{
    ImVec2 statusBarPos()
    {
        const auto size = this->statusBarSize();
        auto viewport = ImGui::GetMainViewport();
        return {viewport->Pos.x, viewport->Size.y - size.y};
    }
    ImVec2 statusBarSize()
    {
        const float height = ImGui::GetFrameHeight() * statusBar.floatRatio;
        return {ImGui::GetMainViewport()->Size.x, height};
    }
    ImVec2 dockSpaceSize() { return this->contentSize(); }
    ImVec2 contentSize()
    {
        // ImGui::GetWindowSize()
        // ImVec2 size = ImGui::GetContentRegionAvail();
        ImVec2 size = ImGui::GetMainViewport()->WorkSize;
        if (mainMenu.viewMenu.showStatusBar)
        {
            size.y -= this->statusBarSize().y;
        }
        return size;
    }

    struct
    {
        /* view menus */
        struct
        {
            bool showStatusBar = true;
            bool showStatusFps = true;
        } viewMenu;
        /* tools menus */
        struct
        {
            bool showMetrics = false;
            bool showDebugLog = false;
            bool showIDStackTool = false;
            bool showStyleEditor = false;
            bool showAbout = false;
        } toolsMenu;
    } mainMenu;

    struct
    {
        float floatRatio = 1.4f;
    } statusBar;

    struct
    {
        bool layoutReset = false;
    } dockLayout;
};

void ShowThemeTweakGuiWindow(bool* p_open, octk::ImGuiTheme::TweakedTheme &tweakedTheme)
{
    bool showWindow = true;
    if (p_open != nullptr)
    {
        showWindow = *p_open;
    }

    if (showWindow)
    {
        float k = ImGui::GetFontSize();
        ImGui::SetNextWindowSize(ImVec2(20.f * k, 46.f * k), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Theme Tweaks", p_open))
        {
            if (octk::ImGuiTheme::showThemeTweakGui(&tweakedTheme))
            {
                octk::ImGuiTheme::applyTweakedTheme(tweakedTheme);
            }
        }
        ImGui::End();
    }
}

int main()
{
    WindowData windowData;
    octk::StringView type = octk::constants::kImGuiApplicationSDLGPU3;
    octk::ImGuiApplication::Properties properties;
    properties.title = "exp_imgui_theme.cpp";
    auto imguiApp = octk::ImGuiApplication::Factory::create(type, properties);
    imguiApp->setInitFunction([&]() {});
    imguiApp->setDrawFunction(
        [&]()
        {
            static bool gShowTweakWindow = false;
            static octk::ImGuiTheme::TweakedTheme tweakedTheme;
            if (ImGui::BeginMainMenuBar())
            {
                if (ImGui::BeginMenu("View"))
                {
                    if (ImGui::BeginMenu("Theme"))
                    {
                        if (ImGui::MenuItem("Theme tweak window", nullptr, gShowTweakWindow))
                        {
                            gShowTweakWindow = !gShowTweakWindow;
                        }
                        ImGui::Separator();
                        for (int i = 0; i < octk::ImGuiTheme::ThemeTypeNum; ++i)
                        {
                            auto theme = static_cast<octk::ImGuiTheme::ThemeType>(i);
                            bool selected = (theme == tweakedTheme.theme);
                            if (ImGui::MenuItem(octk::ImGuiTheme::themeTypeName(theme), nullptr, selected))
                            {
                                tweakedTheme.theme = theme;
                                octk::ImGuiTheme::applyTheme(theme);
                            }
                        }
                        ImGui::EndMenu();
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMainMenuBar();
            }

            ShowThemeTweakGuiWindow(&gShowTweakWindow, tweakedTheme);
            ImGui::ShowDemoWindow();
        });
    return imguiApp->exec();
}
