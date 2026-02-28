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
if (OCTK_FEATURE_MEDIA_ENABLE_CAPTURE_CAMERA_PIPEWIRE)
	octk_internal_extend_target(octk_media
		SOURCES
		${ROOT_DIR}/octk_camera_capture_pipewire.cpp
		${ROOT_DIR}/octk_camera_capture_pipewire_p.hpp
		${ROOT_DIR}/octk_camera_device_info_pipewire.cpp
		${ROOT_DIR}/octk_camera_device_info_pipewire_p.hpp
		${ROOT_DIR}/octk_pipewire_session.cpp
		${ROOT_DIR}/octk_pipewire_session_p.hpp
		LIBRARIES
		PkgConfig::PipeWire)
endif ()
if (OCTK_FEATURE_MEDIA_ENABLE_CAPTURE_CAMERA_LIBARGUS)
	set(JETSON_MULTIMEDIA_API_DIR /usr/src/jetson_multimedia_api)
	set(TEGRA_LIB_DIR /usr/lib/${CMAKE_LIBRARY_ARCHITECTURE}/tegra)
	find_library(EGL_LIBRARY EGL)
	find_library(NVV4L2_LIBRARY nvv4l2 HINTS ${TEGRA_LIB_DIR})
	find_library(NVARGUS_LIBRARY nvargus HINTS ${TEGRA_LIB_DIR})
	find_library(NVARGUS_SOCKETCLIENT_LIBRARY nvargus_socketclient HINTS ${TEGRA_LIB_DIR})
	find_library(NVBUF_SURFACE_LIBRARY nvbufsurface HINTS ${TEGRA_LIB_DIR})
	find_library(NVBUF_SURFACE_TRANSFORM_LIBRARY nvbufsurftransform HINTS ${TEGRA_LIB_DIR})
	octk_internal_extend_target(octk_media
		SOURCES
		${JETSON_MULTIMEDIA_API_DIR}/samples/common/classes/NvBuffer.cpp
		${JETSON_MULTIMEDIA_API_DIR}/samples/common/classes/NvBufSurface.cpp
		${JETSON_MULTIMEDIA_API_DIR}/samples/common/classes/NvElement.cpp
		${JETSON_MULTIMEDIA_API_DIR}/samples/common/classes/NvElementProfiler.cpp
		${JETSON_MULTIMEDIA_API_DIR}/samples/common/classes/NvV4l2Element.cpp
		${JETSON_MULTIMEDIA_API_DIR}/samples/common/classes/NvV4l2ElementPlane.cpp
		${JETSON_MULTIMEDIA_API_DIR}/samples/common/classes/NvVideoEncoder.cpp
		${JETSON_MULTIMEDIA_API_DIR}/samples/common/classes/NvLogging.cpp
		${JETSON_MULTIMEDIA_API_DIR}/argus/samples/utils/Thread.cpp
		${JETSON_MULTIMEDIA_API_DIR}/argus/samples/utils/NativeBuffer.cpp
		${JETSON_MULTIMEDIA_API_DIR}/argus/samples/utils/nvmmapi/NvNativeBuffer.cpp
		INCLUDE_DIRECTORIES
		${JETSON_MULTIMEDIA_API_DIR}/include
		${JETSON_MULTIMEDIA_API_DIR}/argus/samples/utils
		${JETSON_MULTIMEDIA_API_DIR}/samples/common/classes
		LIBRARIES
		${EGL_LIBRARY}
		${NVV4L2_LIBRARY}
		${NVARGUS_LIBRARY}
		${NVBUF_SURNVARGUS_SOCKETCLIENT_LIBRARY}
		${NVBUF_SURFACE_LIBRARY}
		${NVBUF_SURFACE_TRANSFORM_LIBRARY})
endif ()

