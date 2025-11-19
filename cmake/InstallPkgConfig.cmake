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
macro(octk_pkg_add_path PATH)
	set(ENV{PKG_CONFIG_PATH} "${PATH}")
	set(PKG_CONFIG_ARGN "--with-path=${PATH}")
	message(STATUS "Add PkgConfig check modules search path in ${PATH}")
endmacro()

macro(octk_pkg_check_modules PREFIX)
	octk_parse_all_arguments(arg
		"octk_pkg_check_modules"
		"REQUIRED"
		"PATH"
		"IMPORTED_TARGET" ${ARGN})

	find_package(PkgConfig REQUIRED)
	if(arg_PATH)
		octk_pkg_add_path("${arg_PATH}")
	endif()
	if(arg_REQUIRED)
		pkg_check_modules(${PREFIX} REQUIRED IMPORTED_TARGET ${arg_IMPORTED_TARGET})
	else()
		pkg_check_modules(${PREFIX} IMPORTED_TARGET ${arg_IMPORTED_TARGET})
	endif()
endmacro()

if(TARGET OCTK3rdparty::PkgConfig)
	set(OCTKPkgConfig_FOUND ON)
	return()
endif()

if(EXISTS "${PROJECT_SOURCE_DIR}/3rdparty/pkgconf-${OCTK_PLATFORM_NAME}.zip")
	message(STATUS "Install pkgconf-${OCTK_PLATFORM_NAME}...")
	set(OCTKPkgConfig_NAME "pkgconf-${OCTK_PLATFORM_NAME}")
	set(OCTKPkgConfig_PKG_NAME "pkgconf-${OCTK_PLATFORM_NAME}.zip")
	set(OCTKPkgConfig_URL_PATH "${PROJECT_SOURCE_DIR}/3rdparty/${OCTKPkgConfig_PKG_NAME}")
	set(OCTKPkgConfig_ROOT_DIR "${PROJECT_BINARY_DIR}/3rdparty/${OCTKPkgConfig_NAME}")
	set(OCTKPkgConfig_INSTALL_DIR "${OCTKPkgConfig_ROOT_DIR}/source")
	octk_stamp_file_info(PkgConfig OUTPUT_DIR "${OCTKPkgConfig_ROOT_DIR}")
	octk_fetch_3rdparty(PkgConfig URL "${OCTKPkgConfig_URL_PATH}")
	message(STATUS "Set PkgConf executable path.")
	if(WIN32)
		set(OCTKPkgConfig_EXECUTABLE "${OCTKPkgConfig_INSTALL_DIR}/pkgconf.exe"
			CACHE INTERNAL "PkgConf executable path." FORCE)
	else()
		set(OCTKPkgConfig_EXECUTABLE "${OCTKPkgConfig_INSTALL_DIR}/pkgconf"
			CACHE INTERNAL "PkgConf executable path." FORCE)
	endif()
	set(PKG_CONFIG_EXECUTABLE "${OCTKPkgConfig_EXECUTABLE}"
		CACHE INTERNAL "PKG_CONFIG_EXECUTABLE executable path." FORCE)
else()
	find_package(PkgConfig QUIET)
	if(NOT PkgConfig_FOUND)
		set(OCTKPkgConfig_NAME "pkgconf-2.3.0")
		set(OCTKPkgConfig_PKG_NAME "${OCTKPkgConfig_NAME}.zip")
		set(OCTKPkgConfig_URL_PATH "${PROJECT_SOURCE_DIR}/3rdparty/${OCTKPkgConfig_PKG_NAME}")
		set(OCTKPkgConfig_ROOT_DIR "${PROJECT_BINARY_DIR}/3rdparty/${OCTKPkgConfig_NAME}")
		set(OCTKPkgConfig_BUILD_DIR "${OCTKPkgConfig_ROOT_DIR}/source")
		set(OCTKPkgConfig_SOURCE_DIR "${OCTKPkgConfig_ROOT_DIR}/source")
		set(OCTKPkgConfig_INSTALL_DIR "${OCTKPkgConfig_ROOT_DIR}/install")
		octk_stamp_file_info(PkgConfig OUTPUT_DIR "${OCTKPkgConfig_ROOT_DIR}")
		octk_fetch_3rdparty(PkgConfig URL "${OCTKPkgConfig_URL_PATH}")
		if(NOT EXISTS "${OCTKPkgConfig_STAMP_FILE_PATH}")
			if(NOT EXISTS ${OCTKPkgConfig_SOURCE_DIR})
				message(FATAL_ERROR "${OCTKPkgConfig_NAME} FetchContent failed.")
			endif()
			execute_process(
				COMMAND ${CMAKE_COMMAND} -E make_directory ${OCTKPkgConfig_BUILD_DIR}
				WORKING_DIRECTORY "${OCTKPkgConfig_ROOT_DIR}"
				RESULT_VARIABLE MKDIR_RESULT)
			if(NOT (MKDIR_RESULT MATCHES 0))
				message(FATAL_ERROR "${OCTKPkgConfig_DIR_NAME} lib build directory make failed.")
			endif()

			message(STATUS "Autogen ${OCTKPkgConfig_DIR_NAME} lib...")
			execute_process(				
				COMMAND ./autogen.sh
				WORKING_DIRECTORY "${OCTKPkgConfig_SOURCE_DIR}"
				RESULT_VARIABLE AUTOGEN_RESULT)
			if(NOT AUTOGEN_RESULT MATCHES 0)
				message(FATAL_ERROR "${OCTKPkgConfig_DIR_NAME} autogen failed.")
			endif()
			message(STATUS "${OCTKPkgConfig_DIR_NAME} autogen success")

			message(STATUS "Configure ${OCTKPkgConfig_DIR_NAME} lib...")
			execute_process(
				COMMAND ./configure --prefix=${OCTKPkgConfig_INSTALL_DIR}
				WORKING_DIRECTORY "${OCTKPkgConfig_SOURCE_DIR}"
				RESULT_VARIABLE CONFIGURE_RESULT)
			if(NOT CONFIGURE_RESULT MATCHES 0)
				message(FATAL_ERROR "${OCTKPkgConfig_DIR_NAME} configure failed.")
			endif()
			message(STATUS "${OCTKPkgConfig_DIR_NAME} configure success")
			
			execute_process(
				COMMAND make -j${OCTK_NUMBER_OF_ASYNC_JOBS}
				WORKING_DIRECTORY "${OCTKPkgConfig_SOURCE_DIR}"
				RESULT_VARIABLE BUILD_RESULT)
			if(NOT BUILD_RESULT MATCHES 0)
				message(FATAL_ERROR "${OCTKPkgConfig_DIR_NAME} build failed.")
			endif()
			message(STATUS "${OCTKPkgConfig_DIR_NAME} build success")
					
			execute_process(
				COMMAND make install
				WORKING_DIRECTORY "${OCTKPkgConfig_SOURCE_DIR}"
				RESULT_VARIABLE INSTALL_RESULT)
			if(NOT INSTALL_RESULT MATCHES 0)
				message(FATAL_ERROR "${OCTKPkgConfig_DIR_NAME} install failed.")
			endif()
			message(STATUS "${OCTKPkgConfig_DIR_NAME} install success")
			octk_make_stamp_file("${OCTKPkgConfig_STAMP_FILE_PATH}")
		endif()
		message(STATUS "Set PkgConf executable path.")
		set(OCTKPkgConfig_EXECUTABLE "${OCTKPkgConfig_INSTALL_DIR}/bin/pkgconf" 
			CACHE INTERNAL "PkgConf executable path." FORCE)
		set(PKG_CONFIG_EXECUTABLE "${OCTKPkgConfig_EXECUTABLE}"
			CACHE INTERNAL "PKG_CONFIG_EXECUTABLE executable path." FORCE)
	else()
		message(STATUS "Set use system PkgConf executable ${PKG_CONFIG_EXECUTABLE}.")
	endif()
endif()
set(OCTKPkgConfig_FOUND ON)