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


function(octk_vcpkg_install_package NAME)
	octk_parse_all_arguments(arg
		"octk_vcpkg_install_package"
		""
		"TARGET;PREFIX;INSTALL_DIR"
		"COMPONENTS;IMPORTED_TARGETS" ${ARGN})

	if("X${arg_TARGET}" STREQUAL "X")
		set(arg_TARGET ${NAME})
	endif()
	if("X${arg_PREFIX}" STREQUAL "X")
		string(REGEX REPLACE "[^a-zA-Z]" "" arg_PREFIX "${arg_TARGET}")
	endif()
	if("X${arg_OUTPUT_DIR}" STREQUAL "X")
		set(arg_OUTPUT_DIR "${PROJECT_BINARY_DIR}/3rdparty/vcpkg")
	endif()
	if(TARGET ${arg_TARGET})
		set(${arg_PREFIX}_FOUND ON)
		return()
	endif()
	set(${arg_PREFIX}_NAME "${NAME}")
	set(${arg_PREFIX}_ROOT_DIR "${PROJECT_BINARY_DIR}/3rdparty/${${arg_PREFIX}_NAME}")
	if(WIN32)
		set(${arg_PREFIX}_VCPKG_TRIPLET ${OCTK_VCPKG_TRIPLET}-static-md)
	else()
		set(${arg_PREFIX}_VCPKG_TRIPLET ${OCTK_VCPKG_TRIPLET})
	endif()
	set("${arg_PREFIX}_INSTALL_DIR" "${arg_OUTPUT_DIR}/${NAME}/installed/${${arg_PREFIX}_VCPKG_TRIPLET}" CACHE INTERNAL "" FORCE)
	if(NOT EXISTS "${${arg_PREFIX}_INSTALL_DIR}")
		unset(${arg_PREFIX}_COMPONENTS_CONFIG)
		if("X${arg_COMPONENTS}" STREQUAL "X")
			set(${arg_PREFIX}_COMPONENTS_CONFIG "")
		else()
			set(${arg_PREFIX}_COMPONENTS_CONFIG "[")
			foreach(component IN LISTS arg_COMPONENTS)
				if(NOT "${${arg_PREFIX}_COMPONENTS_CONFIG}" STREQUAL "")
					set(${arg_PREFIX}_COMPONENTS_CONFIG "${${arg_PREFIX}_COMPONENTS_CONFIG},")
				endif()
				set(${arg_PREFIX}_COMPONENTS_CONFIG "${${arg_PREFIX}_COMPONENTS_CONFIG}${component}")
			endforeach()
			set(${arg_PREFIX}_COMPONENTS_CONFIG "${${arg_PREFIX}_COMPONENTS_CONFIG}]")
		endif()
		execute_process(
			COMMAND ${OCTKVcpkgTools_EXECUTABLE} list ${NAME}${${arg_PREFIX}_COMPONENTS_CONFIG}:${${arg_PREFIX}_VCPKG_TRIPLET}
			WORKING_DIRECTORY "${OCTKVcpkgTools_ROOT_DIR}"
			OUTPUT_VARIABLE FIND_OUTPUT
			RESULT_VARIABLE FIND_RESULT)
		if("X${FIND_OUTPUT}" STREQUAL "X")
			message(STATUS "${${arg_PREFIX}_NAME} not installed, start install...")
			set(${arg_PREFIX}_VCPKG_CONFIGS ${NAME}${${arg_PREFIX}_COMPONENTS_CONFIGS}:${${arg_PREFIX}_VCPKG_TRIPLET})
			message(STATUS "${${arg_PREFIX}_NAME} vcpkg install configs: ${${arg_PREFIX}_VCPKG_CONFIGS}")
			execute_process(
				COMMAND "${OCTKVcpkgTools_EXECUTABLE}" install ${${arg_PREFIX}_VCPKG_CONFIGS} --recurse
				WORKING_DIRECTORY "${OCTKVcpkgTools_ROOT_DIR}"
				RESULT_VARIABLE INSTALL_RESULT
				COMMAND_ECHO STDOUT)
			if(NOT (INSTALL_RESULT MATCHES 0))
				message(FATAL_ERROR "${${arg_PREFIX}_NAME} install failed.")
			endif()
		endif()

		message(STATUS "${${arg_PREFIX}_NAME} not exported, start export...")
		execute_process(
			COMMAND "${OCTKVcpkgTools_EXECUTABLE}" export ${NAME}:${${arg_PREFIX}_VCPKG_TRIPLET}
			--raw --output=${${arg_PREFIX}_NAME} --output-dir=${arg_OUTPUT_DIR}
			WORKING_DIRECTORY "${OCTKVcpkgTools_ROOT_DIR}"
			RESULT_VARIABLE EXPORT_RESULT
			COMMAND_ECHO STDOUT)
		if(NOT (EXPORT_RESULT MATCHES 0))
			message(FATAL_ERROR "${${arg_PREFIX}_NAME} export failed.")
		endif()
	endif()

	add_library(${arg_TARGET} INTERFACE IMPORTED GLOBAL)
	if(EXISTS "${${arg_PREFIX}_INSTALL_DIR}/share/${NAME}/Find${NAME}.cmake" OR
		EXISTS "${${arg_PREFIX}_INSTALL_DIR}/share/${NAME}/${NAME}Targets.cmake")
		set(CMAKE_MODULE_PATH_CACHE ${CMAKE_MODULE_PATH})
		set(CMAKE_MODULE_PATH "${${arg_PREFIX}_INSTALL_DIR}/share/${NAME}")
		set(${NAME}_DIR "${${arg_PREFIX}_INSTALL_DIR}/share/${NAME}")
		find_package(${NAME} REQUIRED)
		if(TARGET ${NAME}::${NAME})
			target_link_libraries(${arg_TARGET} INTERFACE ${NAME}::${NAME})
		endif()
		target_link_libraries(${arg_TARGET} INTERFACE ${arg_IMPORTED_TARGETS})
		target_include_directories(${arg_TARGET} INTERFACE "${${arg_PREFIX}_INSTALL_DIR}/include")
		set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH_CACHE})
	else()
		if(CMAKE_BUILD_TYPE STREQUAL "Debug")
			set(${arg_PREFIX}_PKGCONFIG_DIR "${${arg_PREFIX}_INSTALL_DIR}/debug/lib")
		else()
			set(${arg_PREFIX}_PKGCONFIG_DIR "${${arg_PREFIX}_INSTALL_DIR}/lib")
		endif()
		if("X${arg_COMPONENTS}" STREQUAL "X")
			octk_pkg_check_modules(${NAME} REQUIRED
				PATH "${${arg_PREFIX}_PKGCONFIG_DIR}/pkgconfig")
		else()
			octk_pkg_check_modules(${NAME} REQUIRED
				PATH "${${arg_PREFIX}_PKGCONFIG_DIR}/pkgconfig"
				IMPORTED_TARGET
				${arg_COMPONENTS})
		endif()
		target_link_libraries(${arg_TARGET} INTERFACE PkgConfig::${NAME})
	endif()
	set(${arg_PREFIX}_FOUND ON)
endfunction()


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