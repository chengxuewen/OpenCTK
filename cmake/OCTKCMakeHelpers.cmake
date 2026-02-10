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

#-----------------------------------------------------------------------------------------------------------------------
# - If NAME of variable not defined, then define this variable
# This module defines
#  NAME - name of target variable
#  VALUE - variable of target variable
#
# Apple readline does not support readline hooks
# So we look for another one by default
#-----------------------------------------------------------------------------------------------------------------------
macro(octk_ensure_define_variable NAME VALUE)
    if(NOT DEFINED ${NAME})
        set(${NAME} ${VALUE})
    endif()
endmacro()

macro(octk_set_input_variable NAME)
    cmake_parse_arguments(arg "CACHE;FORCE" "DEFAULT" "" ${ARGN})
    if(NOT INPUT_${NAME})
        if(NOT arg_DEFAULT)
            set(${NAME})
        else()
            set(${NAME} ${arg_DEFAULT})
        endif()
    else()
        if(NOT ${NAME})
            message(STATUS "${NAME} set as: ${INPUT_${NAME}}")
        endif()
        set(${NAME} ${INPUT_${NAME}})
    endif()
    if(arg_CACHE OR arg_FORCE)
        set(${NAME} "${${NAME}}" CACHE INTERNAL "" FORCE)
    endif()
endmacro()

#-----------------------------------------------------------------------------------------------------------------------
# Normalize the feature name to something CMake can deal with.
# Usage:
#   octk_normalize_name(<name> <output variable>)
# Examples:
#   octk_normalize_name("\\" out_var) # out_var = _
#   octk_normalize_name("\\" out_var) # out_var = _
#   octk_normalize_name("," out_var) # out_var = _
#   octk_normalize_name("=" out_var) # out_var = _
#   octk_normalize_name("hello world!" out_var) # out_var = hello_world_
#-----------------------------------------------------------------------------------------------------------------------
function(octk_normalize_name name out_var)
    if(name MATCHES "c\\+\\+")
        string(REGEX REPLACE "[^a-zA-Z0-9_]" "x" name "${name}")
    else()
        string(REGEX REPLACE "[^a-zA-Z0-9_]" "_" name "${name}")
    endif()

    set(${out_var} "${name}" PARENT_SCOPE)
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
# Evaluate expression_var to boolean value
# Usage:
#   octk_evaluate_to_boolean(<expression_var>)
# Examples:
#   set(test_val "on")
#   octk_evaluate_to_boolean(test_var) # test_var = ON
#   set(test_val "y")
#   octk_evaluate_to_boolean(test_var) # test_var = ON
#   set(test_val "no")
#   octk_evaluate_to_boolean(test_var) # test_var = OFF
#-----------------------------------------------------------------------------------------------------------------------
function(octk_evaluate_to_boolean expression_var)
    if(${${expression_var}})
        set(${expression_var} ON PARENT_SCOPE)
    else()
        set(${expression_var} OFF PARENT_SCOPE)
    endif()
endfunction()


# Evaluate expression result value.
# Usage:
#   octk_evaluate_expression(<result_var> [expression ...NOT/AND/STREQUAL/variable ])
# Examples:
#   set(VAL1 "")
#   octk_evaluate_expression(test_var VAL1) # test_var = OFF
#   set(VAL2 ON)
#   octk_evaluate_expression(test_var VAL1 AND VAL2) # test_var = OFF
#   octk_evaluate_expression(test_var VAL1 OR VAL2) # test_var = ON
function(octk_evaluate_expression result_var)
    set(result "")
    set(nesting_level 0)
    set(skip_next OFF)
    set(expression "${ARGN}")
    list(LENGTH expression length)

    if(0 EQUAL length)
        message(FATAL_ERROR "expression cannot be empty!")
    elseif(1 EQUAL length)
        set(result ${expression})
    else()
        math(EXPR last_index "${length} - 1")
        foreach(member_index RANGE ${last_index})
            if(${skip_next})
                set(skip_next OFF)
                continue()
            endif()

            list(GET expression ${member_index} member)
            if("${member}" STREQUAL "(")
                if(${nesting_level} GREATER 0)
                    list(APPEND result ${member})
                endif()
                math(EXPR nesting_level "${nesting_level} + 1")
                continue()
            elseif("${member}" STREQUAL ")")
                math(EXPR nesting_level "${nesting_level} - 1")
                if(nesting_level LESS 0)
                    break()
                endif()
                if(${nesting_level} EQUAL 0)
                    octk_evaluate_expression(result ${result})
                else()
                    list(APPEND result ${member})
                endif()
                continue()
            elseif(${nesting_level} GREATER 0)
                list(APPEND result ${member})
                continue()
            elseif("${member}" STREQUAL "NOT")
                list(APPEND result ${member})
                continue()
            elseif("${member}" STREQUAL "EXISTS")
                math(EXPR next_index "${member_index} + 1")
                list(GET expression ${next_index} next_member)
                list(APPEND result EXISTS "${next_member}")
                set(skip_next ON)
                continue()
            elseif("${member}" STREQUAL "TARGET")
                math(EXPR next_index "${member_index} + 1")
                list(GET expression ${next_index} next_member)
                list(APPEND result TARGET "${next_member}")
                set(skip_next ON)
                continue()
            elseif("${member}" STREQUAL "AND")
                octk_evaluate_to_boolean(result)
                if(NOT ${result})
                    break()
                endif()
                set(result "")
            elseif("${member}" STREQUAL "OR")
                octk_evaluate_to_boolean(result)
                if(${result})
                    break()
                endif()
                set(result "")
            elseif("${member}" STREQUAL "STREQUAL" AND member_index LESS ${length})
                # Unfortunately the semantics for STREQUAL in if() are broken when the
                # RHS is an empty string and the parameters to if are coming through a variable.
                # So we expect people to write the empty string with single quotes and then we
                # do the comparison manually here.
                list(LENGTH result lhsIndex)
                math(EXPR lhsIndex "${lhsIndex}-1")
                list(GET result ${lhsIndex} lhs)
                list(REMOVE_AT result ${lhsIndex})
                set(lhs "${${lhs}}")

                math(EXPR rhsIndex "${member_index}+1")
                set(skip_next ON)

                list(GET expression ${rhsIndex} rhs)
                # We can't pass through an empty string with double quotes through various
                # stages of substitution, so instead it is represented using single quotes
                # and resolve here.
                string(REGEX REPLACE "'(.*)'" "\\1" rhs "${rhs}")

                string(COMPARE EQUAL "${lhs}" "${rhs}" string_compare_result)
                list(APPEND result ${string_compare_result})
            else()
                if(DEFINED "${member}")
                    list(APPEND result ${member})
                endif()
            endif()
        endforeach()
    endif()


    if("${result}" STREQUAL "")
        set(result ON)
    else()
        octk_evaluate_to_boolean(result)
    endif()

    set(${result_var} ${result} PARENT_SCOPE)
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
# octk_configure_file(OUTPUT output-file <INPUT input-file | CONTENT content>)
# input-file is relative to ${CMAKE_CURRENT_SOURCE_DIR}
# output-file is relative to ${CMAKE_CURRENT_BINARY_DIR}
#
# This function is similar to file(GENERATE OUTPUT) except it writes the content to the file at configure time, rather
# than at generate time. Once CMake 3.18 is released, it can use file(CONFIGURE) in its implmenetation. Until then, it
# uses configure_file() with a generic input file as source, when used with the CONTENT signature.
#-----------------------------------------------------------------------------------------------------------------------
function(octk_configure_file)
    octk_parse_all_arguments(arg "octk_configure_file" "" "OUTPUT;INPUT;CONTENT" "" ${ARGN})

    if(NOT arg_OUTPUT)
        message(FATAL_ERROR "No output file provided to octk_configure_file.")
    endif()

    if(arg_CONTENT)
        set(template_name "OCTKFileConfigure.txt.in")
        set(input_file "${PROJECT_SOURCE_DIR}/cmake/${template_name}")
        set(__octk_file_configure_content "${arg_CONTENT}")
    elseif(arg_INPUT)
        set(input_file "${arg_INPUT}")
    else()
        message(FATAL_ERROR "No input value provided to octk_configure_file.")
    endif()

    configure_file("${input_file}" "${arg_OUTPUT}" @ONLY)
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
# A version of cmake_parse_arguments that makes sure all arguments are processed and errors out
# with a message about ${type} having received unknown arguments.
#
# octk_parse_all_arguments(arg "test_func" "" "OUTPUT;INPUT;CONTENT" "" ${ARGN})
#-----------------------------------------------------------------------------------------------------------------------
macro(octk_parse_all_arguments result type options oneValueArgs multiValueArgs)
    cmake_parse_arguments(${result} "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(DEFINED ${result}_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Unknown arguments were passed to ${type} (${${result}_UNPARSED_ARGUMENTS}).")
    endif()
endmacro()


#-----------------------------------------------------------------------------------------------------------------------
# Print all variables defined in the current scope.
#-----------------------------------------------------------------------------------------------------------------------
macro(octk_debug_print_variables)
    cmake_parse_arguments(__arg "DEDUP" "" "MATCH;IGNORE" ${ARGN})
    message("Known Variables:")
    get_cmake_property(__variableNames VARIABLES)
    list(SORT __variableNames)

    if(__arg_DEDUP)
        list(REMOVE_DUPLICATES __variableNames)
    endif()

    foreach(__var ${__variableNames})
        set(__ignore OFF)

        foreach(__i ${__arg_IGNORE})
            if(__var MATCHES "${__i}")
                set(__ignore ON)
                break()
            endif()
        endforeach()

        if(__ignore)
            continue()
        endif()

        set(__show OFF)

        foreach(__i ${__arg_MATCH})
            if(__var MATCHES "${__i}")
                set(__show ON)
                break()
            endif()
        endforeach()

        if(__show)
            message("    ${__var}=${${__var}}.")
        endif()
    endforeach()
endmacro()


#-----------------------------------------------------------------------------------------------------------------------
#-----------------------------------------------------------------------------------------------------------------------
macro(assert)
    if(${ARGN})
    else()
        message(FATAL_ERROR "ASSERT: ${ARGN}.")
    endif()
endmacro()


#-----------------------------------------------------------------------------------------------------------------------
# Takes a list of path components and joins them into one path separated by forward slashes "/",
# and saves the path in out_var.
#-----------------------------------------------------------------------------------------------------------------------
function(octk_path_join out_var)
    string(JOIN "/" path ${ARGN})
    set(${out_var} ${path} PARENT_SCOPE)
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
# octk_remove_args can remove arguments from an existing list of function
# arguments in order to pass a filtered list of arguments to a different function.
# Parameters:
# out_var: result of remove all arguments specified by ARGS_TO_REMOVE from ALL_ARGS
# ARGS_TO_REMOVE: Arguments to remove.
# ALL_ARGS: All arguments supplied to cmake_parse_arguments or
# octk_parse_all_arguments
# from which ARGS_TO_REMOVE should be removed from. We require all the
# arguments or we can't properly identify the range of the arguments detailed
# in ARGS_TO_REMOVE.
# ARGS: Arguments passed into the function, usually ${ARGV}
#
# E.g.:
# We want to forward all arguments from foo to bar, execpt ZZZ since it will
# trigger an error in bar.
#
# foo(target BAR .... ZZZ .... WWW ...)
# bar(target BAR.... WWW...)
#
# function(foo target)
# octk_parse_all_arguments(arg "" "" "BAR;ZZZ;WWW ${ARGV})
# octk_remove_args(forward_args
# ARGS_TO_REMOVE ${target} ZZZ
# ALL_ARGS ${target} BAR ZZZ WWW
# ARGS ${ARGV}
# )
# bar(${target} ${forward_args})
# endfunction()
#
#-----------------------------------------------------------------------------------------------------------------------
function(octk_remove_args out_var)
    cmake_parse_arguments(arg "" "" "ARGS_TO_REMOVE;ALL_ARGS;ARGS" ${ARGN})
    set(result ${arg_ARGS})

    foreach(arg IN LISTS arg_ARGS_TO_REMOVE)
        # find arg
        list(FIND result ${arg} find_result)

        if(NOT find_result EQUAL -1)
            # remove arg
            list(REMOVE_AT result ${find_result})
            list(LENGTH result result_len)
            list(GET result ${find_result} arg_current)

            # remove values until we hit another arg or the end of the list
            while(NOT ${arg_current} IN_LIST arg_ALL_ARGS AND find_result LESS result_len)
                list(REMOVE_AT result ${find_result})
                list(LENGTH result result_len)

                if(NOT find_result EQUAL result_len)
                    list(GET result ${find_result} arg_current)
                endif()
            endwhile()
        endif()
    endforeach()

    set(${out_var} "${result}" PARENT_SCOPE)
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
# Creates a regular expression that exactly matches the given string
# Found in https://gitlab.kitware.com/cmake/cmake/issues/18580
#-----------------------------------------------------------------------------------------------------------------------
function(octk_re_escape out_var str)
    string(REGEX REPLACE "([][+.*()^])" "\\\\\\1" regex "${str}")
    set(${out_var} ${regex} PARENT_SCOPE)
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
# Gets a target property, and returns "" if the property was not found
#-----------------------------------------------------------------------------------------------------------------------
function(octk_internal_get_target_property out_var target property)
    get_target_property(result "${target}" "${property}")

    if("${result}" STREQUAL "result-NOTFOUND")
        set(result "")
    endif()

    set(${out_var} "${result}" PARENT_SCOPE)
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
# Creates a wrapper ConfigVersion.cmake file to be loaded by find_package when checking for
# compatible versions. It expects a ConfigVersionImpl.cmake file in the same directory which will
# be included to do the regular version checks.
# The version check result might be overridden by the wrapper.
# package_name is used by the content of the wrapper file to include the basic package version file.
#   example: OCTK1Gui
# out_path should be the build path where the write the file.
#-----------------------------------------------------------------------------------------------------------------------
function(octk_internal_write_octk_package_version_file package_name out_path)
    set(extra_code "")

    # Need to check for FEATURE_developer_build as well, because OCTK_FEATURE_developer_build is not
    # yet available when configuring the file for the BuildInternals package.
    if(FEATURE_developer_build OR OCTK_FEATURE_developer_build)
        string(APPEND extra_code "
            # Disabling version check because OpenCTK was configured with -developer-build.
            set(__octk_disable_package_version_check TRUE)
            set(__octk_disable_package_version_check_due_to_developer_build TRUE)")
    endif()

    configure_file("${OCTK_CMAKE_DIR}/OCTKCMakePackageVersionFile.cmake.in" "${out_path}" @ONLY)
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
# Input: string
# Output: regex string to match the string case insensitively
# Example: "Release" -> "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$"
#
# Regular expressions like this are used in cmake_install.cmake files for case-insensitive string comparison.
#-----------------------------------------------------------------------------------------------------------------------
function(octk_create_case_insensitive_regex out_var input)
    set(result "^(")
    string(LENGTH "${input}" n)
    math(EXPR n "${n} - 1")
    foreach(i RANGE 0 ${n})
        string(SUBSTRING "${input}" ${i} 1 c)
        string(TOUPPER "${c}" uc)
        string(TOLOWER "${c}" lc)
        string(APPEND result "[${uc}${lc}]")
    endforeach()
    string(APPEND result ")$")
    set(${out_var} "${result}" PARENT_SCOPE)
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
# octk_get_common_dir
#-----------------------------------------------------------------------------------------------------------------------
function(octk_get_common_dir common_dir input_dirs)
    set(common_folder)
    set(parent_folders)
    foreach(directory ${input_dirs})
        get_filename_component(parent_folder ${directory} DIRECTORY)
        if(NOT "${parent_folder}" IN_LIST input_dirs)
            list(APPEND parent_folders ${parent_folder})
        endif()
    endforeach()
    if(parent_folders)
        list(GET parent_folders 0 common_folder)
        string(LENGTH "${common_folder}" common_folder_strlen)
        foreach(folder ${parent_folders})
            string(LENGTH "${folder}" folder_strlen)
            if(folder_strlen LESS common_folder_strlen)
                set(common_folder "${folder}")
                set(common_folder_strlen ${folder_strlen})
            endif()
        endforeach()
    endif()
    set("${common_dir}" "${common_folder}" PARENT_SCOPE)
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
#-----------------------------------------------------------------------------------------------------------------------
function(octk_get_files input_dir files_var)
    file(GLOB all_files "${input_dir}/*")
    foreach(file ${all_files})
        if(NOT IS_DIRECTORY ${file})
            list(APPEND none_dir_files ${file})
        endif()
    endforeach()
    set("${files_var}" "${none_dir_files}" PARENT_SCOPE)
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
# octk_set_3rdparty_install_info
#-----------------------------------------------------------------------------------------------------------------------
function(octk_set_3rdparty_install_info prefix target)
    set(common_dir)
    set(include_directories)
    if(TARGET ${target})
        get_target_property(include_directories ${target} INTERFACE_INCLUDE_DIRECTORIES)
    else()
        foreach(directory ${target})
            if(EXISTS ${directory} AND IS_DIRECTORY ${directory})
                list(APPEND include_directories ${directory})
            endif()
        endforeach()
    endif()
    foreach(dir ${include_directories})
        string(REGEX MATCH ":(.*)>" content ${dir})
        if(content)
            string(LENGTH ${content} content_length)
            math(EXPR content_end "${content_length} - 2")
            string(SUBSTRING ${content} 1 ${content_end} content)
        else()
            set(content ${dir})
        endif()
        if(EXISTS "${content}")
            list(APPEND trimed_include_dirs ${content})
        endif()
    endforeach()
#    message(trimed_include_dirs=${trimed_include_dirs})
    octk_get_common_dir(common_dir "${trimed_include_dirs}")
#    message(common_dir=${common_dir})
    if("${common_dir}" STREQUAL "")
        message(FATAL_ERROR "${target} install dir not find!")
    endif()
    set(root_dir ${common_dir})
    file(GLOB root_subdirs RELATIVE ${root_dir} ${root_dir}/*)
#    while(NOT "install" IN_LIST root_subdirs)
#        message(root_subdirs=${root_subdirs})
#        get_filename_component(root_dir ${root_dir} DIRECTORY)
#        file(GLOB root_subdirs RELATIVE ${root_dir} ${root_dir}/*)
#    endwhile()
#    message(root_dir=${root_dir})
    get_filename_component(root_name ${root_dir} NAME)
    set(${prefix}_ROOT_DIR "${root_dir}" CACHE INTERNAL "${prefix}_ROOT_DIR var." FORCE)
    set(${prefix}_ROOT_NAME "${root_name}" CACHE INTERNAL "${prefix}_ROOT_DIR var." FORCE)
    set(${prefix}_INSTALL_DIR "${root_dir}/install" CACHE INTERNAL "${prefix}_INSTALL_DIR var." FORCE)
#    message(STATUS "Set 3rdparty ${target} install info finished.")
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
#-----------------------------------------------------------------------------------------------------------------------
function(octk_reset_dir DIR)
	get_filename_component(WORKING_DIR ${DIR} DIRECTORY)
	while(NOT EXISTS "${WORKING_DIR}")
		get_filename_component(WORKING_DIR ${WORKING_DIR} DIRECTORY)
	endwhile()

	execute_process(
		COMMAND ${CMAKE_COMMAND} -E remove_directory "${DIR}"
		WORKING_DIRECTORY "${WORKING_DIR}"
		RESULT_VARIABLE RMDIR_RESULT)
	if(NOT (RMDIR_RESULT MATCHES 0))
		message(FATAL_ERROR "${DIR} dir remove failed.")
	endif()
	execute_process(
		COMMAND ${CMAKE_COMMAND} -E echo "mkdir ${DIR}"
		COMMAND ${CMAKE_COMMAND} -E make_directory "${DIR}"
		WORKING_DIRECTORY "${WORKING_DIR}"
		RESULT_VARIABLE MKDIR_RESULT)
	if(NOT (MKDIR_RESULT MATCHES 0))
		message(FATAL_ERROR "${DIR} dir create failed.")
	endif()
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
#-----------------------------------------------------------------------------------------------------------------------
function(octk_stamp_file_info base_name)
    octk_parse_all_arguments(arg "" "" "SUFFIX;OUTPUT_DIR" "" ${ARGN})
    if("${arg_OUTPUT_DIR}" STREQUAL "")
        set(arg_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})
    endif()
    if("${arg_SUFFIX}" STREQUAL "")
        set("${base_name}_STAMP_FILE_NAME" "${base_name}-stamp.txt" PARENT_SCOPE)
        set("${base_name}_STAMP_FILE_PATH" "${arg_OUTPUT_DIR}/${base_name}-stamp.txt" PARENT_SCOPE)
    else()
        string(TOLOWER "${arg_SUFFIX}" LOWER_SUFFIX)
        set("${base_name}_${arg_SUFFIX}_STAMP_FILE_NAME" "${base_name}-${LOWER_SUFFIX}-stamp.txt" PARENT_SCOPE)
        set("${base_name}_${arg_SUFFIX}_STAMP_FILE_PATH" "${arg_OUTPUT_DIR}/${base_name}-${LOWER_SUFFIX}-stamp.txt" PARENT_SCOPE)
    endif()
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
#-----------------------------------------------------------------------------------------------------------------------
function(octk_make_stamp_file file_path)
    message(STATUS "Creating ${file_path} ...")
    string(TIMESTAMP CURRENT_TIMESTAMP "%Y-%m-%d %H:%M:%S")
    file(WRITE "${file_path}" "${CURRENT_TIMESTAMP}")
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
#-----------------------------------------------------------------------------------------------------------------------
function(octk_fetch_3rdparty name)
    octk_parse_all_arguments(arg "" "" "URL;OUTPUT_DIR;OUTPUT_NAME" "" ${ARGN})
    if(NOT EXISTS "${arg_URL}")
        message(FATAL_ERROR "3rdparty ${name} fetch failed, url ${arg_URL} not exist.")
    endif()
    get_filename_component(url_name ${arg_URL} NAME)
    string(REGEX MATCH "\\.7z$" is_7z ${url_name})
    string(REGEX MATCH "\\.zip$" is_zip ${url_name})
    string(REGEX MATCH "\\.tar\\.gz$" is_tar_gz ${url_name})
    string(REGEX MATCH "\\.tar\\.xz$" is_tar_xz ${url_name})
    string(REGEX MATCH "\\.tar\\.bz2$" is_tar_bz2 ${url_name})
    if(is_7z)
        string(REGEX REPLACE "\\.7z$" "" url_base_name ${url_name})
    elseif(is_zip)
        string(REGEX REPLACE "\\.zip$" "" url_base_name ${url_name})
    elseif(is_tar_gz)
        string(REGEX REPLACE "\\.tar\\.gz$" "" url_base_name ${url_name})
    elseif(is_tar_xz)
        string(REGEX REPLACE "\\.tar\\.xz$" "" url_base_name ${url_name})
    elseif(is_tar_bz2)
            string(REGEX REPLACE "\\.tar\\.bz2$" "" url_base_name ${url_name})
    elseif(IS_DIRECTORY ${arg_URL})
        set(is_dir TRUE)
        set(url_base_name ${url_name})
    else()
        message(FATAL_ERROR "3rdparty ${name} fetch failed, url ${arg_URL} is an unknown format compressed package")
    endif()
    if("${arg_OUTPUT_DIR}" STREQUAL "")
        set(arg_OUTPUT_DIR "${PROJECT_BINARY_DIR}/3rdparty")
    endif()
    if(NOT arg_OUTPUT_NAME)
        set(3rdparty_root_dir ${arg_OUTPUT_DIR}/${url_base_name})
    else()
        set(3rdparty_root_dir ${arg_OUTPUT_DIR}/${arg_OUTPUT_NAME})
    endif()
#    message(3rdparty_root_dir=${3rdparty_root_dir})
    set("${name}_FETCH_STAMP_FILE_PATH" "${3rdparty_root_dir}/${name}-fetch-stamp.txt")
    if(NOT EXISTS "${${name}_FETCH_STAMP_FILE_PATH}")
        message(STATUS "Fetching 3rdparty ${name} from ${arg_URL} ...")
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E make_directory "${3rdparty_root_dir}"
            WORKING_DIRECTORY "${PROJECT_BINARY_DIR}"
            RESULT_VARIABLE MKDIR_RESULT)
        if(NOT MKDIR_RESULT MATCHES 0)
            message(FATAL_ERROR "3rdparty ${name} fetch failed, work directory ${3rdparty_root_dir} make failed.")
        endif()
        if(is_dir)
            execute_process(
                COMMAND ${CMAKE_COMMAND} -E copy_directory "${arg_URL}" "${3rdparty_root_dir}/source"
                WORKING_DIRECTORY "${3rdparty_root_dir}"
                RESULT_VARIABLE COPYDIR_RESULT)
            if(NOT COPYDIR_RESULT MATCHES 0)
                message(FATAL_ERROR "3rdparty ${name} fetch failed, source directory copy failed.")
            endif()
        else()
            execute_process(
                COMMAND ${CMAKE_COMMAND} -E remove_directory source
                WORKING_DIRECTORY "${3rdparty_root_dir}"
                RESULT_VARIABLE RMDIR_RESULT)
            if(NOT RMDIR_RESULT MATCHES 0)
                message(FATAL_ERROR "3rdparty ${name} fetch failed, source directory clear failed.")
            endif()

            execute_process(
                COMMAND ${CMAKE_COMMAND} -E make_directory extract
                WORKING_DIRECTORY "${3rdparty_root_dir}"
                RESULT_VARIABLE MKDIR_RESULT)
            if(NOT MKDIR_RESULT MATCHES 0)
                message(FATAL_ERROR "3rdparty ${name} fetch failed, extract directory create failed.")
            endif()

            execute_process(
                COMMAND ${CMAKE_COMMAND} -E tar xzvf "${arg_URL}"
                WORKING_DIRECTORY "${3rdparty_root_dir}/extract"
                RESULT_VARIABLE EXTRACT_RESULT)
            if(NOT EXTRACT_RESULT EQUAL 0)
                message(FATAL_ERROR "3rdparty ${name} fetch failed, ${arg_URL} extract failed.")
            endif()

            file(GLOB extracted_dirs RELATIVE "${3rdparty_root_dir}/extract" "${3rdparty_root_dir}/extract/*")
            set(3rdparty_extracted_dir "${3rdparty_root_dir}/extract")
            foreach(subdir ${extracted_dirs})
                if(IS_DIRECTORY "${3rdparty_root_dir}/extract/${subdir}")
                    set(3rdparty_extracted_dir "${3rdparty_root_dir}/extract/${subdir}")
                endif()
            endforeach()

            execute_process(
                COMMAND ${CMAKE_COMMAND} -E rename "${3rdparty_extracted_dir}" "${3rdparty_root_dir}/source"
                WORKING_DIRECTORY "${3rdparty_root_dir}"
                RESULT_VARIABLE EXTRACT_RESULT)
            execute_process(
                COMMAND ${CMAKE_COMMAND} -E remove_directory extract
                WORKING_DIRECTORY "${3rdparty_root_dir}"
                RESULT_VARIABLE RMDIR_RESULT)
            if(NOT EXTRACT_RESULT EQUAL 0)
                message(FATAL_ERROR "3rdparty ${name} fetch failed, ${arg_URL} rename failed.")
            endif()
        endif()
        octk_make_stamp_file("${${name}_FETCH_STAMP_FILE_PATH}")
    endif()
    set("${name}_FETCH_STAMP_FILE_PATH" "${name}_FETCH_STAMP_FILE_PATH" PARENT_SCOPE)
endfunction()
