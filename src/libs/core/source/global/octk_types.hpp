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

#ifndef _OCTK_TYPES_HPP
#define _OCTK_TYPES_HPP

#include <octk_macros.hpp>
#include <octk_system.hpp>
#include <octk_core_config.hpp>

#if defined(OCTK_OS_WIN)
#    include <windows.h>
#else
#    include <sys/types.h>
#endif
#include <vector>
#include <memory>
#include <cstdint>
#include <stdint.h>

OCTK_BEGIN_NAMESPACE

using std::int8_t;
using std::int16_t;
using std::int32_t;
using std::int64_t;

using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;

using float_t = float;
using double_t = double;

using std::size_t;
using std::ptrdiff_t;
using uintptr_t = size_t;
using intptr_t = ptrdiff_t;
#if !OCTK_HAS_SSIZE_T && defined(OCTK_OS_WIN)
#    include <BaseTsd.h>
using ssize_t = SSIZE_T;
#else
using ssize_t = ssize_t;
#endif
#ifndef OCTK_SIZEOF_SSIZE_T
#    define OCTK_SIZEOF_SSIZE_T OCTK_SIZEOF_SIZE_T
#endif

using byte_t = uint8_t;
using uchar_t = unsigned char;
using ushort_t = unsigned short;
using uint_t = unsigned int;
using ulong_t = unsigned long;
using ulonglong_t = unsigned long long;

using handle_t = void *;
using pointer_t = void *;
using const_pointer_t = const void *;

using Binary = std::vector<byte_t>;
using TSBinary = std::pair<int64_t, Binary>;
using BinarySharedPtr = std::shared_ptr<Binary>;

template <typename T> Binary makeBinary(const std::vector<T> &data)
{
    return {reinterpret_cast<const byte_t *>(data.data()), reinterpret_cast<const byte_t *>(data.data()) + data.size()};
}

struct None
{
};

template <typename HeadArg, typename... TailArgs> struct Types
{
    using Head = HeadArg;
    using Tail = Types<TailArgs...>;
};

template <typename HeadArg> struct Types<HeadArg>
{
    using Head = HeadArg;
    using Tail = None;
};

template <typename... Args> struct TypeList
{
    using type = Types<Args...>;
    using Type = type;
};
template <typename... Args> using type_list = TypeList<Args...>;

/***********************************************************************************************************************
 * type format define
***********************************************************************************************************************/
#if OCTK_SIZEOF_SHORT == 2
#    define OCTK_INT16_MODIFIER "h"
#    define OCTK_INT16_FORMAT   "hi"
#    define OCTK_UINT16_FORMAT  "hu"
#elif OCTK_SIZEOF_INT == 2
#    define OCTK_INT16_MODIFIER ""
#    define OCTK_INT16_FORMAT   "i"
#    define OCTK_UINT16_FORMAT  "u"
#else
#    error "Compiler provides no native 16-bit integer type"
#endif

#if OCTK_SIZEOF_SHORT == 4
#    define OCTK_INT32_MODIFIER "h"
#    define OCTK_INT32_FORMAT   "hi"
#    define OCTK_UINT32_FORMAT  "hu"
#elif OCTK_SIZEOF_INT == 4
#    define OCTK_INT32_MODIFIER "h"
#    define OCTK_INT32_FORMAT   "i"
#    define OCTK_UINT32_FORMAT  "u"
#elif OCTK_SIZEOF_LONG == 4
#    define OCTK_INT32_MODIFIER "l"
#    define OCTK_INT32_FORMAT   "li"
#    define OCTK_UINT32_FORMAT  "lu"
#else
#    error "Compiler provides no native 32-bit integer type"
#endif

#if OCTK_SIZEOF_INT == 8
#    define OCTK_INT64_MODIFIER ""
#    define OCTK_INT64_FORMAT   "i"
#    define OCTK_UINT64_FORMAT  "u"
#elif (OCTK_SIZEOF_LONG == 8) && (OCTK_SIZEOF_LONG_LONG != OCTK_SIZEOF_LONG || OCTK_INT64_IS_LONG_TYPE)
#    define OCTK_INT64_MODIFIER "l"
#    define OCTK_INT64_FORMAT   "li"
#    define OCTK_UINT64_FORMAT  "lu"
#elif (OCTK_SIZEOF_LONG_LONG == 8) && (OCTK_SIZEOF_LONG_LONG != OCTK_SIZEOF_LONG || OCTK_INT64_IS_LONG_LONG_TYPE)
#    define OCTK_INT64_MODIFIER "ll"
#    define OCTK_INT64_FORMAT   "lli"
#    define OCTK_UINT64_FORMAT  "llu"
#else
#    error "Compiler provides no native 64-bit integer type"
#endif

#if OCTK_SIZET_IS_SHORT_TYPE
#    define OCTK_SIZE_MODIFIER  "h"
#    define OCTK_SSIZE_MODIFIER "h"
#    define OCTK_SIZE_FORMAT    "hu"
#    define OCTK_SSZIE_FORMAT   "hi"
#elif OCTK_SIZET_IS_INT_TYPE
#    define OCTK_SIZE_MODIFIER  ""
#    define OCTK_SSIZE_MODIFIER ""
#    define OCTK_SIZE_FORMAT    "u"
#    define OCTK_SSZIE_FORMAT   "i"
#elif OCTK_SIZET_IS_LONG_TYPE
#    define OCTK_SIZE_MODIFIER  "l"
#    define OCTK_SSIZE_MODIFIER "l"
#    define OCTK_SIZE_FORMAT    "lu"
#    define OCTK_SSZIE_FORMAT   "li"
#elif OCTK_SIZET_IS_LONG_LONG_TYPE
#    define OCTK_SIZE_MODIFIER  "ll"
#    define OCTK_SSIZE_MODIFIER "ll"
#    define OCTK_SIZE_FORMAT    "llu"
#    define OCTK_SSZIE_FORMAT   "lli"
#elif OCTK_SIZEOF_SIZE_T == 8
#    define OCTK_SIZE_MODIFIER  "l"
#    define OCTK_SSIZE_MODIFIER "l"
#    define OCTK_SIZE_FORMAT    "lu"
#    define OCTK_SSZIE_FORMAT   "li"
#elif OCTK_SIZEOF_SIZE_T == 4
#    define OCTK_SIZE_MODIFIER  ""
#    define OCTK_SSIZE_MODIFIER ""
#    define OCTK_SIZE_FORMAT    "u"
#    define OCTK_SSZIE_FORMAT   "i"
#else
#    error "Could not determine size of size_t."
#endif

#if OCTK_SIZEOF_VOID_P == OCTK_SIZEOF_INT
#    define OCTK_INTPTR_MODIFIER ""
#    define OCTK_INTPTR_FORMAT   "i"
#    define OCTK_UINTPTR_FORMAT  "u"
#elif OCTK_SIZEOF_VOID_P == OCTK_SIZEOF_LONG
#    define OCTK_INTPTR_MODIFIER "l"
#    define OCTK_INTPTR_FORMAT   "li"
#    define OCTK_UINTPTR_FORMAT  "lu"
#elif OCTK_SIZEOF_VOID_P == OCTK_SIZEOF_LONG_LONG
#    define OCTK_INTPTR_MODIFIER "ll"
#    define OCTK_INTPTR_FORMAT   "lli"
#    define OCTK_UINTPTR_FORMAT  "llu"
#else
#    error "Could not determine size of void *"
#endif

#if OCTK_SIZEOF_VOID_P == OCTK_SIZEOF_INT
#    define OCTK_INTPTR_MODIFIER ""
#    define OCTK_INTPTR_FORMAT   "i"
#    define OCTK_UINTPTR_FORMAT  "u"
#elif OCTK_SIZEOF_VOID_P == OCTK_SIZEOF_LONG
#    define OCTK_INTPTR_MODIFIER "l"
#    define OCTK_INTPTR_FORMAT   "li"
#    define OCTK_UINTPTR_FORMAT  "lu"
#elif OCTK_SIZEOF_VOID_P == OCTK_SIZEOF_LONG_LONG
#    define OCTK_INTPTR_MODIFIER "ll"
#    define OCTK_INTPTR_FORMAT   "lli"
#    define OCTK_UINTPTR_FORMAT  "llu"
#else
#    error "Could not determine size of void *"
#endif
OCTK_END_NAMESPACE

#endif // _OCTK_TYPES_HPP
