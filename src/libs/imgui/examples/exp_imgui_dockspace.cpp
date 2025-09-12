#include <octk_imgui_application.hpp>
#include <octk_logging.hpp>
#include <octk_imgui.hpp>

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

void setDockSpace(WindowData &windowData)
{
    const auto dockSpaceId = ImGui::GetID(constants::kDockSpace);
    const auto viewport = ImGui::GetMainViewport();
    const auto viewPortId = ImGui::GetID(viewport);

    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowViewport(viewPortId);

    int windowFlags = ImGuiWindowFlags_NoDecoration // 无标题栏、不可改变大小、无滚动条、不可折叠
                      | ImGuiWindowFlags_NoMove     // 不可移动
                      | ImGuiWindowFlags_AlwaysAutoResize // auto resize
                      | ImGuiWindowFlags_NoBackground     // 无背景（背景透明）
                      // | ImGuiWindowFlags_MenuBar               // 菜单栏
                      | ImGuiWindowFlags_NoDocking             // 不可停靠
                      | ImGuiWindowFlags_NoBringToFrontOnFocus // 无法设置前台和聚焦
                      | ImGuiWindowFlags_NoNavFocus            // 无法通过键盘和手柄聚焦
        ;

    // 压入样式设置
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);            // 无边框
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f)); // 无边界
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);              // 无圆角
    // ImGui::SetNextWindowBgAlpha(0.0f); // 窗口 alpha 为 0，同样可以不显示背景
    ImGui::Begin(constants::kDockWindow, nullptr, windowFlags); // 开始停靠窗口
    ImGui::PopStyleVar(3);                                      // 弹出样式设置

    if (windowData.dockLayout.layoutReset)
    {
        ImGui::DockBuilderRemoveNode(dockSpaceId);
        windowData.dockLayout.layoutReset = false;
    }
    const auto dockSpaceSize = windowData.dockSpaceSize();
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        if (!ImGui::DockBuilderGetNode(dockSpaceId))
        {
            ImGui::DockBuilderRemoveNode(dockSpaceId);
            ImGuiID rootDockNodeId = ImGui::DockBuilderAddNode(dockSpaceId, ImGuiDockNodeFlags_None);

            ImGui::DockBuilderSetNodePos(rootDockNodeId, {0.0f, 0.0f});
            ImGui::DockBuilderSetNodeSize(rootDockNodeId, ImGui::GetWindowSize());

            ImGuiID leftDockNodeId =
                ImGui::DockBuilderSplitNode(rootDockNodeId, ImGuiDir_Left, 0.25f, nullptr, &rootDockNodeId);

            // 根节点分割右节点
            ImGuiID rightDockNodeId =
                ImGui::DockBuilderSplitNode(rootDockNodeId, ImGuiDir_Right, 0.25f / 0.75f, nullptr, &rootDockNodeId);

            // 根节点分割下节点
            ImGuiID bottomDockNodeId =
                ImGui::DockBuilderSplitNode(rootDockNodeId, ImGuiDir_Down, 0.25f, nullptr, &rootDockNodeId);

            // 左节点分割上下节点
            ImGuiID leftTopDockNodeId, leftBottomDockNodeId;
            ImGui::DockBuilderSplitNode(leftDockNodeId, ImGuiDir_Up, 0.5f, &leftTopDockNodeId, &leftBottomDockNodeId);

            // 设置节点属性

            // 禁止其他窗口/节点分割根节点
            // ImGui::DockBuilderGetNode(dockspaceID)->LocalFlags |= ImGuiDockNodeFlags_NoDockingSplit;

            ImGui::DockBuilderGetNode(rootDockNodeId)->LocalFlags |= ImGuiDockNodeFlags_HiddenTabBar;

            ImGui::DockBuilderDockWindow(constants::kClientWindow, leftTopDockNodeId);
            ImGui::DockBuilderDockWindow(constants::kLocalMediaWindow, leftBottomDockNodeId);
            ImGui::DockBuilderDockWindow(constants::kStatsReportWindow, rightDockNodeId);

            ImGui::DockBuilderDockWindow(constants::kOutputLogInfoWindow, bottomDockNodeId);
            ImGui::DockBuilderDockWindow(constants::kOutputStatusWindow, bottomDockNodeId);
            ImGui::DockBuilderDockWindow(constants::kRemoteMediaWindow, rootDockNodeId); // CentralNode

            ImGui::DockBuilderFinish(dockSpaceId);
            ImGui::SetWindowFocus(constants::kRemoteMediaWindow);
        }

        // 创建停靠空间
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        ImGui::DockSpace(dockSpaceId, ImGui::GetMainViewport()->WorkSize, ImGuiDockNodeFlags_None);
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
    }
    ImGui::End();
}

void setMainMenuBar(WindowData &windowData)
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("View"))
        {
            auto &viewMenu = windowData.mainMenu.viewMenu;
            if (ImGui::MenuItem("Restore default layout", nullptr, nullptr))
            {
                windowData.dockLayout.layoutReset = true;
            }
            ImGui::SeparatorText("Misc");
            if (ImGui::MenuItem("Status bar##xx", nullptr, viewMenu.showStatusBar))
            {
                viewMenu.showStatusBar = !viewMenu.showStatusBar;
            }
            if (ImGui::MenuItem("FPS in status bar##xxxx", nullptr, viewMenu.showStatusFps))
            {
                viewMenu.showStatusFps = !viewMenu.showStatusFps;
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Tools"))
        {
            auto &toolsMenu = windowData.mainMenu.toolsMenu;
            ImGuiIO &io = ImGui::GetIO();
#ifndef IMGUI_DISABLE_DEBUG_TOOLS
            const bool hasDebugTools = true;
#else
            const bool hasDebugTools = false;
#endif
            ImGui::MenuItem("Metrics/Debugger", nullptr, &toolsMenu.showMetrics, hasDebugTools);
            if (ImGui::BeginMenu("Debug Options"))
            {
                ImGui::BeginDisabled(!hasDebugTools);
                ImGui::Checkbox("Highlight ID Conflicts", &io.ConfigDebugHighlightIdConflicts);
                ImGui::EndDisabled();
                ImGui::Checkbox("Assert on error recovery", &io.ConfigErrorRecoveryEnableAssert);
                ImGui::TextDisabled("(see Demo->Configuration for details & more)");
                ImGui::EndMenu();
            }
            ImGui::MenuItem("Debug Log", nullptr, &toolsMenu.showDebugLog, hasDebugTools);
            ImGui::MenuItem("ID Stack Tool", nullptr, &toolsMenu.showIDStackTool, hasDebugTools);
            bool is_debugger_present = io.ConfigDebugIsDebuggerPresent;
            if (ImGui::MenuItem("Item Picker", nullptr, false, hasDebugTools)) // && is_debugger_present))
            {
                ImGui::DebugStartItemPicker();
            }
            if (!is_debugger_present)
            {
                ImGui::SetItemTooltip(
                    "Requires io.ConfigDebugIsDebuggerPresent=true to be set.\n\nWe otherwise disable some "
                    "extra features to avoid casual users crashing the application.");
            }
            ImGui::MenuItem("Style Editor", nullptr, &toolsMenu.showStyleEditor);
            ImGui::MenuItem("About Dear ImGui", nullptr, &toolsMenu.showAbout);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void showMainMenuBar(WindowData &windowData)
{
    auto &toolsMenu = windowData.mainMenu.toolsMenu;
    if (toolsMenu.showMetrics)
    {
        ImGui::ShowMetricsWindow(&toolsMenu.showMetrics);
    }
    if (toolsMenu.showDebugLog)
    {
        ImGui::ShowDebugLogWindow(&toolsMenu.showDebugLog);
    }
    if (toolsMenu.showIDStackTool)
    {
        ImGui::ShowIDStackToolWindow(&toolsMenu.showIDStackTool);
    }
    if (toolsMenu.showAbout)
    {
        ImGui::ShowAboutWindow(&toolsMenu.showAbout);
    }
}

void showStatusBar(WindowData &windowData)
{
    auto &viewMenu = windowData.mainMenu.viewMenu;
    if (viewMenu.showStatusBar)
    {
        const ImGuiWindowFlags windowsFlags =
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav;
        if (ImGui::BeginViewportSideBar("##MainStatusBar",
                                        ImGui::GetMainViewport(),
                                        ImGuiDir_Down,
                                        windowData.statusBarSize().y,
                                        windowsFlags))
        {
            if (viewMenu.showStatusFps)
            {
                ImGui::SameLine(ImGui::GetIO().DisplaySize.x - 5.f * ImGui::GetFontSize());
                ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
            }
            ImGui::End();
        }
    }
}

int main()
{
    WindowData windowData;
    octk::StringView type = octk::constants::kImGuiApplicationSDLGPU3;
    octk::ImGuiApplication::Properties properties;
    properties.title = "test";
    auto imguiApp = octk::ImGuiApplication::Factory::create(type, properties);
    int count = 0;
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

    imguiApp->setInitFunction([&]() {});
    imguiApp->setDrawFunction(
        [&]()
        {
            setMainMenuBar(windowData);
            setDockSpace(windowData);

            const auto viewWindowFlags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_HorizontalScrollbar;
#if 1
            if (ImGui::Begin(constants::kClientWindow, nullptr, viewWindowFlags))
            {
                ImGui::LabelText("label", "text");
                ImGui::Button("button");
            }
            ImGui::End();

            if (ImGui::Begin(constants::kLocalMediaWindow, nullptr, viewWindowFlags))
            {
                ImGui::LabelText("label", "text");
            }
            ImGui::End();

            if (ImGui::Begin(constants::kStatsReportWindow, nullptr, viewWindowFlags))
            {
                ImGui::LabelText("label", "text");
            }
            ImGui::End();

            if (ImGui::Begin(constants::kOutputLogInfoWindow, nullptr, viewWindowFlags))
            {
                ImGui::LabelText("label", "text");
                ImGui::Button("button");
                ImGui::Button("button");
                ImGui::Button("button");
                ImGui::Button("button");
                ImGui::Button("button");
                ImGui::Button("button");
                ImGui::Button("button");
                ImGui::Button("button");
                ImGui::Button("button");
                ImGui::Button("button");
            }
            ImGui::End();

            if (ImGui::Begin(constants::kOutputStatusWindow, nullptr, viewWindowFlags))
            {
                ImGui::LabelText("label", "text");
                ImGui::Button("button");
            }
            ImGui::End();
#endif

            // 观察窗口，背景设置透明，窗口后面就能进行本地 API 的绘制
            // 压入样式设置
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);            // 无边框
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f)); // 无边界
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);              // 无圆角
            // ImGui::SetNextWindowBgAlpha(0.0f); // 窗口 alpha 为 0，同样可以不显示背景

            if (ImGui::Begin(constants::kRemoteMediaWindow, nullptr, viewWindowFlags | ImGuiWindowFlags_NoBackground))
            {
                ImVec2 pos = ImGui::GetWindowPos();
                ImVec2 size = ImGui::GetWindowSize();

                ImGui::Text("position: %0.2f, %0.2f", pos.x, pos.y);
                ImGui::Text("size: %0.2f, %0.2f", size.x, size.y);

                // 记录下视口位置给本地 API 使用
                // g_viewportPos = glm::ivec2(pos.x, size.y);
                // g_viewportSize = glm::ivec2(size.x, size.y);
            }
            ImGui::End();
            ImGui::PopStyleVar(3); // 弹出样式设置

            showMainMenuBar(windowData);
            showStatusBar(windowData);
        });
    return imguiApp->exec();
}
