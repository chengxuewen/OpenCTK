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
if(TARGET OCTK3rdparty::FFmpeg)
	set(OCTKFFmpegTools_FOUND ON)
	return()
endif()

include(InstallVcpkg)
if(ON)
	octk_vcpkg_install_package(ffmpeg
		NOT_IMPORT
		PACK_NAME
		ffmpeg-tools
		TARGET
		OCTK3rdparty::WrapFFmpegTools
		PREFIX
		OCTKWrapFFmpegTools
		COMPONENTS
		ffmpeg ffplay ffprobe)
else()
	set(OCTKFFmpegTools_NAME "ffmpeg-tools")
	set(OCTKFFmpegTools_ROOT_DIR "${PROJECT_BINARY_DIR}/3rdparty/${OCTKFFmpegTools_NAME}")
	if(WIN32)
		set(OCTKFFmpegTools_VCPKG_TRIPLET ${OCTK_VCPKG_TRIPLET}-static-md)
	else()
		set(OCTKFFmpegTools_VCPKG_TRIPLET ${OCTK_VCPKG_TRIPLET})
	endif()
	set(OCTKFFmpegTools_INSTALL_DIR "${OCTKFFmpegTools_ROOT_DIR}/installed/${OCTKFFmpegTools_VCPKG_TRIPLET}" CACHE INTERNAL "" FORCE)
	if(NOT EXISTS "${OCTKFFmpegTools_INSTALL_DIR}")
		list(APPEND OCTKFFmpegTools_COMPONENTS ffmpeg ffplay ffprobe)
		unset(OCTKFFmpegTools_COMPONENTS_CONFIGS)
		foreach(component IN LISTS OCTKFFmpegTools_COMPONENTS)
			if(NOT "${OCTKFFmpegTools_COMPONENTS_CONFIGS}" STREQUAL "")
				set(OCTKFFmpegTools_COMPONENTS_CONFIGS "${OCTKFFmpegTools_COMPONENTS_CONFIGS},")
			endif()
			set(OCTKFFmpegTools_COMPONENTS_CONFIGS "${OCTKFFmpegTools_COMPONENTS_CONFIGS}${component}")
		endforeach()
		execute_process(
			COMMAND ${OCTKVcpkgTools_EXECUTABLE} list ffmpeg[${OCTKFFmpegTools_COMPONENTS_CONFIGS}]:${OCTKFFmpegTools_VCPKG_TRIPLET}
			WORKING_DIRECTORY "${OCTKVcpkgTools_ROOT_DIR}"
			OUTPUT_VARIABLE FIND_OUTPUT
			RESULT_VARIABLE FIND_RESULT)
		if("X${FIND_OUTPUT}" STREQUAL "X")
			message(STATUS "${OCTKFFmpegTools_NAME} not installed, start install...")
			set(OCTKFFmpegTools_VCPKG_CONFIGS ffmpeg[${OCTKFFmpegTools_COMPONENTS_CONFIGS}]:${OCTKFFmpegTools_VCPKG_TRIPLET})
			message(STATUS "${OCTKFFmpegTools_NAME} vcpkg install configs: ${OCTKFFmpegTools_VCPKG_CONFIGS}")
			execute_process(
				COMMAND "${OCTKVcpkgTools_EXECUTABLE}" install ${OCTKFFmpegTools_VCPKG_CONFIGS} --recurse
				WORKING_DIRECTORY "${OCTKVcpkgTools_ROOT_DIR}"
				RESULT_VARIABLE INSTALL_RESULT
				COMMAND_ECHO STDOUT)
			if(NOT (INSTALL_RESULT MATCHES 0))
				message(FATAL_ERROR "${OCTKFFmpegTools_NAME} install failed.")
			endif()
		endif()

		execute_process(
			COMMAND "${OCTKVcpkgTools_EXECUTABLE}" export ffmpeg:${OCTKFFmpegTools_VCPKG_TRIPLET}
			--raw --output=${OCTKFFmpegTools_NAME} --output-dir=${PROJECT_BINARY_DIR}/3rdparty
			WORKING_DIRECTORY "${OCTKVcpkgTools_ROOT_DIR}"
			RESULT_VARIABLE EXPORT_RESULT
			COMMAND_ECHO STDOUT)
		if(NOT (EXPORT_RESULT MATCHES 0))
			message(FATAL_ERROR "${OCTKFFmpegTools_NAME} export failed.")
		endif()
	endif()
endif()

if(NOT EXISTS "${OCTK_BUILD_DIR}/${OCTK_DEFAULT_LIBEXEC}/ffmpeg")
	execute_process(
		COMMAND ${CMAKE_COMMAND} -E copy_directory "${OCTKFFmpegTools_INSTALL_DIR}/tools/ffmpeg"
		"${OCTK_BUILD_DIR}/${OCTK_DEFAULT_LIBEXEC}/ffmpeg"
		WORKING_DIRECTORY "${OCTKFFmpegTools_ROOT_DIR}"
		ERROR_QUIET)
endif()
set(OCTKFFprobe_EXECUTABLE "${OCTKFFmpegTools_INSTALL_DIR}/tools/ffmpeg/ffprobe${OCTK_EXECUTABLE_SUFFIX}" CACHE INTERNAL "" FORCE)
set(OCTKFFmpeg_EXECUTABLE "${OCTKFFmpegTools_INSTALL_DIR}/tools/ffmpeg/ffmpeg${OCTK_EXECUTABLE_SUFFIX}" CACHE INTERNAL "" FORCE)
set(OCTKFFplay_EXECUTABLE "${OCTKFFmpegTools_INSTALL_DIR}/tools/ffmpeg/ffplay${OCTK_EXECUTABLE_SUFFIX}" CACHE INTERNAL "" FORCE)
octk_install(
	DIRECTORY "${OCTKFFmpegTools_INSTALL_DIR}/tools/ffmpeg/"
	DESTINATION "${OCTK_DEFAULT_LIBEXEC}/ffmpeg"
	PATTERN "ff*${MSRTC_EXECUTABLE_SUFFIX}"
	PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
set(OCTKFFmpegTools_FOUND ON)
