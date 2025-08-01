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

#include <octk_string_utils.hpp>
#include <octk_assert.hpp>
#include <octk_ascii.hpp>
#include <octk_macros.hpp>

#include <algorithm>
#include <cstdint>

OCTK_BEGIN_NAMESPACE

namespace utils
{

const char *extractFileName(const char *filePath)
{
    OCTK_ASSERT(nullptr != filePath);

    char path[OCTK_PATH_MAX] = {0};
    size_t length = std::min<size_t>(OCTK_PATH_MAX, strlen(filePath));
    std::memcpy(path, filePath, length);
    while (length > 0 && ('/' == path[length - 1] || '\\' == path[length - 1]))
    {
        path[--length] = '\0';
    }
    const char *last_slash = strrchr(path, '/');
    const char *last_slash_win = strrchr(path, '\\'); // windows path
    const char *file_name = path;
    if (NULL == last_slash)
    {
        file_name = (NULL == last_slash_win) ? path : last_slash_win + 1;
    }
    else if (NULL != last_slash_win)
    {
        file_name = (last_slash_win - last_slash > 0) ? last_slash_win + 1 : last_slash + 1;
    }
    else
    {
        file_name = last_slash + 1;
    }
    return filePath + (file_name - path);
}

int stringCaseCmp(const char *s1, const char *s2, size_t len)
{
    const unsigned char *us1 = reinterpret_cast<const unsigned char *>(s1);
    const unsigned char *us2 = reinterpret_cast<const unsigned char *>(s2);

    for (size_t i = 0; i < len; i++)
    {
        unsigned char c1 = us1[i];
        unsigned char c2 = us2[i];
        // If bytes are the same, they will be the same when converted to lower.
        // So we only need to convert if bytes are not equal.
        if (c1 != c2)
        {
            c1 = c1 >= 'A' && c1 <= 'Z' ? c1 - 'A' + 'a' : c1;
            c2 = c2 >= 'A' && c2 <= 'Z' ? c2 - 'A' + 'a' : c2;
            const int diff = int{c1} - int{c2};
            if (diff != 0)
            {
                return diff;
            }
        }
    }
    return 0;
}

bool stringContainsIgnoreCase(StringView haystack, StringView needle) noexcept
{
    while (haystack.size() >= needle.size())
    {
        if (utils::stringStartsWithIgnoreCase(haystack, needle))
        {
            return true;
        }
        haystack.remove_prefix(1);
    }
    return false;
}

bool stringContainsIgnoreCase(StringView haystack, char needle) noexcept
{
    char upper_needle = ascii_toupper(static_cast<unsigned char>(needle));
    char lower_needle = ascii_tolower(static_cast<unsigned char>(needle));
    if (upper_needle == lower_needle)
    {
        return stringContains(haystack, needle);
    }
    else
    {
        const char both_cstr[3] = {lower_needle, upper_needle, '\0'};
        return haystack.find_first_of(both_cstr) != StringView::npos;
    }
}

bool stringStartsWithIgnoreCase(StringView text, StringView prefix) noexcept
{
    return (text.size() >= prefix.size()) && stringEqualsIgnoreCase(text.substr(0, prefix.size()), prefix);
}

bool stringEndsWithIgnoreCase(StringView text, StringView suffix) noexcept
{
    return (text.size() >= suffix.size()) && stringEqualsIgnoreCase(text.substr(text.size() - suffix.size()), suffix);
}

bool stringEqualsIgnoreCase(StringView piece1, StringView piece2) noexcept
{
    return (piece1.size() == piece2.size() && 0 == stringCaseCmp(piece1.data(), piece2.data(), piece1.size()));
}

bool Test::stringEqualsIgnoreCase(StringView piece1, StringView piece2) noexcept
{
    return (piece1.size() == piece2.size() && 0 == stringCaseCmp(piece1.data(), piece2.data(), piece1.size()));
}

#ifndef OCTK_BUILDING_CORE_LIB
#error "ndef OCTK_BUILDING_CORE_LIB"
#endif

} // namespace utils

OCTK_END_NAMESPACE
