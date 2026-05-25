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
		pkg_check_modules(${PREFIX} REQUIRED IMPORTED_TARGET GLOBAL ${arg_IMPORTED_TARGET})
	elseif(arg_QUIET)
		pkg_check_modules(${PREFIX} QUIET IMPORTED_TARGET GLOBAL ${arg_IMPORTED_TARGET})
	else()
		pkg_check_modules(${PREFIX} IMPORTED_TARGET GLOBAL ${arg_IMPORTED_TARGET})
	endif()
	set(ENV{PKG_CONFIG_PATH} "${PKG_CONFIG_PATH_CACHE}")
endmacro()

if(TARGET OpenCTKPkgconf::Pkgconf)
	set(OpenCTKPkgconf_FOUND ON)
	return()
endif()

set(OpenCTKPkgconf_NAME "pkgconf-2.5.1")
set(OpenCTKPkgconf_PKG_NAME "${OpenCTKPkgconf_NAME}.7z")
set(OpenCTKPkgconf_URL_PATH "${PROJECT_SOURCE_DIR}/3rdparty/${OpenCTKPkgconf_PKG_NAME}")
set(OpenCTKPkgconf_ROOT_DIR "${PROJECT_BINARY_DIR}/3rdparty/${OpenCTKPkgconf_NAME}")
set(OpenCTKPkgconf_BUILD_DIR "${OpenCTKPkgconf_ROOT_DIR}/build")
set(OpenCTKPkgconf_SOURCE_DIR "${OpenCTKPkgconf_ROOT_DIR}/source")
set(OpenCTKPkgconf_INSTALL_DIR "${OpenCTKPkgconf_ROOT_DIR}/install")
octk_stamp_file_info(OpenCTKPkgconf OUTPUT_DIR "${OpenCTKPkgconf_ROOT_DIR}")
octk_fetch_3rdparty(OpenCTKPkgconf URL "${OpenCTKPkgconf_URL_PATH}")
if(NOT EXISTS "${OpenCTKPkgconf_STAMP_FILE_PATH}")
	if(NOT EXISTS ${OpenCTKPkgconf_SOURCE_DIR})
		message(FATAL_ERROR "${OpenCTKPkgconf_NAME} FetchContent failed.")
	endif()
	octk_reset_dir(${OpenCTKPkgconf_BUILD_DIR})

	set(OpenCTKPkgconf_MESON_BUILD ON)
	find_program(AUTOCONF autoconf
    	HINTS /usr/local/bin
    	PATHS /usr/bin)
	if(AUTOCONF)
    	execute_process(
    	COMMAND ${AUTOCONF} --version
    		OUTPUT_VARIABLE AUTOCONF_VERSION_OUTPUT
    		OUTPUT_STRIP_TRAILING_WHITESPACE
    		ERROR_QUIET)
		string(REGEX MATCH "[0-9]+\\.[0-9]+(\\.[0-9]+)?" AUTOCONF_VERSION "${AUTOCONF_VERSION_OUTPUT}")
		if(AUTOCONF_VERSION)
    		if(${AUTOCONF_VERSION} VERSION_GREATER_EQUAL "2.71")
				find_program(AUTOMAKE automake
    				HINTS /usr/local/bin
    				PATHS /usr/bin)
				if(AUTOMAKE)
					set(OpenCTKPkgconf_MESON_BUILD OFF)
				endif()
			endif()
		endif()
	endif()

	if(OpenCTKPkgconf_MESON_BUILD)
		include(InstallPython)
		message(STATUS "meson setup ${OpenCTKPkgconf_NAME} lib...")
		execute_process(
			COMMAND ${OpenCTKPython_EXECUTABLE} ${OCTKMeson_FILE} setup ../build -Dtests=disabled
			--prefix=${OpenCTKPkgconf_INSTALL_DIR} --libdir=lib
			WORKING_DIRECTORY "${OpenCTKPkgconf_SOURCE_DIR}"
			RESULT_VARIABLE SETUP_RESULT)
		if(SETUP_RESULT MATCHES 0)
			message(STATUS "meson compile ${OpenCTKPkgconf_NAME} lib...")
			execute_process(
				COMMAND ${OpenCTKPython_EXECUTABLE} ${OCTKMeson_FILE} compile -C ../build
				WORKING_DIRECTORY "${OpenCTKPkgconf_SOURCE_DIR}"
				RESULT_VARIABLE COMPILE_RESULT)
			if(COMPILE_RESULT MATCHES 0)
				execute_process(
					COMMAND ${OpenCTKPython_EXECUTABLE} ${OCTKMeson_FILE} install -C ../build
					WORKING_DIRECTORY "${OpenCTKPkgconf_SOURCE_DIR}"
					RESULT_VARIABLE INSTALL_RESULT)
				if(INSTALL_RESULT MATCHES 0)
					octk_make_stamp_file("${OpenCTKPkgconf_STAMP_FILE_PATH}")
					message(STATUS "${OpenCTKPkgconf_NAME} meson install success")
				else()
					message(WARNING "${OpenCTKPkgconf_NAME} meson install failed.")
				endif()
				message(STATUS "${OpenCTKPkgconf_NAME} meson compile success")
			else()
				message(WARNING "${OpenCTKPkgconf_NAME} meson compile failed.")
			endif()
			message(STATUS "${OpenCTKPkgconf_NAME} meson setup success")
		else()
			message(WARNING "${OpenCTKPkgconf_NAME} meson setup failed.")
		endif()
	else()
		message(STATUS "Autogen ${OpenCTKPkgconf_NAME} lib...")
		execute_process(
			COMMAND ./autogen.sh
			WORKING_DIRECTORY "${OpenCTKPkgconf_SOURCE_DIR}"
			RESULT_VARIABLE AUTOGEN_RESULT)
		if(AUTOGEN_RESULT MATCHES 0)
			message(STATUS "Configure ${OpenCTKPkgconf_NAME} lib...")
			execute_process(
				COMMAND ./configure --prefix=${OpenCTKPkgconf_INSTALL_DIR}
				WORKING_DIRECTORY "${OpenCTKPkgconf_SOURCE_DIR}"
				RESULT_VARIABLE CONFIGURE_RESULT)
			if(CONFIGURE_RESULT MATCHES 0)
				execute_process(
					COMMAND make -j${OCTK_NUMBER_OF_ASYNC_JOBS}
					WORKING_DIRECTORY "${OpenCTKPkgconf_SOURCE_DIR}"
					RESULT_VARIABLE BUILD_RESULT)
				if(BUILD_RESULT MATCHES 0)
					execute_process(
						COMMAND make install
						WORKING_DIRECTORY "${OpenCTKPkgconf_SOURCE_DIR}"
						RESULT_VARIABLE INSTALL_RESULT)
					if(INSTALL_RESULT MATCHES 0)
						message(STATUS "${OpenCTKPkgconf_NAME} install success")
						octk_make_stamp_file("${OpenCTKPkgconf_STAMP_FILE_PATH}")
					else()
						message(FATAL_ERROR "${OpenCTKPkgconf_NAME} install failed.")
					endif()
					message(STATUS "${OpenCTKPkgconf_NAME} build success")
				else()
					message(FATAL_ERROR "${OpenCTKPkgconf_NAME} build failed.")
				endif()
				message(STATUS "${OpenCTKPkgconf_NAME} configure success")
			else()
				message(FATAL_ERROR "${OpenCTKPkgconf_NAME} configure failed.")
			endif()
			message(STATUS "${OpenCTKPkgconf_NAME} autogen success")
		else()
			message(FATAL_ERROR "${OpenCTKPkgconf_NAME} autogen failed.")
		endif()
	endif()
	if(NOT EXISTS "${OpenCTKPkgconf_STAMP_FILE_PATH}")
		include(InstallVcpkg)
		octk_vcpkg_install_package(pkgconf
			NOT_IMPORT TOOLS
			TARGET
			OpenCTKPkgconf::Pkgconf
			PREFIX
			OpenCTKPkgconfVcpkg
			OUTPUT_DIR
			${OpenCTKPkgconf_ROOT_DIR})
		octk_reset_dir(${OpenCTKPkgconf_INSTALL_DIR})
		execute_process(
			COMMAND ${CMAKE_COMMAND} -E copy_directory "${OpenCTKPkgconfVcpkg_INSTALL_DIR}/tools/pkgconf"
			"${OpenCTKPkgconf_INSTALL_DIR}/bin"
			WORKING_DIRECTORY "${OpenCTKPkgconf_ROOT_DIR}"
			RESULT_VARIABLE COPYDIR_RESULT)
		if(COPYDIR_RESULT MATCHES 0)
			message(STATUS "${OpenCTKPkgconf_NAME} install success")
			octk_make_stamp_file("${OpenCTKPkgconf_STAMP_FILE_PATH}")
		else()
			message(FATAL_ERROR "${OpenCTKPkgconf_NAME} vcpkg install dir copy failed.")
		endif()
	endif()
endif()
set(OpenCTKPkgconf_EXECUTABLE "${OpenCTKPkgconf_INSTALL_DIR}/bin/pkgconf"
	CACHE INTERNAL "PkgConf executable path." FORCE)
set(PKG_CONFIG_EXECUTABLE "${OpenCTKPkgconf_EXECUTABLE}"
	CACHE INTERNAL "PKG_CONFIG_EXECUTABLE executable path." FORCE)
message(STATUS "Set PkgConf executable path ${OpenCTKPkgconf_EXECUTABLE}")
# Ensure pkgconf binary can find its own libpkgconf.so at runtime.
# Meson may not set RPATH, so prepend install lib dir to LD_LIBRARY_PATH.
set(ENV{LD_LIBRARY_PATH} "${OpenCTKPkgconf_INSTALL_DIR}/lib:$ENV{LD_LIBRARY_PATH}")
set(OpenCTKPkgconf_FOUND ON)
