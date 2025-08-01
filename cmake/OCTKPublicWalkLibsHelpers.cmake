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
# Add libraries to variable ${out_libs_var} in a way that duplicates are added at the end. This ensures the library
# order needed for the linker.
#-----------------------------------------------------------------------------------------------------------------------
function(__octk_internal_merge_libs out_libs_var)
    foreach(dep ${ARGN})
        list(REMOVE_ITEM ${out_libs_var} ${dep})
        list(APPEND ${out_libs_var} ${dep})
    endforeach()
    set(${out_libs_var} ${${out_libs_var}} PARENT_SCOPE)
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
# Walks a target's public link libraries recursively, and performs some actions (poor man's polypmorphism)
#
# Walks INTERFACE_LINK_LIBRARIES for all target types, as well as LINK_LIBRARIES for static library targets.
#
# out_var: the name of the variable where the result will be assigned. The result is a list of
#          libraries, mostly in generator expression form.
# rcc_objects_out_var: the name of the variable where the collected rcc object files will be
#                      assigned (for the initial target and its dependencies)
# dict_name: used for caching the results, and preventing the same target from being processed
#            twice
# operation: a string to tell the function what additional behaviors to execute.
#            'collect_libs' (default) operation is to collect linker file paths and flags.
#                           Used for prl file generation.
#            'promote_global' promotes walked imported targets to global scope.
#            'collect_targets' collects all target names (discards framework or link flags)
#            'direct_targets' collects only the direct target names (discards framework or link
#                             flags)
#
#-----------------------------------------------------------------------------------------------------------------------
function(__octk_internal_walk_libs target out_var rcc_objects_out_var dict_name operation)
    set(collected ${ARGN})
    if(target IN_LIST collected)
        return()
    endif()
    list(APPEND collected ${target})

    if(operation MATCHES "^direct")
        set(direct TRUE)
    else()
        set(direct FALSE)
    endif()

    if(target STREQUAL "${OCTK_CMAKE_EXPORT_NAMESPACE}::EntryPointPrivate")
        # We can't (and don't need to) process EntryPointPrivate because it brings in
        # $<TARGET_PROPERTY:prop> genexes which get replaced with
        # $<TARGET_PROPERTY:EntryPointPrivate,prop> genexes in the code below and that causes
        # 'INTERFACE_LIBRARY targets may only have whitelisted properties.' errors with CMake
        # versions equal to or lower than 3.18. These errors are super unintuitive to debug
        # because there's no mention that it's happening during a file(GENERATE) call.
        return()
    endif()

    if(NOT TARGET ${dict_name})
        add_library(${dict_name} INTERFACE IMPORTED GLOBAL)
    endif()
    __octk_internal_get_dict_key_values(libs "${target}" "${dict_name}" "libs")
    __octk_internal_get_dict_key_values(rcc_objects "${target}" "${dict_name}" "rcc_objects")

    if(libs MATCHES "-NOTFOUND$")
        unset(libs)
        unset(rcc_objects)
        get_target_property(target_libs ${target} INTERFACE_LINK_LIBRARIES)
        if(NOT target_libs)
            unset(target_libs)
        endif()

        get_target_property(target_type ${target} TYPE)
        if(target_type STREQUAL "STATIC_LIBRARY")
            get_target_property(link_libs ${target} LINK_LIBRARIES)
            if(link_libs)
                list(APPEND target_libs ${link_libs})
            endif()
        endif()

        # Need to record the rcc object file info not only for dependencies, but also for
        # the current target too. Otherwise the saved information is incomplete for prl static
        # build purposes.
        get_target_property(main_target_rcc_objects ${target} _octk_rcc_objects)
        if(main_target_rcc_objects)
            __octk_internal_merge_libs(rcc_objects ${main_target_rcc_objects})
        endif()

        foreach(lib ${target_libs})
            # Cannot use $<TARGET_POLICY:...> in add_custom_command.
            # Check the policy now, and replace the generator expression with the value.
            while(lib MATCHES "\\$<TARGET_POLICY:([^>]+)>")
                cmake_policy(GET ${CMAKE_MATCH_1} value)
                if(value STREQUAL "NEW")
                    set(value "TRUE")
                else()
                    set(value "FALSE")
                endif()
                string(REPLACE "${CMAKE_MATCH_0}" "${value}" lib "${lib}")
            endwhile()

            # Fix up $<TARGET_PROPERTY:FOO> expressions that refer to the "current" target.
            # Those cannot be used with add_custom_command.
            while(lib MATCHES "\\$<TARGET_PROPERTY:([^,>]+)>")
                string(REPLACE "${CMAKE_MATCH_0}" "$<TARGET_PROPERTY:${target},${CMAKE_MATCH_1}>" lib "${lib}")
            endwhile()

            # Skip static plugins.
            set(_is_plugin_marker_genex "\\$<BOOL:OCTK_IS_PLUGIN_GENEX>")
            if(lib MATCHES "${_is_plugin_marker_genex}")
                continue()
            endif()

            # Skip octk libs.
            if(lib MATCHES "octk*")
                continue()
            endif()

            # Skip optional dependencies for now. They are likely to be handled manually for prl
            # file purposes (like nolink handling). And for one of the other operations, we don't
            # have a use case yet. This might be revisited.
            if(lib MATCHES "^\\$<TARGET_NAME_IF_EXISTS:")
                continue()
            endif()

            # Strip any directory scope tokens.
            octk_internal_strip_target_directory_scope_token("${lib}" lib)

            if(lib MATCHES "^\\$<TARGET_OBJECTS:")
                # Skip object files.
                continue()
            elseif(lib MATCHES "^\\$<LINK_ONLY:(.*)>$")
                set(lib_target ${CMAKE_MATCH_1})
            else()
                set(lib_target ${lib})
            endif()

            # Skip CMAKE_DIRECTORY_ID_SEP. If a target_link_libraries is applied to a target
            # that was defined in a different scope, CMake appends and prepends a special directory
            # id separator. Filter those out.
            if(lib_target MATCHES "^::@")
                continue()
            elseif(TARGET ${lib_target})
                if("${lib_target}" MATCHES "^OpenCTK::(.*)")
                    # If both, OpenCTK::foo and Foo targets exist, prefer the target name without
                    # namespace. Which one is preferred doesn't really matter. This code exists to
                    # avoid ending up with both, OpenCTK::foo and Foo in our dependencies.
                    set(namespaceless_lib_target "${CMAKE_MATCH_1}")
                    if(TARGET namespaceless_lib_target)
                        set(lib_target ${namespaceless_lib_target})
                    endif()
                endif()
                get_target_property(lib_target_type ${lib_target} TYPE)
                if(lib_target_type STREQUAL "INTERFACE_LIBRARY")
                    if(NOT ${direct})
                        __octk_internal_walk_libs(
                            ${lib_target}
                            lib_libs_${target}
                            lib_rcc_objects_${target}
                            "${dict_name}" "${operation}" ${collected})
                        if(lib_libs_${target})
                            __octk_internal_merge_libs(libs ${lib_libs_${target}})
                            set(is_module 0)
                        endif()
                        if(lib_rcc_objects_${target})
                            __octk_internal_merge_libs(rcc_objects ${lib_rcc_objects_${target}})
                        endif()
                    else()
                        __octk_internal_merge_libs(libs ${lib})
                    endif()
                elseif(NOT lib_target_type STREQUAL "OBJECT_LIBRARY")

                    if(operation MATCHES "^(collect|direct)_targets$")
                        __octk_internal_merge_libs(libs ${lib_target})
                    else()
                        __octk_internal_merge_libs(libs "$<TARGET_LINKER_FILE:${lib_target}>")
                    endif()

                    get_target_property(target_rcc_objects "${lib_target}" _octk_rcc_objects)
                    if(target_rcc_objects)
                        __octk_internal_merge_libs(rcc_objects ${target_rcc_objects})
                    endif()

                    if(NOT ${direct})
                        __octk_internal_walk_libs(
                            ${lib_target}
                            lib_libs_${target}
                            lib_rcc_objects_${target}
                            "${dict_name}" "${operation}" ${collected})
                    endif()
                    if(lib_libs_${target})
                        __octk_internal_merge_libs(libs ${lib_libs_${target}})
                    endif()
                    if(lib_rcc_objects_${target})
                        __octk_internal_merge_libs(rcc_objects ${lib_rcc_objects_${target}})
                    endif()
                endif()
                if(operation STREQUAL "promote_global")
                    set(lib_target_unaliased "${lib_target}")
                    get_target_property(aliased_target ${lib_target} ALIASED_TARGET)
                    if(aliased_target)
                        set(lib_target_unaliased ${aliased_target})
                    endif()

                    get_property(is_imported TARGET ${lib_target_unaliased} PROPERTY IMPORTED)

                    # Allow opting out of promotion. This is useful in certain corner cases
                    # like with WrapLibClang and Threads in octktools.
                    octk_internal_should_not_promote_package_target_to_global(
                        "${lib_target_unaliased}" should_not_promote)
                    if(is_imported AND NOT should_not_promote)
                        __octk_internal_promote_target_to_global(${lib_target_unaliased})
                    endif()
                endif()
            elseif("${lib_target}" MATCHES "^OpenCTK::(.*)")
                message(FATAL_ERROR "The ${CMAKE_MATCH_1} target is mentioned as a dependency for \
                    ${target}, but not declared.")
            else()
                if(NOT operation MATCHES "^(collect|direct)_targets$")
                    set(final_lib_name_to_merge "${lib_target}")
                    if(lib_target MATCHES "/([^/]+).framework$")
                        set(final_lib_name_to_merge "-framework ${CMAKE_MATCH_1}")
                    endif()
                    __octk_internal_merge_libs(libs "${final_lib_name_to_merge}")
                endif()
            endif()
        endforeach()
        __octk_internal_memoize_values_in_dict("${target}" "${dict_name}" "libs" "${libs}")
        __octk_internal_memoize_values_in_dict("${target}" "${dict_name}" "rcc_objects" "${rcc_objects}")

    endif()
    set(${out_var} ${libs} PARENT_SCOPE)
    set(${rcc_objects_out_var} ${rcc_objects} PARENT_SCOPE)
endfunction()


# Extracts value from per-target dict key and assigns it to out_var.
# Assumes dict_name to be an existing INTERFACE target.
function(__octk_internal_get_dict_key_values out_var target_infix dict_name dict_key)
    get_target_property(values "${dict_name}" "INTERFACE_${target_infix}_${dict_key}")
    set(${out_var} "${values}" PARENT_SCOPE)
endfunction()


# Assigns 'values' to per-target dict key, including for aliases of the target.
# Assumes dict_name to be an existing INTERFACE target.
function(__octk_internal_memoize_values_in_dict target dict_name dict_key values)
    # Memoize the computed values for the target as well as its aliases.
    #
    # Aka assigns the contents of ${values} to OpenCTK, octk::core,
    # octk::core.
    #
    # Yes, i know it's crazy that target names are legal property names.
    #
    # Assigning for library aliases is needed to avoid multiple recomputation of values.
    # Scenario in the context of __octk_internal_walk_libs:
    # 'values' are computed for Core target and memoized to INTERFACE_Core.
    # When processing Gui, it depends on octk::core, but there are no values for INTERFACE_OCTK::Core.
    set_target_properties(${dict_name} PROPERTIES INTERFACE_${target}_${dict_key} "${values}")

    get_target_property(versionless_alias "${target}" "_octk_versionless_alias")
    if(versionless_alias)
        __octk_internal_get_dict_key_values(
            versionless_values "${versionless_alias}" "${dict_name}" "${dict_key}")
        if(versionless_values MATCHES "-NOTFOUND$")
            set_target_properties(${dict_name}
                PROPERTIES INTERFACE_${versionless_alias}_${dict_key} "${values}")
        endif()
    endif()

    get_target_property(versionfull_alias "${target}" "_octk_versionfull_alias")
    if(versionfull_alias)
        __octk_internal_get_dict_key_values(
            versionfull_values "${versionfull_alias}" "${dict_name}" "${dict_key}")
        if(versionfull_values MATCHES "-NOTFOUND$")
            set_target_properties(${dict_name}
                PROPERTIES INTERFACE_${versionfull_alias}_${dict_key} "${values}")
        endif()
    endif()
endfunction()
