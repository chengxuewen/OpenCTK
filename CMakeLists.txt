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


message(STATUS "===============================================OpenCTK==================================================")
#-----------------------------------------------------------------------------------------------------------------------
# Set cmake config
#-----------------------------------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.15...3.31)
include(.cmake.conf)


#-----------------------------------------------------------------------------------------------------------------------
# Set project name and version
#-----------------------------------------------------------------------------------------------------------------------
project(OpenCTK VERSION 0.1.1.1 LANGUAGES CXX C)
set(OCTK_VERSION_NAME ${PROJECT_VERSION})
set(OCTK_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(OCTK_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(OCTK_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(OCTK_VERSION_TWEAK ${PROJECT_VERSION_TWEAK})
set(OCTK_VERSION ${OCTK_VERSION_MAJOR}.${OCTK_VERSION_MINOR}.${OCTK_VERSION_PATCH})
set(OCTK_VERSION_STR "${OCTK_VERSION_MAJOR}.${OCTK_VERSION_MINOR}.${OCTK_VERSION_PATCH}")
set(OCTK_COPYRIGHT "Copyright (C) 2025~Present ChengXueWen.")
set(OCTK_LICENSE "MIT License")
set(OCTK_PRODUCT_NAME "OpenCTK")
math(EXPR OCTK_VERSION_CALC "${OCTK_VERSION_MAJOR}*1000 + ${OCTK_VERSION_MINOR}*100 + ${OCTK_VERSION_PATCH}")
message(STATUS "Project version: ${OCTK_VERSION}")
message(STATUS "Project copyright: ${OCTK_COPYRIGHT}")
message(STATUS "Project license: ${OCTK_LICENSE}")


#-----------------------------------------------------------------------------------------------------------------------
# Decide whether output should be verbose or not. # Default to verbose (--log-level=STATUS) in a developer-build and
# non-verbose (--log-level=NOTICE) otherwise. If a custom CMAKE_MESSAGE_LOG_LEVEL was specified, it takes priority.
# Passing an explicit --log-level=Foo has the highest priority.
#-----------------------------------------------------------------------------------------------------------------------
if(NOT CMAKE_MESSAGE_LOG_LEVEL)
	if(OCTK_FEATURE_DEV_BUILD)
		set(CMAKE_MESSAGE_LOG_LEVEL "STATUS")
	else()
		set(CMAKE_MESSAGE_LOG_LEVEL "NOTICE")
	endif()
endif()
set(CMAKE_MESSAGE_LOG_LEVEL "DEBUG")


#-----------------------------------------------------------------------------------------------------------------------
# Set build type
#-----------------------------------------------------------------------------------------------------------------------
set(OCTK_DEFAULT_BUILD_TYPE "Release")
if(OCTK_FEATURE_DEV_BUILD)
	set(OCTK_DEFAULT_BUILD_TYPE "Debug")
endif()
if(NOT CMAKE_BUILD_TYPE)
	message(STATUS "Setting build type to '${OCTK_DEFAULT_BUILD_TYPE}' as none was specified.")
	set(CMAKE_BUILD_TYPE "${OCTK_DEFAULT_BUILD_TYPE}" CACHE STRING "Choose the type of build." FORCE)
	set_property(CACHE CMAKE_BUILD_TYPE
		PROPERTY STRINGS
		"Debug" "Release" "MinSizeRel" "RelWithDebInfo") # Set the possible values for cmake-gui.
endif()
message(STATUS "CMAKE_BUILD_TYPE was set to: '${CMAKE_BUILD_TYPE}'")
if(CMAKE_CONFIGURATION_TYPES)
	message(STATUS "CMAKE_CONFIGURATION_TYPES was set to: '${CMAKE_BUILD_TYPE}'")
	set(CMAKE_CONFIGURATION_TYPES "${CMAKE_BUILD_TYPE}" CACHE INTERNAL "Force disable CMAKE_CONFIGURATION_TYPES." FORCE)
endif()

# Override the generic debug postfixes above with custom debug postfixes (even in a single config build) to follow the
# conventions we had. lib/liboctk_core_debug.6.3.0.dylib
if(WIN32)
	if(MINGW)
		# On MinGW we don't have "d" suffix for debug libraries like on Linux, unless we're building debug and release
		# libraries in one go.
		if(OCTK_GENERATOR_IS_MULTI_CONFIG)
			set(CMAKE_DEBUG_POSTFIX "d")
		endif()
	else()
		set(CMAKE_DEBUG_POSTFIX "d")
	endif()
elseif(APPLE)
	set(CMAKE_DEBUG_POSTFIX "_debug")
	set(CMAKE_FRAMEWORK_MULTI_CONFIG_POSTFIX_DEBUG "_debug")
endif()


#-----------------------------------------------------------------------------------------------------------------------
# Detect prefix and install
#-----------------------------------------------------------------------------------------------------------------------
# Pre-calculate the developer_build feature if it's set by the user via INPUT_OCTK_FEATURE_DEV_BUILD
if(NOT DEFINED OCTK_FEATURE_DEV_BUILD)
	if(NOT DEFINED INPUT_OCTK_FEATURE_DEV_BUILD)
		set(OCTK_FEATURE_DEV_BUILD ON)
	else()
		set(OCTK_FEATURE_DEV_BUILD ${INPUT_OCTK_FEATURE_DEV_BUILD})
	endif()
endif()
message(STATUS "Project feature dev build ${OCTK_FEATURE_DEV_BUILD}")

# Pre-calculate the no_prefix feature if it's set by configure via INPUT_OCTK_FEATURE_NO_PREFIX.
# This needs to be done before configure.cmake is processed.
if(NOT DEFINED OCTK_FEATURE_NO_PREFIX)
	if(NOT DEFINED INPUT_OCTK_FEATURE_NO_PREFIX)
		set(OCTK_FEATURE_NO_PREFIX ON)
	else()
		set(OCTK_FEATURE_NO_PREFIX ${INPUT_OCTK_FEATURE_NO_PREFIX})
	endif()
endif()

set(OCTK_BINARY_DIR ${OpenCTK_BINARY_DIR})
# here we specify the installation directory
# Handle both FEATURE_ and OCTK_FEATURE_ cases when they are specified on the command line explicitly.
# It's possible for one to be set, but not the other, because core/configure.cmake is not processed by this point.
if((OCTK_FEATURE_DEV_BUILD OR OCTK_FEATURE_NO_PREFIX) AND NOT CMAKE_STAGING_PREFIX)
	# Handle non-prefix builds by setting the CMake install prefix to point to OpenCTK's build dir.
	set(OCTK_DEFAULT_PREFIX "${OCTK_BINARY_DIR}/install")
else()
	if(CMAKE_HOST_WIN32)
		set(OCTK_DEFAULT_PREFIX "C:/OpenCTK/")
	else()
		set(OCTK_DEFAULT_PREFIX "/usr/local/")
	endif()
	string(APPEND OCTK_DEFAULT_PREFIX "OpenCTK-${OCTK_VERSION}")
endif()
if(NOT CMAKE_INSTALL_PREFIX AND OCTK_FEATURE_DEV_BUILD)
	set(CMAKE_INSTALL_PREFIX "${OCTK_DEFAULT_PREFIX}")
endif()

if(CMAKE_STAGING_PREFIX)
	set(OCTK_PREFIX "${CMAKE_STAGING_PREFIX}")
else()
	set(OCTK_PREFIX "${CMAKE_INSTALL_PREFIX}")
endif()

if(OCTK_PREFIX STREQUAL OCTK_BINARY_DIR)
	set(OCTK_WILL_INSTALL_VALUE OFF)
else()
	set(OCTK_WILL_INSTALL_VALUE ON)
endif()

if(OCTK_FEATURE_DEV_BUILD)
	if(DEFINED OCTK_CMAKE_EXPORT_COMPILE_COMMANDS)
		set(CMAKE_EXPORT_COMPILE_COMMANDS ${OCTK_CMAKE_EXPORT_COMPILE_COMMANDS})
	else()
		set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
	endif()
	set(OCTK_DEFAULT_BUILD_WARNINGS_ARE_ERRORS ON)
	set(OCTK_DEFAULT_BUILD_TESTS ON)
	set(OCTK_DEFAULT_BUILD_BENCHMARKS ON)

	# Tests are not built by default with qmake for iOS and friends, and thus the overall build
	# tends to fail. Disable them by default when targeting uikit.
	if(UIKIT OR ANDROID)
		set(OCTK_DEFAULT_BUILD_TESTS OFF)
	endif()

	# Disable benchmarks for single configuration generators which do not build
	# with release configuration.
	if(CMAKE_BUILD_TYPE AND CMAKE_BUILD_TYPE STREQUAL Debug)
		set(OCTK_DEFAULT_BUILD_BENCHMARKS OFF)
	endif()
else()
	set(OCTK_DEFAULT_BUILD_WARNINGS_ARE_ERRORS OFF)
	set(OCTK_DEFAULT_BUILD_TESTS OFF)
	set(OCTK_DEFAULT_BUILD_BENCHMARKS OFF)
endif()
message(STATUS "OpenCTK Install prefix set as '${CMAKE_INSTALL_PREFIX}'.")


#-----------------------------------------------------------------------------------------------------------------------
# Set Position independent
#-----------------------------------------------------------------------------------------------------------------------
## Position independent code: PIC
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
# Does the linker support position independent code? 
include(CheckPIESupported)
check_pie_supported()
# Do not relink dependent libraries when no header has changed:
set(CMAKE_LINK_DEPENDS_NO_SHARED ON)


#-----------------------------------------------------------------------------------------------------------------------
# Specify the C++ standard and flags
#-----------------------------------------------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Using C++: ${CMAKE_CXX_STANDARD}")
message(STATUS "Set C++ extensions: ${CMAKE_CXX_EXTENSIONS}")
set(OCTK_MSVC_STATIC_RUNTIME OFF)


#-----------------------------------------------------------------------------------------------------------------------
# Set CMAKE_OBJECT_PATH_MAX, avoid The maximum full path to an object file warning.
#-----------------------------------------------------------------------------------------------------------------------
set(CMAKE_OBJECT_PATH_MAX 1024)


#-----------------------------------------------------------------------------------------------------------------------
# Fetch processor count
#-----------------------------------------------------------------------------------------------------------------------
include(ProcessorCount)
processorcount(OCTK_PROCESSOR_COUNT)
set(OCTK_NUMBER_OF_ASYNC_JOBS ${OCTK_PROCESSOR_COUNT})


#-----------------------------------------------------------------------------------------------------------------------
# Add CMake function(s) and macro(s)
#-----------------------------------------------------------------------------------------------------------------------
set(OCTK_CMAKE_DIR "${PROJECT_SOURCE_DIR}/cmake")
set(CMAKE_MODULE_PATH ${OCTK_CMAKE_DIR} ${CMAKE_MODULE_PATH})
include(OCTKPrecompiledHeadersHelpers)
include(OCTKScopeFinalizerHelpers)
include(OCTKPublicWalkLibsHelpers)
include(OCTKPublicResourceHelpers)
include(OCTKPublicTargetHelpers)
include(OCTKFlagHandlingHelpers)
include(OCTKSubdirectoryHelpers)
include(OCTKCMakeVersionHelpers)
include(OCTKGlobalStateHelpers)
include(OCTKSyncIncludeHelpers)
include(OCTKFindPackageHelpers)
include(OCTKPrecompiledHelpers)
include(OCTKExecutableHelpers)
include(OCTKSeparateDebugInfo)
include(OCTKConfigureHelpers)
include(OCTKFrameworkHelpers)
include(OCTKPkgConfigHelpers)
include(OCTKCompilerHelpers)
include(OCTKLibraryHelpers)
include(OCTKInstallHelpers)
include(OCTKModuleHelpers)
include(OCTKTargetHelpers)
include(OCTKOptionHelpers)
include(OCTKCMakeHelpers)
include(OCTKRpathHelpers)
include(OCTKTestHelpers)


#-----------------------------------------------------------------------------------------------------------------------
# Set build options
#-----------------------------------------------------------------------------------------------------------------------
octk_option(OCTK_BUILD_ALL "Enable this to build all artifacts" OFF)
octk_option(OCTK_BUILD_SHARED_LIBS "Enable this to build as dynamically" ON
	SET BUILD_SHARED_LIBS)
octk_option(OCTK_BUILD_USE_PCH "Enable this to build use precompiled header files for compilation" ON
	DEPENDS BUILD_SHARED_LIBS)
octk_option(OCTK_BUILD_COMPILER_WARNING "Enable this to build target with compiler warnings" OFF)
octk_option(OCTK_BUILD_WARNINGS_ARE_ERRORS "Enable this to build with warnings as errors"
	${OCTK_DEFAULT_BUILD_WARNINGS_ARE_ERRORS})
octk_option(OCTK_BUILD_BENCHMARKS "Enable this to build the benchmarks" ${OCTK_DEFAULT_BUILD_BENCHMARKS}
	SET OCTK_DEFAULT_BUILD_TESTS)
octk_option(OCTK_BUILD_DOCS "Enable this to build the documentation" OFF)
octk_option(OCTK_BUILD_APPS "Enable this to build apps" ON
	OR_CONDITION OCTK_BUILD_ALL)
octk_option(OCTK_BUILD_TESTS "Enable this to build tests" ${OCTK_DEFAULT_BUILD_TESTS}
	OR_CONDITION OCTK_BUILD_ALL)
octk_option(OCTK_BUILD_EXAMPLES "Enable this to build examples" ON
	OR_CONDITION OCTK_BUILD_ALL)
octk_option(OCTK_BUILD_INSTALL "Enable this to build the installer" ${OCTK_WILL_INSTALL_VALUE}
	OR_CONDITION OCTK_BUILD_ALL)

octk_option(OCTK_ENABLE_LIB_NETWORK "Enable this to build enable network lib" OFF
	OR_CONDITION OCTK_BUILD_ALL)
octk_option(OCTK_ENABLE_LIB_MEDIA "Enable this to build enable media lib" OFF
	DEPENDS OCTK_ENABLE_LIB_NETWORK
	OR_CONDITION OCTK_BUILD_ALL)
octk_option(OCTK_ENABLE_LIB_GUI "Enable this to build enable gui lib" OFF
	OR_CONDITION OCTK_BUILD_ALL)


#-----------------------------------------------------------------------------------------------------------------------
# Set doxygen build config
#-----------------------------------------------------------------------------------------------------------------------
if(OCTK_BUILD_DOCS)
	message(STATUS "Project build documentation")
	include(InstallDoxygen)
endif()


#-----------------------------------------------------------------------------------------------------------------------
# Set install and paths
#-----------------------------------------------------------------------------------------------------------------------
# Install locations:
octk_configure_process_path(OCTK_INSTALL_LIBDIR "lib" "Libraries [PREFIX/lib]")
octk_configure_process_path(OCTK_INSTALL_BINDIR "bin" "Executables [PREFIX/bin]")
octk_configure_process_path(OCTK_INSTALL_ARCHDATADIR "." "Arch-dependent data [PREFIX]")
octk_configure_process_path(OCTK_INSTALL_INCLUDEDIR "include" "Header files [PREFIX/include]")

if(WIN32)
	set(OCTK_DEFAULT_DLLDIR "bin")
	set(OCTK_DEFAULT_TESTSDIR "bin")
	set(OCTK_DEFAULT_EXAMPLESDIR "bin")
	set(OCTK_DEFAULT_LIBEXEC "${OCTK_INSTALL_ARCHDATADIR}/bin")
else()
	set(OCTK_DEFAULT_DLLDIR "lib")
	set(OCTK_DEFAULT_TESTSDIR "tests")
	set(OCTK_DEFAULT_EXAMPLESDIR "examples")
	set(OCTK_DEFAULT_LIBEXEC "${OCTK_INSTALL_ARCHDATADIR}/libexec")
endif()

octk_configure_process_path(OCTK_INSTALL_DLLDIR
	"${OCTK_DEFAULT_DLLDIR}" "[PREFIX/bin] on Windows, [PREFIX/lib] otherwise")
octk_configure_process_path(OCTK_INSTALL_TESTSDIR
	"${OCTK_DEFAULT_TESTSDIR}" "[PREFIX/bin] on Windows, [PREFIX/tests] otherwise")
octk_configure_process_path(OCTK_INSTALL_EXAMPLESDIR
	"${OCTK_DEFAULT_EXAMPLESDIR}" "[PREFIX/bin] on Windows, [PREFIX/examples] otherwise")
octk_configure_process_path(OCTK_INSTALL_LIBEXECDIR
	"${OCTK_DEFAULT_LIBEXEC}" "Helper programs [ARCHDATADIR/bin on Windows, ARCHDATADIR/libexec otherwise]")
octk_configure_process_path(OCTK_INSTALL_DATADIR "." "Arch-independent data [PREFIX]")
octk_configure_process_path(OCTK_INSTALL_DOCDIR "${OCTK_INSTALL_DATADIR}/doc" "Documentation [DATADIR/doc]")

if(APPLE)
	set(OCTK_DEFAULT_SYS_CONF_DIR "/Library/Preferences/OpenCTK")
else()
	set(OCTK_DEFAULT_SYS_CONF_DIR "etc/xdg")
endif()
octk_configure_process_path(OCTK_INSTALL_SYSCONFDIR
	"${OCTK_DEFAULT_SYS_CONF_DIR}" "Settings used by OpenCTK programs [PREFIX/etc/xdg]/[/Library/Preferences/OpenCTK]")
octk_configure_process_path(OCTK_INSTALL_DESCRIPTIONSDIR
	"${OCTK_INSTALL_DATADIR}/modules" "Module description files directory")

if(NOT "${CMAKE_STAGING_PREFIX}" STREQUAL "")
	set(OCTK_STAGING_PREFIX "${CMAKE_STAGING_PREFIX}")
else()
	set(OCTK_STAGING_PREFIX "${CMAKE_INSTALL_PREFIX}")
endif()

function(octk_internal_set_up_global_paths)
	# Compute the values of OCTK_BUILD_DIR, OCTK_INSTALL_DIR, OCTK_CONFIG_BUILD_DIR, OCTK_CONFIG_INSTALL_DIR
	# taking into account whether the current build is a prefix build or a non-prefix build,
	# and whether it is a superbuild or non-superbuild.
	# A third case is when another module or standalone tests are built against a super-built OpenCTK.
	# The layout for the third case is the same as for non-superbuilds.
	#
	# These values should be prepended to file paths in commands or properties,
	# in order to correctly place generated Config files, generated Targets files,
	# executables / libraries, when copying / installing files, etc.
	#
	# The build dir variables will always be absolute paths.
	# The OCTK_INSTALL_DIR variable will have a relative path in a prefix build,
	# which means that it can be empty, so use octk_join_path to prevent accidental absolute paths.
	if(OCTK_SUPERBUILD)
		# In this case, we always copy all the build products in OCTK_BUILD_DIR/{bin,lib,...}
		if(OCTK_BUILD_INSTALL)
			set(OCTK_BUILD_DIR "${OCTK_BINARY_DIR}")
			set(OCTK_INSTALL_DIR "")
		else()
			if("${CMAKE_STAGING_PREFIX}" STREQUAL "")
				set(OCTK_BUILD_DIR "${OCTK_BINARY_DIR}")
				set(OCTK_INSTALL_DIR "${OCTK_BINARY_DIR}")
			else()
				set(OCTK_BUILD_DIR "${CMAKE_STAGING_PREFIX}")
				set(OCTK_INSTALL_DIR "${CMAKE_STAGING_PREFIX}")
			endif()
		endif()
	else()
		if(OCTK_BUILD_INSTALL)
			# In the usual prefix build case, the build dir is the current module build dir,
			# and the install dir is the prefix, so we don't set it.
			set(OCTK_BUILD_DIR "${CMAKE_BINARY_DIR}")
			set(OCTK_INSTALL_DIR "")
		else()
			# When doing a non-prefix build, both the build dir and install dir are the same, pointing to the
			# OpenCTK build dir.
			set(OCTK_BUILD_DIR "${OCTK_STAGING_PREFIX}")
			set(OCTK_INSTALL_DIR "${OCTK_BUILD_DIR}")
		endif()
	endif()

	set(__config_path_part "${OCTK_INSTALL_LIBDIR}/cmake")
	set(OCTK_CONFIG_BUILD_DIR "${OCTK_BUILD_DIR}/${__config_path_part}")
	set(OCTK_CONFIG_INSTALL_DIR "${OCTK_INSTALL_DIR}")
	if(OCTK_CONFIG_INSTALL_DIR)
		string(APPEND OCTK_CONFIG_INSTALL_DIR "/")
	endif()
	string(APPEND OCTK_CONFIG_INSTALL_DIR ${__config_path_part})

	set(OCTK_BUILD_DIR "${OCTK_BUILD_DIR}" PARENT_SCOPE)
	set(OCTK_INSTALL_DIR "${OCTK_INSTALL_DIR}" PARENT_SCOPE)
	set(OCTK_CONFIG_BUILD_DIR "${OCTK_CONFIG_BUILD_DIR}" PARENT_SCOPE)
	set(OCTK_CONFIG_INSTALL_DIR "${OCTK_CONFIG_INSTALL_DIR}" PARENT_SCOPE)
	message(STATUS "OCTK_BUILD_DIR was set to: '${OCTK_BUILD_DIR}'")
endfunction()
octk_internal_set_up_global_paths()

# the default RPATH to be used when installing, but only if it's not a system directory
list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/${OCTK_INSTALL_LIBDIR}" isSystemDir)
if("${isSystemDir}" STREQUAL "-1")
	set(_default_install_rpath "${CMAKE_INSTALL_PREFIX}/${OCTK_INSTALL_LIBDIR}")
endif("${isSystemDir}" STREQUAL "-1")

# The default rpath settings for installed targets is empty.
# The rpaths will instead be computed for each target separately using octk_apply_rpaths().
# Additional rpaths can be passed via OCTK_EXTRA_RPATHS.
# By default this will include $ORIGIN / @loader_path, so the installation is relocatable.
# Bottom line: No need to pass anything to CMAKE_INSTALL_RPATH.
set(CMAKE_INSTALL_RPATH "" CACHE STRING "RPATH for installed binaries")

# By default, don't embed auto-determined RPATHs pointing to directories
# outside of the build tree, into the installed binaries.
# This ended up adding rpaths like ${CMAKE_INSTALL_PREFIX}/lib (or /Users/octk/work/install/lib into
# the official libraries created by the CI) into the non-octkbase libraries, plugins, etc.
#
# It should not be necessary, given that octk_apply_rpaths() already adds the necessary rpaths, either
# relocatable ones or absolute ones, depending on what the platform supports.
if(NOT OCTK_NO_DISABLE_CMAKE_INSTALL_RPATH_USE_LINK_PATH)
	set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)
endif()

# Ensure that GNUInstallDirs's CMAKE_INSTALL_LIBDIR points to the same lib dir that OpenCTK was configured with.
set(CMAKE_INSTALL_LIBDIR "${OCTK_INSTALL_LIBDIR}")

function(octk_setup_tool_path_command)
	if(NOT CMAKE_HOST_WIN32)
		return()
	endif()
	set(bindir "${OCTK_BUILD_INTERNALS_RELOCATABLE_INSTALL_PREFIX}/${OCTK_INSTALL_BINDIR}")
	file(TO_NATIVE_PATH "${bindir}" bindir)
	list(APPEND command COMMAND)
	list(APPEND command set PATH=${bindir}$<SEMICOLON>%PATH%)
	set(OCTK_TOOL_PATH_SETUP_COMMAND "${command}" CACHE INTERNAL "internal command prefix for tool invocations" FORCE)
	# OCTK_TOOL_PATH_SETUP_COMMAND is deprecated. Please use _octk_internal_get_wrap_tool_script_path instead.
endfunction()
octk_setup_tool_path_command()

file(MAKE_DIRECTORY "${OCTK_BUILD_DIR}/${OCTK_INSTALL_LIBDIR}")
file(MAKE_DIRECTORY "${OCTK_BUILD_DIR}/${OCTK_INSTALL_BINDIR}")
file(MAKE_DIRECTORY "${OCTK_BUILD_DIR}/${OCTK_INSTALL_DLLDIR}")
file(MAKE_DIRECTORY "${OCTK_BUILD_DIR}/${OCTK_INSTALL_LIBEXECDIR}")

if(CMAKE_HOST_WIN32)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${OCTK_BUILD_DIR}/${OCTK_INSTALL_BINDIR}")
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${OCTK_BUILD_DIR}/${OCTK_INSTALL_LIBDIR}")
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${OCTK_BUILD_DIR}/${OCTK_INSTALL_LIBDIR}")
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OCTK_BUILD_DIR}/${OCTK_INSTALL_BINDIR}")
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${OCTK_BUILD_DIR}/${OCTK_INSTALL_LIBDIR}")
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${OCTK_BUILD_DIR}/${OCTK_INSTALL_LIBDIR}")
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${OCTK_BUILD_DIR}/${OCTK_INSTALL_BINDIR}")
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${OCTK_BUILD_DIR}/${OCTK_INSTALL_LIBDIR}")
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "${OCTK_BUILD_DIR}/${OCTK_INSTALL_LIBDIR}")
endif()


#-----------------------------------------------------------------------------------------------------------------------
# Set Platform and InternalTargets define
#-----------------------------------------------------------------------------------------------------------------------
include(OCTKPlatformSupport)
include(OCTKInternalTargets)


#-----------------------------------------------------------------------------------------------------------------------
# Set namespace and separator
#-----------------------------------------------------------------------------------------------------------------------
set(OCTK_NAMESPACE_VERSION ${OCTK_VERSION_MAJOR})
set(OCTK_NAMESPACE "octk" CACHE STRING "OpenCTK Namespace")
if(NOT OCTK_CMAKE_INSTALL_NAMESPACE)
	set(OCTK_CMAKE_INSTALL_NAMESPACE "octk${OCTK_NAMESPACE_VERSION}"
		CACHE STRING "CMake namespace [octk${OCTK_NAMESPACE_VERSION}]")
endif()
if(NOT OCTK_CMAKE_EXPORT_NAMESPACE)
	set(OCTK_CMAKE_EXPORT_NAMESPACE "octk${OCTK_NAMESPACE_VERSION}"
		CACHE STRING "CMake namespace used when exporting targets [octk${OCTK_NAMESPACE_VERSION}]")
endif()

set(OCTK_KNOWN_LIBRARIES_WITH_TOOLS "" CACHE INTERNAL "Known OpenCTK modules with tools" FORCE)

# For adjusting variables when running tests, we need to know what
# the correct variable is for separating entries in PATH-alike
# variables.
if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
	set(OCTK_PATH_SEPARATOR "\\;")
else()
	set(OCTK_PATH_SEPARATOR ":")
endif()

# Save the value of the current first project source dir.
# This will be /path/to/OCTKCore for OCTKCore both in a super-build and a non super-build.
# This will be /path/to/OCTKCore/tests when building standalone tests.
set(OCTK_TOP_LEVEL_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(OCTK_INSTALL_COMPONENT_NAME "${OCTK_CMAKE_INSTALL_NAMESPACE}sdk")


#-----------------------------------------------------------------------------------------------------------------------
# Prevent warnings about object files without any symbols. This is a common thing in OpenCTK as we tend to build files
# unconditionally, and then use ifdefs to compile out parts that are not relevant.
#-----------------------------------------------------------------------------------------------------------------------
if(CMAKE_HOST_APPLE AND APPLE)
	foreach(lang ASM C CXX)
		# We have to tell 'ar' to not run ranlib by itself, by passing the 'S' option
		set(CMAKE_${lang}_ARCHIVE_CREATE "<CMAKE_AR> qcS <TARGET> <LINK_FLAGS> <OBJECTS>")
		set(CMAKE_${lang}_ARCHIVE_APPEND "<CMAKE_AR> qS <TARGET> <LINK_FLAGS> <OBJECTS>")
		set(CMAKE_${lang}_ARCHIVE_FINISH "<CMAKE_RANLIB> -no_warning_for_no_symbols <TARGET>")
	endforeach()
endif()


#-----------------------------------------------------------------------------------------------------------------------
# Output static runtime info
#-----------------------------------------------------------------------------------------------------------------------
if(OCTK_MSVC_STATIC_RUNTIME)
	message(STATUS "Build as static runtime")
endif()


#-----------------------------------------------------------------------------------------------------------------------
# build directory check
#-----------------------------------------------------------------------------------------------------------------------
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run "
		"CMake from there. You may need to remove CMakeCache.txt.")
endif()


#-----------------------------------------------------------------------------------------------------------------------
# top project check
#-----------------------------------------------------------------------------------------------------------------------
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
	set(OCTK_PROJECT_IS_TOP_LEVEL ON)
else()
	set(OCTK_PROJECT_IS_TOP_LEVEL OFF)
endif()


#-----------------------------------------------------------------------------------------------------------------------
# Set custom build target
#-----------------------------------------------------------------------------------------------------------------------
if(OCTK_PROJECT_IS_TOP_LEVEL)
	add_custom_target(BuildAll
		COMMAND ${CMAKE_COMMAND} --build ./ --parallel ${OCTK_NUMBER_OF_ASYNC_JOBS} --config ${CMAKE_BUILD_TYPE}
		WORKING_DIRECTORY "${OCTK_BUILD_DIR}")
	add_custom_target(BuildInstall
		COMMAND ${CMAKE_COMMAND} --build ./ --parallel ${OCTK_NUMBER_OF_ASYNC_JOBS} --config ${CMAKE_BUILD_TYPE}
		COMMAND ${CMAKE_COMMAND} --install ./
		WORKING_DIRECTORY "${OCTK_BUILD_DIR}")
endif()


#-----------------------------------------------------------------------------------------------------------------------
# Set default param args
#-----------------------------------------------------------------------------------------------------------------------
set(OCTK_DEFAULT_PRIVATE_ARGS
	DEFINES
	SOURCES
	LIBRARIES
	INCLUDE_DIRECTORIES
	FEATURE_DEPENDENCIES
	COMPILE_OPTIONS
	LINK_OPTIONS
	DISABLE_AUTOGEN_TOOLS
	ENABLE_AUTOGEN_TOOLS
	PLUGIN_TYPES)
set(OCTK_DEFAULT_PUBLIC_ARGS
	PUBLIC_DEFINES
	PUBLIC_LIBRARIES
	PUBLIC_INCLUDE_DIRECTORIES
	PUBLIC_COMPILE_OPTIONS
	PUBLIC_LINK_OPTIONS)
set(OCTK_DEFAULT_PRIVATE_LIBRARY_ARGS
	PRIVATE_LIBRARY_INTERFACE)
set(OCTK_DEFAULT_TARGET_INFO_ARGS
	TARGET_VERSION
	TARGET_PRODUCT
	TARGET_DESCRIPTION
	TARGET_COMPANY
	TARGET_COPYRIGHT)

# Collection of arguments so they can be shared across octk_internal_add_executable and octk_internal_add_test_helper.
set(OCTK_INTERNAL_ADD_EXECUTABLE_OPTIONAL_ARGS
	GUI
	INSTALL
	EXCEPTIONS
	DELAY_RC
	DELAY_TARGET_INFO
	OCTK_APP)
set(OCTK_INTERNAL_ADD_EXECUTABLE_SINGLE_ARGS
	CORE_LIBRARY
	OUTPUT_SUBDIR
	OUTPUT_DIRECTORY
	INSTALL_DIRECTORY
	VERSION
	${OCTK_DEFAULT_TARGET_INFO_ARGS})
set(OCTK_INTERNAL_ADD_EXECUTABLE_MULTI_ARGS
	${OCTK_DEFAULT_PRIVATE_ARGS}
	${OCTK_DEFAULT_PUBLIC_ARGS})


#-----------------------------------------------------------------------------------------------------------------------
# add 3rdparty subdirectory
#-----------------------------------------------------------------------------------------------------------------------
include(InstallPython)
include(InstallPkgConfig)
include(InstallRequiredSystemLibraries)

if(OCTK_BUILD_DOCS)
	include(InstallDoxygen)
endif()
if(OCTK_BUILD_TESTS)
	include(CTest)
	enable_testing()
	octk_find_package(WrapGTest PROVIDED_TARGETS OCTK3rdparty::WrapGTest)
	octk_find_package(WrapBenchmark PROVIDED_TARGETS OCTK3rdparty::WrapBenchmark)
endif()


#-----------------------------------------------------------------------------------------------------------------------
# add subdirectory
#-----------------------------------------------------------------------------------------------------------------------
add_subdirectory(src/tools)
add_subdirectory(src/libs)
