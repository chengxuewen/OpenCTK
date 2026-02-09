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
macro(octk_pkgconf_add_path PATH)
	set(ENV{PKG_CONFIG_PATH} "${PATH}")
	set(PKG_CONFIG_ARGN "--with-path=${PATH}")
	message(STATUS "Add Pkgconf check modules search path in ${PATH}")
endmacro()

macro(octk_pkgconf_check_modules PREFIX)
	octk_parse_all_arguments(arg
		"octk_pkgconf_check_modules"
		"REQUIRED;QUIET"
		"PATH"
		"IMPORTED_TARGET" ${ARGN})

	find_package(PkgConfig REQUIRED)
	set(PKG_CONFIG_PATH_CACHE "$ENV{PKG_CONFIG_PATH}")
	if(arg_PATH)
		octk_pkgconf_add_path("${arg_PATH}")
	endif()
	if(arg_REQUIRED)
		pkg_check_modules(${PREFIX} REQUIRED IMPORTED_TARGET ${arg_IMPORTED_TARGET})
	elseif(arg_QUIET)
		pkg_check_modules(${PREFIX} QUIET IMPORTED_TARGET ${arg_IMPORTED_TARGET})
	else()
		pkg_check_modules(${PREFIX} IMPORTED_TARGET ${arg_IMPORTED_TARGET})
	endif()
	set(ENV{PKG_CONFIG_PATH} "${PKG_CONFIG_PATH_CACHE}")
endmacro()

if(TARGET OCTK3rdparty::Pkgconf)
	set(OCTKPkgconf_FOUND ON)
	return()
endif()

set(OCTKPkgconf_NAME "pkgconf-2.5.1")
set(OCTKPkgconf_PKG_NAME "${OCTKPkgconf_NAME}.7z")
set(OCTKPkgconf_URL_PATH "${PROJECT_SOURCE_DIR}/3rdparty/${OCTKPkgconf_PKG_NAME}")
set(OCTKPkgconf_ROOT_DIR "${PROJECT_BINARY_DIR}/3rdparty/${OCTKPkgconf_NAME}")
set(OCTKPkgconf_BUILD_DIR "${OCTKPkgconf_ROOT_DIR}/build")
set(OCTKPkgconf_SOURCE_DIR "${OCTKPkgconf_ROOT_DIR}/source")
set(OCTKPkgconf_INSTALL_DIR "${OCTKPkgconf_ROOT_DIR}/install")
octk_stamp_file_info(OCTKPkgconf OUTPUT_DIR "${OCTKPkgconf_ROOT_DIR}")
octk_fetch_3rdparty(OCTKPkgconf URL "${OCTKPkgconf_URL_PATH}")
if(NOT EXISTS "${OCTKPkgconf_STAMP_FILE_PATH}")
	if(NOT EXISTS ${OCTKPkgconf_SOURCE_DIR})
		message(FATAL_ERROR "${OCTKPkgconf_NAME} FetchContent failed.")
	endif()
	octk_reset_dir(${OCTKPkgconf_BUILD_DIR})

	if(WIN32)
		include(InstallPython)
		message(STATUS "meson setup ${OCTKPkgconf_NAME} lib...")
		execute_process(
			COMMAND ${OCTKPython_EXECUTABLE} ${OCTKMeson_FILE} setup ../build -Dtests=disabled
			--prefix=${OCTKPkgconf_INSTALL_DIR}
			WORKING_DIRECTORY "${OCTKPkgconf_SOURCE_DIR}"
			RESULT_VARIABLE SETUP_RESULT)
		if(SETUP_RESULT MATCHES 0)
			message(STATUS "meson compile ${OCTKPkgconf_NAME} lib...")
			execute_process(
				COMMAND ${OCTKPython_EXECUTABLE} ${OCTKMeson_FILE} compile -C ../build
				WORKING_DIRECTORY "${OCTKPkgconf_SOURCE_DIR}"
				RESULT_VARIABLE COMPILE_RESULT)
			if(COMPILE_RESULT MATCHES 0)
				execute_process(
					COMMAND ${OCTKPython_EXECUTABLE} ${OCTKMeson_FILE} install -C ../build
					WORKING_DIRECTORY "${OCTKPkgconf_SOURCE_DIR}"
					RESULT_VARIABLE INSTALL_RESULT)
				if(INSTALL_RESULT MATCHES 0)
					octk_make_stamp_file("${OCTKPkgconf_STAMP_FILE_PATH}")
					message(STATUS "${OCTKPkgconf_NAME} meson install success")
				else()
					message(WARNING "${OCTKPkgconf_NAME} meson install failed.")
				endif()
				message(STATUS "${OCTKPkgconf_NAME} meson compile success")
			else()
				message(WARNING "${OCTKPkgconf_NAME} meson compile failed.")
			endif()
			message(STATUS "${OCTKPkgconf_NAME} meson setup success")
		else()
			message(WARNING "${OCTKPkgconf_NAME} meson setup failed.")
		endif()
	else()
		message(STATUS "Autogen ${OCTKPkgconf_NAME} lib...")
		execute_process(
			COMMAND ./autogen.sh
			WORKING_DIRECTORY "${OCTKPkgconf_SOURCE_DIR}"
			RESULT_VARIABLE AUTOGEN_RESULT)
		if(AUTOGEN_RESULT MATCHES 0)
			message(STATUS "Configure ${OCTKPkgconf_NAME} lib...")
			execute_process(
				COMMAND ./configure --prefix=${OCTKPkgconf_INSTALL_DIR}
				WORKING_DIRECTORY "${OCTKPkgconf_SOURCE_DIR}"
				RESULT_VARIABLE CONFIGURE_RESULT)
			if(CONFIGURE_RESULT MATCHES 0)
				execute_process(
					COMMAND make -j${OCTK_NUMBER_OF_ASYNC_JOBS}
					WORKING_DIRECTORY "${OCTKPkgconf_SOURCE_DIR}"
					RESULT_VARIABLE BUILD_RESULT)
				if(BUILD_RESULT MATCHES 0)
					execute_process(
						COMMAND make install
						WORKING_DIRECTORY "${OCTKPkgconf_SOURCE_DIR}"
						RESULT_VARIABLE INSTALL_RESULT)
					if(INSTALL_RESULT MATCHES 0)
						message(STATUS "${OCTKPkgconf_NAME} install success")
						octk_make_stamp_file("${OCTKPkgconf_STAMP_FILE_PATH}")
					else()
						message(FATAL_ERROR "${OCTKPkgconf_NAME} install failed.")
					endif()
					message(STATUS "${OCTKPkgconf_NAME} build success")
				else()
					message(FATAL_ERROR "${OCTKPkgconf_NAME} build failed.")
				endif()
				message(STATUS "${OCTKPkgconf_NAME} configure success")
			else()
				message(FATAL_ERROR "${OCTKPkgconf_NAME} configure failed.")
			endif()
			message(STATUS "${OCTKPkgconf_NAME} autogen success")
		else()
			message(FATAL_ERROR "${OCTKPkgconf_NAME} autogen failed.")
		endif()
	endif()
	if(NOT EXISTS "${OCTKPkgconf_STAMP_FILE_PATH}")
		include(InstallVcpkg)
		octk_vcpkg_install_package(pkgconf
			NOT_IMPORT TOOLS
			TARGET
			OCTK3rdparty::Pkgconf
			PREFIX
			OCTKPkgconfVcpkg
			OUTPUT_DIR
			${OCTKPkgconf_ROOT_DIR})
		octk_reset_dir(${OCTKPkgconf_INSTALL_DIR})
		execute_process(
			COMMAND ${CMAKE_COMMAND} -E copy_directory "${OCTKPkgconfVcpkg_INSTALL_DIR}/tools/pkgconf"
			"${OCTKPkgconf_INSTALL_DIR}/bin"
			WORKING_DIRECTORY "${OCTKPkgconf_ROOT_DIR}"
			RESULT_VARIABLE COPYDIR_RESULT)
		if(COPYDIR_RESULT MATCHES 0)
			message(STATUS "${OCTKPkgconf_NAME} install success")
			octk_make_stamp_file("${OCTKPkgconf_STAMP_FILE_PATH}")
		else()
			message(FATAL_ERROR "${OCTKPkgconf_NAME} vcpkg install dir copy failed.")
		endif()
	endif()
endif()
set(OCTKPkgconf_EXECUTABLE "${OCTKPkgconf_INSTALL_DIR}/bin/pkgconf"
	CACHE INTERNAL "PkgConf executable path." FORCE)
set(PKG_CONFIG_EXECUTABLE "${OCTKPkgconf_EXECUTABLE}"
	CACHE INTERNAL "PKG_CONFIG_EXECUTABLE executable path." FORCE)
message(STATUS "Set PkgConf executable path ${OCTKPkgconf_EXECUTABLE}")
set(OCTKPkgconf_FOUND ON)