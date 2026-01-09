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

set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
#-----------------------------------------------------------------------------------------------------------------------
# extend lib target
#-----------------------------------------------------------------------------------------------------------------------
octk_internal_extend_target(octk_media
    SOURCES
    ${ROOT_DIR}/octk_camera_capture.cpp
    ${ROOT_DIR}/octk_camera_capture.hpp
    ${ROOT_DIR}/octk_camera_capture_p.hpp)
octk_internal_extend_target(octk_media
    SOURCES
    ${ROOT_DIR}/octk_camera_capture_v4l2.cpp
    ${ROOT_DIR}/octk_camera_capture_v4l2_p.hpp
    ${ROOT_DIR}/octk_camera_device_info_v4l2.cpp
    ${ROOT_DIR}/octk_camera_device_info_v4l2_p.hpp
    LIBRARIES
    CONDITION OCTK_SYSTEM_LINUX)
octk_internal_extend_target(octk_media
    SOURCES
    LIBRARIES
#    "-framework AppKit"
#    "-framework CoreAudio"
#    "-framework CoreVideo"
#    "-framework CoreMedia"
#    "-framework IOSurface"
#    "-framework CoreServices"
#    "-framework AVFoundation"
#    "-framework AudioToolbox"
#    "-framework ScreenCaptureKit"
#    "-framework ApplicationServices"
    CONDITION OCTK_SYSTEM_MAC)
if(OCTK_FEATURE_ENABLE_CAPTURE_CAMERA_PIPEWIRE)
    pkg_check_modules(pipewire QUIET IMPORTED_TARGET libpipewire-0.3)
    octk_internal_extend_target(octk_media
        SOURCES
        ${ROOT_DIR}/octk_camera_capture_pipewire.cpp
        ${ROOT_DIR}/octk_camera_capture_pipewire_p.hpp
        ${ROOT_DIR}/octk_camera_device_info_pipewire.cpp
        ${ROOT_DIR}/octk_camera_device_info_pipewire_p.hpp
        LIBRARIES
        PkgConfig::pipewire)
endif()

