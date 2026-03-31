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
if(TARGET OpenCTKWrapOpenSSL::WrapOpenSSL)
	set(OpenCTKWrapOpenSSL_FOUND ON)
	return()
endif()

include(InstallVcpkg)
if(ON)
	octk_vcpkg_install_package(openssl
		NOT_IMPORT
		TARGET
		OpenCTKWrapOpenSSL::WrapOpenSSL
		PREFIX
		OpenCTKWrapOpenSSL)
else()
	set(OpenCTKWrapOpenSSL_NAME "openssl")
	set(OpenCTKWrapOpenSSL_ROOT_DIR "${PROJECT_BINARY_DIR}/3rdparty/${OpenCTKWrapOpenSSL_NAME}")
	if(WIN32)
		set(OpenCTKWrapOpenSSL_VCPKG_TRIPLET ${OCTK_VCPKG_TRIPLET}-static-md)
	else()
		set(OpenCTKWrapOpenSSL_VCPKG_TRIPLET ${OCTK_VCPKG_TRIPLET})
	endif()
	set(OpenCTKWrapOpenSSL_INSTALL_DIR "${OpenCTKWrapOpenSSL_ROOT_DIR}/installed/${OpenCTKWrapOpenSSL_VCPKG_TRIPLET}" CACHE INTERNAL "" FORCE)
	if(NOT EXISTS "${OpenCTKWrapOpenSSL_INSTALL_DIR}")
		execute_process(
			COMMAND ${OpenCTKVcpkg_EXECUTABLE} list ${OpenCTKWrapOpenSSL_NAME}:${OpenCTKWrapOpenSSL_VCPKG_TRIPLET}
			WORKING_DIRECTORY "${OpenCTKVcpkg_ROOT_DIR}"
			OUTPUT_VARIABLE FIND_OUTPUT
			RESULT_VARIABLE FIND_RESULT)
		if("X${FIND_OUTPUT}" STREQUAL "X")
			message(STATUS "${OpenCTKWrapOpenSSL_NAME} not installed, start install...")
			set(OpenCTKWrapOpenSSL_VCPKG_CONFIGS ${OpenCTKWrapOpenSSL_NAME}:${OpenCTKWrapOpenSSL_VCPKG_TRIPLET})
			message(STATUS "${OpenCTKWrapOpenSSL_NAME} vcpkg install configs: ${OpenCTKWrapOpenSSL_VCPKG_CONFIGS}")
			execute_process(
				COMMAND "${OpenCTKVcpkg_EXECUTABLE}" install ${OpenCTKWrapOpenSSL_VCPKG_CONFIGS} --recurse
				WORKING_DIRECTORY "${OpenCTKVcpkg_ROOT_DIR}"
				RESULT_VARIABLE INSTALL_RESULT
				COMMAND_ECHO STDOUT)
			if(NOT (INSTALL_RESULT MATCHES 0))
				message(FATAL_ERROR "${OpenCTKWrapOpenSSL_NAME} install failed.")
			endif()
		endif()

		execute_process(
			COMMAND "${OpenCTKVcpkg_EXECUTABLE}" export ${OpenCTKWrapOpenSSL_NAME}:${OpenCTKWrapOpenSSL_VCPKG_TRIPLET}
			--raw --output=${OpenCTKWrapOpenSSL_NAME} --output-dir=${PROJECT_BINARY_DIR}/3rdparty
			WORKING_DIRECTORY "${OpenCTKVcpkg_ROOT_DIR}"
			RESULT_VARIABLE EXPORT_RESULT
			COMMAND_ECHO STDOUT)
		if(NOT (EXPORT_RESULT MATCHES 0))
			message(FATAL_ERROR "${OpenCTKWrapOpenSSL_NAME} export failed.")
		endif()
	endif()
endif()
set(OPENSSL_ROOT_DIR ${OpenCTKWrapOpenSSL_INSTALL_DIR})
find_package(OpenSSL PATHS ${OpenCTKWrapOpenSSL_INSTALL_DIR} NO_DEFAULT_PATH REQUIRED)
target_link_libraries(OpenCTKWrapOpenSSL::WrapOpenSSL INTERFACE OpenSSL::SSL OpenSSL::Crypto)
set(OpenCTKWrapOpenSSL_FOUND ON)
