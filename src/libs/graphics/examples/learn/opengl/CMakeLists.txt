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
list(APPEND OCTK_EXAMPLE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR})


#-----------------------------------------------------------------------------------------------------------------------
# Set example link libraries
#-----------------------------------------------------------------------------------------------------------------------
octk_find_package(WrapFreeType PROVIDED_TARGETS OCTK3rdparty::WrapFreeType)
octk_find_package(WrapAssimp PROVIDED_TARGETS OCTK3rdparty::WrapAssimp)
octk_find_package(WrapSOIL2 PROVIDED_TARGETS OCTK3rdparty::WrapSOIL2)
octk_find_package(WrapGLAD PROVIDED_TARGETS OCTK3rdparty::WrapGLAD)
octk_find_package(WrapGLFW PROVIDED_TARGETS OCTK3rdparty::WrapGLFW)
octk_find_package(WrapGLM PROVIDED_TARGETS OCTK3rdparty::WrapGLM)
octk_find_package(WrapSTB PROVIDED_TARGETS OCTK3rdparty::WrapSTB)
list(APPEND OCTK_EXAMPLE_LINK_LIBRARIES
	OCTK3rdparty::WrapFreeType
	OCTK3rdparty::WrapAssimp
	OCTK3rdparty::WrapSOIL2
	OCTK3rdparty::WrapGLAD
	OCTK3rdparty::WrapGLFW
	OCTK3rdparty::WrapGLM
	OCTK3rdparty::WrapSTB
	octk::graphics)


#-----------------------------------------------------------------------------------------------------------------------
# Add examples
#-----------------------------------------------------------------------------------------------------------------------
configure_file(root_directory.h.in ${CMAKE_CURRENT_BINARY_DIR}/root_directory.h)
list(APPEND OCTK_EXAMPLE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_BINARY_DIR})
file(GLOB_RECURSE INCLUDE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/learnopengl/*.h")
function(add_learn_opengl_executable TARGET)
	set(oneValueArgs DIRECTORY)
	set(multiValueArgs SOURCES INCLUDE_DIRS LINK_LIBS)
	cmake_parse_arguments(arg "" "${oneValueArgs}" "${multiValueArgs}" "${ARGN}")
	file(GLOB_RECURSE HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${arg_DIRECTORY}/*.h*")
	file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${arg_DIRECTORY}/*.c*")
	octk_add_executable(learn_opengl_${TARGET}
		EXCEPTIONS
		SOURCES
		${arg_SOURCES}
		${HEADER_FILES}
		${SOURCE_FILES}
		${INCLUDE_FILES}
		INCLUDE_DIRECTORIES
		${arg_INCLUDE_DIRS}
		${CMAKE_CURRENT_SOURCE_DIR}/${arg_DIRECTORY}
		${OCTK_EXAMPLE_INCLUDE_DIRECTORIES}
		LIBRARIES
		${arg_LINK_LIBS}
		${OCTK_EXAMPLE_LINK_LIBRARIES}
		OUTPUT_DIRECTORY
		${OCTK_EXAMPLE_OUTPUT_DIR}
		DEFINES
		SOURCE_PATH="${CMAKE_CURRENT_SOURCE_DIR}/${arg_DIRECTORY}")
endfunction()

add_subdirectory(1.getting_started)
add_subdirectory(2.lighting)
add_subdirectory(3.model_loading)
add_subdirectory(4.advanced_opengl)
add_subdirectory(5.advanced_lighting)
add_subdirectory(6.pbr)
add_subdirectory(7.in_practice)
add_subdirectory(8.guest)
