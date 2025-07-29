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
# This function creates a CMake target for a generic console or GUI binary.
# Please consider to use a more specific version target like the one created
# by octk_add_test or octk_add_tool below.
#-----------------------------------------------------------------------------------------------------------------------
function(octk_add_executable name)
    octk_parse_all_arguments(arg "octk_add_executable"
        "${OCTK_INTERNAL_ADD_EXECUTABLE_OPTIONAL_ARGS}"
        "${OCTK_INTERNAL_ADD_EXECUTABLE_SINGLE_ARGS}"
        "${OCTK_INTERNAL_ADD_EXECUTABLE_MULTI_ARGS}"
        ${ARGN})

    if("x${arg_OUTPUT_DIRECTORY}" STREQUAL "x")
        set(arg_OUTPUT_DIRECTORY "${OCTK_BUILD_DIR}/${OCTK_INSTALL_BINDIR}")
    endif()

    get_filename_component(arg_OUTPUT_DIRECTORY "${arg_OUTPUT_DIRECTORY}"
        ABSOLUTE BASE_DIR "${OCTK_BUILD_DIR}")

    if("x${arg_INSTALL_DIRECTORY}" STREQUAL "x")
        set(arg_INSTALL_DIRECTORY "${OCTK_INSTALL_BINDIR}")
    endif()

    octk_internal_create_executable(${name})
    if(ANDROID)
        octk_internal_android_executable_finalizer(${name})
    endif()

    if(arg_OCTK_APP AND OCTK_FEATURE_DEBUG_AND_RELEASE AND CMAKE_VERSION VERSION_GREATER_EQUAL "3.19.0")
        set_property(TARGET "${name}"
            PROPERTY EXCLUDE_FROM_ALL "$<NOT:$<CONFIG:${OCTK_MULTI_CONFIG_FIRST_CONFIG}>>")
    endif()

    if(WASM)
        octk_internal_wasm_add_finalizers("${name}")
        octk_internal_wasm_add_target_helpers("${name}")
    endif()

    if(arg_VERSION)
        if(arg_VERSION MATCHES "[0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+")
            # nothing to do
        elseif(arg_VERSION MATCHES "[0-9]+\\.[0-9]+\\.[0-9]+")
            set(arg_VERSION "${arg_VERSION}.0")
        elseif(arg_VERSION MATCHES "[0-9]+\\.[0-9]+")
            set(arg_VERSION "${arg_VERSION}.0.0")
        elseif(arg_VERSION MATCHES "[0-9]+")
            set(arg_VERSION "${arg_VERSION}.0.0.0")
        else()
            message(FATAL_ERROR "Invalid version format")
        endif()
    endif()

    if(arg_DELAY_TARGET_INFO)
        # Delay the setting of target info properties if requested. Needed for scope finalization of OpenCTK apps.
        set_target_properties("${name}" PROPERTIES
            OCTK_DELAYED_TARGET_VERSION "${arg_VERSION}"
            OCTK_DELAYED_TARGET_PRODUCT "${arg_TARGET_PRODUCT}"
            OCTK_DELAYED_TARGET_DESCRIPTION "${arg_TARGET_DESCRIPTION}"
            OCTK_DELAYED_TARGET_COMPANY "${arg_TARGET_COMPANY}"
            OCTK_DELAYED_TARGET_COPYRIGHT "${arg_TARGET_COPYRIGHT}")
    else()
        if("${arg_TARGET_DESCRIPTION}" STREQUAL "")
            set(arg_TARGET_DESCRIPTION "OpenCTK ${name}")
        endif()
        octk_set_target_info_properties(${name} ${ARGN}
            TARGET_DESCRIPTION "${arg_TARGET_DESCRIPTION}"
            TARGET_VERSION "${arg_VERSION}")
    endif()

    if(WIN32 AND NOT arg_DELAY_RC)
        octk_internal_generate_win32_rc_file(${name})
    endif()

    octk_set_common_target_properties(${name})
    octk_internal_add_repo_local_defines(${name})

    if(ANDROID)
        # The above call to octk_set_common_target_properties() sets the symbol
        # visibility to hidden, but for Android, we need main() to not be hidden
        # because it has to be loadable at runtime using dlopen().
        set_property(TARGET ${name} PROPERTY C_VISIBILITY_PRESET default)
        set_property(TARGET ${name} PROPERTY CXX_VISIBILITY_PRESET default)
    endif()

    octk_skip_warnings_are_errors_when_repo_unclean("${name}")

    set(private_includes
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_BINARY_DIR}"
        ${arg_INCLUDE_DIRECTORIES})

    octk_internal_extend_target("${name}"
        SOURCES ${arg_SOURCES}
        INCLUDE_DIRECTORIES ${private_includes}
        DEFINES ${arg_DEFINES}
        LIBRARIES ${arg_LIBRARIES}
        PUBLIC_LIBRARIES ${extra_libraries} ${arg_PUBLIC_LIBRARIES}
        COMPILE_OPTIONS ${arg_COMPILE_OPTIONS}
        LINK_OPTIONS ${arg_LINK_OPTIONS})
    set_target_properties("${name}" PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${arg_OUTPUT_DIRECTORY}"
        LIBRARY_OUTPUT_DIRECTORY "${arg_OUTPUT_DIRECTORY}"
        WIN32_EXECUTABLE "${arg_GUI}"
        MACOSX_BUNDLE "${arg_GUI}")

    octk_internal_set_exceptions_flags("${name}" ${arg_EXCEPTIONS})

    # Check if target needs to be excluded from all target. Also affects octk_install.
    # Set by octk_exclude_tool_directories_from_default_target.
    set(exclude_from_all FALSE)
    if(__octk_exclude_tool_directories)
        foreach(absolute_dir ${__octk_exclude_tool_directories})
            string(FIND "${CMAKE_CURRENT_SOURCE_DIR}" "${absolute_dir}" dir_starting_pos)
            if(dir_starting_pos EQUAL 0)
                set(exclude_from_all TRUE)
                set_target_properties("${name}" PROPERTIES EXCLUDE_FROM_ALL TRUE)
                break()
            endif()
        endforeach()
    endif()

    if(arg_INSTALL)
        set(additional_install_args "")
        if(exclude_from_all)
            list(APPEND additional_install_args EXCLUDE_FROM_ALL COMPONENT "ExcludedExecutables")
        endif()

        octk_get_cmake_configurations(cmake_configs)
        foreach(cmake_config ${cmake_configs})
            octk_get_install_target_default_args(
                OUT_VAR install_targets_default_args
                CMAKE_CONFIG "${cmake_config}"
                ALL_CMAKE_CONFIGS "${cmake_configs}"
                RUNTIME "${arg_INSTALL_DIRECTORY}"
                LIBRARY "${arg_INSTALL_DIRECTORY}"
                BUNDLE "${arg_INSTALL_DIRECTORY}")

            # Make installation optional for targets that are not built by default in this config
            if(NOT exclude_from_all AND arg_OCTK_APP AND OCTK_FEATURE_DEBUG_AND_RELEASE
                AND NOT (cmake_config STREQUAL OCTK_MULTI_CONFIG_FIRST_CONFIG))
                set(install_optional_arg "OPTIONAL")
            else()
                unset(install_optional_arg)
            endif()

            octk_install(TARGETS "${name}"
                ${additional_install_args} # Needs to be before the DESTINATIONS.
                ${install_optional_arg}
                CONFIGURATIONS ${cmake_config}
                ${install_targets_default_args})
        endforeach()

        if(NOT exclude_from_all AND arg_OCTK_APP AND OCTK_FEATURE_DEBUG_AND_RELEASE)
            set(separate_debug_info_executable_arg "OCTK_EXECUTABLE")
        else()
            unset(separate_debug_info_executable_arg)
        endif()
        octk_enable_separate_debug_info(${name} "${arg_INSTALL_DIRECTORY}"
            ${separate_debug_info_executable_arg}
            ADDITIONAL_INSTALL_ARGS ${additional_install_args})
        octk_internal_install_pdb_files(${name} "${arg_INSTALL_DIRECTORY}")

        if(BUILD_SHARED_LIBS)
            octk_apply_rpaths(TARGET "${name}" INSTALL_PATH "${OCTK_INSTALL_LIBDIR}" RELATIVE_RPATH)
            octk_internal_apply_staging_prefix_build_rpath_workaround()
        endif()
    endif()

    # If linking against Gui, make sure to also build the default PA(Platform Abstraction) plugin.
    # This makes the experience of an initial OpenCTK configuration to build and run one single test / executable nicer.
    #    get_target_property(linked_libs "${name}" LINK_LIBRARIES)
    #    if("octk::gui" IN_LIST linked_libs AND TARGET pa_default_plugins)
    #        add_dependencies("${name}" pa_default_plugins)
    #    endif()

    if(NOT BUILD_SHARED_LIBS)
        # For static builds, we need to explicitly link to plugins we want to be
        # loaded with the executable. User projects get that automatically, but
        # for tools built as part of OpenCTK, we can't use that mechanism because it
        # would pollute the targets we export as part of an install and lead to
        # circular dependencies. The logic here is a simpler equivalent of the
        # more dynamic logic in OCTKPlugins.cmake.in, but restricted to only
        # adding plugins that are provided by the same module as the module
        # libraries the executable links to.
        set(libs
            ${arg_LIBRARIES}
            ${arg_PUBLIC_LIBRARIES}
            ${extra_libraries})

        set(deduped_libs "")
        foreach(lib IN LISTS libs)
            if(NOT TARGET "${lib}")
                continue()
            endif()

            # Normalize module by stripping any leading "octk::", because properties are set on the
            # versioned target (either Gui when building the module, or OCTK6::Gui when it's
            # imported).
            if(lib MATCHES "octk::([-_A-Za-z0-9]+)")
                set(new_lib "${OCTK_CMAKE_EXPORT_NAMESPACE}::${CMAKE_MATCH_1}")
                if(TARGET "${new_lib}")
                    set(lib "${new_lib}")
                endif()
            endif()

            # Unalias the target.
            get_target_property(aliased_target ${lib} ALIASED_TARGET)
            if(aliased_target)
                set(lib ${aliased_target})
            endif()

            list(APPEND deduped_libs "${lib}")
        endforeach()

        list(REMOVE_DUPLICATES deduped_libs)

        foreach(lib IN LISTS deduped_libs)
            string(MAKE_C_IDENTIFIER "${name}_plugin_imports_${lib}" out_file)
            string(APPEND out_file .cpp)

            # Initialize plugins that are built in the same repository as the OpenCTK module 'lib'.
            set(class_names_regular "$<GENEX_EVAL:$<TARGET_PROPERTY:${lib},_octk_initial_repo_plugin_class_names>>")

            # Initialize plugins that are built in the current OpenCTK repository, but are associated with a OpenCTK libraries
            # from a different repository .
            string(MAKE_C_IDENTIFIER "${PROJECT_NAME}" current_project_name)
            set(prop_prefix "_octk_repo_${current_project_name}")
            set(class_names_current_project "$<GENEX_EVAL:$<TARGET_PROPERTY:${lib},${prop_prefix}_plugin_class_names>>")

            # Only add separator if first list is not empty, so we don't trigger the file generation when all lists are empty.
            set(class_names_separator "$<$<NOT:$<STREQUAL:${class_names_regular},>>:;>")
            set(class_names "${class_names_regular}${class_names_separator}${class_names_current_project}")

            set(out_file_path "${CMAKE_CURRENT_BINARY_DIR}/${out_file}")

            # CMake versions earlier than 3.18.0 can't find the generated file for some reason,
            # failing at generation phase.
            # Explicitly marking the file as GENERATED fixes the issue.
            set_source_files_properties("${out_file_path}" PROPERTIES GENERATED TRUE)

            target_sources(${name} PRIVATE
                "$<$<NOT:$<STREQUAL:${class_names},>>:${out_file_path}>")
            target_link_libraries(${name} PRIVATE
                "$<TARGET_PROPERTY:${lib},_octk_initial_repo_plugins>"
                "$<TARGET_PROPERTY:${lib},${prop_prefix}_plugins>")
        endforeach()
    endif()
endfunction()


function(octk_internal_create_executable target)
    if(ANDROID)
        list(REMOVE_ITEM ARGN "WIN32" "MACOSX_BUNDLE")
        add_library("${target}" MODULE ${ARGN})
        # On our builds we do don't compile the executables with
        # visibility=hidden. Not having this flag set will cause the
        # executable to have main() hidden and can then no longer be loaded
        # through dlopen()
        set_property(TARGET "${target}" PROPERTY C_VISIBILITY_PRESET default)
        set_property(TARGET "${target}" PROPERTY CXX_VISIBILITY_PRESET default)
        set_property(TARGET "${target}" PROPERTY OBJC_VISIBILITY_PRESET default)
        set_property(TARGET "${target}" PROPERTY OBJCXX_VISIBILITY_PRESET default)
        octk_android_apply_arch_suffix("${target}")
        set_property(TARGET "${target}" PROPERTY _octk_is_android_executable TRUE)
    else()
        add_executable("${target}" ${ARGN})
    endif()
endfunction()