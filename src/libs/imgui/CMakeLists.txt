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
octk_find_package(WrapLibyuv PROVIDED_TARGETS OpenCTKWrapLibyuv::WrapLibyuv)
octk_find_package(WrapImGui PROVIDED_TARGETS OpenCTKWrapImGui::WrapImGui)
octk_find_package(WrapSDL3 PROVIDED_TARGETS OpenCTKWrapSDL3::WrapSDL3)
octk_find_package(WrapSTB PROVIDED_TARGETS OpenCTKWrapSTB::WrapSTB)


#-----------------------------------------------------------------------------------------------------------------------
# Add lib target
#-----------------------------------------------------------------------------------------------------------------------
octk_add_library(Imgui
	LIBRARY_INCLUDE_NAME "openctk/imgui"
	EXCEPTIONS
	SOURCES
	${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
	${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdl3.h
	${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdlgpu3.cpp
	${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdlgpu3.h
	${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdlgpu3_shaders.h
	${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp
	${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.h
	source/imgui.cpp
	source/imgui.hpp
	source/imgui_application.cpp
	source/imgui_application.hpp
	source/imgui_application_p.hpp
	source/imgui_application_sdlgpu3.cpp
	source/imgui_application_sdlgpu3.hpp
	source/imgui_application_sdlrenderer3.cpp
	source/imgui_application_sdlrenderer3.hpp
	source/imgui_constants.hpp
	source/imgui_global.hpp
	source/imgui_theme.cpp
	source/imgui_theme.hpp
	source/sdl_p.hpp
	PRECOMPILED_HEADER
	source/imgui_pch.hpp
	EXTERNAL_HEADERS_DIRS
	${OpenCTKWrapImGui_INSTALL_DIR}/include
	INCLUDE_DIRECTORIES
	${OpenCTKWrapImGui_SOURCE_DIR}
	${OpenCTKWrapImGui_SOURCE_DIR}/backends
	LIBRARIES
	OpenCTKWrapLibyuv::WrapLibyuv
	OpenCTKWrapSDL3::WrapSDL3
	OpenCTKWrapSTB::WrapSTB
	PUBLIC_LIBRARIES
	OpenCTKWrapImGui::WrapImGui
	OpenCTK::Core)
octk_internal_extend_target(Imgui
	SOURCES
	${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_metal.mm
	${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_metal.h
	LIBRARIES
	"-framework Cocoa"
	"-framework IOKit"
	"-framework Metal"
	"-framework MetalKit"
	"-framework CoreVideo"
	"-framework QuartzCore"
	CONDITION OCTK_SYSTEM_MAC)
if(OCTK_FEATURE_IMGUI_USE_GLFW)
	octk_find_package(WrapGLFW PROVIDED_TARGETS OpenCTKWrapGLFW::WrapGLFW)
	octk_internal_extend_target(Imgui
		SOURCES
		${OpenCTKWrapGLFW_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
		${OpenCTKWrapGLFW_SOURCE_DIR}/backends/imgui_impl_glfw.h
		LIBRARIES
		OpenCTKWrapGLFW::WrapGLFW)
endif()
if(OCTK_FEATURE_IMGUI_USE_SDL_OPENGL3)
	if(TARGET OpenGL::GL)
		set(OpenGL_TARGET OpenGL::GL)
	elseif(TARGET OpenGL::OpenGL)
		set(OpenGL_TARGET OpenGL::OpenGL)
	endif()
	octk_internal_extend_target(Imgui
		SOURCES
		${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
		${OpenCTKWrapImGui_SOURCE_DIR}/backends/imgui_impl_opengl3.h
		source/imgui_application_sdlopengl3.cpp
		source/imgui_application_sdlopengl3.hpp
		LIBRARIES
		${OpenGL_TARGET}
		${OPENGL_LIBRARIES})
endif()



octk_install_public_wrap_headers(Imgui
    WRAPS
    OpenCTKWrapImGui::WrapImGui|imgui
)
# Add lib examples and tests
#-----------------------------------------------------------------------------------------------------------------------
octk_add_subdirectory(examples OCTK_BUILD_EXAMPLES)
octk_add_subdirectory(tests OCTK_BUILD_TESTS)
