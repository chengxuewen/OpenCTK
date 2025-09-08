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

#include <private/octk_sdl_p.hpp>
#include <octk_core_config.hpp>
#include <octk_logging.hpp>
#include <octk_imgui.hpp>

#include <SDL3/SDL.h>

#include <mutex>

OCTK_BEGIN_NAMESPACE

IMGui::IMGui() { }

void IMGui::init()
{
    SDL::init();
    OCTK_TRACE() << "IMGui::init()";
}

const char *IMGui::version() { return OCTK_VERSION_NAME; }

const char *IMGui::sdlVersion()
{
    static std::once_flag once;
    static char versionBuff[OCTK_LINE_MAX] = {0};
    std::call_once(
        once,
        [=]()
        {
            snprintf(versionBuff, OCTK_LINE_MAX, "v%d.%d.%d", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_MICRO_VERSION);
        });
    return versionBuff;
}

OCTK_END_NAMESPACE

#include <octk_imgui.h>

const char *octk_imgui_version() { return octk::IMGui::version(); }

const char *octk_sdl_version() { return octk::IMGui::sdlVersion(); }

void octk_imgui_init() { octk::IMGui::init(); }