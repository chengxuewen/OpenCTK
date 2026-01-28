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
if(TARGET OCTK3rdparty::WrapOpenh264)
    set(OCTKWrapOpenh264_FOUND ON)
    return()
endif()

octk_find_package(WrapMbedTLS PROVIDED_TARGETS OCTK3rdparty::WrapMbedTLS)
set(OCTKWrapOpenh264_NAME "openh264-2.6.0")
set(OCTKWrapOpenh264_PKG_NAME "${OCTKWrapOpenh264_NAME}.tar.gz")
set(OCTKWrapOpenh264_DIR_NAME "${OCTKWrapOpenh264_NAME}-${OCTK_LOWER_BUILD_TYPE}")
set(OCTKWrapOpenh264_URL_PATH "${PROJECT_SOURCE_DIR}/3rdparty/${OCTKWrapOpenh264_PKG_NAME}")
set(OCTKWrapOpenh264_ROOT_DIR "${PROJECT_BINARY_DIR}/3rdparty/${OCTKWrapOpenh264_DIR_NAME}")
set(OCTKWrapOpenh264_BUILD_DIR "${OCTKWrapOpenh264_ROOT_DIR}/source" CACHE INTERNAL "" FORCE)
set(OCTKWrapOpenh264_SOURCE_DIR "${OCTKWrapOpenh264_ROOT_DIR}/source" CACHE INTERNAL "" FORCE)
set(OCTKWrapOpenh264_INSTALL_DIR "${OCTKWrapOpenh264_ROOT_DIR}/install" CACHE INTERNAL "" FORCE)
octk_stamp_file_info(OCTKWrapOpenh264 OUTPUT_DIR "${OCTKWrapOpenh264_ROOT_DIR}")
octk_fetch_3rdparty(OCTKWrapOpenh264 URL "${OCTKWrapOpenh264_URL_PATH}" OUTPUT_NAME "${OCTKWrapOpenh264_DIR_NAME}")
if(NOT EXISTS "${OCTKWrapOpenh264_STAMP_FILE_PATH}")
    if(NOT EXISTS ${OCTKWrapOpenh264_SOURCE_DIR})
        message(FATAL_ERROR "${OCTKWrapOpenh264_NAME} FetchContent failed.")
    endif()
#    octk_reset_dir(${OCTKWrapOpenh264_BUILD_DIR})

	message(STATUS "Configure ${OCTKWrapOpenh264_NAME} lib...")
	if(WIN32)

	else()
		if(OCTK_PROCESSOR_I386 OR OCTK_PROCESSOR_I686)
			set(OCTKWrapOpenh264_ARCH x86)
		elseif(OCTK_PROCESSOR_X86_64 OR OCTK_PROCESSOR_AMD64)
			set(OCTKWrapOpenh264_ARCH x86_64)
		elseif(OCTK_PROCESSOR_ARM64)
			set(OCTKWrapOpenh264_ARCH arm64)
		elseif(OCTK_PROCESSOR_ARM32)
			set(OCTKWrapOpenh264_ARCH arm32)
		else()
			message(FATAL_ERROR "${OCTKWrapOpenh264_NAME} not support arch ${OCTK_SYSTEM_PROCESSOR}.")
		endif()
		if(CMAKE_BUILD_TYPE MATCHES "Debug")
			set(OCTKWrapOpenh264_DEBUGSYMBOLS True)
		else()
			set(OCTKWrapOpenh264_DEBUGSYMBOLS False)
		endif()
		execute_process(
			COMMAND make ARCH=${OCTKWrapOpenh264_ARCH} DEBUGSYMBOLS=${OCTKWrapOpenh264_DEBUGSYMBOLS}
			-j${OCTK_NUMBER_OF_ASYNC_JOBS}
			WORKING_DIRECTORY "${OCTKWrapOpenh264_BUILD_DIR}"
			RESULT_VARIABLE CONFIGURE_RESULT)
		if(NOT CONFIGURE_RESULT MATCHES 0)
			message(FATAL_ERROR "${OCTKWrapOpenh264_NAME} make failed.")
		endif()
		message(STATUS "${OCTKWrapOpenh264_NAME} make success")

		execute_process(
			COMMAND make install PREFIX=${OCTKWrapOpenh264_INSTALL_DIR}
			WORKING_DIRECTORY "${OCTKWrapOpenh264_BUILD_DIR}"
			RESULT_VARIABLE INSTALL_RESULT)
		if(NOT INSTALL_RESULT MATCHES 0)
			message(FATAL_ERROR "${OCTKWrapOpenh264_NAME} install failed.")
		endif()
		message(STATUS "${OCTKWrapOpenh264_NAME} install success")
	endif()
    octk_make_stamp_file("${OCTKWrapOpenh264_STAMP_FILE_PATH}")
endif()
# wrap lib
execute_process(
	COMMAND ${CMAKE_COMMAND} -E copy_directory "${OCTKWrapOpenh264_INSTALL_DIR}/include/wels"
	"${OCTKWrapOpenh264_INSTALL_DIR}/include/openh264/wels"
	WORKING_DIRECTORY "${OCTKWrapOpenh264_INSTALL_DIR}"
	ERROR_QUIET)
add_library(OCTK3rdparty::WrapOpenh264 INTERFACE IMPORTED)
target_include_directories(OCTK3rdparty::WrapOpenh264 INTERFACE "${OCTKWrapOpenh264_INSTALL_DIR}/include")
if(WIN32)
	target_link_libraries(OCTK3rdparty::WrapOpenh264 INTERFACE ${OCTKWrapOpenh264_INSTALL_DIR}/lib/openh264.lib)
else()
	target_link_libraries(OCTK3rdparty::WrapOpenh264 INTERFACE ${OCTKWrapOpenh264_INSTALL_DIR}/lib/libopenh264.a)
endif()
set(OCTKWrapOpenh264_FOUND ON)