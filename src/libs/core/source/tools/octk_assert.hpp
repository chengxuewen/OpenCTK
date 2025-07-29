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

#ifndef _OCTK_ASSERT_HPP
#define _OCTK_ASSERT_HPP

#include <octk_global.hpp>

/***********************************************************************************************************************
   OpenCTK assert macro
***********************************************************************************************************************/
OCTK_CORE_API void octk_assert_x(const char *where, const char *what, const char *file, int line) OCTK_NOTHROW;
OCTK_CORE_API void octk_assert(const char *assertion, const char *file, int line) OCTK_NOTHROW;
static inline void octk_noop(void) {}
#if !defined(OCTK_ASSERT)
#   if defined(OCTK_NO_DEBUG) && !defined(OCTK_FORCE_ASSERTS)
#       define OCTK_ASSERT(cond)  do { } while ((false) && (cond))
#   else
#       define OCTK_ASSERT(cond) ((!(cond)) ? octk_assert(#cond, __FILE__, __LINE__) : octk_noop())
#   endif
#endif
#if !defined(OCTK_ASSERT_X)
#   if defined(OCTK_NO_DEBUG) && !defined(OCTK_FORCE_ASSERTS)
#       define OCTK_ASSERT_X(cond, where, what) do { } while ((false) && (cond))
#   else
#       define OCTK_ASSERT_X(cond, where, what) ((!(cond)) ? octk_assert_x(where, what, __FILE__, __LINE__) : octk_noop())
#   endif
#endif
#define OCTK_STATIC_ASSERT(Condition) static_assert(bool(Condition), #Condition)
#define OCTK_STATIC_ASSERT_X(Condition, Message) static_assert(bool(Condition), Message)

#endif // _OCTK_ASSERT_HPP
