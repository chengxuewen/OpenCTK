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

#ifndef _OCTK_STRING_HPP
#define _OCTK_STRING_HPP

#include <octk_global.hpp>

OCTK_BEGIN_NAMESPACE

class OCTK_CORE_API String
{
public:
    // static String vasprintf(const char *format, va_list ap) OCTK_ATTRIBUTE_FORMAT_PRINTF(1, 0);
    // static String asprintf(const char *format, ...) OCTK_ATTRIBUTE_FORMAT_PRINTF(1, 2);

    /**
     * @brief Duplicates the first @n bytes of a string, returning a newly-allocated buffer @n + 1 bytes long which
     * will always be nul-terminated.
     * If @a str is less than @n bytes long the buffer is padded with nuls.
     * If @a str is %NULL it returns %NULL. The returned value should be freed when no longer needed.
     *
     * @param str   the string to duplicate
     * @param n     the maximum number of bytes to copy from @a str
     * @return a newly-allocated buffer containing the first @n bytes of @a str, nul-terminated
     */
    static char *strndup(const char *str, size_t n);

    /**
     * @brief Duplicates a string. If @a str is %NULL it returns %NULL.
     * The returned string should be freed with free() when no longer needed.
     * @param str The string to duplicate
     * @return A newly-allocated copy of @a str
     */
    static char *strdup(const char *str) { return str ? String::strndup(str, strlen(str) + 1) : nullptr; }
};

OCTK_END_NAMESPACE

#endif // _OCTK_STRING_HPP
