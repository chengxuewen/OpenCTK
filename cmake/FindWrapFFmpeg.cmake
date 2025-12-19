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
	list(APPEND OCTKWrapFFmpeg_COMPONENTS nvcodec amf)
endif()
if(NOT OCTK_VCPKG_TRIPLET_ARCH_ARM)
	list(APPEND OCTKWrapFFmpeg_COMPONENTS qsv) # （mfx）only in intel cpu
endif()
octk_vcpkg_install_package(ffmpeg
	NOT_IMPORT
	TARGET
	OCTK3rdparty::WrapFFmpeg
	PREFIX
	OCTKWrapFFmpeg
	COMPONENTS
	${OCTKWrapFFmpeg_COMPONENTS})


if(EXISTS "${OCTKWrapFFmpeg_INSTALL_DIR}/share/ffmpeg/FindFFMPEG.cmake" AND UNIX)
	set(CMAKE_MODULE_PATH_CACHE ${CMAKE_MODULE_PATH})
	set(CMAKE_MODULE_PATH "${OCTKWrapFFmpeg_INSTALL_DIR}/share/ffmpeg")
	set(FFMPEG_DIR "${OCTKWrapFFmpeg_INSTALL_DIR}")
	find_package(FFMPEG REQUIRED)
	foreach(library IN LISTS FFMPEG_LIBRARIES)
		if(EXISTS "${library}")
			target_link_libraries(OCTK3rdparty::WrapFFmpeg INTERFACE ${library})
		endif()
	endforeach()
	target_include_directories(OCTK3rdparty::WrapFFmpeg INTERFACE ${FFMPEG_INCLUDE_DIRS})
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH_CACHE})
else()
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(OCTKWrapFFmpeg_PKGCONFIG_DIR "${OCTKWrapFFmpeg_INSTALL_DIR}/debug/lib")
	else()
		set(OCTKWrapFFmpeg_PKGCONFIG_DIR "${OCTKWrapFFmpeg_INSTALL_DIR}/lib")
	endif()
	octk_pkgconf_check_modules(FFmpeg REQUIRED
		PATH "${OCTKWrapFFmpeg_PKGCONFIG_DIR}/pkgconfig"
		IMPORTED_TARGET
		libswresample
		libavdevice
		libavformat
		libavfilter
		libavcodec
		libavutil
		libswscale)
	target_link_libraries(OCTK3rdparty::WrapFFmpeg INTERFACE PkgConfig::FFmpeg)
endif()
if(WIN32)
	target_link_libraries(OCTK3rdparty::WrapFFmpeg INTERFACE bcrypt.lib)
endif()
find_package(Threads REQUIRED)
target_link_libraries(OCTK3rdparty::WrapFFmpeg INTERFACE Threads::Threads)
set(OCTKWrapFFmpeg_FOUND ON)

