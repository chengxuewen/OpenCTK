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
	${ROOT_DIR}/octk_device_info_impl.cpp
	${ROOT_DIR}/octk_device_info_impl.hpp
	${ROOT_DIR}/octk_raw_video_sink_interface.hpp
	${ROOT_DIR}/octk_video_capture.hpp
	${ROOT_DIR}/octk_video_capture_config.hpp
	${ROOT_DIR}/octk_video_capture_defines.hpp
	${ROOT_DIR}/octk_video_capture_factory.cpp
	${ROOT_DIR}/octk_video_capture_factory.hpp
	${ROOT_DIR}/octk_video_capture_factory_null.cpp
	${ROOT_DIR}/octk_video_capture_impl.cpp
	${ROOT_DIR}/octk_video_capture_impl.hpp
	${ROOT_DIR}/octk_video_capture_options.cpp
	${ROOT_DIR}/octk_video_capture_options.hpp)
octk_internal_extend_target(octk_media
	SOURCES
	${ROOT_DIR}/mac/octk_camera_video_capturer_mac.hpp
	${ROOT_DIR}/mac/octk_camera_video_capturer_mac.mm
	${ROOT_DIR}/mac/octk_file_video_capturer_mac.hpp
	${ROOT_DIR}/mac/octk_file_video_capturer_mac.mm
	${ROOT_DIR}/octk_device_info_impl.cpp
	${ROOT_DIR}/octk_device_info_impl.hpp
	${ROOT_DIR}/octk_raw_video_sink_interface.hpp
	${ROOT_DIR}/octk_video_capture.hpp
	${ROOT_DIR}/octk_video_capture_config.hpp
	${ROOT_DIR}/octk_video_capture_defines.hpp
	${ROOT_DIR}/octk_video_capture_factory.cpp
	${ROOT_DIR}/octk_video_capture_factory.hpp
	${ROOT_DIR}/octk_video_capture_factory_null.cpp
	${ROOT_DIR}/octk_video_capture_impl.cpp
	${ROOT_DIR}/octk_video_capture_impl.hpp
	${ROOT_DIR}/octk_video_capture_options.cpp
	${ROOT_DIR}/octk_video_capture_options.hpp
	LIBRARIES
	"-framework AppKit"
	"-framework CoreAudio"
	"-framework CoreVideo"
	"-framework CoreMedia"
	"-framework IOSurface"
	"-framework CoreServices"
	"-framework AVFoundation"
	"-framework AudioToolbox"
	"-framework ScreenCaptureKit"
	"-framework ApplicationServices"
	CONDITION OCTK_SYSTEM_MAC)