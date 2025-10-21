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

# We can't create the same interface imported target multiple times, CMake will complain if we do
# that. This can happen if the find_package call is done in multiple different subdirectories.
if(TARGET OCTK3rdparty::WrapFFmpeg)
    set(OCTKWrapFFmpeg_FOUND ON)
    return()
endif()

include(InstallVcpkg)
set(OCTKWrapFFmpeg_NAME "ffmpeg")
set(OCTKWrapFFmpeg_ROOT_DIR "${PROJECT_BINARY_DIR}/3rdparty/${OCTKWrapFFmpeg_NAME}")
if(WIN32)
    set(OCTKWrapFFmpeg_VCPKG_TRIPLET ${OCTK_VCPKG_TRIPLET}-static-md)
else()
    set(OCTKWrapFFmpeg_VCPKG_TRIPLET ${OCTK_VCPKG_TRIPLET})
endif()
set(OCTKWrapFFmpeg_INSTALL_DIR "${OCTKWrapFFmpeg_ROOT_DIR}/installed/${OCTKWrapFFmpeg_VCPKG_TRIPLET}" CACHE INTERNAL "" FORCE)
if(NOT EXISTS "${OCTKWrapFFmpeg_INSTALL_DIR}")
    execute_process(
        COMMAND ${OCTKVcpkg_EXECUTABLE} list ffmpeg:${OCTKWrapFFmpeg_VCPKG_TRIPLET}
        WORKING_DIRECTORY "${OCTKVcpkg_ROOT_DIR}"
        OUTPUT_VARIABLE FIND_OUTPUT
        RESULT_VARIABLE FIND_RESULT)
    if("X${FIND_OUTPUT}" STREQUAL "X")
        message(STATUS "${OCTKWrapFFmpeg_NAME} not installed, start install...")
        list(APPEND OCTKWrapFFmpeg_COMPONENTS
            swresample
            avresample
            avdevice
            avfilter
            avformat
            avcodec
            openh264
            swscale
            fdk-aac
            snappy
            speex
            opus
            soxr
            vpx)
        if(WIN32)
            list(APPEND OCTKWrapFFmpeg_COMPONENTS nvcodec amf)
        elseif(NOT OCTK_SYSTEM_DARWIN)
            list(APPEND OCTKWrapFFmpeg_COMPONENTS nvcodec qsv amf)
        endif()
        unset(OCTKWrapFFmpeg_COMPONENTS_CONFIGS)
        foreach(component IN LISTS OCTKWrapFFmpeg_COMPONENTS)
            if(NOT "${OCTKWrapFFmpeg_COMPONENTS_CONFIGS}" STREQUAL "")
                set(OCTKWrapFFmpeg_COMPONENTS_CONFIGS "${OCTKWrapFFmpeg_COMPONENTS_CONFIGS},")
            endif()
            set(OCTKWrapFFmpeg_COMPONENTS_CONFIGS "${OCTKWrapFFmpeg_COMPONENTS_CONFIGS}${component}")
        endforeach()
        set(OCTKWrapFFmpeg_VCPKG_CONFIGS ffmpeg[${OCTKWrapFFmpeg_COMPONENTS_CONFIGS}]:${OCTKWrapFFmpeg_VCPKG_TRIPLET})
        message(STATUS "${OCTKWrapFFmpeg_NAME} vcpkg install configs: ${OCTKWrapFFmpeg_VCPKG_CONFIGS}")
        execute_process(
            COMMAND "${OCTKVcpkg_EXECUTABLE}" install ${OCTKWrapFFmpeg_VCPKG_CONFIGS} --recurse
            WORKING_DIRECTORY "${OCTKVcpkg_ROOT_DIR}"
            RESULT_VARIABLE INSTALL_RESULT
            COMMAND_ECHO STDOUT)
        if(NOT (INSTALL_RESULT MATCHES 0))
            message(FATAL_ERROR "${OCTKWrapFFmpeg_NAME} install failed.")
        endif()
    endif()

    execute_process(
        COMMAND "${OCTKVcpkg_EXECUTABLE}" export ${OCTKWrapFFmpeg_NAME}:${OCTKWrapFFmpeg_VCPKG_TRIPLET}
        --raw --output=${OCTKWrapFFmpeg_NAME} --output-dir=${PROJECT_BINARY_DIR}/3rdparty
        WORKING_DIRECTORY "${OCTKVcpkg_ROOT_DIR}"
        RESULT_VARIABLE EXPORT_RESULT
        COMMAND_ECHO STDOUT)
    if(NOT (EXPORT_RESULT MATCHES 0))
        message(FATAL_ERROR "${OCTKWrapFFmpeg_NAME} export failed.")
    endif()
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(OCTKWrapFFmpeg_PKGCONFIG_DIR "${OCTKWrapFFmpeg_INSTALL_DIR}/debug/lib")
else()
    set(OCTKWrapFFmpeg_PKGCONFIG_DIR "${OCTKWrapFFmpeg_INSTALL_DIR}/lib")
endif()
octk_pkg_check_modules(FFmpeg REQUIRED
    PATH "${OCTKWrapFFmpeg_PKGCONFIG_DIR}/pkgconfig"
    IMPORTED_TARGET
    libswresample
    libavdevice
    libavformat
    libavfilter
    libavcodec
    libavutil
    libswscale)
add_library(OCTK3rdparty::WrapFFmpeg INTERFACE IMPORTED)
target_link_libraries(OCTK3rdparty::WrapFFmpeg INTERFACE PkgConfig::FFmpeg)
set(OCTKWrapFFmpeg_FOUND ON)
