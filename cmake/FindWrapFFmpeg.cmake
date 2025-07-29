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

if(EXISTS "${PROJECT_SOURCE_DIR}/3rdparty/ffmpeg7-${OCTK_PLATFORM_NAME}.7z")
    #:x64-osx // no support : alsa,qsv,nvcodec
    #./vcpkg install ffmpeg[avcodec,avdevice,avfilter,avformat,avresample,swresample,swscale,fdk-aac,opus,snappy,soxr,speex,openh264,vpx,amf]:x64-osx --recurse
    #:x64-linux //
    #./vcpkg install ffmpeg[alsa,avcodec,avdevice,avfilter,avformat,avresample,swresample,swscale,fdk-aac,opus,snappy,soxr,speex,openh264,vpx,nvcodec,amf,qsv]:x64-linux --recurse
    #:x64-windows // no support : alsa
    #.\vcpkg.exe install ffmpeg[avcodec,avdevice,avfilter,avformat,avresample,swresample,swscale,fdk-aac,opus,snappy,soxr,speex,openh264,vpx,nvcodec,amf,qsv]:x64-windows-static-md --recurse
    set(OCTKWrapFFmpeg_DIR_NAME "ffmpeg7-${OCTK_PLATFORM_NAME}")
    set(OCTKWrapFFmpeg_PKG_NAME "ffmpeg7-${OCTK_PLATFORM_NAME}.7z")
    set(OCTKWrapFFmpeg_URL_PATH "${PROJECT_SOURCE_DIR}/3rdparty/${OCTKWrapFFmpeg_PKG_NAME}")
    set(OCTKWrapFFmpeg_ROOT_DIR "${PROJECT_BINARY_DIR}/3rdparty/${OCTKWrapFFmpeg_DIR_NAME}")
    set(OCTKWrapFFmpeg_BUILD_DIR "${OCTKWrapFFmpeg_ROOT_DIR}/build" CACHE INTERNAL "" FORCE)
    set(OCTKWrapFFmpeg_SOURCE_DIR "${OCTKWrapFFmpeg_ROOT_DIR}/source" CACHE INTERNAL "" FORCE)
    set(OCTKWrapFFmpeg_INSTALL_DIR "${OCTKWrapFFmpeg_ROOT_DIR}/source" CACHE INTERNAL "" FORCE)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(OCTKWrapFFmpeg_LIBS_DIR "${OCTKWrapFFmpeg_INSTALL_DIR}/debug/bin")
        set(OCTKWrapFFmpeg_PKGCONFIG_DIR "${OCTKWrapFFmpeg_INSTALL_DIR}/debug/lib")
    else()
        set(OCTKWrapFFmpeg_LIBS_DIR "${OCTKWrapFFmpeg_INSTALL_DIR}/bin")
        set(OCTKWrapFFmpeg_PKGCONFIG_DIR "${OCTKWrapFFmpeg_INSTALL_DIR}/lib")
    endif()
    octk_stamp_file_info(OCTKWrapFFmpeg OUTPUT_DIR "${OCTKWrapFFmpeg_ROOT_DIR}")
    octk_fetch_3rdparty(OCTKWrapFFmpeg URL "${OCTKWrapFFmpeg_URL_PATH}")
    octk_pkg_check_modules(FFmpeg REQUIRED
        PATH "${OCTKWrapFFmpeg_PKGCONFIG_DIR}/pkgconfig"
        IMPORTED_TARGET
        libavcodec
        libavdevice
        libavformat
        libavfilter
        libavutil
        libswresample
        libswscale)
    add_library(OCTK3rdparty::WrapFFmpeg INTERFACE IMPORTED)
    target_link_libraries(OCTK3rdparty::WrapFFmpeg INTERFACE PkgConfig::FFmpeg)
    # copy lib to build dir
    execute_process(
#        COMMAND ${CMAKE_COMMAND} -E copy_directory "${OCTKWrapFFmpeg_LIBS_DIR}" "${OCTK_BUILD_DIR}/${OCTK_DEFAULT_DLLDIR}/"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${OCTKWrapFFmpeg_INSTALL_DIR}/tools" "${OCTK_BUILD_DIR}/${OCTK_DEFAULT_LIBEXEC}/"
        WORKING_DIRECTORY "${OCTKWrapFFmpeg_ROOT_DIR}"
        ERROR_QUIET)
    if(WIN32)
#        octk_install(
#            DIRECTORY "${OCTKWrapFFmpeg_LIBS_DIR}"
#            DESTINATION "${CMAKE_INSTALL_PREFIX}"
#            PATTERN "bin/*" PERMISSIONS OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE)
        octk_install(
            DIRECTORY "${OCTKWrapFFmpeg_INSTALL_DIR}/tools"
            DESTINATION "${CMAKE_INSTALL_PREFIX}"
            PATTERN "tools/*" PERMISSIONS OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE)
    else()
#        octk_install(
#            DIRECTORY "${OCTKWrapFFmpeg_LIBS_DIR}"
#            DESTINATION "${CMAKE_INSTALL_PREFIX}/${OCTK_DEFAULT_DLLDIR}"
#            PATTERN "lib/*" PERMISSIONS OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE)
        octk_install(
            DIRECTORY "${OCTKWrapFFmpeg_INSTALL_DIR}/tools"
            DESTINATION "${CMAKE_INSTALL_PREFIX}/${OCTK_DEFAULT_LIBEXEC}"
            PATTERN "tools/*" PERMISSIONS OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE)
    endif()
else()
    octk_pkg_check_modules(FFmpeg REQUIRED
        IMPORTED_TARGET libavcodec libavdevice libavformat libavfilter libavutil libswresample libswscale libpostproc)
    add_library(OCTK3rdparty::WrapFFmpeg INTERFACE IMPORTED)
    target_link_libraries(OCTK3rdparty::WrapFFmpeg INTERFACE PkgConfig::FFmpeg)
endif()
set(OCTKWrapFFmpeg_FOUND ON)
