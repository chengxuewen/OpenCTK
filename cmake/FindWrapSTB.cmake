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
if(TARGET OCTK3rdparty::WrapSTB)
	set(OCTKWrapSTB_FOUND ON)
	return()
endif()

set(OCTKWrapSTB_NAME "stb-master")
set(OCTKWrapSTB_DIR_NAME "${OCTKWrapSTB_NAME}")
set(OCTKWrapSTB_PKG_NAME "${OCTKWrapSTB_NAME}.zip")
set(OCTKWrapSTB_URL_PATH "${PROJECT_SOURCE_DIR}/3rdparty/${OCTKWrapSTB_PKG_NAME}")
set(OCTKWrapSTB_ROOT_DIR "${PROJECT_BINARY_DIR}/3rdparty/${OCTKWrapSTB_DIR_NAME}")
set(OCTKWrapSTB_BUILD_DIR "${OCTKWrapSTB_ROOT_DIR}/source" CACHE INTERNAL "" FORCE)
set(OCTKWrapSTB_SOURCE_DIR "${OCTKWrapSTB_ROOT_DIR}/source" CACHE INTERNAL "" FORCE)
set(OCTKWrapSTB_INSTALL_DIR "${OCTKWrapSTB_ROOT_DIR}/source" CACHE INTERNAL "" FORCE)
octk_stamp_file_info(OCTKWrapSTB OUTPUT_DIR "${OCTKWrapSTB_ROOT_DIR}")
octk_fetch_3rdparty(OCTKWrapSTB URL "${OCTKWrapSTB_URL_PATH}")
if(NOT EXISTS "${OCTKWrapSTB_STAMP_FILE_PATH}")
	if(NOT EXISTS ${OCTKWrapSTB_SOURCE_DIR})
		message(FATAL_ERROR "${OCTKWrapSTB_DIR_NAME} FetchContent failed.")
	endif()
	octk_make_stamp_file("${OCTKWrapSTB_STAMP_FILE_PATH}")
endif()
# wrap lib
add_library(OCTK3rdparty::WrapSTB INTERFACE IMPORTED)
target_include_directories(OCTK3rdparty::WrapSTB INTERFACE "${OCTKWrapSTB_SOURCE_DIR}")
set(OCTKWrapSTB_FOUND ON)