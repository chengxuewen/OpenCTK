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
#-----------------------------------------------------------------------------------------------------------------------
function(octk_internal_strip_target_directory_scope_token target out_var)
    # In CMake versions earlier than CMake 3.18, a subdirectory scope id is appended to the
    # target name if the target is referenced in a target_link_libraries command from a
    # different directory scope than where the target was created.
    # Strip it.
    #
    # For informational purposes, in CMake 3.18, the target name looks as follows:
    # ::@(0x5604cb3f6b50);Threads::Threads;::@
    # This case doesn't have to be stripped (at least for now), because when we iterate over
    # link libraries, the tokens appear as separate target names.
    #
    # Example: Threads::Threads::@<0x5604cb3f6b50>
    # Output: Threads::Threads
    string(REGEX REPLACE "::@<.+>$" "" target "${target}")
    set("${out_var}" "${target}" PARENT_SCOPE)
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
# Tests if linker could resolve circular dependencies between object files and static libraries.
#-----------------------------------------------------------------------------------------------------------------------
function(__octk_internal_static_link_order_public_test result)
    # We could trust iOS linker
    if(IOS)
        set(OCTK_HAVE_LINK_ORDER_MATTERS "FALSE" CACHE INTERNAL "Link order matters")
    endif()

    if(DEFINED OCTK_HAVE_LINK_ORDER_MATTERS)
        set(${result} "${OCTK_HAVE_LINK_ORDER_MATTERS}" PARENT_SCOPE)
        return()
    endif()

    if(EXISTS "${OCTK_CMAKE_DIR}")
        set(test_source_basedir "${OCTK_CMAKE_DIR}/..")
    else()
        set(test_source_basedir "${_octk_cmake_dir}/${OCTK_CMAKE_EXPORT_NAMESPACE}")
    endif()

    try_compile(${result}
        "${CMAKE_CURRENT_BINARY_DIR}/config.tests/static_link_order"
        "${test_source_basedir}/config.tests/static_link_order"
        static_link_order_test
        static_link_order_test)
    message(STATUS "Check if linker can resolve circular dependencies - ${${result}}")

    # Invert the result
    if(${result})
        set(${result} FALSE)
    else()
        set(${result} TRUE)
    endif()

    set(OCTK_HAVE_LINK_ORDER_MATTERS "${${result}}" CACHE INTERNAL "Link order matters")

    set(${result} "${${result}}" PARENT_SCOPE)
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
# Sets _octk_link_order_matters flag for the target.
#-----------------------------------------------------------------------------------------------------------------------
function(__octk_internal_set_link_order_matters target link_order_matters)
    if(NOT TARGET ${target})
        message(FATAL_ERROR "Unable to set _octk_link_order_matters flag. ${target} is not a target.")
    endif()

    get_target_property(aliased_target ${target} ALIASED_TARGET)
    if(aliased_target)
        set(target "${aliased_target}")
    endif()

    if(link_order_matters)
        set(link_order_matters TRUE)
    else()
        set(link_order_matters FALSE)
    endif()
    set_target_properties(${target} PROPERTIES _octk_link_order_matters "${link_order_matters}")
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
# Function combines __octk_internal_static_link_order_public_test and
# __octk_internal_set_link_order_matters calls on OpenCTK::platform target.
#-----------------------------------------------------------------------------------------------------------------------
function(__octk_internal_check_link_order_matters)
    __octk_internal_static_link_order_public_test(link_order_matters)
    __octk_internal_set_link_order_matters(${OCTK_CMAKE_EXPORT_NAMESPACE}::platform "${link_order_matters}")

    if("${ARGC}" GREATER "0" AND NOT ARGV0 STREQUAL "")
        set(${ARGV0} ${link_order_matters} PARENT_SCOPE)
    endif()
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
# Constructs a TARGET_POLICY genex expression if the policy is available.
#-----------------------------------------------------------------------------------------------------------------------
function(__octk_internal_get_cmp0099_genex_check result)
    if(POLICY CMP0099)
        set(${result} "$<BOOL:$<TARGET_POLICY:CMP0099>>" PARENT_SCOPE)
    else()
        set(${result} "$<BOOL:FALSE>" PARENT_SCOPE)
    endif()
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
#-----------------------------------------------------------------------------------------------------------------------
function(__octk_internal_check_cmp0099_available)
    set(platform_target ${OCTK_CMAKE_EXPORT_NAMESPACE}::platform)
    get_target_property(aliased_target ${platform_target} ALIASED_TARGET)
    if(aliased_target)
        set(platform_target "${aliased_target}")
    endif()

    __octk_internal_get_cmp0099_genex_check(cmp0099_check)
    set_target_properties(${platform_target} PROPERTIES _octk_cmp0099_policy_check "${cmp0099_check}")

    set(result TRUE)
    if(NOT POLICY CMP0099)
        set(result FALSE)
    endif()

    if("${ARGC}" GREATER "0" AND NOT ARGV0 STREQUAL "")
        set(${ARGV0} ${result} PARENT_SCOPE)
    endif()
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
#-----------------------------------------------------------------------------------------------------------------------
function(__octk_internal_process_dependency_object_libraries target)
    # The CMake versions greater than 3.21 take care about the order of object files in a
    # linker line, it's expected that all object files are located at the beginning of the linker
    # line.
    # So circular dependencies between static libraries and object files are resolved and no need
    # to call the finalizer code.
    if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.21)
        return()
    endif()
    get_target_property(processed ${target} _octk_object_libraries_finalizer_processed)
    if(processed)
        return()
    endif()
    set_target_properties(${target} PROPERTIES _octk_object_libraries_finalizer_processed TRUE)

    get_target_property(octk_link_order_matters
        ${OCTK_CMAKE_EXPORT_NAMESPACE}::platform _octk_link_order_matters)
    __octk_internal_check_finalizer_mode(${target}
        use_finalizer_mode
        object_libraries
        DEFAULT_VALUE "${octk_link_order_matters}")

    if(NOT use_finalizer_mode)
        return()
    endif()

    __octk_internal_collect_dependency_object_libraries(${target} objects)
    target_sources(${target} PRIVATE "${objects}")
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
#-----------------------------------------------------------------------------------------------------------------------
function(__octk_internal_collect_dependency_object_libraries target out_var)
    set_property(GLOBAL PROPERTY _octk_processed_object_libraries "")

    __octk_internal_collect_object_libraries_recursively(object_libraries ${target} ${target})

    # Collect object libraries of plugins and plugin dependencies.
    __octk_internal_collect_plugin_targets_from_dependencies(${target} plugin_targets)
    __octk_internal_collect_dependency_plugin_object_libraries(${target}
        "${plugin_targets}"
        plugin_objects)

    set_property(GLOBAL PROPERTY _octk_processed_object_libraries "")
    __octk_internal_get_cmp0099_genex_check(cmp0099_check)

    list(REMOVE_DUPLICATES object_libraries)
    set(objects "")
    foreach(dep IN LISTS object_libraries)
        list(PREPEND objects "$<$<NOT:${cmp0099_check}>:$<TARGET_OBJECTS:${dep}>>")
    endforeach()

    set(${out_var} "${plugin_objects};${objects}" PARENT_SCOPE)
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
#-----------------------------------------------------------------------------------------------------------------------
function(__octk_internal_collect_dependency_plugin_object_libraries target plugin_targets out_var)
    __octk_internal_get_cmp0099_genex_check(cmp0099_check)
    set(plugin_objects "")
    foreach(plugin_target IN LISTS plugin_targets)
        __octk_internal_collect_object_libraries_recursively(plugin_object_libraries
            "${OCTK_CMAKE_EXPORT_NAMESPACE}::${plugin_target}"
            ${target})
        __octk_internal_get_static_plugin_condition_genex("${plugin_target}" plugin_condition)

        foreach(plugin_object_library IN LISTS plugin_object_libraries)
            string(JOIN "" plugin_objects_genex
                "$<"
                "$<AND:"
                "$<NOT:${cmp0099_check}>,"
                "${plugin_condition}"
                ">"
                ":$<TARGET_OBJECTS:${plugin_object_library}>"
                ">")
            list(APPEND plugin_objects "${plugin_objects_genex}")
        endforeach()
    endforeach()
    set(${out_var} "${plugin_objects}" PARENT_SCOPE)
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
#-----------------------------------------------------------------------------------------------------------------------
function(__octk_internal_collect_object_libraries_recursively out_var target initial_target)
    get_property(processed_object_libraries GLOBAL PROPERTY _octk_processed_object_libraries)

    set(interface_libs "")
    set(libs "")
    if(NOT "${target}" STREQUAL "${initial_target}")
        get_target_property(interface_libs ${target} INTERFACE_LINK_LIBRARIES)
    endif()
    get_target_property(type ${target} TYPE)
    if(NOT type STREQUAL "INTERFACE_LIBRARY")
        get_target_property(libs ${target} LINK_LIBRARIES)
    endif()

    set(object_libraries "")
    foreach(lib IN LISTS libs interface_libs)
        # Extract possible target from exported LINK_ONLY dependencies.
        # This is super important for traversing backing library dependencies of qml plugins.
        if(lib MATCHES "^\\$<LINK_ONLY:(.*)>$")
            set(lib "${CMAKE_MATCH_1}")
        endif()
        if(TARGET ${lib})
            get_target_property(aliased_target ${lib} ALIASED_TARGET)
            if(aliased_target)
                set(lib ${aliased_target})
            endif()

            if(${lib} IN_LIST processed_object_libraries)
                continue()
            else()
                list(APPEND processed_object_libraries ${lib})
                set_property(GLOBAL APPEND PROPERTY _octk_processed_object_libraries ${lib})
            endif()

            get_target_property(is_octk_propagated_object_library ${lib} _is_octk_propagated_object_library)
            if(is_octk_propagated_object_library)
                list(APPEND object_libraries ${lib})
            else()
                __octk_internal_collect_object_libraries_recursively(next_level_object_libraries
                    ${lib}
                    ${initial_target})
                list(APPEND object_libraries ${next_level_object_libraries})
            endif()
        endif()
    endforeach()
    set(${out_var} "${object_libraries}" PARENT_SCOPE)
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
#-----------------------------------------------------------------------------------------------------------------------
function(__octk_internal_promote_target_to_global target)
    get_property(is_global TARGET ${target} PROPERTY IMPORTED_GLOBAL)
    if(NOT is_global)
        message(DEBUG "Promoting target to global: '${target}'")
        set_property(TARGET ${target} PROPERTY IMPORTED_GLOBAL TRUE)
    endif()
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
#-----------------------------------------------------------------------------------------------------------------------
function(__octk_internal_promote_target_to_global_checked target)
    # With CMake version 3.21 we use a different mechanism that allows us to promote all targets
    # within a scope.
    if(OCTK_PROMOTE_TO_GLOBAL_TARGETS AND CMAKE_VERSION VERSION_LESS 3.21)
        __octk_internal_promote_target_to_global(${target})
    endif()
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
#-----------------------------------------------------------------------------------------------------------------------
function(__octk_internal_promote_targets_in_dir_scope_to_global)
    # IMPORTED_TARGETS got added in 3.21.
    if(CMAKE_VERSION VERSION_LESS 3.21)
        return()
    endif()

    get_directory_property(targets IMPORTED_TARGETS)
    foreach(target IN LISTS targets)
        __octk_internal_promote_target_to_global(${target})
    endforeach()
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
#-----------------------------------------------------------------------------------------------------------------------
function(__octk_internal_promote_targets_in_dir_scope_to_global_checked)
    if(OCTK_PROMOTE_TO_GLOBAL_TARGETS)
        __octk_internal_promote_targets_in_dir_scope_to_global()
    endif()
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
# This function ends up being called multiple times as part of a find_package(OCTKFoo) call,
# due sub-packages depending on the OpenCTK package. Ensure the finalizer is ran only once per
# directory scope.
#-----------------------------------------------------------------------------------------------------------------------
function(__octk_internal_defer_promote_targets_in_dir_scope_to_global)
    get_directory_property(is_deferred _octk_promote_targets_is_deferred)
    if(NOT is_deferred)
        set_property(DIRECTORY PROPERTY _octk_promote_targets_is_deferred TRUE)

        if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.19)
            cmake_language(DEFER CALL __octk_internal_promote_targets_in_dir_scope_to_global_checked)
        endif()
    endif()
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
#-----------------------------------------------------------------------------------------------------------------------
function(octk_internal_set_up_static_runtime_library target)
    if(OCTK_MSVC_STATIC_RUNTIME)
        if(MSVC)
            set_property(TARGET ${target} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        elseif(MINGW)
            target_link_options(${target} INTERFACE "LINKER:-Bstatic")
        endif()
    endif()
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
#-----------------------------------------------------------------------------------------------------------------------
function(octk_internal_set_iterator_debug_level target level)
    if(MSVC)
        target_compile_definitions(${target} PRIVATE _ITERATOR_DEBUG_LEVEL=${level})
    endif()
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
# octk_copy_target_external_dependent_libraries
#-----------------------------------------------------------------------------------------------------------------------
function(octk_copy_target_external_dependent_libraries)
    set(multiValueArgs LIBS)
    set(oneValueArgs TARGET SOURCE DESTINATION)
    set(options PRE_BUILD PRE_LINK POST_BUILD VERBOSE MATCHING)
    cmake_parse_arguments(arg "${options}" "${oneValueArgs}" "${multiValueArgs}" "${ARGN}")

    set(HAPPENS)
    if(arg_PRE_BUILD)
        list(APPEND HAPPENS PRE_BUILD)
    endif()
    if(arg_PRE_LINK)
        list(APPEND HAPPENS PRE_LINK)
    endif()
    if(arg_POST_BUILD)
        list(APPEND HAPPENS POST_BUILD)
    endif()

    foreach(child ${arg_LIBS})
        find_path(LIB_PATH ${arg_SOURCE}/${child} ${arg_SOURCE})
        if(NOT LIB_PATH EQUAL "LIB_PATH-NOTFOUND")
            if(arg_MATCHING)
                add_custom_command(
                    TARGET ${arg_TARGET}
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${arg_SOURCE}/${child}*" ${arg_DESTINATION}
                    COMMENT "copy the depends external lib file ${child} to the ${arg_DESTINATION} folder"
                    ${HAPPENS})
            else()
                add_custom_command(
                    TARGET ${arg_TARGET}
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${arg_SOURCE}/${child}" ${arg_DESTINATION}
                    COMMENT "copy the depends external lib file ${child} to the ${arg_DESTINATION} folder"
                    ${HAPPENS})
            endif()
            if(arg_VERBOSE)
                message(STATUS "Add command to copy external dependency ${arg_SOURCE}/${child}* to ${arg_DESTINATION} with ${HAPPENS} happen")
            endif()
        else()
            message(FATAL_ERROR "Could not find ${arg_SOURCE}/${child} lib file")
        endif()
    endforeach()
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
# octk_copy_target_internal_dependent_libraries
#-----------------------------------------------------------------------------------------------------------------------
function(octk_copy_target_internal_dependent_libraries)
    set(multiValueArgs LIBS)
    set(oneValueArgs TARGET DESTINATION)
    set(options PRE_BUILD PRE_LINK POST_BUILD VERBOSE MATCHING)
    cmake_parse_arguments(arg "${options}" "${oneValueArgs}" "${multiValueArgs}" "${ARGN}")

    set(HAPPENS)
    if(arg_PRE_BUILD)
        list(APPEND HAPPENS PRE_BUILD)
    endif()
    if(arg_PRE_LINK)
        list(APPEND HAPPENS PRE_LINK)
    endif()
    if(arg_POST_BUILD)
        list(APPEND HAPPENS POST_BUILD)
    endif()

    foreach(child ${arg_LIBS})
        if(arg_MATCHING)
            add_custom_command(
                TARGET ${arg_TARGET}
                COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_LINKER_FILE:${child}>*" ${arg_DESTINATION}
                COMMENT "copy the depends internal lib ${child} to the ${arg_DESTINATION} folder"
                ${HAPPENS})
        else()
            add_custom_command(
                TARGET ${arg_TARGET}
                COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${child}> ${arg_DESTINATION}
                COMMENT "copy the depends internal lib ${child} to the ${arg_DESTINATION} folder"
                ${HAPPENS})
        endif()
        if(arg_VERBOSE)
            message(STATUS "Add command to copy internal dependency ${child} to ${arg_DESTINATION} with ${HAPPENS} happen")
        endif()
    endforeach()
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
# Adds warnings compiler options to the target depending on the category target Target name
#-----------------------------------------------------------------------------------------------------------------------
function(octk_set_target_compiler_warnings target)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(WARNINGS "-Werror" "-Wall")
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(WARNINGS "-Werror" "-Wall")
    elseif(MSVC)
        set(WARNINGS "/WX" "/W4")
    endif()
    target_compile_options(${target} PRIVATE ${WARNINGS})
endfunction()
