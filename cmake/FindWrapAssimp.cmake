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
if(TARGET OCTK3rdparty::WrapAssimp)
	set(OCTKWrapAssimp_FOUND ON)
	return()
endif()

set(OCTKWrapAssimp_NAME "assimp-6.0.2")
set(OCTKWrapAssimp_DIR_NAME "${OCTKWrapAssimp_NAME}")
set(OCTKWrapAssimp_PKG_NAME "${OCTKWrapAssimp_NAME}.7z")
set(OCTKWrapAssimp_URL_PATH "${PROJECT_SOURCE_DIR}/3rdparty/${OCTKWrapAssimp_PKG_NAME}")
set(OCTKWrapAssimp_ROOT_DIR "${PROJECT_BINARY_DIR}/3rdparty/${OCTKWrapAssimp_DIR_NAME}")
set(OCTKWrapAssimp_BUILD_DIR "${OCTKWrapAssimp_ROOT_DIR}/build" CACHE INTERNAL "" FORCE)
set(OCTKWrapAssimp_SOURCE_DIR "${OCTKWrapAssimp_ROOT_DIR}/source" CACHE INTERNAL "" FORCE)
set(OCTKWrapAssimp_INSTALL_DIR "${OCTKWrapAssimp_ROOT_DIR}/install" CACHE INTERNAL "" FORCE)
octk_stamp_file_info(OCTKWrapAssimp OUTPUT_DIR "${OCTKWrapAssimp_ROOT_DIR}")
octk_fetch_3rdparty(OCTKWrapAssimp URL "${OCTKWrapAssimp_URL_PATH}")
if(NOT EXISTS "${OCTKWrapAssimp_STAMP_FILE_PATH}")
	if(NOT EXISTS ${OCTKWrapAssimp_SOURCE_DIR})
		message(FATAL_ERROR "${OCTKWrapAssimp_DIR_NAME} FetchContent failed.")
	endif()
	octk_reset_dir(PARENT_DIR ${OCTKWrapAssimp_ROOT_DIR} TARGET_NAME build)

	message(STATUS "Configure ${OCTKWrapAssimp_DIR_NAME} lib...")
	execute_process(
		COMMAND ${CMAKE_COMMAND}
		-G ${CMAKE_GENERATOR}
		-DBUILD_SHARED_LIBS=OFF
		-DASSIMP_BUILD_TESTS=OFF
		-DASSIMP_WARNINGS_AS_ERRORS=OFF
		-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
		-DCMAKE_CONFIGURATION_TYPES=${CMAKE_BUILD_TYPE}
		-DCMAKE_INSTALL_PREFIX=${OCTKWrapAssimp_INSTALL_DIR}
		${OCTKWrapAssimp_SOURCE_DIR}
		WORKING_DIRECTORY "${OCTKWrapAssimp_BUILD_DIR}"
		RESULT_VARIABLE CONFIGURE_RESULT)
	if(NOT CONFIGURE_RESULT MATCHES 0)
		message(FATAL_ERROR "${OCTKWrapAssimp_DIR_NAME} configure failed.")
	endif()
	message(STATUS "${OCTKWrapAssimp_DIR_NAME} configure success")

	execute_process(
		COMMAND ${CMAKE_COMMAND} --build ./ --parallel ${OCTK_NUMBER_OF_ASYNC_JOBS} --config
		${CMAKE_BUILD_TYPE} --target install
		WORKING_DIRECTORY "${OCTKWrapAssimp_BUILD_DIR}"
		RESULT_VARIABLE BUILD_RESULT)
	if(NOT BUILD_RESULT MATCHES 0)
		message(FATAL_ERROR "${OCTKWrapAssimp_DIR_NAME} build failed.")
	endif()
	message(STATUS "${OCTKWrapAssimp_DIR_NAME} build success")

	execute_process(
		COMMAND ${CMAKE_COMMAND} --install ./
		WORKING_DIRECTORY "${OCTKWrapAssimp_BUILD_DIR}"
		RESULT_VARIABLE INSTALL_RESULT)
	if(NOT INSTALL_RESULT MATCHES 0)
		message(FATAL_ERROR "${OCTKWrapAssimp_DIR_NAME} install failed.")
	endif()
	message(STATUS "${OCTKWrapAssimp_DIR_NAME} install success")
	octk_make_stamp_file("${OCTKWrapAssimp_STAMP_FILE_PATH}")
endif()
# wrap lib
add_library(OCTK3rdparty::WrapAssimp INTERFACE IMPORTED)
find_package(assimp 6 PATHS ${OCTKWrapAssimp_INSTALL_DIR} NO_DEFAULT_PATH REQUIRED)
target_link_libraries(OCTK3rdparty::WrapAssimp INTERFACE assimp::assimp)
set(OCTKWrapAssimp_FOUND ON)