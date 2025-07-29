/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present chengxuewen.
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

#ifndef _OCTK_GUI_H
#define _OCTK_GUI_H

#include <stdint.h>

#if defined(__GNUC__) && ((100 * __GNUC__ + __GNUC_MINOR__) > 400)
#   define OCTK_GUI_EXPORT __attribute__((visibility("default")))
#   define OCTK_GUI_IMPORT __attribute__((visibility("default")))
#elif defined(__MINGW32__) || defined(_MSC_VER)
#   define OCTK_GUI_EXPORT __declspec(dllexport)
#   define OCTK_GUI_IMPORT __declspec(dllimport)
#elif defined(__clang__)
#   define OCTK_GUI_EXPORT __attribute__((visibility("default")))
#   define OCTK_GUI_IMPORT __attribute__((visibility("default")))
#endif

#ifndef OCTK_BUILD_STATIC // compiled as a dynamic lib.
#   ifdef OCTK_BUILDING_GUI_LIB    // defined if we are building the lib
#       define OCTK_GUI_C_API OCTK_GUI_EXPORT
#   else
#       define OCTK_GUI_C_API OCTK_GUI_IMPORT
#   endif
#else // compiled as a static lib.
#   define OCTK_GUI_C_API
#endif

#ifdef _WIN32
#   ifndef OCTK_NO_STDCALL
#       define OCTK_GUI_STDCALL __stdcall
#   else
#       define OCTK_GUI_STDCALL
#   endif
#else // not WIN32
#   define OCTK_GUI_STDCALL
#endif

#ifdef  __cplusplus
extern "C"
{
#endif

OCTK_GUI_C_API const char *octk_gui_version(void);

OCTK_GUI_C_API const char *octk_sdl_version(void);

OCTK_GUI_C_API void octk_gui_init(void);

#ifdef  __cplusplus
}
#endif

#endif // _OCTK_GUI_H
