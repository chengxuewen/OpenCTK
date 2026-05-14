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
if(TARGET OpenCTKWrapImGui::WrapImGui)
	set(OpenCTKWrapImGui_FOUND ON)
	return()
endif()

set(OpenCTKWrapImGui_NAME "imgui-1.92.2b-docking")
set(OpenCTKWrapImGui_DIR_NAME "${OpenCTKWrapImGui_NAME}")
set(OpenCTKWrapImGui_PKG_NAME "${OpenCTKWrapImGui_NAME}.7z")
set(OpenCTKWrapImGui_URL_PATH "${PROJECT_SOURCE_DIR}/3rdparty/${OpenCTKWrapImGui_PKG_NAME}")
set(OpenCTKWrapImGui_ROOT_DIR "${PROJECT_BINARY_DIR}/3rdparty/${OpenCTKWrapImGui_DIR_NAME}")
set(OpenCTKWrapImGui_BUILD_DIR "${OpenCTKWrapImGui_ROOT_DIR}/build" CACHE INTERNAL "" FORCE)
set(OpenCTKWrapImGui_SOURCE_DIR "${OpenCTKWrapImGui_ROOT_DIR}/source" CACHE INTERNAL "" FORCE)
set(OpenCTKWrapImGui_INSTALL_DIR "${OpenCTKWrapImGui_ROOT_DIR}/install" CACHE INTERNAL "" FORCE)
octk_stamp_file_info(OpenCTKWrapImGui OUTPUT_DIR "${OpenCTKWrapImGui_ROOT_DIR}")
octk_fetch_3rdparty(OpenCTKWrapImGui URL "${OpenCTKWrapImGui_URL_PATH}")
if(NOT EXISTS "${OpenCTKWrapImGui_STAMP_FILE_PATH}")
	if(NOT EXISTS ${OpenCTKWrapImGui_SOURCE_DIR})
		message(FATAL_ERROR "${OpenCTKWrapImGui_DIR_NAME} FetchContent failed.")
	endif()
	octk_make_stamp_file("${OpenCTKWrapImGui_STAMP_FILE_PATH}")
endif()
# wrap lib
add_library(OpenCTKWrapImGui::WrapImGui INTERFACE IMPORTED)
set_target_properties(OpenCTKWrapImGui::WrapImGui PROPERTIES FOLDER "OpenCTK/3rdparty")
if(NOT TARGET ImGui)
	add_library(ImGui STATIC
		${OpenCTKWrapImGui_SOURCE_DIR}/imgui.cpp
		${OpenCTKWrapImGui_SOURCE_DIR}/imgui.h
		${OpenCTKWrapImGui_SOURCE_DIR}/imgui_demo.cpp
		${OpenCTKWrapImGui_SOURCE_DIR}/imgui_draw.cpp
		${OpenCTKWrapImGui_SOURCE_DIR}/imgui_internal.h
		${OpenCTKWrapImGui_SOURCE_DIR}/imgui_tables.cpp
		${OpenCTKWrapImGui_SOURCE_DIR}/imgui_widgets.cpp
		${OpenCTKWrapImGui_SOURCE_DIR}/imstb_rectpack.h
		${OpenCTKWrapImGui_SOURCE_DIR}/imstb_textedit.h
		${OpenCTKWrapImGui_SOURCE_DIR}/imstb_truetype.h)
endif()
set_target_properties(ImGui PROPERTIES FOLDER "OpenCTK/3rdparty")
target_link_libraries(OpenCTKWrapImGui::WrapImGui INTERFACE ImGui)
execute_process(
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${OpenCTKWrapImGui_SOURCE_DIR}/imgui_internal.h"
	"${OpenCTKWrapImGui_INSTALL_DIR}/include/imgui/imgui_internal.h"
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${OpenCTKWrapImGui_SOURCE_DIR}/imconfig.h"
	"${OpenCTKWrapImGui_INSTALL_DIR}/include/imgui/imconfig.h"
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${OpenCTKWrapImGui_SOURCE_DIR}/imgui.h"
	"${OpenCTKWrapImGui_INSTALL_DIR}/include/imgui/imgui.h"
	WORKING_DIRECTORY "${OpenCTKWrapImGui_ROOT_DIR}"
	ERROR_QUIET)
target_include_directories(OpenCTKWrapImGui::WrapImGui INTERFACE "${OpenCTKWrapImGui_INSTALL_DIR}/include")
set(OpenCTKWrapImGui_FOUND ON)