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

#ifndef _OCTK_IMGUI_THEME_HPP
#define _OCTK_IMGUI_THEME_HPP

#include <octk_imgui.hpp>

OCTK_BEGIN_NAMESPACE

namespace ImGuiTheme
{
enum class ThemeType
{
    ImGuiColorsClassic = 0,
    ImGuiColorsDark,
    ImGuiColorsLight,
    MaterialFlat,
    PhotoshopStyle,
    GrayVariations,
    GrayVariations_Darker,
    MicrosoftStyle,
    Cherry,
    Darcula,
    DarculaDarker,
    LightRounded,
    SoDark_AccentBlue,
    SoDark_AccentYellow,
    SoDark_AccentRed,
    BlackIsBlack,
    WhiteIsWhite
};
OCTK_STATIC_CONSTANT_NUMBER(ThemeTypeNum, static_cast<int>(ThemeType::WhiteIsWhite) + 1);

OCTK_IMGUI_API const char *themeTypeName(ThemeType theme);
OCTK_IMGUI_API ThemeType themeTypeFromName(const char *themeName);

OCTK_IMGUI_API ImGuiStyle themeToStyle(ThemeType theme);
OCTK_IMGUI_API void applyTheme(ThemeType theme);

struct Tweaks
{
    // Common rounding for widgets. If < 0, this is ignored.
    float rounding = -1.f;
    // If rounding is applied, scrollbar rounding needs to be adjusted to be visually pleasing in conjunction with other widgets roundings. Only applied if Rounding > 0.f)
    float roundingScrollbarRatio = 4.f;
    // Change the alpha that will be applied to windows, popups, etc. If < 0, this is ignored.
    float alphaMultiplier = -1.f;

    //
    // HSV Color tweaks
    //
    // Change the hue of all widgets (gray widgets will remain gray, since their saturation is zero). If < 0, this is ignored.
    float hue = -1.f;
    // Multiply the saturation of all widgets (gray widgets will remain gray, since their saturation is zero). If < 0, this is ignored.
    float saturationMultiplier = -1.f;
    // Multiply the value (luminance) of all front widgets. If < 0, this is ignored.
    float valueMultiplierFront = -1.f;
    // Multiply the value (luminance) of all backgrounds. If < 0, this is ignored.
    float valueMultiplierBg = -1.f;
    // Multiply the value (luminance) of text. If < 0, this is ignored.
    float valueMultiplierText = -1.f;
    // Multiply the value (luminance) of FrameBg. If < 0, this is ignored.
    // (Background of checkbox, radio button, plot, slider, text input)
    float valueMultiplierFrameBg = -1.f;

    Tweaks() { }
};

struct TweakedTheme
{
    ThemeType theme = ThemeType::DarculaDarker;
    Tweaks tweaks = Tweaks();

    TweakedTheme(ThemeType theme = ThemeType::DarculaDarker, const Tweaks &tweaks = Tweaks())
        : theme(theme)
        , tweaks(tweaks)
    {
    }
};

OCTK_IMGUI_API ImGuiStyle tweakedThemeThemeToStyle(const TweakedTheme &tweakedTheme);
OCTK_IMGUI_API void applyTweakedTheme(const TweakedTheme &tweakedTheme);

// PushTweakedTheme() / PopTweakedTheme()
// Push and pop a tweaked theme
//
// Note: If you want the theme to apply globally to a window, you need to apply it
//       *before* calling ImGui::Begin
//
//     For example, within Hello ImGui, given a dockable window, you should set this option:
//        myDockableWindow.callBeginEnd = false;
//     And then:
//     - call ImGuiTheme::PushTweakedTheme
//     - call ImGui::Begin
//     - display your content
//     - call ImGui::End
//     - call ImGuiTheme::PopTweakedTheme
//
// See demo inside src/hello_imgui_demos/hello_imgui_demodocking/hello_imgui_demodocking.main.cpp:
//     look at `GuiWindowAlternativeTheme()`
OCTK_IMGUI_API void pushTweakedTheme(const TweakedTheme &tweakedTheme);
OCTK_IMGUI_API void popTweakedTheme();

// Show the theme selection listbox, the theme tweak widgets, as well as ImGui::ShowStyleEditor.
// Returns true if modified (Warning, when using ShowStyleEditor, no info about modification is transmitted)
OCTK_IMGUI_API bool showThemeTweakGui(TweakedTheme *tweakedTheme);

// Some tweakable themes
OCTK_IMGUI_API ImGuiStyle soDark(float hue);
OCTK_IMGUI_API ImGuiStyle shadesOfGray(float rounding = 0.f,
                                       float valueMultiplierFront = 1.f,
                                       float valueMultiplierBG = 1.f);
OCTK_IMGUI_API ImGuiStyle darcula(float rounding = 1.f,
                                  float hue = -1.f,
                                  float saturationMultiplier = 1.f,
                                  float valueMultiplierFront = 1.f,
                                  float valueMultiplierBG = 1.f,
                                  float alphaBGTransparency = 1.f);

} // namespace ImGuiTheme

OCTK_END_NAMESPACE

#endif // _OCTK_IMGUI_THEME_HPP
