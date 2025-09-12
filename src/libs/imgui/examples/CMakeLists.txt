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
# Set example output path
#-----------------------------------------------------------------------------------------------------------------------
set(OCTK_EXAMPLE_OUTPUT_DIR ${OCTK_BUILD_DIR}/${OCTK_INSTALL_EXAMPLESDIR})

#-----------------------------------------------------------------------------------------------------------------------
# Set example link libraries
#-----------------------------------------------------------------------------------------------------------------------
octk_find_package(WrapImGui PROVIDED_TARGETS OCTK3rdparty::WrapImGui)
octk_find_package(WrapGLFW PROVIDED_TARGETS OCTK3rdparty::WrapGLFW)
octk_find_package(WrapSDL PROVIDED_TARGETS OCTK3rdparty::WrapSDL)

if(APPLE)
	list(APPEND OCTK_EXAMPLE_LINK_LIBRARIES
		"-framework Cocoa"
		"-framework IOKit"
		"-framework CoreVideo")
endif()
list(APPEND OCTK_EXAMPLE_LINK_LIBRARIES octk::imgui)


#-----------------------------------------------------------------------------------------------------------------------
# Add examples
#-----------------------------------------------------------------------------------------------------------------------
octk_add_executable(octk_imgui_exp_dockspace
	SOURCES
	exp_imgui_dockspace.cpp
	INCLUDE_DIRECTORIES
	${CMAKE_CURRENT_SOURCE_DIR}
	LIBRARIES
	${OCTK_EXAMPLE_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_EXAMPLE_OUTPUT_DIR})
octk_add_executable(octk_imgui_exp_window
	SOURCES
	exp_imgui_window.cpp
	INCLUDE_DIRECTORIES
	${CMAKE_CURRENT_SOURCE_DIR}
	LIBRARIES
	${OCTK_EXAMPLE_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_EXAMPLE_OUTPUT_DIR})
octk_add_executable(octk_imgui_exp_version
	SOURCES
	exp_imgui_version.cpp
	INCLUDE_DIRECTORIES
	${CMAKE_CURRENT_SOURCE_DIR}
	LIBRARIES
	${OCTK_EXAMPLE_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_EXAMPLE_OUTPUT_DIR})

if(APPLE)
	octk_add_executable(octk_imgui_exp_apple_metal
		SOURCES
		${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_osx.h
		${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_osx.mm
		${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_metal.h
		${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_metal.mm
		${OCTKWrapImGui_SOURCE_DIR}/examples/example_apple_metal/main.mm
		INCLUDE_DIRECTORIES
		${OCTKWrapImGui_SOURCE_DIR}
		${OCTKWrapImGui_SOURCE_DIR}/backends
		LIBRARIES
		"-framework AppKit"
		"-framework Metal"
		"-framework MetalKit"
		"-framework QuartzCore"
		"-framework GameController"
		OCTK3rdparty::WrapImGui
		${OCTK_EXAMPLE_LINK_LIBRARIES}
		OUTPUT_DIRECTORY
		${OCTK_EXAMPLE_OUTPUT_DIR})
	octk_add_executable(octk_imgui_exp_apple_opengl2
		SOURCES
		${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_osx.h
		${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_osx.mm
		${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_opengl2.h
		${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_opengl2.cpp
		${OCTKWrapImGui_SOURCE_DIR}/examples/example_apple_opengl2/main.mm
		INCLUDE_DIRECTORIES
		${OCTKWrapImGui_SOURCE_DIR}
		${OCTKWrapImGui_SOURCE_DIR}/backends
		LIBRARIES
		"-framework Cocoa"
		"-framework OpenGL"
		"-framework GameController"
		OCTK3rdparty::WrapImGui
		${OCTK_EXAMPLE_LINK_LIBRARIES}
		OUTPUT_DIRECTORY
		${OCTK_EXAMPLE_OUTPUT_DIR})
	octk_add_executable(octk_imgui_exp_sdl3_metal
		SOURCES
		${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
		${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdl3.h
		${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_metal.mm
		${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_metal.h
		${OCTKWrapImGui_SOURCE_DIR}/examples/example_sdl3_metal/main.mm
		INCLUDE_DIRECTORIES
		${OCTKWrapImGui_SOURCE_DIR}
		${OCTKWrapImGui_SOURCE_DIR}/backends
		LIBRARIES
		OCTK3rdparty::WrapSDL
		OCTK3rdparty::WrapImGui
		"-framework Metal"
		"-framework MetalKit"
		"-framework QuartzCore"
		${OCTK_EXAMPLE_LINK_LIBRARIES}
		OUTPUT_DIRECTORY
		${OCTK_EXAMPLE_OUTPUT_DIR})
	octk_add_executable(octk_imgui_exp_glfw_metal
		SOURCES
		${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
		${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_glfw.h
		${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_metal.mm
		${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_metal.h
		${OCTKWrapImGui_SOURCE_DIR}/examples/example_glfw_metal/main.mm
		INCLUDE_DIRECTORIES
		${OCTKWrapImGui_SOURCE_DIR}
		${OCTKWrapImGui_SOURCE_DIR}/backends
		LIBRARIES
		OCTK3rdparty::WrapGLFW
		OCTK3rdparty::WrapImGui
		"-framework Metal"
		"-framework MetalKit"
		"-framework Cocoa"
		"-framework IOKit"
		"-framework CoreVideo"
		"-framework QuartzCore"
		${OCTK_EXAMPLE_LINK_LIBRARIES}
		OUTPUT_DIRECTORY
		${OCTK_EXAMPLE_OUTPUT_DIR})
endif()
set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL 2 QUIET)
if(OpenGL_FOUND)
	octk_add_executable(octk_imgui_exp_glfw_opengl2
		SOURCES
		${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
		${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_glfw.h
		${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_opengl2.cpp
		${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_opengl2.h
		${OCTKWrapImGui_SOURCE_DIR}/examples/example_glfw_opengl2/main.cpp
		INCLUDE_DIRECTORIES
		${OCTKWrapImGui_SOURCE_DIR}
		${OCTKWrapImGui_SOURCE_DIR}/backends
		LIBRARIES
		OpenGL::GL
		OCTK3rdparty::WrapGLFW
		OCTK3rdparty::WrapImGui
		${OCTK_EXAMPLE_LINK_LIBRARIES}
		OUTPUT_DIRECTORY
		${OCTK_EXAMPLE_OUTPUT_DIR})
endif()
find_package(OpenGL 3 QUIET)
if(OpenGL_FOUND)
	octk_add_executable(octk_imgui_exp_glfw_opengl3
		SOURCES
		${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
		${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_glfw.h
		${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
		${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_opengl3.h
		${OCTKWrapImGui_SOURCE_DIR}/examples/example_glfw_opengl3/main.cpp
		INCLUDE_DIRECTORIES
		${OCTKWrapImGui_SOURCE_DIR}
		${OCTKWrapImGui_SOURCE_DIR}/backends
		LIBRARIES
		OpenGL::GL
		OCTK3rdparty::WrapGLFW
		OCTK3rdparty::WrapImGui
		${OCTK_EXAMPLE_LINK_LIBRARIES}
		OUTPUT_DIRECTORY
		${OCTK_EXAMPLE_OUTPUT_DIR})
	octk_add_executable(octk_imgui_exp_sdl3_opengl3
		SOURCES
		${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
		${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdl3.h
		${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
		${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_opengl3.h
		${OCTKWrapImGui_SOURCE_DIR}/examples/example_sdl3_opengl3/main.cpp
		INCLUDE_DIRECTORIES
		${OCTKWrapImGui_SOURCE_DIR}
		${OCTKWrapImGui_SOURCE_DIR}/backends
		LIBRARIES
		OpenGL::GL
		OCTK3rdparty::WrapSDL
		OCTK3rdparty::WrapImGui
		${OCTK_EXAMPLE_LINK_LIBRARIES}
		OUTPUT_DIRECTORY
		${OCTK_EXAMPLE_OUTPUT_DIR})
endif()
octk_add_executable(octk_imgui_exp_sdl3_sdlgpu3
	SOURCES
	${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
	${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdl3.h
	${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdlgpu3.cpp
	${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdlgpu3.h
	${OCTKWrapImGui_SOURCE_DIR}/examples/example_sdl3_sdlgpu3/main.cpp
	INCLUDE_DIRECTORIES
	${OCTKWrapImGui_SOURCE_DIR}
	${OCTKWrapImGui_SOURCE_DIR}/backends
	LIBRARIES
	OCTK3rdparty::WrapSDL
	OCTK3rdparty::WrapImGui
	${OCTK_EXAMPLE_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_EXAMPLE_OUTPUT_DIR})
octk_add_executable(octk_imgui_exp_sdl3_sdlrenderer3
	SOURCES
	${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
	${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdl3.h
	${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp
	${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.h
	${OCTKWrapImGui_SOURCE_DIR}/examples/example_sdl3_sdlrenderer3/main.cpp
	INCLUDE_DIRECTORIES
	${OCTKWrapImGui_SOURCE_DIR}
	${OCTKWrapImGui_SOURCE_DIR}/backends
	LIBRARIES
	OCTK3rdparty::WrapSDL
	OCTK3rdparty::WrapImGui
	${OCTK_EXAMPLE_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_EXAMPLE_OUTPUT_DIR})
find_package(Vulkan QUIET)
if(Vulkan_FOUND)
	octk_add_executable(octk_imgui_exp_glfw_vulkan
		SOURCES
		${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
		${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_glfw.h
		${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
		${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_vulkan.h
		${OCTKWrapImGui_SOURCE_DIR}/examples/example_glfw_vulkan/main.cpp
		INCLUDE_DIRECTORIES
		${OCTKWrapImGui_SOURCE_DIR}
		${OCTKWrapImGui_SOURCE_DIR}/backends
		LIBRARIES
		Vulkan::Vulkan
		OCTK3rdparty::WrapGLFW
		OCTK3rdparty::WrapImGui
		${OCTK_EXAMPLE_LINK_LIBRARIES}
		OUTPUT_DIRECTORY
		${OCTK_EXAMPLE_OUTPUT_DIR})
	octk_add_executable(octk_imgui_exp_sdl3_vulkan
		SOURCES
		${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
		${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdl3.h
		${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
		${OCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_vulkan.h
		${OCTKWrapImGui_SOURCE_DIR}/examples/example_sdl3_vulkan/main.cpp
		INCLUDE_DIRECTORIES
		${OCTKWrapImGui_SOURCE_DIR}
		${OCTKWrapImGui_SOURCE_DIR}/backends
		LIBRARIES
		Vulkan::Vulkan
		OCTK3rdparty::WrapSDL
		OCTK3rdparty::WrapImGui
		${OCTK_EXAMPLE_LINK_LIBRARIES}
		OUTPUT_DIRECTORY
		${OCTK_EXAMPLE_OUTPUT_DIR})
endif()

