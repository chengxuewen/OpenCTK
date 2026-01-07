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

#pragma once

#include <octk_assert.hpp>
#include <octk_logging.hpp>
#include <octk_safe_compare.hpp>

/**
 * If you for some reson need to know if DCHECKs are on, test the value of OCTK_DCHECK_IS_ON.
 * (Test its value, not if it's defined; it'll always be defined, to either a true or a false value.)
 */
#if !defined(NDEBUG) || defined(DCHECK_ALWAYS_ON)
#    define OCTK_DCHECK_IS_ON 1
#else
#    define OCTK_DCHECK_IS_ON 0
#endif

// #define OCTK_CHECK(condition)                                                                                          \
//     if (!(condition))                                                                                                  \
//     octk::Logger::FatalLogCall("Check \"" #condition "\" failed!") & OCTK_FATAL()

#define OCTK_CHECK(condition)                                                                                          \
    if (!(condition))                                                                                                  \
    OCTK_FATAL() << "Check \"" #condition "\" failed!"

#define OCTK_CHECK_OP(name, op, val1, val2)                                                                            \
    if (!octk::Safe##name((val1), (val2)))                                                                             \
    OCTK_FATAL(octk::StringView("Check \"" #val1 " " #op " " #val2 "\" failed!"))

#define OCTK_CHECK_EQ(val1, val2) OCTK_CHECK_OP(Eq, ==, val1, val2)
#define OCTK_CHECK_NE(val1, val2) OCTK_CHECK_OP(Ne, !=, val1, val2)
#define OCTK_CHECK_LE(val1, val2) OCTK_CHECK_OP(Le, <=, val1, val2)
#define OCTK_CHECK_LT(val1, val2) OCTK_CHECK_OP(Lt, <, val1, val2)
#define OCTK_CHECK_GE(val1, val2) OCTK_CHECK_OP(Ge, >=, val1, val2)
#define OCTK_CHECK_GT(val1, val2) OCTK_CHECK_OP(Gt, >, val1, val2)

/**
 * The OCTK_DCHECK macro is equivalent to RTC_CHECK except that it only generates code in debug builds.
 * It does reference the condition parameter in all cases, though, so callers won't risk getting warnings
 * about unused variables.
 */
#if OCTK_DCHECK_IS_ON
#    define OCTK_DCHECK(condition) OCTK_CHECK(condition)
#    define OCTK_DCHECK_EQ(v1, v2) OCTK_CHECK_EQ(v1, v2)
#    define OCTK_DCHECK_NE(v1, v2) OCTK_CHECK_NE(v1, v2)
#    define OCTK_DCHECK_LE(v1, v2) OCTK_CHECK_LE(v1, v2)
#    define OCTK_DCHECK_LT(v1, v2) OCTK_CHECK_LT(v1, v2)
#    define OCTK_DCHECK_GE(v1, v2) OCTK_CHECK_GE(v1, v2)
#    define OCTK_DCHECK_GT(v1, v2) OCTK_CHECK_GT(v1, v2)
#else
#    define OCTK_DCHECK(condition) OCTK_CHECK(true)
#    define OCTK_DCHECK_EQ(v1, v2) OCTK_CHECK(true)
#    define OCTK_DCHECK_NE(v1, v2) OCTK_CHECK(true)
#    define OCTK_DCHECK_LE(v1, v2) OCTK_CHECK(true)
#    define OCTK_DCHECK_LT(v1, v2) OCTK_CHECK(true)
#    define OCTK_DCHECK_GE(v1, v2) OCTK_CHECK(true)
#    define OCTK_DCHECK_GT(v1, v2) OCTK_CHECK(true)
#endif

#define OCTK_DCHECK_NOTREACHED() OCTK_DCHECK(false)

/**
 * Kills the process with an error message.
 * Never returns. Use when you wish to assert that a point in the code is never reached.
 */
#define OCTK_CHECK_NOTREACHED() octk::Logger::FatalLogCall("Unreachable Code Reached!") & OCTK_FATAL()