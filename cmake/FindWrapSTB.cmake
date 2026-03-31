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
if(TARGET OpenCTKWrapSTB::WrapSTB)
	set(OpenCTKWrapSTB_FOUND ON)
	return()
endif()

set(OpenCTKWrapSTB_NAME "stb-master")
set(OpenCTKWrapSTB_DIR_NAME "${OpenCTKWrapSTB_NAME}")
set(OpenCTKWrapSTB_PKG_NAME "${OpenCTKWrapSTB_NAME}.zip")
set(OpenCTKWrapSTB_URL_PATH "${PROJECT_SOURCE_DIR}/3rdparty/${OpenCTKWrapSTB_PKG_NAME}")
set(OpenCTKWrapSTB_ROOT_DIR "${PROJECT_BINARY_DIR}/3rdparty/${OpenCTKWrapSTB_DIR_NAME}")
set(OpenCTKWrapSTB_BUILD_DIR "${OpenCTKWrapSTB_ROOT_DIR}/source" CACHE INTERNAL "" FORCE)
set(OpenCTKWrapSTB_SOURCE_DIR "${OpenCTKWrapSTB_ROOT_DIR}/source" CACHE INTERNAL "" FORCE)
set(OpenCTKWrapSTB_INSTALL_DIR "${OpenCTKWrapSTB_ROOT_DIR}/source" CACHE INTERNAL "" FORCE)
octk_stamp_file_info(OpenCTKWrapSTB OUTPUT_DIR "${OpenCTKWrapSTB_ROOT_DIR}")
octk_fetch_3rdparty(OpenCTKWrapSTB URL "${OpenCTKWrapSTB_URL_PATH}")
if(NOT EXISTS "${OpenCTKWrapSTB_STAMP_FILE_PATH}")
	if(NOT EXISTS ${OpenCTKWrapSTB_SOURCE_DIR})
		message(FATAL_ERROR "${OpenCTKWrapSTB_DIR_NAME} FetchContent failed.")
	endif()
	octk_make_stamp_file("${OpenCTKWrapSTB_STAMP_FILE_PATH}")
endif()
# wrap lib
add_library(OpenCTKWrapSTB::WrapSTB INTERFACE IMPORTED)
target_include_directories(OpenCTKWrapSTB::WrapSTB INTERFACE "${OpenCTKWrapSTB_SOURCE_DIR}")
set(OpenCTKWrapSTB_FOUND ON)