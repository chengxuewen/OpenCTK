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

#-----------------------------------------------------------------------------------------------------------------------
# octk_set01 finction
#-----------------------------------------------------------------------------------------------------------------------
function(octk_set01 result)
    if(${ARGN})
        set("${result}" 1 PARENT_SCOPE)
    else()
        set("${result}" 0 PARENT_SCOPE)
    endif()
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
# OpenCTK set system variable
#-----------------------------------------------------------------------------------------------------------------------
message(STATUS "Build in system: ${CMAKE_SYSTEM_NAME}")
set(OCTK_SYSTEM_NAME ${CMAKE_SYSTEM_NAME})
set(OCTK_SYSTEM_VERSION ${CMAKE_SYSTEM_VERSION})
set(OCTK_SYSTEM_PROCESSOR ${CMAKE_SYSTEM_PROCESSOR})
octk_set01(OCTK_SYSTEM_LINUX
    CMAKE_SYSTEM_NAME STREQUAL "Linux")
octk_set01(OCTK_SYSTEM_WINCE
    CMAKE_SYSTEM_NAME STREQUAL "WindowsCE")
octk_set01(OCTK_SYSTEM_WIN
    OCTK_SYSTEM_WINCE OR CMAKE_SYSTEM_NAME STREQUAL "Windows")
octk_set01(OCTK_SYSTEM_HPUX
    CMAKE_SYSTEM_NAME STREQUAL "HPUX")
octk_set01(OCTK_SYSTEM_ANDROID
    CMAKE_SYSTEM_NAME STREQUAL "Android")
octk_set01(OCTK_SYSTEM_NACL
    CMAKE_SYSTEM_NAME STREQUAL "NaCl")
octk_set01(OCTK_SYSTEM_INTEGRITY
    CMAKE_SYSTEM_NAME STREQUAL "Integrity")
octk_set01(OCTK_SYSTEM_VXWORKS
    CMAKE_SYSTEM_NAME STREQUAL "VxWorks")
octk_set01(OCTK_SYSTEM_QNX
    CMAKE_SYSTEM_NAME STREQUAL "QNX")
octk_set01(OCTK_SYSTEM_OPENBSD
    CMAKE_SYSTEM_NAME STREQUAL "OpenBSD")
octk_set01(OCTK_SYSTEM_FREEBSD
    CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
octk_set01(OCTK_SYSTEM_NETBSD
    CMAKE_SYSTEM_NAME STREQUAL "NetBSD")
octk_set01(OCTK_SYSTEM_WASM
    CMAKE_SYSTEM_NAME STREQUAL "Emscripten" OR EMSCRIPTEN)
octk_set01(OCTK_SYSTEM_SOLARIS
    CMAKE_SYSTEM_NAME STREQUAL "SunOS")
octk_set01(OCTK_SYSTEM_HURD
    CMAKE_SYSTEM_NAME STREQUAL "GNU")
# This is the only reliable way we can determine the webOS platform as the yocto recipe adds this compile definition
# into its generated toolchain.cmake file
octk_set01(OCTK_SYSTEM_WEBOS
    CMAKE_CXX_FLAGS MATCHES "-D__WEBOS__")
octk_set01(OCTK_SYSTEM_BSD
    APPLE OR OPENBSD OR FREEBSD OR NETBSD)
octk_set01(OCTK_SYSTEM_DARWIN
    APPLE OR CMAKE_SYSTEM_NAME STREQUAL "Darwin")
octk_set01(OCTK_SYSTEM_IOS
    APPLE AND CMAKE_SYSTEM_NAME STREQUAL "iOS")
octk_set01(OCTK_SYSTEM_TVOS
    APPLE AND CMAKE_SYSTEM_NAME STREQUAL "tvOS")
octk_set01(OCTK_SYSTEM_WATCHOS
    APPLE AND CMAKE_SYSTEM_NAME STREQUAL "watchOS")
octk_set01(OCTK_SYSTEM_UIKIT
    APPLE AND (IOS OR TVOS OR WATCHOS))
octk_set01(OCTK_SYSTEM_MACOS
    APPLE AND NOT UIKIT)
octk_set01(OCTK_SYSTEM_UNIX UNIX)
octk_set01(OCTK_SYSTEM_WIN32 WIN32)
octk_set01(OCTK_SYSTEM_APPLE APPLE)
octk_set01(OCTK_SYSTEM_MAC APPLE)


#-----------------------------------------------------------------------------------------------------------------------
# OpenCTK set processor variable
#-----------------------------------------------------------------------------------------------------------------------
message(STATUS "Build in processor: ${CMAKE_SYSTEM_PROCESSOR}")
set(OCTK_SYSTEM_PROCESSOR ${CMAKE_SYSTEM_PROCESSOR})
octk_set01(OCTK_PROCESSOR_I386
    CMAKE_SYSTEM_PROCESSOR STREQUAL "i386")
octk_set01(OCTK_PROCESSOR_I686
    CMAKE_CXX_COMPILER_ID MATCHES "i686")
octk_set01(OCTK_PROCESSOR_X86_64
    CMAKE_CXX_COMPILER_ID MATCHES "x86_64")
octk_set01(OCTK_PROCESSOR_X86
    CMAKE_CXX_COMPILER_ID MATCHES "x86")
octk_set01(OCTK_PROCESSOR_AMD64
    CMAKE_CXX_COMPILER_ID STREQUAL "amd64")
octk_set01(OCTK_PROCESSOR_ARM64
    CMAKE_CXX_COMPILER_ID STREQUAL "arm64")
octk_set01(OCTK_PROCESSOR_ARM32
    CMAKE_CXX_COMPILER_ID STREQUAL "arm32")
octk_set01(OCTK_PROCESSOR_ARM
    OCTK_PROCESSOR_ARM64 OR OCTK_PROCESSOR_ARM32)


#-----------------------------------------------------------------------------------------------------------------------
# OpenCTK set cxx compiler variable
#-----------------------------------------------------------------------------------------------------------------------
message(STATUS "Build in cxx compiler: ${CMAKE_CXX_COMPILER_ID}")
set(OCTK_CXX_COMPILER_ID ${CMAKE_CXX_COMPILER_ID})
set(OCTK_CXX_COMPILER_VERSION ${CMAKE_CXX_COMPILER_VERSION})
octk_set01(OCTK_CXX_COMPILER_GNU
    CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
octk_set01(OCTK_CXX_COMPILER_MSVC
    MSVC OR CMAKE_CXX_COMPILER_ID STREQUAL "Msvc")
octk_set01(OCTK_CXX_COMPILER_MINGW
    MINGW OR CMAKE_CXX_COMPILER_ID STREQUAL "Mingw")
octk_set01(OCTK_CXX_COMPILER_CLANG
    CMAKE_CXX_COMPILER_ID MATCHES "Clang|IntelLLVM")
octk_set01(OCTK_CXX_COMPILER_APPLE_CLANG
    CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
octk_set01(OCTK_CXX_COMPILER_INTEL_LLVM
    CMAKE_CXX_COMPILER_ID STREQUAL "IntelLLVM")
octk_set01(OCTK_CXX_COMPILER_QCC
    CMAKE_CXX_COMPILER_ID STREQUAL "QCC") # CMP0047


#-----------------------------------------------------------------------------------------------------------------------
# OpenCTK arch size variable
#-----------------------------------------------------------------------------------------------------------------------
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(OCTK_ARCH_64BIT TRUE)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(OCTK_ARCH_32BIT TRUE)
endif()


#-----------------------------------------------------------------------------------------------------------------------
# OpenCTK platform compile arch variable
#-----------------------------------------------------------------------------------------------------------------------
string(TOUPPER "${CMAKE_BUILD_TYPE}" OCTK_UPPER_BUILD_TYPE)
string(TOLOWER "${CMAKE_BUILD_TYPE}" OCTK_LOWER_BUILD_TYPE)
string(TOLOWER "${CMAKE_SYSTEM_NAME}" OCTK_LOWER_SYSTEM_NAME)
string(TOLOWER "${CMAKE_CXX_COMPILER_ID}" OCTK_LOWER_CXX_COMPILER_ID)
string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" OCTK_LOWER_SYSTEM_PROCESSOR)
set(OCTK_X64_PROCESSORS "amd64" "x64" "x86_64")
set(OCTK_X86_PROCESSORS "i386" "i686" "x86")
set(OCTK_ARM32_PROCESSORS "arm32" "arm")
set(OCTK_ARM64_PROCESSORS "arm64")
if(OCTK_LOWER_SYSTEM_PROCESSOR IN_LIST OCTK_X64_PROCESSORS)
    set(OCTK_PROCESSOR_MERGE_NAME x64)
elseif(OCTK_LOWER_SYSTEM_PROCESSOR IN_LIST OCTK_X86_PROCESSORS)
    set(OCTK_PROCESSOR_MERGE_NAME x86)
elseif(OCTK_LOWER_SYSTEM_PROCESSOR IN_LIST OCTK_ARM32_PROCESSORS)
    set(OCTK_PROCESSOR_MERGE_NAME arm32)
elseif(OCTK_LOWER_SYSTEM_PROCESSOR IN_LIST OCTK_ARM64_PROCESSORS)
    set(OCTK_PROCESSOR_MERGE_NAME arm64)
else()
    message(FATAL_ERROR "Unknown system processor.")
endif()
set(OCTK_PLATFORM_NAME "${OCTK_LOWER_SYSTEM_NAME}-${OCTK_PROCESSOR_MERGE_NAME}")
set(OCTK_PLATFORM_COMPILER_NAME "${OCTK_PLATFORM_NAME}-${OCTK_LOWER_CXX_COMPILER_ID}")
message(STATUS "Platform name: ${OCTK_PLATFORM_NAME}")
message(STATUS "Platform compiler name: ${OCTK_PLATFORM_COMPILER_NAME}")


#-----------------------------------------------------------------------------------------------------------------------
# OpenCTK arch size variable
#-----------------------------------------------------------------------------------------------------------------------
if(win32)
    set(OCTK_EXECUTABLE_SUFFIX ".exe")
else()
    set(OCTK_EXECUTABLE_SUFFIX "")
endif()


#-----------------------------------------------------------------------------------------------------------------------
# OpenCTK mkspecs version
#-----------------------------------------------------------------------------------------------------------------------
if(OCTK_SYSTEM_WIN32)
    set(OCTK_DEFAULT_PLATFORM_DEFINITIONS WIN32 _ENABLE_EXTENDED_ALIGNED_STORAGE)
    if(OCTK_ARCH_64BIT)
        list(APPEND OCTK_DEFAULT_PLATFORM_DEFINITIONS WIN64 _WIN64)
    endif()
    if(OCTK_CXX_COMPILER_MSVC)
        if(OCTK_CXX_COMPILER_CLANG)
            set(OCTK_DEFAULT_MKSPEC win32-clang-msvc)
        elseif(OCTK_PROCESSOR_ARM64)
            set(OCTK_DEFAULT_MKSPEC win32-arm64-msvc)
        else()
            set(OCTK_DEFAULT_MKSPEC win32-msvc)
        endif()
    elseif(OCTK_CXX_COMPILER_CLANG AND OCTK_CXX_COMPILER_MINGW)
        set(OCTK_DEFAULT_MKSPEC win32-clang-g++)
    elseif(OCTK_CXX_COMPILER_MINGW)
        set(OCTK_DEFAULT_MKSPEC win32-g++)
    endif()

    if(OCTK_CXX_COMPILER_MINGW)
        list(APPEND OCTK_DEFAULT_PLATFORM_DEFINITIONS MINGW_HAS_SECURE_API=1)
    endif()
elseif(OCTK_SYSTEM_LINUX)
    if(OCTK_CXX_COMPILER_GNU)
        set(OCTK_DEFAULT_MKSPEC linux-g++)
    elseif(OCTK_CXX_COMPILER_CLANG)
        set(OCTK_DEFAULT_MKSPEC linux-clang)
    endif()
elseif(OCTK_SYSTEM_ANDROID)
    if(OCTK_CXX_COMPILER_GNU)
        set(OCTK_DEFAULT_MKSPEC android-g++)
    elseif(OCTK_CXX_COMPILER_CLANG)
        set(OCTK_DEFAULT_MKSPEC android-clang)
    endif()
elseif(OCTK_SYSTEM_IOS)
    set(OCTK_DEFAULT_MKSPEC macx-ios-clang)
elseif(OCTK_SYSTEM_APPLE)
    set(OCTK_DEFAULT_MKSPEC macx-clang)
elseif(OCTK_SYSTEM_WASM)
    set(OCTK_DEFAULT_MKSPEC wasm-emscripten)
elseif(OCTK_SYSTEM_QNX)
    # Certain POSIX defines are not set if we don't compile with -std=gnuXX
    set(OCTK_ENABLE_CXX_EXTENSIONS ON)

    list(APPEND OCTK_DEFAULT_PLATFORM_DEFINITIONS _FORTIFY_SOURCE=2 _REENTRANT)

    set(compiler_aarch64le aarch64le)
    set(compiler_armle-v7 armv7le)
    set(compiler_x86-64 x86_64)
    set(compiler_x86 x86)
    foreach(arch aarch64le armle-v7 x86-64 x86)
        if(CMAKE_CXX_COMPILER_TARGET MATCHES "${compiler_${arch}}$")
            set(OCTK_DEFAULT_MKSPEC qnx-${arch}-qcc)
        endif()
    endforeach()
elseif(OCTK_SYSTEM_FREEBSD)
    if(OCTK_CXX_COMPILER_CLANG)
        set(OCTK_DEFAULT_MKSPEC freebsd-clang)
    elseif(OCTK_CXX_COMPILER_GNU)
        set(OCTK_DEFAULT_MKSPEC freebsd-g++)
    endif()
elseif(OCTK_SYSTEM_NETBSD)
    set(OCTK_DEFAULT_MKSPEC netbsd-g++)
elseif(OCTK_SYSTEM_OPENBSD)
    set(OCTK_DEFAULT_MKSPEC openbsd-g++)
elseif(OCTK_SYSTEM_SOLARIS)
    if(OCTK_CXX_COMPILER_GNU)
        if(OCTK_ARCH_64BIT)
            set(OCTK_DEFAULT_MKSPEC solaris-g++-64)
        else()
            set(OCTK_DEFAULT_MKSPEC solaris-g++)
        endif()
    else()
        if(OCTK_ARCH_64BIT)
            set(OCTK_DEFAULT_MKSPEC solaris-cc-64)
        else()
            set(OCTK_DEFAULT_MKSPEC solaris-cc)
        endif()
    endif()
elseif(OCTK_SYSTEM_HURD)
    set(OCTK_DEFAULT_MKSPEC hurd-g++)
endif()

if(NOT OCTK_DEFAULT_MKSPEC)
    message(FATAL_ERROR "mkspec not Detected!")
else()
    message(STATUS "Build in mkspec: ${OCTK_DEFAULT_MKSPEC}")
endif()

if(NOT DEFINED OCTK_DEFAULT_PLATFORM_DEFINITIONS)
    set(OCTK_DEFAULT_PLATFORM_DEFINITIONS "")
endif()

set(OCTK_PLATFORM_DEFINITIONS ${OCTK_DEFAULT_PLATFORM_DEFINITIONS} CACHE STRING "OpenCTK platform specific pre-processor defines")


#-----------------------------------------------------------------------------------------------------------------------
# OpenCTK parse version
#-----------------------------------------------------------------------------------------------------------------------
# Parses a version string like "xx.yy.zz" and sets the major, minor and patch variables.
function(octk_parse_version_string version_string out_var_prefix)
    string(REPLACE "." ";" version_list ${version_string})
    list(LENGTH version_list length)

    set(out_var "${out_var_prefix}_MAJOR")
    set(value "")
    if(length GREATER 0)
        list(GET version_list 0 value)
        list(REMOVE_AT version_list 0)
        math(EXPR length "${length}-1")
    endif()
    set(${out_var} "${value}" PARENT_SCOPE)

    set(out_var "${out_var_prefix}_MINOR")
    set(value "")
    if(length GREATER 0)
        list(GET version_list 0 value)
        set(${out_var} "${value}" PARENT_SCOPE)
        list(REMOVE_AT version_list 0)
        math(EXPR length "${length}-1")
    endif()
    set(${out_var} "${value}" PARENT_SCOPE)

    set(out_var "${out_var_prefix}_PATCH")
    set(value "")
    if(length GREATER 0)
        list(GET version_list 0 value)
        set(${out_var} "${value}" PARENT_SCOPE)
        list(REMOVE_AT version_list 0)
        math(EXPR length "${length}-1")
    endif()
    set(${out_var} "${value}" PARENT_SCOPE)
endfunction()

# Set up the separate version components for the compiler version, to allow mapping of qmake
# conditions like 'equals(OCTK_GCC_MAJOR_VERSION,5)'.
if(CMAKE_CXX_COMPILER_VERSION)
    octk_parse_version_string("${CMAKE_CXX_COMPILER_VERSION}" "OCTK_COMPILER_VERSION")
endif()
