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
if(TARGET OCTK3rdparty::WrapImGui)
	set(OCTKWrapImGui_FOUND ON)
	return()
endif()

set(OCTKWrapImGui_NAME "imgui-1.92.2b-docking")
set(OCTKWrapImGui_DIR_NAME "${OCTKWrapImGui_NAME}")
set(OCTKWrapImGui_PKG_NAME "${OCTKWrapImGui_NAME}.7z")
set(OCTKWrapImGui_URL_PATH "${PROJECT_SOURCE_DIR}/3rdparty/${OCTKWrapImGui_PKG_NAME}")
set(OCTKWrapImGui_ROOT_DIR "${PROJECT_BINARY_DIR}/3rdparty/${OCTKWrapImGui_DIR_NAME}")
set(OCTKWrapImGui_BUILD_DIR "${OCTKWrapImGui_ROOT_DIR}/build" CACHE INTERNAL "" FORCE)
set(OCTKWrapImGui_SOURCE_DIR "${OCTKWrapImGui_ROOT_DIR}/source" CACHE INTERNAL "" FORCE)
set(OCTKWrapImGui_INSTALL_DIR "${OCTKWrapImGui_ROOT_DIR}/install" CACHE INTERNAL "" FORCE)
octk_stamp_file_info(OCTKWrapImGui OUTPUT_DIR "${OCTKWrapImGui_ROOT_DIR}")
octk_fetch_3rdparty(OCTKWrapImGui URL "${OCTKWrapImGui_URL_PATH}")
if(NOT EXISTS "${OCTKWrapImGui_STAMP_FILE_PATH}")
	if(NOT EXISTS ${OCTKWrapImGui_SOURCE_DIR})
		message(FATAL_ERROR "${OCTKWrapImGui_DIR_NAME} FetchContent failed.")
	endif()
	octk_make_stamp_file("${OCTKWrapImGui_STAMP_FILE_PATH}")
endif()
# wrap lib
add_library(OCTK3rdparty::WrapImGui INTERFACE IMPORTED)
if(NOT TARGET ImGui)
	add_library(ImGui STATIC
		${OCTKWrapImGui_SOURCE_DIR}/imgui.cpp
		${OCTKWrapImGui_SOURCE_DIR}/imgui.h
		${OCTKWrapImGui_SOURCE_DIR}/imgui_demo.cpp
		${OCTKWrapImGui_SOURCE_DIR}/imgui_draw.cpp
		${OCTKWrapImGui_SOURCE_DIR}/imgui_internal.h
		${OCTKWrapImGui_SOURCE_DIR}/imgui_tables.cpp
		${OCTKWrapImGui_SOURCE_DIR}/imgui_widgets.cpp
		${OCTKWrapImGui_SOURCE_DIR}/imstb_rectpack.h
		${OCTKWrapImGui_SOURCE_DIR}/imstb_textedit.h
		${OCTKWrapImGui_SOURCE_DIR}/imstb_truetype.h)
endif()
target_link_libraries(OCTK3rdparty::WrapImGui INTERFACE ImGui)
execute_process(
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${OCTKWrapImGui_SOURCE_DIR}/imconfig.h"
	"${OCTKWrapImGui_INSTALL_DIR}/include/imgui/imconfig.h"
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${OCTKWrapImGui_SOURCE_DIR}/imgui.h"
	"${OCTKWrapImGui_INSTALL_DIR}/include/imgui/imgui.h"
	WORKING_DIRECTORY "${OCTKWrapImGui_ROOT_DIR}"
	ERROR_QUIET)
target_include_directories(OCTK3rdparty::WrapImGui INTERFACE "${OCTKWrapImGui_INSTALL_DIR}/include")
set(OCTKWrapImGui_FOUND ON)