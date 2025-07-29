########################################################################################################################
#
# Library: OpenCTK
#
# Copyright (C) 2025~Present ChengXueWen.
#
# License: MIT License
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
# documentation files (the "Software"), to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and
# to permit persons to whom the Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all copies or substantial portions
# of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
# WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE  AUTHORS
# OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
# OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
########################################################################################################################

octk_configure_definition("OCTK_VERSION_NAME" PUBLIC VALUE "\"${OCTK_VERSION_NAME}\"")
octk_configure_definition("OCTK_VERSION_MAJOR" PUBLIC VALUE ${OCTK_VERSION_MAJOR})
octk_configure_definition("OCTK_VERSION_MINOR" PUBLIC VALUE ${OCTK_VERSION_MINOR})
octk_configure_definition("OCTK_VERSION_PATCH" PUBLIC VALUE ${OCTK_VERSION_PATCH})
octk_configure_definition("OCTK_VERSION_TWEAK" PUBLIC VALUE ${OCTK_VERSION_TWEAK})

octk_configure_definition("OCTK_SYSTEM_NAME" PUBLIC VALUE ${OCTK_SYSTEM_NAME})
octk_configure_definition("OCTK_SYSTEM_VERSION" PUBLIC VALUE ${OCTK_SYSTEM_VERSION})
octk_configure_definition("OCTK_SYSTEM_PROCESSOR" PUBLIC VALUE ${OCTK_SYSTEM_PROCESSOR})
octk_configure_definition("OCTK_CXX_COMPILER_ID" PUBLIC VALUE ${OCTK_CXX_COMPILER_ID})

# OpenCTK lib type
if(BUILD_SHARED_LIBS)
	octk_configure_definition("OCTK_BUILD_SHARED" PUBLIC)
else()
	octk_configure_definition("OCTK_BUILD_STATIC" PUBLIC)
endif()

# OpenCTK debug/optimization type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	octk_configure_definition("OCTK_BUILD_DEBUG" PUBLIC)
endif()

string(TIMESTAMP COMPILE_TIME %Y%m%d_%H%M%S)
octk_configure_definition("OCTK_COMPILE_TIME" PUBLIC VALUE "${COMPILE_TIME}")


octk_configure_feature("ENABLE_ASSERT" PUBLIC
	LABEL "Enable this to build enable assert"
	CONDITION ON)

octk_configure_feature("ENABLE_CHECK" PUBLIC
	LABEL "Enable this to build enable check"
	CONDITION ON)

octk_configure_feature("ENABLE_DEBUG" PUBLIC
	LABEL "Enable this to build enable debug"
	CONDITION ON)

octk_configure_feature("USE_STD_THREAD" PUBLIC
	LABEL "Enable this to build use std thread"
	CONDITION ON)


# int8_t type
octk_configure_compile_test_type(INT8_T
	TYPE "int8_t"
	LABEL "Check int8_t type")

# int16_t type
octk_configure_compile_test_type(INT16_T
	TYPE "int16_t"
	LABEL "Check int16_t type")

# int32_t type
octk_configure_compile_test_type(INT32_T
	TYPE "int32_t"
	LABEL "Check int32_t type")

# int64_t type
octk_configure_compile_test_type(INT64_T
	TYPE "int64_t"
	LABEL "Check int64_t type")

# uint8_t type
octk_configure_compile_test_type(UINT8_T
	TYPE "uint8_t"
	LABEL "Check uint8_t type")

# uint16_t type
octk_configure_compile_test_type(UINT16_T
	TYPE "uint16_t"
	LABEL "Check uint16_t type")

# uint32_t type
octk_configure_compile_test_type(UINT32_T
	TYPE "uint32_t"
	LABEL "Check uint32_t type")

# uint64_t type
octk_configure_compile_test_type(UINT64_T
	TYPE "uint64_t"
	LABEL "Check uint64_t type")
octk_configure_compile_test_type(INT64
	TYPE "__int64"
	LABEL "Check __int64 type")

# __uint128_t type
octk_configure_compile_test_type(INT128
	TYPE "__int128"
	LABEL "Check __int128 type")
octk_configure_compile_test_type(FLOAT128
	TYPE "__float128"
	LABEL "Check __float128 type")

# wchar_t type
octk_configure_compile_test_type(WCHAR_T
	TYPE "wchar_t"
	LABEL "Check wchar_t type")

# char8_t type
octk_configure_compile_test_type(CHAR8_T
	TYPE "char8_t"
	LABEL "Check char8_t type")

# char16_t type
octk_configure_compile_test_type(CHAR16_T
	TYPE "char16_t"
	LABEL "Check char16_t type")

# char32_t type
octk_configure_compile_test_type(CHAR32_T
	TYPE "char32_t"
	LABEL "Check char32_t type")

# char type
octk_configure_compile_test_type(CHAR
	TYPE "char"
	LABEL "Check char type")

# short type
octk_configure_compile_test_type(SHORT
	TYPE "short"
	LABEL "Check short type")

# int type
octk_configure_compile_test_type(INT
	TYPE "int"
	LABEL "Check int type")

# float type
octk_configure_compile_test_type(FLOAT
	TYPE "float"
	LABEL "Check float type")

# double type
octk_configure_compile_test_type(DOUBLE
	TYPE "double"
	LABEL "Check double type")

# long type
octk_configure_compile_test_type(LONG
	TYPE "long"
	LABEL "Check long type")

# long long type
octk_configure_compile_test_type(LONG_LONG
	TYPE "long long"
	LABEL "Check long long type")

# unsigned char type
octk_configure_compile_test_type(UCHAR
	TYPE "unsigned char"
	LABEL "Check unsigned char type")

# unsigned short type
octk_configure_compile_test_type(USHORT
	TYPE "unsigned short"
	LABEL "Check unsigned short type")

# unsigned int type
octk_configure_compile_test_type(UINT
	TYPE "unsigned int"
	LABEL "Check unsigned int type")

# unsigned long type
octk_configure_compile_test_type(ULONG
	TYPE "unsigned long"
	LABEL "Check unsigned long type")

# unsigned long long type
octk_configure_compile_test_type(ULONG_LONG
	TYPE "unsigned long long"
	LABEL "Check unsigned long long type")

# void* type
octk_configure_compile_test_type(VOIDP
	TYPE "void*"
	LABEL "Check void* type")

# size_t type
octk_configure_compile_test_type(SIZE_T
	TYPE "size_t"
	LABEL "Check size_t type")

# ssize_t type
octk_configure_compile_test_type(SSIZE_T
	TYPE "ssize_t"
	LABEL "Check ssize_t type")

if(TEST_SSIZE_T)
	octk_configure_definition("OCTK_SSIZE_T_SIZE" PUBLIC VALUE ${SIZEOF_SSIZE_T})
endif()

# ptrdiff_t type
octk_configure_compile_test_type(PTRDIFF_T
	TYPE "ptrdiff_t"
	LABEL "Check ptrdiff_t type")

# int64_t is long type
octk_configure_compile_test(INT64_IS_LONG_TYPE
	LABEL "Check int64_t is long type"
	FLAGS -Werror
	CODE
	"#if defined(_AIX) && !defined(__GNUC__)
    #pragma options langlvl=stdc99
    #endif
    #pragma GCC diagnostic error \"-Wincompatible-pointer-types\"
    #include <stdint.h>
    #include <stdio.h>
    int main(void)
    {
    int64_t i1 = 1;
    long *i2 = &i1;
    return 0;
    }")

# int64_t is long long type
octk_configure_compile_test(INT64_IS_LONG_LONG_TYPE
	LABEL "Check int64_t is long long type"
	FLAGS -Werror
	CODE
	"#if defined(_AIX) && !defined(__GNUC__)
    #pragma options langlvl=stdc99
    #endif
    #pragma GCC diagnostic error \"-Wincompatible-pointer-types\"
    #include <stdint.h>
    #include <stdio.h>
    int main(void)
    {
    int64_t i1 = 1;
    long long *i2 = &i1;
    return 0;
    }")

# size_t is short type
octk_configure_compile_test(SIZET_IS_SHORT_TYPE
	LABEL "Check size_t is short type"
	FLAGS -Werror
	CODE
	"#include <stddef.h>
    size_t f (size_t *i) { return *i + 1; }
    int main(void)
    {
    unsigned short i = 0;
    f (&i);
    return 0;
    }")

# size_t is int type
octk_configure_compile_test(SIZET_IS_INT_TYPE
	LABEL "Check size_t is int type"
	FLAGS -Werror
	CODE
	"#include <stddef.h>
    size_t f (size_t *i) { return *i + 1; }
    int main(void)
    {
    unsigned int i = 0;
    f (&i);
    return 0;
    }")

# size_t is long type
octk_configure_compile_test(SIZET_IS_LONG_TYPE
	LABEL "Check size_t is long type"
	FLAGS -Werror
	CODE
	"#include <stddef.h>
    size_t f (size_t *i) { return *i + 1; }
    int main(void)
    {
    unsigned long i = 0;
    f (&i);
    return 0;
    }")

# size_t is long long type
octk_configure_compile_test(SIZET_IS_LONG_LONG_TYPE
	LABEL "Check size_t is long long type"
	FLAGS -Werror
	CODE
	"#include <stddef.h>
    size_t f (size_t *i) { return *i + 1; }
    int main(void)
    {
    unsigned long long i = 0;
    f (&i);
    return 0;
    }")


octk_configure_definition("OCTK_HAS_INT8_T" PUBLIC VALUE ${TEST_INT8_T})
octk_configure_definition("OCTK_HAS_INT16_T" PUBLIC VALUE ${TEST_INT16_T})
octk_configure_definition("OCTK_HAS_INT32_T" PUBLIC VALUE ${TEST_INT32_T})
octk_configure_definition("OCTK_HAS_INT64_T" PUBLIC VALUE ${TEST_INT64_T})
octk_configure_definition("OCTK_HAS_UINT8_T" PUBLIC VALUE ${TEST_UINT8_T})
octk_configure_definition("OCTK_HAS_UINT16_T" PUBLIC VALUE ${TEST_UINT16_T})
octk_configure_definition("OCTK_HAS_UINT32_T" PUBLIC VALUE ${TEST_UINT32_T})
octk_configure_definition("OCTK_HAS_UINT64_T" PUBLIC VALUE ${TEST_UINT64_T})
octk_configure_definition("OCTK_HAS_INT64" PUBLIC VALUE ${TEST_INT64})
octk_configure_definition("OCTK_HAS_INT128" PUBLIC VALUE ${TEST_INT128})
octk_configure_definition("OCTK_HAS_FLOAT128" PUBLIC VALUE ${TEST_FLOAT128})
octk_configure_definition("OCTK_HAS_LONG_LONG" PUBLIC VALUE ${TEST_LONG_LONG})
octk_configure_definition("OCTK_HAS_WCHAR_T" PUBLIC VALUE ${TEST_WCHAR_T})
octk_configure_definition("OCTK_HAS_CHAR8_T" PUBLIC VALUE ${TEST_CHAR8_T})
octk_configure_definition("OCTK_HAS_CHAR16_T" PUBLIC VALUE ${TEST_CHAR16_T})
octk_configure_definition("OCTK_HAS_CHAR32_T" PUBLIC VALUE ${TEST_CHAR32_T})
octk_configure_definition("OCTK_HAS_SSIZE_T" PUBLIC VALUE ${TEST_SSIZE_T})
octk_configure_definition("OCTK_HAS_PTRDIFF_T" PUBLIC VALUE ${TEST_PTRDIFF_T})
octk_configure_definition("OCTK_SIZEOF_CHAR" PUBLIC VALUE ${SIZEOF_CHAR})
octk_configure_definition("OCTK_SIZEOF_SHORT" PUBLIC VALUE ${SIZEOF_SHORT})
octk_configure_definition("OCTK_SIZEOF_INT" PUBLIC VALUE ${SIZEOF_INT})
octk_configure_definition("OCTK_SIZEOF_FLOAT" PUBLIC VALUE ${SIZEOF_FLOAT})
octk_configure_definition("OCTK_SIZEOF_DOUBLE" PUBLIC VALUE ${SIZEOF_DOUBLE})
octk_configure_definition("OCTK_SIZEOF_LONG" PUBLIC VALUE ${SIZEOF_LONG})
octk_configure_definition("OCTK_SIZEOF_LONG_LONG" PUBLIC VALUE ${SIZEOF_LONG_LONG})
octk_configure_definition("OCTK_SIZEOF_UCHAR" PUBLIC VALUE ${SIZEOF_UCHAR})
octk_configure_definition("OCTK_SIZEOF_USHORT" PUBLIC VALUE ${SIZEOF_USHORT})
octk_configure_definition("OCTK_SIZEOF_UINT" PUBLIC VALUE ${SIZEOF_UINT})
octk_configure_definition("OCTK_SIZEOF_ULONG" PUBLIC VALUE ${SIZEOF_ULONG})
octk_configure_definition("OCTK_SIZEOF_ULONG_LONG" PUBLIC VALUE ${SIZEOF_ULONG_LONG})
octk_configure_definition("OCTK_SIZEOF_VOID_P" PUBLIC VALUE ${SIZEOF_VOIDP})
octk_configure_definition("OCTK_SIZEOF_SIZE_T" PUBLIC VALUE ${SIZEOF_SIZE_T})
octk_configure_definition("OCTK_SIZEOF_PTRDIFF_T" PUBLIC VALUE ${SIZEOF_PTRDIFF_T})
octk_configure_definition("OCTK_INT64_IS_LONG_TYPE" PUBLIC VALUE ${TEST_INT64_IS_LONG_TYPE})
octk_configure_definition("OCTK_INT64_IS_LONG_LONG_TYPE" PUBLIC VALUE ${TEST_INT64_IS_LONG_LONG_TYPE})
octk_configure_definition("OCTK_SIZET_IS_SHORT_TYPE" PUBLIC VALUE ${TEST_SIZET_IS_SHORT_TYPE})
octk_configure_definition("OCTK_SIZET_IS_INT_TYPE" PUBLIC VALUE ${TEST_SIZET_IS_INT_TYPE})
octk_configure_definition("OCTK_SIZET_IS_LONG_TYPE" PUBLIC VALUE ${TEST_SIZET_IS_LONG_TYPE})
octk_configure_definition("OCTK_SIZET_IS_LONG_LONG_TYPE" PUBLIC VALUE ${TEST_SIZET_IS_LONG_LONG_TYPE})