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

#include <octk_string_utils.hpp>
#include <octk_assert.hpp>
#include <octk_macros.hpp>
#include <octk_checks.hpp>
#include <octk_ascii.hpp>

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

std::string extractFunctionName(const char *function, const char *suffix)
{
    const StringView funcStringView(function);
    const auto end = funcStringView.find_last_of('(');
    if (std::string::npos != end)
    {
        const auto start = funcStringView.find_last_of(' ', end);
        if (std::string::npos != start)
        {
            return std::string(function + start, function + end) + suffix;
        }
    }
    return function;
}

bool stringCompare(const char *s1, const char *s2, size_t len, bool ignoreCase)
{
    const unsigned char *us1 = reinterpret_cast<const unsigned char *>(s1);
    const unsigned char *us2 = reinterpret_cast<const unsigned char *>(s2);

    for (size_t i = 0; i < len; i++)
    {
        unsigned char c1 = us1[i];
        unsigned char c2 = us2[i];
        // If bytes are the same, they will be the same when converted to lower.
        // So we only need to convert if bytes are not equal.
        if (c1 != c2 && ignoreCase)
        {
            c1 = c1 >= 'A' && c1 <= 'Z' ? c1 - 'A' + 'a' : c1;
            c2 = c2 >= 'A' && c2 <= 'Z' ? c2 - 'A' + 'a' : c2;
            const int diff = int{c1} - int{c2};
            if (diff != 0)
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    return true;
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

namespace
{
// This is an arbitrary limitation that can be changed if necessary, or removed if someone has the time and
// inclination to replicate the fancy logic from Chromium's base::StringPrinf().
constexpr int kMaxSize = 512;
} // namespace

std::string StringFormat(const char *format, ...)
{
    char buffer[kMaxSize];
    va_list args;
    va_start(args, format);
    int result = vsnprintf(buffer, kMaxSize, format, args);
    va_end(args);
    OCTK_DCHECK_GE(result, 0) << "ERROR: vsnprintf() failed with error " << result;
    OCTK_DCHECK_LT(result, kMaxSize) << "WARNING: string was truncated from " << result << " to " << (kMaxSize - 1)
                                     << " characters";
    return std::string(buffer);
}
} // namespace utils

OCTK_END_NAMESPACE
