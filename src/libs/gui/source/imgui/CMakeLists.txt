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

set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
octk_find_package(WrapSDL PROVIDED_TARGETS OCTK3rdparty::WrapSDL)
octk_find_package(WrapImGui PROVIDED_TARGETS OCTK3rdparty::WrapImGui)
#-----------------------------------------------------------------------------------------------------------------------
# extend lib target
#-----------------------------------------------------------------------------------------------------------------------
octk_internal_extend_target(octk_gui
	SOURCES
	${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
	${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdl3.h
	${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdlgpu3.cpp
	${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdlgpu3.h
	${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdlgpu3_shaders.h
	${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp
	${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.h
	backends/octk_imgui_window_sdlgpu3.cpp
	backends/octk_imgui_window_sdlgpu3.hpp
	octk_imgui.cpp
	octk_imgui.hpp
	octk_imgui_constants.hpp
	octk_imgui_window.cpp
	octk_imgui_window.hpp
	octk_imgui_window_p.hpp
	octk_imgui_window_factory.cpp
	octk_imgui_window_factory.hpp
	INCLUDE_DIRECTORIES
	${OCTKWrapImGui_SOURCE_DIR}
	${OCTKWrapImGui_SOURCE_DIR}/backends
	LIBRARIES
	OCTK3rdparty::WrapSDL
	OCTK3rdparty::WrapImGui)
octk_internal_extend_target(octk_gui
	SOURCES
	${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_metal.mm
	${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_metal.h
	LIBRARIES
	"-framework Cocoa"
	"-framework IOKit"
	"-framework Metal"
	"-framework MetalKit"
	"-framework CoreVideo"
	"-framework QuartzCore"
	CONDITION OCTK_SYSTEM_MAC)
set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL 3 QUIET)
octk_internal_extend_target(octk_gui
	SOURCES
	${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
	${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_opengl3.h
	LIBRARIES
	OpenGL::GL
	CONDITION OpenGL_FOUND)