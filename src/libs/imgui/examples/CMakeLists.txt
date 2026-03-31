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
octk_find_package(WrapImGui PROVIDED_TARGETS OpenCTKWrapImGui::WrapImGui)
octk_find_package(WrapSDL3 PROVIDED_TARGETS OpenCTKWrapSDL3::WrapSDL3)
if(OCTK_FEATURE_IMGUI_USE_GLFW)
	octk_find_package(WrapGLFW PROVIDED_TARGETS OpenCTKWrapGLFW::WrapGLFW)
endif()

if(APPLE)
	list(APPEND OCTK_EXAMPLE_LINK_LIBRARIES
		"-framework Cocoa"
		"-framework IOKit"
		"-framework CoreVideo")
endif()
list(APPEND OCTK_EXAMPLE_LINK_LIBRARIES OpenCTKImgui)


#-----------------------------------------------------------------------------------------------------------------------
# Add examples
#-----------------------------------------------------------------------------------------------------------------------
execute_process(
	COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/test.bmp" "./"
	WORKING_DIRECTORY "${OCTK_EXAMPLE_OUTPUT_DIR}")

octk_add_executable(OpenCTKImguiExpDockspace
	SOURCES
	exp_imgui_dockspace.cpp
	INCLUDE_DIRECTORIES
	${CMAKE_CURRENT_SOURCE_DIR}
	LIBRARIES
	${OCTK_EXAMPLE_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_EXAMPLE_OUTPUT_DIR})
octk_add_executable(OpenCTKImguiExpImage
	SOURCES
	exp_imgui_image.cpp
	INCLUDE_DIRECTORIES
	${CMAKE_CURRENT_SOURCE_DIR}
	LIBRARIES
	OpenCTKWrapSDL3::WrapSDL3
	${OCTK_EXAMPLE_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_EXAMPLE_OUTPUT_DIR})
octk_add_executable(OpenCTKImguiExpRwbmp
	SOURCES
	exp_imgui_rwbmp.cpp
	INCLUDE_DIRECTORIES
	${CMAKE_CURRENT_SOURCE_DIR}
	LIBRARIES
	OpenCTKWrapSDL3::WrapSDL3
	${OCTK_EXAMPLE_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_EXAMPLE_OUTPUT_DIR})
octk_add_executable(OpenCTKImguiExpTheme
	SOURCES
	exp_imgui_theme.cpp
	INCLUDE_DIRECTORIES
	${CMAKE_CURRENT_SOURCE_DIR}
	LIBRARIES
	${OCTK_EXAMPLE_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_EXAMPLE_OUTPUT_DIR})
octk_add_executable(OpenCTKImguiExpWindow
	SOURCES
	exp_imgui_window.cpp
	INCLUDE_DIRECTORIES
	${CMAKE_CURRENT_SOURCE_DIR}
	LIBRARIES
	${OCTK_EXAMPLE_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_EXAMPLE_OUTPUT_DIR})
octk_add_executable(OpenCTKImguiExpVersion
	SOURCES
	exp_imgui_version.cpp
	INCLUDE_DIRECTORIES
	${CMAKE_CURRENT_SOURCE_DIR}
	LIBRARIES
	${OCTK_EXAMPLE_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_EXAMPLE_OUTPUT_DIR})

if(APPLE)
	octk_add_executable(OpenCTKImguiExpAppleMetal
		SOURCES
		${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_osx.h
		${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_osx.mm
		${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_metal.h
		${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_metal.mm
		${OpenCTKWrapImGui_SOURCE_DIR}/examples/example_apple_metal/main.mm
		INCLUDE_DIRECTORIES
		${OpenCTKWrapImGui_SOURCE_DIR}
		${OpenCTKWrapImGui_SOURCE_DIR}/backends
		LIBRARIES
		"-framework AppKit"
		"-framework Metal"
		"-framework MetalKit"
		"-framework QuartzCore"
		"-framework GameController"
		OpenCTKWrapImGui::WrapImGui
		${OCTK_EXAMPLE_LINK_LIBRARIES}
		OUTPUT_DIRECTORY
		${OCTK_EXAMPLE_OUTPUT_DIR})
	octk_add_executable(OpenCTKImguiExpAppleOpengl2
		SOURCES
		${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_osx.h
		${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_osx.mm
		${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_opengl2.h
		${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_opengl2.cpp
		${OpenCTKWrapImGui_SOURCE_DIR}/examples/example_apple_opengl2/main.mm
		INCLUDE_DIRECTORIES
		${OpenCTKWrapImGui_SOURCE_DIR}
		${OpenCTKWrapImGui_SOURCE_DIR}/backends
		LIBRARIES
		"-framework Cocoa"
		"-framework OpenGL"
		"-framework GameController"
		OpenCTKWrapImGui::WrapImGui
		${OCTK_EXAMPLE_LINK_LIBRARIES}
		OUTPUT_DIRECTORY
		${OCTK_EXAMPLE_OUTPUT_DIR})
	octk_add_executable(OpenCTKImguiExpSdl3Metal
		SOURCES
		${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
		${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdl3.h
		${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_metal.mm
		${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_metal.h
		${OpenCTKWrapImGui_SOURCE_DIR}/examples/example_sdl3_metal/main.mm
		INCLUDE_DIRECTORIES
		${OpenCTKWrapImGui_SOURCE_DIR}
		${OpenCTKWrapImGui_SOURCE_DIR}/backends
		LIBRARIES
		OpenCTKWrapSDL3::WrapSDL3
		OpenCTKWrapImGui::WrapImGui
		"-framework Metal"
		"-framework MetalKit"
		"-framework QuartzCore"
		${OCTK_EXAMPLE_LINK_LIBRARIES}
		OUTPUT_DIRECTORY
		${OCTK_EXAMPLE_OUTPUT_DIR})
	if(OCTK_FEATURE_IMGUI_USE_GLFW)
		octk_add_executable(OpenCTKImguiExpGlfwMetal
			SOURCES
			${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
			${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_glfw.h
			${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_metal.mm
			${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_metal.h
			${OpenCTKWrapImGui_SOURCE_DIR}/examples/example_glfw_metal/main.mm
			INCLUDE_DIRECTORIES
			${OpenCTKWrapImGui_SOURCE_DIR}
			${OpenCTKWrapImGui_SOURCE_DIR}/backends
			LIBRARIES
			OpenCTKWrapGLFW::WrapGLFW
			OpenCTKWrapImGui::WrapImGui
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
endif()
set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL 2 QUIET)
if(OpenGL_FOUND)
	if(OCTK_FEATURE_IMGUI_USE_GLFW)
		octk_add_executable(OpenCTKImguiExpGlfwOpengl2
			SOURCES
			${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
			${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_glfw.h
			${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_opengl2.cpp
			${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_opengl2.h
			${OpenCTKWrapImGui_SOURCE_DIR}/examples/example_glfw_opengl2/main.cpp
			INCLUDE_DIRECTORIES
			${OpenCTKWrapImGui_SOURCE_DIR}
			${OpenCTKWrapImGui_SOURCE_DIR}/backends
			LIBRARIES
			OpenGL::GL
			OpenCTKWrapGLFW::WrapGLFW
			OpenCTKWrapImGui::WrapImGui
			${OCTK_EXAMPLE_LINK_LIBRARIES}
			OUTPUT_DIRECTORY
			${OCTK_EXAMPLE_OUTPUT_DIR})
	endif()
endif()
find_package(OpenGL 3 QUIET)
if(OpenGL_FOUND)
	if(OCTK_FEATURE_IMGUI_USE_GLFW)
		octk_add_executable(OpenCTKImguiExpGlfwOpengl3
			SOURCES
			${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
			${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_glfw.h
			${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
			${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_opengl3.h
			${OpenCTKWrapImGui_SOURCE_DIR}/examples/example_glfw_opengl3/main.cpp
			INCLUDE_DIRECTORIES
			${OpenCTKWrapImGui_SOURCE_DIR}
			${OpenCTKWrapImGui_SOURCE_DIR}/backends
			LIBRARIES
			OpenGL::GL
			OpenCTKWrapGLFW::WrapGLFW
			OpenCTKWrapImGui::WrapImGui
			${OCTK_EXAMPLE_LINK_LIBRARIES}
			OUTPUT_DIRECTORY
			${OCTK_EXAMPLE_OUTPUT_DIR})
	endif()
	octk_add_executable(OpenCTKImguiExpSdl3Opengl3
		SOURCES
		${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
		${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdl3.h
		${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
		${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_opengl3.h
		${OpenCTKWrapImGui_SOURCE_DIR}/examples/example_sdl3_opengl3/main.cpp
		INCLUDE_DIRECTORIES
		${OpenCTKWrapImGui_SOURCE_DIR}
		${OpenCTKWrapImGui_SOURCE_DIR}/backends
		LIBRARIES
		OpenGL::GL
		OpenCTKWrapSDL3::WrapSDL3
		OpenCTKWrapImGui::WrapImGui
		${OCTK_EXAMPLE_LINK_LIBRARIES}
		OUTPUT_DIRECTORY
		${OCTK_EXAMPLE_OUTPUT_DIR})
endif()
octk_add_executable(OpenCTKImguiExpSdl3SdlGPU3
	SOURCES
	${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
	${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdl3.h
	${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdlgpu3.cpp
	${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdlgpu3.h
	${OpenCTKWrapImGui_SOURCE_DIR}/examples/example_sdl3_sdlgpu3/main.cpp
	INCLUDE_DIRECTORIES
	${OpenCTKWrapImGui_SOURCE_DIR}
	${OpenCTKWrapImGui_SOURCE_DIR}/backends
	LIBRARIES
	OpenCTKWrapSDL3::WrapSDL3
	OpenCTKWrapImGui::WrapImGui
	${OCTK_EXAMPLE_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_EXAMPLE_OUTPUT_DIR})
octk_add_executable(OpenCTKImguiExpSdl3SdlRenderer3
	SOURCES
	${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
	${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdl3.h
	${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp
	${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.h
	${OpenCTKWrapImGui_SOURCE_DIR}/examples/example_sdl3_sdlrenderer3/main.cpp
	INCLUDE_DIRECTORIES
	${OpenCTKWrapImGui_SOURCE_DIR}
	${OpenCTKWrapImGui_SOURCE_DIR}/backends
	LIBRARIES
	OpenCTKWrapSDL3::WrapSDL3
	OpenCTKWrapImGui::WrapImGui
	${OCTK_EXAMPLE_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_EXAMPLE_OUTPUT_DIR})
find_package(Vulkan QUIET)
if(Vulkan_FOUND)
	if(OCTK_FEATURE_IMGUI_USE_GLFW)
		octk_add_executable(OpenCTKImguiExpGlfwVulkan
			SOURCES
			${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
			${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_glfw.h
			${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
			${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_vulkan.h
			${OpenCTKWrapImGui_SOURCE_DIR}/examples/example_glfw_vulkan/main.cpp
			INCLUDE_DIRECTORIES
			${OpenCTKWrapImGui_SOURCE_DIR}
			${OpenCTKWrapImGui_SOURCE_DIR}/backends
			LIBRARIES
			Vulkan::Vulkan
			OpenCTKWrapGLFW::WrapGLFW
			OpenCTKWrapImGui::WrapImGui
			${OCTK_EXAMPLE_LINK_LIBRARIES}
			OUTPUT_DIRECTORY
			${OCTK_EXAMPLE_OUTPUT_DIR})
	endif()
	octk_add_executable(OpenCTKImguiExpSdl3Vulkan
		SOURCES
		${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
		${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdl3.h
		${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
		${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_vulkan.h
		${OpenCTKWrapImGui_SOURCE_DIR}/examples/example_sdl3_vulkan/main.cpp
		INCLUDE_DIRECTORIES
		${OpenCTKWrapImGui_SOURCE_DIR}
		${OpenCTKWrapImGui_SOURCE_DIR}/backends
		LIBRARIES
		Vulkan::Vulkan
		OpenCTKWrapSDL3::WrapSDL3
		OpenCTKWrapImGui::WrapImGui
		${OCTK_EXAMPLE_LINK_LIBRARIES}
		OUTPUT_DIRECTORY
		${OCTK_EXAMPLE_OUTPUT_DIR})
endif()

