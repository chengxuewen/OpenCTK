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

#ifndef _OCTK_STRING_ENCODE_HPP
#define _OCTK_STRING_ENCODE_HPP

#include <octk_string_to_number.hpp>
#include <octk_string_view.hpp>
#include <octk_array_view.hpp>
#include <octk_iterator.hpp>
#include <octk_optional.hpp>

OCTK_BEGIN_NAMESPACE

namespace utils
{

std::string hex_encode(StringView str);
std::string hex_encode_with_delimiter(StringView source, char delimiter);

// hex_decode converts ascii hex to binary.
size_t hex_decode(ArrayView<char> buffer, StringView source);

// hex_decode, assuming that there is a delimiter between every byte
// pair.
// `delimiter` == 0 means no delimiter
// If the buffer is too short or the data is invalid, we return 0.
size_t hex_decode_with_delimiter(ArrayView<char> buffer, StringView source, char delimiter);

// Splits the source string into multiple fields separated by delimiter,
// with duplicates of delimiter creating empty fields. Empty input produces a
// single, empty, field.
std::vector<StringView> split(StringView source, char delimiter);

// Splits the source string into multiple fields separated by delimiter,
// with duplicates of delimiter ignored.  Trailing delimiter ignored.
size_t tokenize(StringView source, char delimiter, std::vector<std::string> *fields);

// Extract the first token from source as separated by delimiter, with
// duplicates of delimiter ignored. Return false if the delimiter could not be
// found, otherwise return true.
bool tokenize_first(StringView source, char delimiter, std::string *token, std::string *rest);

template <typename T, typename std::enable_if<std::is_integral<T>::value>::type * = nullptr>
std::string toString(T value)
{
    // return {absl::StrCat(value)}; //TODO
    return std::is_same<T, bool>::value ? (value ? "true" : "false") : std::to_string(value);
}
template <typename T, typename std::enable_if<std::is_convertible<T, const char *>::value>::type * = nullptr>
std::string toString(T value)
{
    return {value};
}
template <typename T, typename std::enable_if<std::is_same<T, std::string>::value>::type * = nullptr>
std::string toString(T value)
{
    return value;
}
template <typename T, typename std::enable_if<std::is_floating_point<T>::value>::type * = nullptr>
std::string toString(T value)
{
    char buf[32];
    long double ld = value;
    const int len = std::snprintf(&buf[0], OCTK_ARRAY_SIZE(buf), "%Lg", ld);
    OCTK_DCHECK_LE(len, OCTK_ARRAY_SIZE(buf));
    return std::string(&buf[0], len);
}

template <typename T,
          typename std::enable_if<std::is_pointer<T>::value && !std::is_convertible<T, const char *>::value>::type * =
              nullptr>
std::string toString(T p)
{
    char buf[32];
    const int len = std::snprintf(&buf[0], utils::size(buf), "%p", p);
    OCTK_DCHECK_LE(len, utils::size(buf));
    return std::string(&buf[0], len);
}

template <typename T,
          typename std::enable_if<std::is_arithmetic<T>::value && !std::is_same<T, bool>::value, int>::type = 0>
static bool FromString(StringView s, T *t)
{
    OCTK_DCHECK(t);
    Optional<T> result = stringToNumber<T>(s);

    if (result)
    {
        *t = *result;
    }

    return result.has_value();
}

bool FromString(StringView s, bool *b);

template <typename T> static inline T FromString(StringView str)
{
    T val;
    FromString(str, &val);
    return val;
}
} // namespace utils

OCTK_END_NAMESPACE

#endif // _OCTK_STRING_ENCODE_HPP
