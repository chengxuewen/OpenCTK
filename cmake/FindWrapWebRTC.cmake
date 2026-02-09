########################################################################################################################
#
# Library: OpenCTK
#
# Copyright (C) 2026~Present ChengXueWen.
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
if(TARGET OCTK3rdparty::WrapWebRTC)
	set(OCTKWrapWebRTC_FOUND ON)
	return()
endif()

if(NOT OCTK_3RDPARTY_WEBRTC_VERSION)
	message(FATAL_ERROR "3rdparty webrtc version not set.")
endif()
if(EXISTS "${OCTK_3RDPARTY_WEBRTC_INCLUDE_DIR}" AND EXISTS "${OCTK_3RDPARTY_WEBRTC_LIBRARY}")
	set(OCTKWrapWebRTC_INCLUDE_DIR "${OCTK_3RDPARTY_WEBRTC_INCLUDE_DIR}")
	set(OCTKWrapWebRTC_LIBRARY "${OCTK_3RDPARTY_WEBRTC_LIBRARY}")
elseif(EXISTS "${OCTK_3RDPARTY_WEBRTC_PATH}")
	get_filename_component(OCTKWrapWebRTC_DIR_NAME "${OCTK_3RDPARTY_WEBRTC_PATH}" NAME_WE)
	set(OCTKWrapWebRTC_URL_PATH "${OCTK_3RDPARTY_WEBRTC_PATH}")
	set(OCTKWrapWebRTC_ROOT_DIR "${PROJECT_BINARY_DIR}/3rdparty/${OCTKWrapWebRTC_DIR_NAME}")
	set(OCTKWrapWebRTC_BUILD_DIR "${OCTKWrapWebRTC_ROOT_DIR}/build" CACHE INTERNAL "" FORCE)
	set(OCTKWrapWebRTC_SOURCE_DIR "${OCTKWrapWebRTC_ROOT_DIR}/source" CACHE INTERNAL "" FORCE)
	set(OCTKWrapWebRTC_INSTALL_DIR "${OCTKWrapWebRTC_ROOT_DIR}/source" CACHE INTERNAL "" FORCE)
	octk_stamp_file_info(OCTKWrapWebRTC OUTPUT_DIR "${OCTKWrapWebRTC_ROOT_DIR}")
	octk_fetch_3rdparty(OCTKWrapWebRTC URL "${OCTKWrapWebRTC_URL_PATH}")
	if(NOT EXISTS "${OCTKWrapWebRTC_STAMP_FILE_PATH}")
		if(NOT EXISTS ${OCTKWrapWebRTC_SOURCE_DIR})
			message(FATAL_ERROR "${OCTKWrapWebRTC_DIR_NAME} FetchContent failed.")
		endif()
		octk_make_stamp_file("${OCTKWrapWebRTC_STAMP_FILE_PATH}")
	endif()
	set(OCTKWrapWebRTC_INCLUDE_DIR "${OCTKWrapWebRTC_INSTALL_DIR}/include")
	if(CMAKE_BUILD_TYPE MATCHES "Debug")
		set(OCTKWrapWebRTC_LIBRARY_DIR "${OCTKWrapWebRTC_INSTALL_DIR}/debug")
	else()
		set(OCTKWrapWebRTC_LIBRARY_DIR "${OCTKWrapWebRTC_INSTALL_DIR}/release")
	endif()
	if(WIN32)
		set(OCTKWrapWebRTC_LIBRARY "${OCTKWrapWebRTC_LIBRARY_DIR}/webrtc.lib")
	else()
		set(OCTKWrapWebRTC_LIBRARY "${OCTKWrapWebRTC_LIBRARY_DIR}/libwebrtc.a")
	endif()
else()
	message(FATAL_ERROR "3rdparty webrtc OCTK_3RDPARTY_WEBRTC_PATH or "
		"OCTK_3RDPARTY_WEBRTC_INCLUDE_DIR/OCTK_3RDPARTY_WEBRTC_LIBRARY not set.")
endif()
# add wrap lib
add_library(OCTK3rdparty::WrapWebRTC STATIC IMPORTED)
if(NOT CMAKE_BUILD_TYPE MATCHES "Debug")
	target_compile_definitions(OCTK3rdparty::WrapWebRTC INTERFACE NDEBUG)
endif()
if(WIN32)
	target_link_libraries(OCTK3rdparty::WrapWebRTC INTERFACE
		advapi32.lib
		comdlg32.lib
		dbghelp.lib
		dnsapi.lib
		gdi32.lib
		msimg32.lib
		odbc32.lib
		odbccp32.lib
		oleaut32.lib
		shell32.lib
		shlwapi.lib
		user32.lib
		usp10.lib
		uuid.lib
		version.lib
		wininet.lib
		winmm.lib
		winspool.lib
		delayimp.lib
		kernel32.lib
		ole32.lib
		crypt32.lib
		iphlpapi.lib
		secur32.lib
		dmoguids.lib
		wmcodecdspuuid.lib
		amstrmid.lib
		msdmo.lib
		strmiids.lib
		dwmapi.lib # __imp_DwmGetWindowAttribute
		Shcore.lib # GetDpiForMonitor
		d3d11.lib # D3D11CreateDevice
		dxgi.lib # CreateDXGIFactory1
		ws2_32.lib)
	target_compile_definitions(OCTK3rdparty::WrapWebRTC INTERFACE
		# _CRT_SECURE_NO_WARNINGS
		# _HAS_NODISCARD
		# _CRT_NONSTDC_NO_WARNINGS
		# _WINSOCK_DEPRECATED_NO_WARNINGS
		# __STD_C
		# _CRT_RAND_S
		# _CRT_SECURE_NO_DEPRECATE
		# _SCL_SECURE_NO_DEPRECATE
		# _ATL_NO_OPENGL
		# _WINDOWS
		# USE_AURA=1
		# CR_CLANG_REVISION="llvmorg-18-init-9505-g10664813-1"
		# CERT_CHAIN_PARA_HAS_EXTRA_FIELDS
		# PSAPI_VERSION=2
		# _SECURE_ATL
		# WINAPI_FAMILY=WINAPI_FAMILY_DESKTOP_APP
		# _UNICODE
		# UNICODE
		# _DEBUG
		# DYNAMIC_ANNOTATIONS_ENABLED=1
		# _ENABLE_EXTENDED_ALIGNED_STORAGE
		#			RTC_ENABLE_VP9
		#			RTC_DAV1D_IN_INTERNAL_DECODER_FACTORY
		#			RTC_ENABLE_WIN_WGC
		#			WEBRTC_ENABLE_PROTOBUF=1
		#			WEBRTC_STRICT_FIELD_TRIALS=0
		#			WEBRTC_INCLUDE_INTERNAL_AUDIO_DEVICE
		#			WEBRTC_HAVE_SCTP
		WEBRTC_USE_H264
		#			WEBRTC_ENABLE_AVX2
		#			WEBRTC_NON_STATIC_TRACE_EVENT_HANDLERS=0
		WEBRTC_WIN
		#			ABSL_ALLOCATOR_NOTHROW=1
		#			ABSL_FLAGS_STRIP_NAMES=0
		#			LIBYUV_DISABLE_NEON
		#			HAVE_WEBRTC_VIDEO
		#			WIN32_LEAN_AND_MEAN
		NOMINMAX
		UNICODE
		WIN32
		NOGDI)
else()
	if(APPLE)
		target_link_libraries(OCTK3rdparty::WrapWebRTC INTERFACE
			"-framework AppKit"
			"-framework CoreAudio"
			"-framework CoreVideo"
			"-framework CoreMedia"
			"-framework IOSurface"
			"-framework CoreServices"
			"-framework AVFoundation"
			"-framework AudioToolbox"
			"-framework ScreenCaptureKit"
			"-framework ApplicationServices")
		target_compile_definitions(OCTK3rdparty::WrapWebRTC INTERFACE WEBRTC_MAC)
	elseif(OCTK_SYSTEM_LINUX)
		target_compile_definitions(OCTK3rdparty::WrapWebRTC INTERFACE WEBRTC_ANDROID WEBRTC_LINUX)
	elseif(OCTK_SYSTEM_ANDROID)
		target_compile_definitions(OCTK3rdparty::WrapWebRTC INTERFACE WEBRTC_ANDROID WEBRTC_LINUX)
	elseif(OCTK_SYSTEM_IOS)
		target_compile_definitions(OCTK3rdparty::WrapWebRTC INTERFACE WEBRTC_IOS)
	endif()
	target_compile_definitions(OCTK3rdparty::WrapWebRTC INTERFACE WEBRTC_POSIX)
endif()
target_compile_definitions(OCTK3rdparty::WrapWebRTC INTERFACE WEBRTC_USE_H264)
set_target_properties(OCTK3rdparty::WrapWebRTC PROPERTIES
	INTERFACE_INCLUDE_DIRECTORIES
	"${OCTKWrapWebRTC_INCLUDE_DIR}"
	IMPORTED_LOCATION
	"${OCTKWrapWebRTC_LIBRARY}")
target_include_directories(OCTK3rdparty::WrapWebRTC
	INTERFACE
	"${OCTKWrapWebRTC_INCLUDE_DIR}/third_party"
	"${OCTKWrapWebRTC_INCLUDE_DIR}/third_party/abseil-cpp"
	"${OCTKWrapWebRTC_INCLUDE_DIR}/third_party/libyuv/include"
	"${OCTKWrapWebRTC_INCLUDE_DIR}/third_party/jsoncpp/generated"
	"${OCTKWrapWebRTC_INCLUDE_DIR}/third_party/jsoncpp/source/include")
set(OCTKWrapWebRTC_FOUND ON)
