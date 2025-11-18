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

octk_vcpkg_install_package(ffmpeg
	TOOLS NOT_IMPORT
	PACK_NAME
	ffmpeg-tools
	TARGET
	OCTK3rdparty::WrapFFmpegTools
	PREFIX
	OCTKWrapFFmpegTools
	COMPONENTS
	ffmpeg ffplay ffprobe)

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
