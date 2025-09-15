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

#-----------------------------------------------------------------------------------------------------------------------
# Add lib link libraries
#-----------------------------------------------------------------------------------------------------------------------
octk_find_package(WrapImGui PROVIDED_TARGETS OCTK3rdparty::WrapImGui)
octk_find_package(WrapGLFW PROVIDED_TARGETS OCTK3rdparty::WrapGLFW)
octk_find_package(WrapSDL PROVIDED_TARGETS OCTK3rdparty::WrapSDL)
octk_find_package(WrapSTB PROVIDED_TARGETS OCTK3rdparty::WrapSTB)


#-----------------------------------------------------------------------------------------------------------------------
# Add lib target
#-----------------------------------------------------------------------------------------------------------------------
octk_add_library(imgui
	EXCEPTIONS
	SOURCES
	${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
	${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdl3.h
	${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdlgpu3.cpp
	${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdlgpu3.h
	${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdlgpu3_shaders.h
	${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp
	${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.h
	source/octk_imgui.cpp
	source/octk_imgui.hpp
	source/octk_imgui_application.cpp
	source/octk_imgui_application.hpp
	source/octk_imgui_application_p.hpp
#	source/octk_imgui_application_sdlgpu3.cpp
#	source/octk_imgui_application_sdlgpu3.hpp
	source/octk_imgui_application_sdlrenderer3.cpp
	source/octk_imgui_application_sdlrenderer3.hpp
	source/octk_imgui_constants.hpp
	source/octk_imgui_global.hpp
	source/octk_imgui_theme.cpp
	source/octk_imgui_theme.hpp
	source/octk_sdl_p.hpp
	PRECOMPILED_HEADER
	source/octk_imgui_pch.hpp
	EXTERNAL_HEADERS_DIRS
	${OCTKWrapImGui_INSTALL_DIR}/include
	INCLUDE_DIRECTORIES
	${OCTKWrapImGui_SOURCE_DIR}
	${OCTKWrapImGui_SOURCE_DIR}/backends
	LIBRARIES
	OCTK3rdparty::WrapGLFW
	OCTK3rdparty::WrapSDL
	OCTK3rdparty::WrapSTB
	octk::core_private
	PUBLIC_LIBRARIES
	OCTK3rdparty::WrapImGui
	octk::core)
octk_internal_extend_target(octk_imgui
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
octk_internal_extend_target(octk_imgui
	SOURCES
	${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
	${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_opengl3.h
	source/octk_imgui_application_sdlopengl3.cpp
	source/octk_imgui_application_sdlopengl3.hpp
	LIBRARIES
	OpenGL::GL
	CONDITION OpenGL_FOUND)


#-----------------------------------------------------------------------------------------------------------------------
# Add lib examples and tests
#-----------------------------------------------------------------------------------------------------------------------
octk_add_subdirectory(examples OCTK_BUILD_EXAMPLES)
octk_add_subdirectory(tests OCTK_BUILD_TESTS)
