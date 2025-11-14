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
if(TARGET OCTK3rdparty::WrapOpenSSL)
	set(OCTKWrapOpenSSL_FOUND ON)
	return()
endif()

include(InstallVcpkg)
if(ON)
	octk_vcpkg_install_package(openssl
		NOT_IMPORT
		TARGET
		OCTK3rdparty::WrapOpenSSL
		PREFIX
		OCTKWrapOpenSSL)
else()
	set(OCTKWrapOpenSSL_NAME "openssl")
	set(OCTKWrapOpenSSL_ROOT_DIR "${PROJECT_BINARY_DIR}/3rdparty/${OCTKWrapOpenSSL_NAME}")
	if(WIN32)
		set(OCTKWrapOpenSSL_VCPKG_TRIPLET ${OCTK_VCPKG_TRIPLET}-static-md)
	else()
		set(OCTKWrapOpenSSL_VCPKG_TRIPLET ${OCTK_VCPKG_TRIPLET})
	endif()
	set(OCTKWrapOpenSSL_INSTALL_DIR "${OCTKWrapOpenSSL_ROOT_DIR}/installed/${OCTKWrapOpenSSL_VCPKG_TRIPLET}" CACHE INTERNAL "" FORCE)
	if(NOT EXISTS "${OCTKWrapOpenSSL_INSTALL_DIR}")
		execute_process(
			COMMAND ${OCTKVcpkg_EXECUTABLE} list ${OCTKWrapOpenSSL_NAME}:${OCTKWrapOpenSSL_VCPKG_TRIPLET}
			WORKING_DIRECTORY "${OCTKVcpkg_ROOT_DIR}"
			OUTPUT_VARIABLE FIND_OUTPUT
			RESULT_VARIABLE FIND_RESULT)
		if("X${FIND_OUTPUT}" STREQUAL "X")
			message(STATUS "${OCTKWrapOpenSSL_NAME} not installed, start install...")
			set(OCTKWrapOpenSSL_VCPKG_CONFIGS ${OCTKWrapOpenSSL_NAME}:${OCTKWrapOpenSSL_VCPKG_TRIPLET})
			message(STATUS "${OCTKWrapOpenSSL_NAME} vcpkg install configs: ${OCTKWrapOpenSSL_VCPKG_CONFIGS}")
			execute_process(
				COMMAND "${OCTKVcpkg_EXECUTABLE}" install ${OCTKWrapOpenSSL_VCPKG_CONFIGS} --recurse
				WORKING_DIRECTORY "${OCTKVcpkg_ROOT_DIR}"
				RESULT_VARIABLE INSTALL_RESULT
				COMMAND_ECHO STDOUT)
			if(NOT (INSTALL_RESULT MATCHES 0))
				message(FATAL_ERROR "${OCTKWrapOpenSSL_NAME} install failed.")
			endif()
		endif()

		execute_process(
			COMMAND "${OCTKVcpkg_EXECUTABLE}" export ${OCTKWrapOpenSSL_NAME}:${OCTKWrapOpenSSL_VCPKG_TRIPLET}
			--raw --output=${OCTKWrapOpenSSL_NAME} --output-dir=${PROJECT_BINARY_DIR}/3rdparty
			WORKING_DIRECTORY "${OCTKVcpkg_ROOT_DIR}"
			RESULT_VARIABLE EXPORT_RESULT
			COMMAND_ECHO STDOUT)
		if(NOT (EXPORT_RESULT MATCHES 0))
			message(FATAL_ERROR "${OCTKWrapOpenSSL_NAME} export failed.")
		endif()
	endif()
endif()
set(OPENSSL_ROOT_DIR ${OCTKWrapOpenSSL_INSTALL_DIR})
find_package(OpenSSL PATHS ${OCTKWrapOpenSSL_INSTALL_DIR} NO_DEFAULT_PATH REQUIRED)
target_link_libraries(OCTK3rdparty::WrapOpenSSL INTERFACE OpenSSL::SSL OpenSSL::Crypto)
set(OCTKWrapOpenSSL_FOUND ON)
