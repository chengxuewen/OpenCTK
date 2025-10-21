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

# This function recursively walks transitive link libraries of the given target
# and promotes those targets to be IMPORTED_GLOBAL if they are not.
#
# This is required for .prl file generation in top-level builds, to make sure that imported 3rd
# party library targets in any repo are made global, so there are no scoping issues.
#
# Only works if called from octk_find_package(), because the promotion needs to happen in the same
# directory scope where the imported target is first created.
#
# Uses __octk_internal_walk_libs.
function(octk_find_package_promote_targets_to_global_scope target)
    __octk_internal_walk_libs("${target}" _discarded_out_var _discarded_out_var_2
        "octk_find_package_targets_dict" "promote_global")
endfunction()

macro(octk_find_package)
    # Get the target names we expect to be provided by the package.
    set(find_package_options CONFIG NO_MODULE MODULE REQUIRED)
    set(options ${find_package_options} MARK_OPTIONAL)
    set(oneValueArgs MODULE_NAME)
    set(multiValueArgs PROVIDED_TARGETS COMPONENTS OPTIONAL_COMPONENTS)
    cmake_parse_arguments(arg "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # If some OpenCTK internal project calls octk_find_package(WrapFreeType), but WrapFreeType was already
    # found as part of a find_dependency() call from a ModuleDependencies.cmake file (or similar),
    # and the provided target is also found, that means this might have been an unnecessary
    # octk_find_package() call, because the dependency was already found via some other transitive
    # dependency. Return early, so that CMake doesn't fail with an error with trying to promote the
    # targets to be global. This behavior is not enabled by default, because there are cases
    # when a regular find_package() (non octk_) can find a package (Freetype -> PNG), and a subsequent
    # octk_find_package(PNG PROVIDED_TARGET PNG::PNG) still needs to succeed and register the provided
    # targets. To enable the debugging behavior, set OCTK_DEBUG_OCTK_FIND_PACKAGE to 1.
    set(_octk_find_package_skip_find_package FALSE)
    if(OCTK_DEBUG_OCTK_FIND_PACKAGE AND OpenCTK${ARGV0}_FOUND AND arg_PROVIDED_TARGETS)
        set(_octk_find_package_skip_find_package TRUE)
        foreach(octk_find_package_target_name ${arg_PROVIDED_TARGETS})
            if(NOT TARGET ${octk_find_package_target_name})
                set(_octk_find_package_skip_find_package FALSE)
            endif()
        endforeach()

        if(_octk_find_package_skip_find_package)
            message(AUTHOR_WARNING "octk_find_package(${ARGV0}) called even though the package "
                "was already found. Consider removing the call.")
        endif()
    endif()

    # When configure.cmake is included only to record summary entries, there's no point in looking
    # for the packages.
    if(__octk_feature_only_record_summary_entries)
        set(_octk_find_package_skip_find_package TRUE)
    endif()

    # Get the version if specified.
    set(package_version "")
    if(${ARGC} GREATER_EQUAL 2)
        if(${ARGV1} MATCHES "^[0-9\.]+$")
            set(package_version "${ARGV1}")
        endif()
    endif()

    if(arg_COMPONENTS)
        # Re-append components to forward them.
        list(APPEND arg_UNPARSED_ARGUMENTS "COMPONENTS;${arg_COMPONENTS}")
    endif()
    if(arg_OPTIONAL_COMPONENTS)
        # Re-append optional components to forward them.
        list(APPEND arg_UNPARSED_ARGUMENTS "OPTIONAL_COMPONENTS;${arg_OPTIONAL_COMPONENTS}")
    endif()

    # Don't look for packages in PATH if requested to.
    if(OCTK_NO_USE_FIND_PACKAGE_SYSTEM_ENVIRONMENT_PATH)
        set(_octk_find_package_use_system_env_backup "${CMAKE_FIND_USE_SYSTEM_ENVIRONMENT_PATH}")
        set(CMAKE_FIND_USE_SYSTEM_ENVIRONMENT_PATH "OFF")
    endif()

    if(NOT (arg_CONFIG OR arg_NO_MODULE OR arg_MODULE) AND NOT _octk_find_package_skip_find_package)
        # Try to find a config package first in quiet mode
        set(config_package_arg ${arg_UNPARSED_ARGUMENTS})
        list(APPEND config_package_arg "CONFIG;QUIET")
        find_package(${config_package_arg})

        # Double check that in config mode the targets become visible. Sometimes
        # only the module mode creates the targets. For example with vcpkg, the sqlite
        # package provides sqlite3-config.cmake, which offers multi-config targets but
        # in their own way. CMake has FindSQLite3.cmake and with the original
        # octk_find_package(SQLite3) call it is our intention to use the cmake package
        # in module mode.
        unset(_octk_any_target_found)
        unset(_octk_should_unset_found_var)
        if(OpenCTK${ARGV0}_FOUND AND arg_PROVIDED_TARGETS)
            foreach(expected_target ${arg_PROVIDED_TARGETS})
                if(TARGET ${expected_target})
                    set(_octk_any_target_found TRUE)
                    break()
                endif()
            endforeach()
            if(NOT _octk_any_target_found)
                set(_octk_should_unset_found_var TRUE)
            endif()
        endif()
        # If we consider the package not to be found, make sure to unset both regular
        # and CACHE vars, otherwise CMP0126 set to NEW might cause issues with
        # packages not being found correctly.
        if(NOT OpenCTK${ARGV0}_FOUND OR _octk_should_unset_found_var)
            unset(OpenCTK${ARGV0}_FOUND)
            unset(OpenCTK${ARGV0}_FOUND CACHE)

            # Unset the NOTFOUND ${package}_DIR var that might have been set by the previous
            # find_package call, to get rid of "not found" messages in the feature summary
            # if the package is found by the next find_package call.
            if(DEFINED CACHE{${ARGV0}_DIR} AND NOT ${ARGV0}_DIR)
                unset(${ARGV0}_DIR CACHE)
            endif()
        endif()
    endif()

    # Ensure the options are back in the original unparsed arguments
    foreach(opt IN LISTS find_package_options)
        if(arg_${opt})
            list(APPEND arg_UNPARSED_ARGUMENTS ${opt})
        endif()
    endforeach()

    # TODO: Handle packages with components where a previous component is already found.
    # E.g. find_package(OpenCTK COMPONENTS BuildInternals) followed by
    # octk_find_package(OpenCTK COMPONENTS Core) doesn't end up calling find_package(OCTK6Core).
    if(NOT OpenCTK${ARGV0}_FOUND AND NOT _octk_find_package_skip_find_package)
        # Call original function without our custom arguments.
        find_package(${arg_UNPARSED_ARGUMENTS})
    endif()

    if(OCTK_NO_USE_FIND_PACKAGE_SYSTEM_ENVIRONMENT_PATH)
        if("${_octk_find_package_use_system_env_backup}" STREQUAL "")
            unset(CMAKE_FIND_USE_SYSTEM_ENVIRONMENT_PATH)
        else()
            set(CMAKE_FIND_USE_SYSTEM_ENVIRONMENT_PATH "${_octk_find_package_use_system_env_backup}")
        endif()
    endif()

    # message(OpenCTK${ARGV0}_FOUND=${OpenCTK${ARGV0}_FOUND})
    # message(arg_PROVIDED_TARGETS=${arg_PROVIDED_TARGETS})
    # message(_octk_find_package_skip_find_package=${_octk_find_package_skip_find_package})
    if(OCTK${ARGV0}_FOUND AND arg_PROVIDED_TARGETS AND NOT _octk_find_package_skip_find_package)
        # If package was found, associate each target with its package name. This will be used
        # later when creating Config files for OpenCTK libraries, to generate correct find_dependency()
        # calls. Also make the provided targets global, so that the properties can be read in
        # all scopes.
        foreach(octk_find_package_target_name ${arg_PROVIDED_TARGETS})
            if(TARGET ${octk_find_package_target_name})
                # Allow usage of aliased targets by setting properties on the actual target
                get_target_property(aliased_target ${octk_find_package_target_name} ALIASED_TARGET)
                if(aliased_target)
                    set(octk_find_package_target_name ${aliased_target})
                endif()

                set_target_properties(${octk_find_package_target_name} PROPERTIES
                    INTERFACE_OCTK_PACKAGE_NAME ${ARGV0}
                    INTERFACE_OCTK_PACKAGE_IS_OPTIONAL ${arg_MARK_OPTIONAL})
                if(package_version)
                    set_target_properties(${octk_find_package_target_name}
                        PROPERTIES INTERFACE_OCTK_PACKAGE_VERSION ${ARGV1})
                endif()

                if(arg_COMPONENTS)
                    string(REPLACE ";" " " components_as_string "${arg_COMPONENTS}")
                    set_property(TARGET ${octk_find_package_target_name}
                        PROPERTY INTERFACE_OCTK_PACKAGE_COMPONENTS ${components_as_string})
                endif()

                if(arg_OPTIONAL_COMPONENTS)
                    string(REPLACE ";" " " components_as_string "${arg_OPTIONAL_COMPONENTS}")
                    set_property(TARGET ${octk_find_package_target_name}
                        PROPERTY INTERFACE_OCTK_PACKAGE_OPTIONAL_COMPONENTS
                        ${components_as_string})
                endif()

                get_property(is_global TARGET ${octk_find_package_target_name} PROPERTY IMPORTED_GLOBAL)
                octk_internal_should_not_promote_package_target_to_global("${octk_find_package_target_name}" 
                    should_not_promote)
                if(NOT is_global AND NOT should_not_promote)
                    __octk_internal_promote_target_to_global(${octk_find_package_target_name})
                    octk_find_package_promote_targets_to_global_scope(
                        "${octk_find_package_target_name}")
                endif()
            endif()

        endforeach()
        #
        #        if(arg_MODULE_NAME AND arg_QMAKE_LIB
        #            AND (NOT arg_QMAKE_LIB IN_LIST OCTK_QMAKE_LIBS_FOR_${arg_MODULE_NAME}))
        #            set(OCTK_QMAKE_LIBS_FOR_${arg_MODULE_NAME}
        #                ${OCTK_QMAKE_LIBS_FOR_${arg_MODULE_NAME}};${arg_QMAKE_LIB} CACHE INTERNAL "")
        #            set(OCTK_TARGETS_OF_QMAKE_LIB_${arg_QMAKE_LIB} ${arg_PROVIDED_TARGETS} CACHE INTERNAL "")
        #            foreach(provided_target ${arg_PROVIDED_TARGETS})
        #                set(OCTK_QMAKE_LIB_OF_TARGET_${provided_target} ${arg_QMAKE_LIB} CACHE INTERNAL "")
        #            endforeach()
        #        endif()
    endif()
endmacro()


#-----------------------------------------------------------------------------------------------------------------------
# This function stores the list of OpenCTK targets a library depend on,
# along with their version info, for usage in ${target}Depends.cmake file
#-----------------------------------------------------------------------------------------------------------------------
function(octk_register_target_dependencies target public_libs private_libs)
    get_target_property(target_deps "${target}" _octk_target_deps)
    if(NOT target_deps)
        set(target_deps "")
    endif()

    get_target_property(target_type ${target} TYPE)
    set(lib_list ${public_libs})

    set(target_is_shared FALSE)
    set(target_is_static FALSE)
    if(target_type STREQUAL "SHARED_LIBRARY")
        set(target_is_shared TRUE)
    elseif(target_type STREQUAL "STATIC_LIBRARY")
        set(target_is_static TRUE)
    endif()

    # Record 'octk::foo'-like private dependencies of static library targets, this will be used to
    # generate find_dependency() calls.
    #
    # Private static library dependencies will become $<LINK_ONLY:> dependencies in
    # INTERFACE_LINK_LIBRARIES.
    if(target_is_static)
        list(APPEND lib_list ${private_libs})
    endif()

    foreach(lib IN LISTS lib_list)
        if("${lib}" MATCHES "^octk::(.*)")
            set(lib "${CMAKE_MATCH_1}")
            octk_internal_get_package_name_of_target("${lib}" package_name)
            octk_internal_get_package_version_of_target("${lib}" package_version)
            list(APPEND target_deps "${package_name}\;${package_version}")
        endif()
    endforeach()

    # Record 'octk::foo'-like shared private dependencies of shared library targets.
    #
    # Private shared library dependencies are listed in the target's
    # IMPORTED_LINK_DEPENDENT_LIBRARIES and used in rpath-link calculation.
    # We filter out static libraries and common platform targets, but include both SHARED and
    # INTERFACE libraries. INTERFACE libraries in most cases will be FooPrivate libraries.
    if(target_is_shared AND private_libs)
        foreach(lib IN LISTS private_libs)
            if("${lib}" MATCHES "^octk::(.*)")
                set(lib_namespaced "${lib}")
                set(lib "${CMAKE_MATCH_1}")

                get_target_property(lib_type "${lib_namespaced}" TYPE)
                if(NOT lib_type STREQUAL "STATIC_LIBRARY")
                    octk_internal_get_package_name_of_target("${lib}" package_name)
                    octk_internal_get_package_version_of_target("${lib}" package_version)
                    list(APPEND target_deps "${package_name}\;${package_version}")
                endif()
            endif()
        endforeach()
    endif()

    set_target_properties("${target}" PROPERTIES _octk_target_deps "${target_deps}")
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
# Get the CMake package name that contains / exported the OpenCTK module target.
#-----------------------------------------------------------------------------------------------------------------------
function(octk_internal_get_package_name_of_target target package_name_out_var)
    # Get the package name from the module's target property.
    # If not set, fallback to a name based on the target name.
    #
    # TODO: Remove fallback once sufficient time has passed, aka all developers updated
    # their builds not to contain stale FooDependencies.cmakes files without the
    # _octk_package_name property.
    set(package_name "")
    set(package_name_default "${OCTK_CMAKE_INSTALL_NAMESPACE}_${target}")
    set(target_namespaced "${OCTK_CMAKE_EXPORT_NAMESPACE}::${target}")
    #    message(target_namespaced=${target_namespaced})
    if(TARGET "${target_namespaced}")
        get_target_property(package_name_from_prop "${target_namespaced}" _octk_package_name)
        if(package_name_from_prop)
            set(package_name "${package_name_from_prop}")
        endif()
    endif()
    #    message(package_name=${package_name})
    if(NOT package_name)
        message(WARNING
            "Could not find target ${target_namespaced} to query its package name. "
            "Defaulting to package name ${package_name_default}. Consider re-arranging the "
            "project structure to ensure the target exists by this point.")
        set(package_name "${package_name_default}")
    endif()

    set(${package_name_out_var} "${package_name}" PARENT_SCOPE)
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
# Try to get the CMake package version of a OpenCTK target.
#
# Query the target's _octk_package_version property, or try to read it from the CMake package version
# variable set from calling find_package(OpenCTK${target}).
# Not all targets will have a find_package _VERSION variable, for example if the target is an
# executable.
# A heuristic is used to handle OCTKFooPrivate module targets.
# If no version can be found, fall back to ${PROJECT_VERSION} and issue a warning.
#-----------------------------------------------------------------------------------------------------------------------
function(octk_internal_get_package_version_of_target target package_version_out_var)
    # Try to get the version from the target.
    # Try the Private target first and if it doesn't exist, try the non-Private target later.
    if(TARGET "${OCTK_CMAKE_EXPORT_NAMESPACE}::${target}")
        get_target_property(package_version "${OCTK_CMAKE_EXPORT_NAMESPACE}::${target}" _octk_package_version)
    endif()

    # Try to get the version from the corresponding package version variable.
    if(NOT package_version)
        set(package_version "${${OCTK_CMAKE_EXPORT_NAMESPACE}_${target}_VERSION}")
    endif()

    # Try non-Private target.
    if(NOT package_version AND target MATCHES "(.*)_private$")
        set(target "${CMAKE_MATCH_1}")
    endif()

    if(NOT package_version AND TARGET "${OCTK_CMAKE_EXPORT_NAMESPACE}::${target}")
        get_target_property(package_version "${OCTK_CMAKE_EXPORT_NAMESPACE}::${target}" _octk_package_version)
    endif()

    if(NOT package_version)
        set(package_version "${${OCTK_CMAKE_EXPORT_NAMESPACE}_${target}_VERSION}")
    endif()

    if(NOT package_version)
        set(package_version "${PROJECT_VERSION}")
        if(FEATURE_developer_build)
            message(WARNING
                "Could not determine package version of target ${target}. "
                "Defaulting to project version ${PROJECT_VERSION}.")
        endif()
    endif()

    set(${package_version_out_var} "${package_version}" PARENT_SCOPE)
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
# Sets out_var to to TRUE if the target was marked to not be promoted to global scope.
#-----------------------------------------------------------------------------------------------------------------------
function(octk_internal_should_not_promote_package_target_to_global target out_var)
    get_property(should_not_promote TARGET "${target}" PROPERTY _octk_no_promote_global)
    set("${out_var}" "${should_not_promote}" PARENT_SCOPE)
endfunction()
