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

#ifndef _OCTK_STRINGS_UTILS_HPP
#define _OCTK_STRINGS_UTILS_HPP

#include <octk_string_view.hpp>

#include <cstring>
#include <string.h>

/**
 * @brief
 * This file contains simple utilities for performing string matching checks.
 * All of these function parameters are specified as `StringView`,
 * meaning that these functions can accept `std::string`, `StringView` or NUL-terminated C-style strings.
 *
 * Examples:
 *  std::string s = "foo";
 *  StringView sv = "f";
 *  assert(absl::stringContains(s, sv));
 *
 *  Note: The order of parameters in these functions is designed to mimic the
 *  order an equivalent member function would exhibit;
 *  e.g. `s.Contains(x)` ==> `absl::stringContains(s, x).
 */

OCTK_BEGIN_NAMESPACE

namespace utils
{
/**
 * @brief
 * @param filePath
 * @return
 */
OCTK_CORE_API const char *extractFileName(const char *filePath);

/**
 * @brief Performs a byte-by-byte comparison of `len` bytes of the strings `s1` and `s2`,
 * ignoring the case of the characters.
 * It returns an integer less than, equal to, or greater than zero if `s1` is found, respectively, to be less than,
 * to match, or be greater than `s2`.
 */
OCTK_CORE_API int stringCaseCmp(const char *s1, const char *s2, size_t len);

/**
 * @brief Returns whether a given ASCII string `haystack` contains the ASCII substring `needle`,
 * ignoring case in the comparison.
 */
OCTK_CORE_API bool stringContainsIgnoreCase(StringView haystack, StringView needle) noexcept;

OCTK_CORE_API bool stringContainsIgnoreCase(StringView haystack, char needle) noexcept;

/**
 * @brief Returns whether a given string `haystack` contains the substring `needle`.
 */
static inline bool stringContains(StringView haystack, StringView needle) noexcept
{
    return haystack.find(needle, 0) != haystack.npos;
}

static inline bool stringContains(StringView haystack, char needle) noexcept
{
    return haystack.find(needle) != haystack.npos;
}

/**
 * @brief Returns whether a given ASCII string `text` starts with `prefix`, ignoring case in the comparison.s
 */
OCTK_CORE_API bool stringStartsWithIgnoreCase(StringView text, StringView prefix) noexcept;

/**
 * @brief Returns whether a given string `text` begins with `prefix`.
 */
inline bool stringStartsWith(StringView text, StringView prefix) noexcept
{
    return prefix.empty() || (text.size() >= prefix.size() && memcmp(text.data(), prefix.data(), prefix.size()) == 0);
}

/**
 * @brief Returns whether a given ASCII string `text` ends with `suffix`, ignoring case in the comparison.
 */
OCTK_CORE_API bool stringEndsWithIgnoreCase(StringView text, StringView suffix) noexcept;

/**
 * @brief Returns whether a given string `text` ends with `suffix`.
 */
inline bool stringEndsWith(StringView text, StringView suffix) noexcept
{
    return suffix.empty() || (text.size() >= suffix.size() &&
                              memcmp(text.data() + (text.size() - suffix.size()), suffix.data(), suffix.size()) == 0);
}

/**
 * @brief Returns whether given ASCII strings `piece1` and `piece2` are equal, ignoring case in the comparison.
 */
OCTK_CORE_API bool stringEqualsIgnoreCase(StringView piece1, StringView piece2) noexcept;

///////////////////////////////////////////////////////////////////////////////
// UTF helpers (Windows only)
///////////////////////////////////////////////////////////////////////////////

#if defined(OCTK_OS_WIN)

inline std::wstring toUtf16(const char *utf8, size_t len)
{
    if (len == 0)
    {
        return std::wstring();
    }
    int len16 = ::MultiByteToWideChar(CP_UTF8, 0, utf8, static_cast<int>(len), nullptr, 0);
    std::wstring ws(len16, 0);
    ::MultiByteToWideChar(CP_UTF8, 0, utf8, static_cast<int>(len), &*ws.begin(), len16);
    return ws;
}

inline std::wstring toUtf16(StringView str) { return toUtf16(str.data(), str.length()); }

inline std::string toUtf8(const wchar_t *wide, size_t len)
{
    if (len == 0)
    {
        return std::string();
    }
    int len8 = ::WideCharToMultiByte(CP_UTF8, 0, wide, static_cast<int>(len), nullptr, 0, nullptr, nullptr);
    std::string ns(len8, 0);
    ::WideCharToMultiByte(CP_UTF8, 0, wide, static_cast<int>(len), &*ns.begin(), len8, nullptr, nullptr);
    return ns;
}

inline std::string toUtf8(const wchar_t *wide) { return toUtf8(wide, wcslen(wide)); }

inline std::string toUtf8(const std::wstring &wstr) { return toUtf8(wstr.data(), wstr.length()); }

#endif // defined(OCTK_OS_WIN)

} // namespace utils

OCTK_END_NAMESPACE

#endif // _OCTK_STRINGS_UTILS_HPP
