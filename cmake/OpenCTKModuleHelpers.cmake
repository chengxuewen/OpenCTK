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
# Populates $out_module_list with all subdirectories that have a CMakeLists.txt file
#-----------------------------------------------------------------------------------------------------------------------
function(octk_internal_find_modules out_module_list)
    set(module_list "")
    file(GLOB directories LIST_DIRECTORIES true RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *)
    foreach(directory IN LISTS directories)
        if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${directory}"
                AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${directory}/CMakeLists.txt")
            list(APPEND module_list "${directory}")
        endif()
    endforeach()
    message(DEBUG "octk_internal_find_modules: ${module_list}")
    set(${out_module_list} "${module_list}" PARENT_SCOPE)
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
# poor man's yaml parser, populating $out_dependencies with all dependencies
# in the $depends_file
# Each entry will be in the format dependency/sha1/required
#-----------------------------------------------------------------------------------------------------------------------
function(octk_internal_parse_dependencies depends_file out_dependencies)
    file(STRINGS "${depends_file}" lines)
    set(eof_marker "---EOF---")
    list(APPEND lines "${eof_marker}")
    set(required_default TRUE)
    set(dependencies "")
    set(dependency "")
    set(revision "")
    set(required "${required_default}")
    foreach(line IN LISTS lines)
        if(line MATCHES "^  (.+):$" OR line STREQUAL "${eof_marker}")
            # Found a repo entry or end of file. Add the last seen dependency.
            if(NOT dependency STREQUAL "")
                if(revision STREQUAL "")
                    message(FATAL_ERROR "Format error in ${depends_file} - ${dependency} does not specify revision!")
                endif()
                list(APPEND dependencies "${dependency}/${revision}/${required}")
            endif()
            # Remember the current dependency
            if(NOT line STREQUAL "${eof_marker}")
                set(dependency "${CMAKE_MATCH_1}")
                set(revision "")
                set(required "${required_default}")
                # dependencies are specified with relative path to this module
                string(REPLACE "../" "" dependency ${dependency})
            endif()
        elseif(line MATCHES "^    ref: (.+)$")
            set(revision "${CMAKE_MATCH_1}")
        elseif(line MATCHES "^    required: (.+)$")
            string(TOUPPER "${CMAKE_MATCH_1}" required)
        endif()
    endforeach()
    message(DEBUG "octk_internal_parse_dependencies for ${depends_file}\n    dependencies: ${dependencies}")
    set(${out_dependencies} "${dependencies}" PARENT_SCOPE)
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
# Resolve the dependencies of the given module.
# "Module" in the sense of OpenCTK repository.
#
# Side effects: Sets the global properties OCTK_DEPS_FOR_${module} and OCTK_REQUIRED_DEPS_FOR_${module}
# with the direct (required) dependencies of module.
#
#
# Positional arguments:
#
# module is the OpenCTK repository.
#
# out_ordered is where the result is stored. This is a list of all dependencies, including
# transitive ones, in topologically sorted order. Note that ${module} itself is also part of
# out_ordered.
#
# out_revisions is a list of git commit IDs for each of the dependencies in ${out_ordered}. This
# list has the same length as ${out_ordered}.
#
#
# Keyword arguments:
#
# PARSED_DEPENDENCIES is a list of dependencies of module in the format that
# octk_internal_parse_dependenciesdependencies returns. If this argument is not provided, dependencies.yaml of the
# module is parsed.
#
# IN_RECURSION is an internal option that is set when the function is in recursion.
#
# REVISION is an internal value with the git commit ID that belongs to ${module}.
#
# SKIPPED_VAR is an output variable name that is set to TRUE if the module was skipped, to FALSE
# otherwise.
#
# NORMALIZE_REPO_NAME_IF_NEEDED Will remove 'toctkc-' from the beginning of submodule dependencies
# if a toctkc- named directory does not exist.
#-----------------------------------------------------------------------------------------------------------------------
function(octk_internal_resolve_module_dependencies module out_ordered out_revisions)
    set(options IN_RECURSION NORMALIZE_REPO_NAME_IF_NEEDED)
    set(oneValueArgs REVISION SKIPPED_VAR)
    set(multiValueArgs PARSED_DEPENDENCIES)
    cmake_parse_arguments(arg "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Clear the property that stores the repositories we've already seen.
    if(NOT arg_IN_RECURSION)
        set_property(GLOBAL PROPERTY _octk_internal_seen_repos)
    endif()

    # Bail out if we've seen the module already.
    octk_internal_resolve_module_dependencies_set_skipped(FALSE)
    get_property(seen GLOBAL PROPERTY _octk_internal_seen_repos)
    if(module IN_LIST seen)
        octk_internal_resolve_module_dependencies_set_skipped(TRUE)
        return()
    endif()

    set_property(GLOBAL APPEND PROPERTY _octk_internal_seen_repos ${module})

    # Set a default REVISION.
    if("${arg_REVISION}" STREQUAL "")
        set(arg_REVISION HEAD)
    endif()

    # Retrieve the dependencies.
    if(DEFINED arg_PARSED_DEPENDENCIES)
        set(dependencies "${arg_PARSED_DEPENDENCIES}")
    else()
        set(depends_file "${CMAKE_CURRENT_SOURCE_DIR}/${module}/dependencies.yaml")
        set(dependencies "")
        if(EXISTS "${depends_file}")
            octk_internal_parse_dependencies("${depends_file}" dependencies)
        endif()
    endif()

    # Traverse the dependencies.
    set(ordered)
    set(revisions)
    foreach(dependency IN LISTS dependencies)
        if(dependency MATCHES "(.*)/([^/]+)/([^/]+)")
            set(dependency "${CMAKE_MATCH_1}")
            set(revision "${CMAKE_MATCH_2}")
            set(required "${CMAKE_MATCH_3}")
        else()
            message(FATAL_ERROR "Internal Error: wrong dependency format ${dependency}")
        endif()

        set(normalize_arg "")
        if(arg_NORMALIZE_REPO_NAME_IF_NEEDED)
            octk_internal_use_normalized_repo_name_if_needed("${dependency}" dependency)
            set(normalize_arg "NORMALIZE_REPO_NAME_IF_NEEDED")
        endif()

        set_property(GLOBAL APPEND PROPERTY OCTK_DEPS_FOR_${module} ${dependency})
        if(required)
            set_property(GLOBAL APPEND PROPERTY OCTK_REQUIRED_DEPS_FOR_${module} ${dependency})
        endif()

        octk_internal_resolve_module_dependencies(${dependency} dep_ordered dep_revisions
            REVISION "${revision}"
            SKIPPED_VAR skipped
            IN_RECURSION
            ${normalize_arg})
        if(NOT skipped)
            list(APPEND ordered ${dep_ordered})
            list(APPEND revisions ${dep_revisions})
        endif()
    endforeach()

    list(APPEND ordered ${module})
    list(APPEND revisions ${arg_REVISION})
    set(${out_ordered} "${ordered}" PARENT_SCOPE)
    set(${out_revisions} "${revisions}" PARENT_SCOPE)
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
# Helper macro for octk_internal_resolve_module_dependencies.
#-----------------------------------------------------------------------------------------------------------------------
macro(octk_internal_resolve_module_dependencies_set_skipped value)
    if(DEFINED arg_SKIPPED_VAR)
        set(${arg_SKIPPED_VAR} ${value} PARENT_SCOPE)
    endif()
endmacro()


#-----------------------------------------------------------------------------------------------------------------------
# Strips toctkc- prefix from a repo name.
#-----------------------------------------------------------------------------------------------------------------------
function(octk_internal_normalize_repo_name repo_name out_var)
    string(REGEX REPLACE "^toctkc-" "" normalized "${repo_name}")
    set(${out_var} "${normalized}" PARENT_SCOPE)
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
# Checks if a directory with the given repo name exists in the current
# source / working directory. If it doesn't, it strips the toctkc- prefix.
#-----------------------------------------------------------------------------------------------------------------------
function(octk_internal_use_normalized_repo_name_if_needed repo_name out_var)
    set(base_dir "${CMAKE_CURRENT_SOURCE_DIR}")
    set(repo_dir "${base_dir}/${repo_name}")
    if(NOT IS_DIRECTORY "${repo_dir}")
        octk_internal_normalize_repo_name("${repo_name}" repo_name)
    endif()
    set(${out_var} "${repo_name}" PARENT_SCOPE)
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
# Generate a module description file based on the template in ModuleDescription.json.in
#-----------------------------------------------------------------------------------------------------------------------
function(octk_describe_module target)
    set(path_suffix "${OCTK_INSTALL_DESCRIPTIONSDIR}")
    octk_path_join(build_dir ${OCTK_BUILD_DIR} ${path_suffix})
    octk_path_join(install_dir ${OCTK_INSTALL_DIR} ${path_suffix})

    set(descfile_in "${OCTK_CMAKE_DIR}/ModuleDescription.json.in")
    set(descfile_out "${build_dir}/${target}.json")
    set(cross_compilation "false")
    if(CMAKE_CROSSCOMPILING)
        set(cross_compilation "true")
    endif()

    configure_file("${descfile_in}" "${descfile_out}")

    octk_install(FILES "${descfile_out}" DESTINATION "${install_dir}")
endfunction()