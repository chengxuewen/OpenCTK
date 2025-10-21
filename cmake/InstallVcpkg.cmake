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
if(TARGET OCTK3rdparty::Vcpkg)
	set(OCTKVcpkg_FOUND ON)
	return()
endif()

set(OCTKVcpkg_NAME "vcpkg")
set(OCTKVcpkg_ROOT_DIR "${PROJECT_SOURCE_DIR}/vcpkg" CACHE INTERNAL "" FORCE)
set(OCTKVcpkg_INSTALL_DIR "${OCTKVcpkg_ROOT_DIR}/installed" CACHE INTERNAL "" FORCE)
set(OCTKVcpkgTools_ROOT_DIR "${PROJECT_SOURCE_DIR}/vcpkg-tools" CACHE INTERNAL "" FORCE)
set(OCTKVcpkgTools_INSTALL_DIR "${OCTKVcpkgTools_ROOT_DIR}/installed" CACHE INTERNAL "" FORCE)
find_package(Git REQUIRED)
if(GIT_EXECUTABLE)
	if(NOT EXISTS "${OCTKVcpkg_ROOT_DIR}/.git")
		execute_process(
			COMMAND "${GIT_EXECUTABLE}" clone https://github.com/microsoft/vcpkg.git
			WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
			RESULT_VARIABLE CLONE_RESULT
			COMMAND_ECHO STDOUT)
		if(NOT (CLONE_RESULT MATCHES 0))
			message(FATAL_ERROR "${OCTKVcpkg_NAME} clone failed.")
		endif()
	endif()
	if(WIN32)
		set(OCTKVcpkg_EXECUTABLE_NAME "vcpkg.exe")
		set(OCTKVcpkg_BOOTSTRAP_NAME "bootstrap-vcpkg.bat")
	else()
		set(OCTKVcpkg_EXECUTABLE_NAME "vcpkg")
		set(OCTKVcpkg_BOOTSTRAP_NAME "./bootstrap-vcpkg.sh")
	endif()
	if(NOT EXISTS "${OCTKVcpkg_ROOT_DIR}/${OCTKVcpkg_EXECUTABLE_NAME}")
		execute_process(
			COMMAND "${OCTKVcpkg_BOOTSTRAP_NAME}"
			WORKING_DIRECTORY "${OCTKVcpkg_ROOT_DIR}"
			RESULT_VARIABLE INIT_RESULT
			COMMAND_ECHO STDOUT)
		if(NOT (INIT_RESULT MATCHES 0))
			message(FATAL_ERROR "${OCTKVcpkg_NAME} init failed.")
		endif()
	endif()
	set(OCTKVcpkg_EXECUTABLE "${OCTKVcpkg_ROOT_DIR}/${OCTKVcpkg_EXECUTABLE_NAME}" CACHE INTERNAL "" FORCE)
	if(NOT EXISTS "${OCTKVcpkgTools_ROOT_DIR}/.git")
		execute_process(
			COMMAND ${CMAKE_COMMAND} -E copy_directory "${OCTKVcpkg_ROOT_DIR}" "${OCTKVcpkgTools_ROOT_DIR}"
			WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
			RESULT_VARIABLE COPY_RESULT
			COMMAND_ECHO STDOUT)
		if(NOT (COPY_RESULT MATCHES 0))
			message(FATAL_ERROR "${OCTKVcpkg_NAME} copy to vcpkg-tools failed.")
		endif()
	endif()
	set(OCTKVcpkgTools_EXECUTABLE "${OCTKVcpkgTools_ROOT_DIR}/${OCTKVcpkg_EXECUTABLE_NAME}" CACHE INTERNAL "" FORCE)
endif()
set(OCTKVcpkg_FOUND ON)