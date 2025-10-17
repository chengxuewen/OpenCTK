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
if(TARGET OCTK3rdparty::WrapGLAD)
	set(OCTKWrapGLAD_FOUND ON)
	return()
endif()

set(OCTKWrapGLAD_NAME "glad-0.1.36")
set(OCTKWrapGLAD_DIR_NAME "${OCTKWrapGLAD_NAME}")
set(OCTKWrapGLAD_PKG_NAME "${OCTKWrapGLAD_NAME}.7z")
set(OCTKWrapGLAD_URL_PATH "${PROJECT_SOURCE_DIR}/3rdparty/${OCTKWrapGLAD_PKG_NAME}")
set(OCTKWrapGLAD_ROOT_DIR "${PROJECT_BINARY_DIR}/3rdparty/${OCTKWrapGLAD_DIR_NAME}")
set(OCTKWrapGLAD_BUILD_DIR "${OCTKWrapGLAD_ROOT_DIR}/source" CACHE INTERNAL "" FORCE)
set(OCTKWrapGLAD_SOURCE_DIR "${OCTKWrapGLAD_ROOT_DIR}/source" CACHE INTERNAL "" FORCE)
set(OCTKWrapGLAD_INSTALL_DIR "${OCTKWrapGLAD_ROOT_DIR}/source" CACHE INTERNAL "" FORCE)
octk_stamp_file_info(OCTKWrapGLAD OUTPUT_DIR "${OCTKWrapGLAD_ROOT_DIR}")
octk_fetch_3rdparty(OCTKWrapGLAD URL "${OCTKWrapGLAD_URL_PATH}")
if(NOT EXISTS "${OCTKWrapGLAD_STAMP_FILE_PATH}")
	if(NOT EXISTS ${OCTKWrapGLAD_SOURCE_DIR})
		message(FATAL_ERROR "${OCTKWrapGLAD_DIR_NAME} FetchContent failed.")
	endif()
	octk_make_stamp_file("${OCTKWrapGLAD_STAMP_FILE_PATH}")
endif()
# wrap lib
add_library(glad STATIC
	${OCTKWrapGLAD_SOURCE_DIR}/src/glad.c
	${OCTKWrapGLAD_SOURCE_DIR}/include/glad/glad.h
	${OCTKWrapGLAD_SOURCE_DIR}/include/KHR/khrplatform.h)
target_include_directories(glad PUBLIC ${OCTKWrapGLAD_SOURCE_DIR}/include)
add_library(OCTK3rdparty::WrapGLAD INTERFACE IMPORTED)
target_link_libraries(OCTK3rdparty::WrapGLAD INTERFACE glad)
set(OCTKWrapGLAD_FOUND ON)