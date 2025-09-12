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

#include <private/octk_imgui_application_p.hpp>
#include "3rdparty/imgui_spectrum.h"

namespace ImGui
{
namespace ChronoShenanigans
{
class ClockSeconds_
{
private:
    using Clock = std::chrono::high_resolution_clock;
    using second = std::chrono::duration<float, std::ratio<1>>;
    std::chrono::time_point<Clock> mStart;

public:
    ClockSeconds_()
        : mStart(Clock::now())
    {
    }

    float elapsed() const { return std::chrono::duration_cast<second>(Clock::now() - mStart).count(); }
};

float ClockSeconds()
{
    static ClockSeconds_ watch;
    return watch.elapsed();
}

} // namespace ChronoShenanigans
static std::deque<float> gFrameTimes;

void UpdateFrameRateStats()
{
    float now = ChronoShenanigans::ClockSeconds();
    gFrameTimes.push_back(now);

    size_t maxFrameCount = 300;
    while (gFrameTimes.size() > maxFrameCount)
    {
        gFrameTimes.pop_front();
    }
};
float FrameRate(float durationForMean)
{
    if (gFrameTimes.size() <= 1)
    {
        return 0.f;
    }

    float lastFrameTime = gFrameTimes.back();
    int lastFrameIdx = (int)gFrameTimes.size() - 1;

    // Go back in frame times to find the first frame that is not too old
    int i = (int)gFrameTimes.size() - 1;
    while (i > 0)
    {
        if (lastFrameTime - gFrameTimes[i] > durationForMean)
        {
            break;
        }
        --i;
    }
    if (i == lastFrameIdx)
    {
        return 0.f;
    }
    // printf("i=%d, lastFrameIdx=%d\n", i, lastFrameIdx);

    // Compute the mean frame rate
    float totalTime = lastFrameTime - gFrameTimes[i];
    int nbFrames = lastFrameIdx - i;
    float fps = (float)nbFrames / totalTime;
    return fps;
}

void StyleColorsCinder(ImGuiStyle *dst)
{
    ImGuiStyle *style = dst ? dst : &::ImGui::GetStyle();
    ImVec4 *colors = style->Colors;

    style->WindowMinSize = ImVec2(160, 20);
    style->FramePadding = ImVec2(4, 2);
    style->ItemSpacing = ImVec2(6, 2);
    style->ItemInnerSpacing = ImVec2(6, 4);
    style->Alpha = 0.95f;
    style->WindowRounding = 4.0f;
    style->FrameRounding = 2.0f;
    style->IndentSpacing = 6.0f;
    style->ItemInnerSpacing = ImVec2(2, 4);
    style->ColumnsMinSpacing = 50.0f;
    style->GrabMinSize = 14.0f;
    style->GrabRounding = 16.0f;
    style->ScrollbarSize = 12.0f;
    style->ScrollbarRounding = 16.0f;

    colors[ImGuiCol_Text] = ImVec4(0.86f, 0.93f, 0.89f, 0.78f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.86f, 0.93f, 0.89f, 0.28f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
    colors[ImGuiCol_Border] = ImVec4(0.31f, 0.31f, 1.00f, 0.00f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.20f, 0.22f, 0.27f, 0.75f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.20f, 0.22f, 0.27f, 0.47f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.09f, 0.15f, 0.16f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.86f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.92f, 0.18f, 0.29f, 0.76f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.86f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.14f, 0.16f, 0.19f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.47f, 0.77f, 0.83f, 0.04f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.92f, 0.18f, 0.29f, 0.43f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.20f, 0.22f, 0.27f, 0.9f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.22f, 0.27f, 0.73f);
}

void StyleColorsSpectrum(ImGuiStyle *dst) { ::ImGui::Spectrum::StyleColorsSpectrum(dst); }
} // namespace ImGui

OCTK_BEGIN_NAMESPACE

namespace internal
{
using ImGuiApplicationCreaterMap = std::unordered_map<std::string, ImGuiApplication::Factory::CreaterFunction>;
using ImGuiApplicationCreaterMapItem = std::pair<std::string, ImGuiApplication::Factory::CreaterFunction>;
static ImGuiApplicationCreaterMap &imGuiApplicationCreaterMap()
{
    static ImGuiApplicationCreaterMap map;
    return map;
}
} // namespace internal

std::vector<std::string> ImGuiApplication::Factory::registeredTypes()
{
    std::vector<std::string> keys;
    auto map = internal::imGuiApplicationCreaterMap();
    keys.reserve(map.size());
    std::transform(map.begin(),
                   map.end(),
                   std::back_inserter(keys),
                   [](const internal::ImGuiApplicationCreaterMapItem &pair) { return pair.first; });
    return keys;
}

ImGuiApplication::UniquePtr ImGuiApplication::Factory::create(StringView typeName, const Properties &properties)
{
    if ("" == typeName)
    {
        typeName = constants::kImGuiApplicationSDLGPU3;
    }
    auto map = internal::imGuiApplicationCreaterMap();
    const auto iter = map.find(typeName.data());
    if (map.cend() != iter)
    {
        auto func = iter->second;
        if (func)
        {
            return func(properties);
        }
    }
    return nullptr;
}

void ImGuiApplication::Factory::registerApplication(StringView typeName, CreaterFunction func)
{
    internal::imGuiApplicationCreaterMap().insert(std::make_pair(typeName, func));
}

ImGuiApplicationPrivate::ImGuiApplicationPrivate(ImGuiApplication *p)
    : mPPtr(p)
{
}

ImGuiApplicationPrivate::~ImGuiApplicationPrivate() { }

ImGuiApplication::ImGuiApplication(const Properties &properties)
    : ImGuiApplication(new ImGuiApplicationPrivate(this), properties)
{
}
ImGuiApplication::ImGuiApplication(ImGuiApplicationPrivate *d, const Properties &properties)
    : mDPtr(d)
{
    mDPtr->mProperties = properties;
}

ImGuiApplication::~ImGuiApplication() { this->destroy(); }

bool ImGuiApplication::isReady() const
{
    OCTK_D(const ImGuiApplication);
    return d->mInitSuccess.load();
}

bool ImGuiApplication::isFinished() const
{
    OCTK_D(const ImGuiApplication);
    return d->mFinished.load();
}

std::string ImGuiApplication::lastError() const
{
    OCTK_D(const ImGuiApplication);
    return d->mLastError;
}

void ImGuiApplication::setInitFunction(Callback func)
{
    OCTK_D(ImGuiApplication);
    SpinLock::Locker locker(d->mCallbackSpinLock);
    d->mInitFunction = func;
}

void ImGuiApplication::setDrawFunction(Callback func)
{
    OCTK_D(ImGuiApplication);
    SpinLock::Locker locker(d->mCallbackSpinLock);
    d->mDrawFunction = func;
}

void ImGuiApplication::setQuitFunction(Callback func)
{
    OCTK_D(ImGuiApplication);
    SpinLock::Locker locker(d->mCallbackSpinLock);
    d->mQuitFunction = func;
}

bool ImGuiApplication::init()
{
    OCTK_D(ImGuiApplication);
    d->mInitSuccess.store(true);
    return d->mInitSuccess.load();
}

bool ImGuiApplication::exec() { return true; }

void ImGuiApplication::destroy() { }

OCTK_END_NAMESPACE