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

#ifndef _OCTK_MEDIA_GLOBAL_HPP
#define _OCTK_MEDIA_GLOBAL_HPP

#include <octk_global.hpp>
#include <octk_types.hpp>

/***********************************************************************************************************************
   OpenCTK Compiler specific cmds for export and import code to DLL
***********************************************************************************************************************/
#ifdef OCTK_BUILD_SHARED           // compiled as a dynamic lib.
#    ifdef OCTK_BUILDING_MEDIA_LIB // defined if we are building the lib
#        define OCTK_MEDIA_API OCTK_DECLARE_EXPORT
#    else
#        define OCTK_MEDIA_API OCTK_DECLARE_IMPORT
#    endif
#    define OCTK_MEDIA_HIDDEN OCTK_DECLARE_HIDDEN
#else // compiled as a static lib.
#    define OCTK_MEDIA_API
#    define OCTK_MEDIA_HIDDEN
#endif

#endif // _OCTK_MEDIA_GLOBAL_HPP
