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

# Wraps install() command. In a prefix build, simply passes along arguments to install().
# In a non-prefix build, handles association of targets to export names, and also calls export().
function(octk_install)
	set(flags)
	set(options EXPORT DESTINATION NAMESPACE)
	set(multiopts TARGETS)
	cmake_parse_arguments(arg "${flags}" "${options}" "${multiopts}" ${ARGN})

	if(arg_TARGETS)
		set(is_install_targets TRUE)
		foreach(target ${arg_TARGETS})
			if(TARGET ${target})
				get_target_property(target_base_name ${target} _octk_target_base_name)
				if(target_base_name)
					set_target_properties(${target} PROPERTIES EXPORT_NAME "${target_base_name}")
				endif()
			endif()
		endforeach()
	endif()

	if(NOT OCTK_BUILD_INSTALL)
		return()
	endif()

	# In a prefix build, always invoke install() without modification.
	# In a non-prefix build, pass install(TARGETS) commands to allow association of targets to export names,
	# so we can later use the export names in export() commands.
	if(OCTK_BUILD_INSTALL OR is_install_targets)
		install(${ARGV})
	endif()

	# When install(EXPORT) is called, also call export(EXPORT) to generate build tree target files.
	if(NOT is_install_targets AND arg_EXPORT)
		# For prefixed builds (both top-level and per-repo) export build tree CMake Targets files so they can be used in
		# CMake ExternalProjects. One such case is examples built as ExternalProjects as part of the OpenCTK build.
		# In a top-level build the exported config files are placed under octk_core/lib/cmake.
		# In a per-repo build, they will be placed in each repo's build dir/lib/cmake.
		if(OCTK_BUILD_INSTALL)
			octk_path_join(arg_DESTINATION "${OCTK_BUILD_DIR}" "${arg_DESTINATION}")
		endif()

		set(namespace_option "")
		if(arg_NAMESPACE)
			set(namespace_option NAMESPACE ${arg_NAMESPACE})
		endif()
		export(EXPORT ${arg_EXPORT} ${namespace_option} FILE "${arg_DESTINATION}/${arg_EXPORT}.cmake")
	endif()
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
#-----------------------------------------------------------------------------------------------------------------------
function(octk_configure_process_path name default docstring)
	# No value provided, set the default.
	if(NOT DEFINED "${name}")
		set("${name}" "${default}" CACHE STRING "${docstring}")
	else()
		get_filename_component(given_path_as_abs "${${name}}" ABSOLUTE BASE_DIR "${CMAKE_INSTALL_PREFIX}")
		file(RELATIVE_PATH rel_path "${CMAKE_INSTALL_PREFIX}" "${given_path_as_abs}")

		# If absolute path given, check that it's inside the prefix (error out if not).
		# TODO: Figure out if we need to support paths that are outside the prefix.
		#
		# If relative path given, it's relative to the install prefix (rather than the binary dir,
		# which is what qmake does for some reason).
		# In both cases, store the value as a relative path.
		if("${rel_path}" STREQUAL "")
			# file(RELATIVE_PATH) returns an empty string if the given absolute paths are equal
			set(rel_path ".")
		elseif(rel_path MATCHES "^\.\./")
			# OCTK_INSTALL_SYSCONFDIR is allowed to be outside the prefix.
			if(NOT name STREQUAL "OCTK_INSTALL_SYSCONFDIR")
				message(FATAL_ERROR "Path component '${name}' is outside computed install prefix: ${rel_path} ")
				return()
			endif()
			set("${name}" "${${name}}" CACHE STRING "${docstring}" FORCE)
		else()
			set("${name}" "${rel_path}" CACHE STRING "${docstring}" FORCE)
		endif()
	endif()
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
# Given CMAKE_CONFIG and ALL_CMAKE_CONFIGS, determines if a directory suffix needs to be appended
# to each destination, and sets the computed install target destination arguments in OUT_VAR.
# Defaults used for each of the destination types, and can be configured per destination type.
#-----------------------------------------------------------------------------------------------------------------------
function(octk_get_install_target_default_args)
	octk_parse_all_arguments(arg "octk_get_install_target_default_args"
		"" "OUT_VAR;CMAKE_CONFIG;RUNTIME;LIBRARY;ARCHIVE;INCLUDES;BUNDLE"
		"ALL_CMAKE_CONFIGS" ${ARGN})

	if(NOT arg_CMAKE_CONFIG)
		message(FATAL_ERROR "No value given for CMAKE_CONFIG.")
	endif()
	if(NOT arg_ALL_CMAKE_CONFIGS)
		message(FATAL_ERROR "No value given for ALL_CMAKE_CONFIGS.")
	endif()
	list(LENGTH arg_ALL_CMAKE_CONFIGS all_configs_count)
	list(GET arg_ALL_CMAKE_CONFIGS 0 first_config)

	set(suffix "")
	if(all_configs_count GREATER 1 AND NOT arg_CMAKE_CONFIG STREQUAL first_config)
		set(suffix "/${arg_CMAKE_CONFIG}")
	endif()

	set(runtime "${OCTK_INSTALL_BINDIR}")
	if(arg_RUNTIME)
		set(runtime "${arg_RUNTIME}")
	endif()

	set(library "${OCTK_INSTALL_LIBDIR}")
	if(arg_LIBRARY)
		set(library "${arg_LIBRARY}")
	endif()

	set(archive "${OCTK_INSTALL_LIBDIR}")
	if(arg_ARCHIVE)
		set(archive "${arg_ARCHIVE}")
	endif()

	set(includes "${OCTK_INSTALL_INCLUDEDIR}")
	if(arg_INCLUDES)
		set(includes "${arg_INCLUDES}")
	endif()

	set(bundle "${OCTK_INSTALL_BINDIR}")
	if(arg_BUNDLE)
		set(bundle "${arg_BUNDLE}")
	endif()

	set(args
		RUNTIME DESTINATION "${runtime}${suffix}"
		LIBRARY DESTINATION "${library}${suffix}"
		ARCHIVE DESTINATION "${archive}${suffix}" COMPONENT Devel
		BUNDLE DESTINATION "${bundle}${suffix}"
		INCLUDES DESTINATION "${includes}${suffix}")
	set(${arg_OUT_VAR} "${args}" PARENT_SCOPE)
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
# Create a versioned hard-link for the given target.
# E.g. "bin/qmake6" -> "bin/qmake".
# If no hard link can be created, make a copy instead.
#
# In a multi-config build, create the link for the main config only.
#-----------------------------------------------------------------------------------------------------------------------
function(octk_internal_install_versioned_link install_dir target)
	if(NOT OCTK_BUILD_INSTALL)
		return()
	endif()

	if(NOT OCTK_CREATE_VERSIONED_HARD_LINK)
		return()
	endif()

	octk_path_join(install_base_file_path "$\{octk_full_install_prefix}"
		"${install_dir}" "$<TARGET_FILE_BASE_NAME:${target}>")
	set(original "${install_base_file_path}$<TARGET_FILE_SUFFIX:${target}>")
	set(linkname "${install_base_file_path}${PROJECT_VERSION_MAJOR}$<TARGET_FILE_SUFFIX:${target}>")
	set(code "set(octk_full_install_prefix \"$\{CMAKE_INSTALL_PREFIX}\")"
		"  if(NOT \"$ENV\{DESTDIR}\" STREQUAL \"\")")
	if(CMAKE_HOST_WIN32)
		list(APPEND code
			"    if(octk_full_install_prefix MATCHES \"^[a-zA-Z]:\")"
			"        string(SUBSTRING \"$\{octk_full_install_prefix}\" 2 -1 octk_full_install_prefix)"
			"    endif()")
	endif()
	list(APPEND code
		"    string(PREPEND octk_full_install_prefix \"$ENV\{DESTDIR}\")"
		"  endif()"
		"  message(STATUS \"Creating hard link ${original} -> ${linkname}\")"
		"  file(CREATE_LINK \"${original}\" \"${linkname}\" COPY_ON_ERROR)")

	if(OCTK_GENERATOR_IS_MULTI_CONFIG)
		# Wrap the code in a configuration check,
		# because install(CODE) does not support a CONFIGURATIONS argument.
		octk_create_case_insensitive_regex(main_config_regex ${OCTK_MULTI_CONFIG_FIRST_CONFIG})
		list(PREPEND code "if(\"\${CMAKE_INSTALL_CONFIG_NAME}\" MATCHES \"${main_config_regex}\")")
		list(APPEND code "endif()")
	endif()

	list(JOIN code "\n" code)
	install(CODE "${code}")
endfunction()
