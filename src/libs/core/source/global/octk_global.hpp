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

#ifndef _OCTK_GLOBAL_HPP
#define _OCTK_GLOBAL_HPP

#include <octk_core_config.hpp>
#include <octk_compiler.hpp>
#include <octk_system.hpp>
#include <octk_macros.hpp>
#include <octk_types.hpp>

#include <stdarg.h>

/***********************************************************************************************************************
 * compiler dll visibility macro declare
***********************************************************************************************************************/
#if defined(OCTK_CC_GNU) && (OCTK_CC_GNU > 400)
#    define OCTK_DECLARE_EXPORT __attribute__((visibility("default")))
#    define OCTK_DECLARE_IMPORT __attribute__((visibility("default")))
#    define OCTK_DECLARE_HIDDEN __attribute__((visibility("hidden")))
#elif defined(OCTK_CC_MINGW) || defined(OCTK_CC_MSVC)
#    define OCTK_DECLARE_EXPORT __declspec(dllexport)
#    define OCTK_DECLARE_IMPORT __declspec(dllimport)
#    define OCTK_DECLARE_HIDDEN
#elif defined(OCTK_CC_CLANG)
#    define OCTK_DECLARE_EXPORT __attribute__((visibility("default")))
#    define OCTK_DECLARE_IMPORT __attribute__((visibility("default")))
#    define OCTK_DECLARE_HIDDEN __attribute__((visibility("hidden")))
#endif

#ifndef OCTK_DECLARE_EXPORT
#    define OCTK_DECLARE_EXPORT
#endif
#ifndef OCTK_DECLARE_IMPORT
#    define OCTK_DECLARE_IMPORT
#endif
#ifndef OCTK_DECLARE_HIDDEN
#    define OCTK_DECLARE_HIDDEN
#endif

/***********************************************************************************************************************
 * compiler specific cmds for export and import code to DLL and declare namespace
***********************************************************************************************************************/
#ifdef OCTK_BUILD_SHARED_CORE // compiled as a dynamic lib.
#    ifdef OCTK_BUILDING_CORE_LIB // defined if we are building the lib
#        define OCTK_CORE_API OCTK_DECLARE_EXPORT
#    else
#        define OCTK_CORE_API OCTK_DECLARE_IMPORT
#    endif
#    define OCTK_CORE_HIDDEN OCTK_DECLARE_HIDDEN
#else // compiled as a static lib.
#    define OCTK_CORE_API
#    define OCTK_CORE_HIDDEN
#endif

#endif // _OCTK_GLOBAL_HPP
