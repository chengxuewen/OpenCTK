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
function(octk_android_apply_arch_suffix target)
    get_target_property(target_type ${target} TYPE)
    if(target_type STREQUAL "SHARED_LIBRARY" OR target_type STREQUAL "MODULE_LIBRARY")
        set_property(TARGET "${target}" PROPERTY SUFFIX "_${CMAKE_ANDROID_ARCH_ABI}.so")
    elseif(target_type STREQUAL "STATIC_LIBRARY")
        set_property(TARGET "${target}" PROPERTY SUFFIX "_${CMAKE_ANDROID_ARCH_ABI}.a")
    endif()
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
# The function configures external projects for ABIs that target packages need to build with.
# Each target adds build step to the external project that is linked to the
# octk_internal_android_${abi}-${target}_build target in the primary ABI build tree.
#-----------------------------------------------------------------------------------------------------------------------
function(octk_internal_configure_android_multiabi_target target)
    # Functionality is only applicable for the primary ABI
    if(OCTK_IS_ANDROID_MULTI_ABI_EXTERNAL_PROJECT)
        return()
    endif()

    get_target_property(target_abis ${target} OCTK_ANDROID_ABIS)
    if(target_abis)
        # Use target-specific octk for Android ABIs.
        set(android_abis ${target_abis})
    elseif(OCTK_ANDROID_BUILD_ALL_ABIS)
        # Use autodetected octk for Android ABIs.
        set(android_abis ${OCTK_DEFAULT_ANDROID_ABIS})
    elseif(OCTK_ANDROID_ABIS)
        # Use project-wide octk for Android ABIs.
        set(android_abis ${OCTK_ANDROID_ABIS})
    else()
        # User have an empty list of octk for Android ABIs.
        message(FATAL_ERROR
            "The list of Android ABIs is empty, when building ${target}.\n"
            "You have the following options to select ABIs for a target:\n"
            " - Set the OCTK_ANDROID_ABIS variable before calling octk_add_executable\n"
            " - Set the ANDROID_ABIS property for ${target}\n"
            " - Set OCTK_ANDROID_BUILD_ALL_ABIS flag to try building with\n"
            "   the list of autodetected octk for Android:\n ${OCTK_DEFAULT_ANDROID_ABIS}"
            )
    endif()

    get_cmake_property(is_multi_config GENERATOR_IS_MULTI_CONFIG)
    if(is_multi_config)
        list(JOIN CMAKE_CONFIGURATION_TYPES "$<SEMICOLON>" escaped_configuration_types)
        set(config_arg "-DCMAKE_CONFIGURATION_TYPES=${escaped_configuration_types}")
    else()
        set(config_arg "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
    endif()

    unset(extra_cmake_args)

    # The flag is needed when building octk standalone tests only to avoid building
    # octk repo itself
    if(OCTK_BUILD_STANDALONE_TESTS)
        list(APPEND extra_cmake_args "-DOCTK_BUILD_STANDALONE_TESTS=ON")
    endif()

    if(NOT OCTK_ADDITIONAL_PACKAGES_PREFIX_PATH STREQUAL "")
        list(JOIN OCTK_ADDITIONAL_PACKAGES_PREFIX_PATH "$<SEMICOLON>" escaped_packages_prefix_path)
        list(APPEND extra_cmake_args
            "-DOCTK_ADDITIONAL_PACKAGES_PREFIX_PATH=${escaped_packages_prefix_path}")
    endif()

    if(NOT OCTK_ADDITIONAL_HOST_PACKAGES_PREFIX_PATH STREQUAL "")
        list(JOIN OCTK_ADDITIONAL_HOST_PACKAGES_PREFIX_PATH "$<SEMICOLON>"
            escaped_host_packages_prefix_path)
        list(APPEND extra_cmake_args
            "-DOCTK_ADDITIONAL_HOST_PACKAGES_PREFIX_PATH=${escaped_host_packages_prefix_path}")
    endif()

    if(ANDROID_SDK_ROOT)
        list(APPEND extra_cmake_args "-DANDROID_SDK_ROOT=${ANDROID_SDK_ROOT}")
    endif()

    # ANDROID_NDK_ROOT is invented by octk and is what the octk toolchain file expects
    if(ANDROID_NDK_ROOT)
        list(APPEND extra_cmake_args "-DANDROID_NDK_ROOT=${ANDROID_NDK_ROOT}")

        # ANDROID_NDK is passed by octk Creator and is also present in the android toolchain file.
    elseif(ANDROID_NDK)
        list(APPEND extra_cmake_args "-DANDROID_NDK_ROOT=${ANDROID_NDK}")
    endif()

    if(DEFINED OCTK_NO_PACKAGE_VERSION_CHECK)
        list(APPEND extra_cmake_args "-DOCTK_NO_PACKAGE_VERSION_CHECK=${OCTK_NO_PACKAGE_VERSION_CHECK}")
    endif()

    if(DEFINED OCTK_HOST_PATH_CMAKE_DIR)
        list(APPEND extra_cmake_args "-DOCTK_HOST_PATH_CMAKE_DIR=${OCTK_HOST_PATH_CMAKE_DIR}")
    endif()

    if(CMAKE_MAKE_PROGRAM)
        list(APPEND extra_cmake_args "-DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}")
    endif()

    if(CMAKE_C_COMPILER_LAUNCHER)
        list(JOIN CMAKE_C_COMPILER_LAUNCHER "$<SEMICOLON>"
            compiler_launcher)
        list(APPEND extra_cmake_args
            "-DCMAKE_C_COMPILER_LAUNCHER=${compiler_launcher}")
    endif()

    if(CMAKE_CXX_COMPILER_LAUNCHER)
        list(JOIN CMAKE_CXX_COMPILER_LAUNCHER "$<SEMICOLON>"
            compiler_launcher)
        list(APPEND extra_cmake_args
            "-DCMAKE_CXX_COMPILER_LAUNCHER=${compiler_launcher}")
    endif()

    unset(user_cmake_args)
    foreach(var IN LISTS OCTK_ANDROID_MULTI_ABI_FORWARD_VARS)
        string(REPLACE ";" "$<SEMICOLON>" var_value "${${var}}")
        list(APPEND user_cmake_args "-D${var}=${var_value}")
    endforeach()

    set(missing_octk_abi_toolchains "")
    set(previous_copy_apk_dependencies_target ${target})
    # Create external projects for each android ABI except the main one.
    list(REMOVE_ITEM android_abis "${CMAKE_ANDROID_ARCH_ABI}")
    include(ExternalProject)
    foreach(abi IN ITEMS ${android_abis})
        if(NOT "${abi}" IN_LIST OCTK_DEFAULT_ANDROID_ABIS)
            list(APPEND missing_octk_abi_toolchains ${abi})
            list(REMOVE_ITEM android_abis "${abi}")
            continue()
        endif()

        set(android_abi_build_dir "${CMAKE_BINARY_DIR}/android_abi_builds/${abi}")
        get_property(abi_external_projects GLOBAL
            PROPERTY _octk_internal_abi_external_projects)
        if(NOT abi_external_projects
            OR NOT "octk_internal_android_${abi}" IN_LIST abi_external_projects)
            _octk_internal_get_android_abi_path(octk_abi_path ${abi})
            set(octk_abi_toolchain_path
                "${octk_abi_path}/lib/cmake/${OCTK_CMAKE_EXPORT_NAMESPACE}/octk.toolchain.cmake")
            ExternalProject_Add("octk_internal_android_${abi}"
                SOURCE_DIR "${CMAKE_SOURCE_DIR}"
                BINARY_DIR "${android_abi_build_dir}"
                CONFIGURE_COMMAND
                "${CMAKE_COMMAND}"
                "-G${CMAKE_GENERATOR}"
                "-DCMAKE_TOOLCHAIN_FILE=${octk_abi_toolchain_path}"
                "-DOCTK_HOST_PATH=${OCTK_HOST_PATH}"
                "-DOCTK_IS_ANDROID_MULTI_ABI_EXTERNAL_PROJECT=ON"
                "-DOCTK_INTERNAL_ANDROID_MULTI_ABI_BINARY_DIR=${CMAKE_BINARY_DIR}"
                "${config_arg}"
                "${extra_cmake_args}"
                "${user_cmake_args}"
                "-B" "${android_abi_build_dir}"
                "-S" "${CMAKE_SOURCE_DIR}"
                EXCLUDE_FROM_ALL TRUE
                BUILD_COMMAND "" # avoid top-level build of external project
                )
            set_property(GLOBAL APPEND PROPERTY
                _octk_internal_abi_external_projects "octk_internal_android_${abi}")
        endif()
        ExternalProject_Add_Step("octk_internal_android_${abi}"
            "${target}_build"
            DEPENDEES configure
            # TODO: Remove this when the step will depend on DEPFILE generated by
            # androiddeployoctk for the ${target}.
            ALWAYS TRUE
            EXCLUDE_FROM_MAIN TRUE
            COMMAND "${CMAKE_COMMAND}"
            "--build" "${android_abi_build_dir}"
            "--config" "$<CONFIG>"
            "--target" "${target}"
            )
        ExternalProject_Add_StepTargets("octk_internal_android_${abi}"
            "${target}_build")
        add_dependencies(${target} "octk_internal_android_${abi}-${target}_build")

        ExternalProject_Add_Step("octk_internal_android_${abi}"
            "${target}_copy_apk_dependencies"
            DEPENDEES "${target}_build"
            # TODO: Remove this when the step will depend on DEPFILE generated by
            # androiddeployoctk for the ${target}.
            ALWAYS TRUE
            EXCLUDE_FROM_MAIN TRUE
            COMMAND "${CMAKE_COMMAND}"
            "--build" "${android_abi_build_dir}"
            "--config" "$<CONFIG>"
            "--target" "octk_internal_${target}_copy_apk_dependencies"
            )
        ExternalProject_Add_StepTargets("octk_internal_android_${abi}"
            "${target}_copy_apk_dependencies")
        set(external_project_copy_target
            "octk_internal_android_${abi}-${target}_copy_apk_dependencies")

        # Need to build dependency chain between the
        # octk_internal_android_${abi}-${target}_copy_apk_dependencies targets for all ABI's, to
        # prevent parallel execution of androiddeployoctk processes. We cannot use Ninja job pools
        # here because it's not possible to define job pool for the step target in ExternalProject.
        # All tricks with interlayer targets don't work, because we only can bind interlayer target
        # to the job pool, but its dependencies can still be built in parallel.
        add_dependencies(${previous_copy_apk_dependencies_target}
            "${external_project_copy_target}")
        set(previous_copy_apk_dependencies_target "${external_project_copy_target}")
    endforeach()

    if(missing_octk_abi_toolchains)
        list(JOIN missing_octk_abi_toolchains ", " missing_octk_abi_toolchains_string)
        message(FATAL_ERROR "Cannot find toolchain files for the manually specified Android"
            " ABIs: ${missing_octk_abi_toolchains_string}"
            "\nNote that you also may manually specify the path to the required octk for"
            " Android ABI using OCTK_PATH_ANDROID_ABI_<abi> CMake variable.\n")
    endif()

    list(JOIN android_abis ", " android_abis_string)
    if(android_abis_string)
        set(android_abis_string "${CMAKE_ANDROID_ARCH_ABI} (default), ${android_abis_string}")
    else()
        set(android_abis_string "${CMAKE_ANDROID_ARCH_ABI} (default)")
    endif()
    if(NOT OCTK_NO_ANDROID_ABI_STATUS_MESSAGE)
        message(STATUS "Configuring '${target}' for the following Android ABIs:"
            " ${android_abis_string}")
    endif()
    set_target_properties(${target} PROPERTIES _octk_android_abis "${android_abis}")
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
# Generate the deployment settings json file for a cmake target.
#-----------------------------------------------------------------------------------------------------------------------
function(octk_android_generate_deployment_settings target)
    # Information extracted from mkspecs/features/android/android_deployment_settings.prf
    if(NOT TARGET ${target})
        message(FATAL_ERROR "${target} is not a cmake target")
    endif()

    # When parsing JSON file format backslashes and follow up symbols are regarded as special
    # characters. This puts Windows path format into a trouble.
    # _octk_internal_android_format_deployment_paths converts sensitive paths to the CMake format
    # that is supported by JSON as well. The function should be called as many times as
    # octk6_android_generate_deployment_settings, because users may change properties that contain
    # paths in between the calls.
    _octk_internal_android_format_deployment_paths(${target})

    # Avoid calling the function body twice because of 'file(GENERATE'.
    get_target_property(is_called ${target} _octk_is_android_generate_deployment_settings_called)
    if(is_called)
        return()
    endif()
    set_target_properties(${target} PROPERTIES
        _octk_is_android_generate_deployment_settings_called TRUE
        )

    get_target_property(target_type ${target} TYPE)

    if(NOT "${target_type}" STREQUAL "MODULE_LIBRARY")
        message(SEND_ERROR "OCTK_ANDROID_GENERATE_DEPLOYMENT_SETTINGS only works on Module targets")
        return()
    endif()

    get_target_property(target_source_dir ${target} SOURCE_DIR)
    get_target_property(target_binary_dir ${target} BINARY_DIR)
    get_target_property(target_output_name ${target} OUTPUT_NAME)
    if(NOT target_output_name)
        set(target_output_name ${target})
    endif()

    # QtCreator requires the file name of deployment settings has no config related suffixes
    # to run androiddeployoctk correctly. If we use multi-config generator for the first config
    # in a list avoid adding any configuration-specific suffixes.
    get_cmake_property(is_multi_config GENERATOR_IS_MULTI_CONFIG)
    if(is_multi_config)
        list(GET CMAKE_CONFIGURATION_TYPES 0 first_config_type)
        set(config_suffix "$<$<NOT:$<CONFIG:${first_config_type}>>:-$<CONFIG>>")
    endif()
    set(deploy_file
        "${target_binary_dir}/android-${target_output_name}-deployment-settings${config_suffix}.json")

    set(file_contents "{\n")
    # content begin
    string(APPEND file_contents
        "   \"description\": \"This file is generated by cmake to be read by androiddeployoctk and should not be modified by hand.\",\n")

    # Host octk Android install path
    if(NOT OCTK_BUILDING_OCTK OR OCTK_STANDALONE_TEST_PATH)
        set(octk_path "${OCTK_INSTALL_PREFIX}")
        set(android_plugin_dir_path "${octk_path}/${OCTK6_INSTALL_PLUGINS}/platforms")
        set(glob_expression "${android_plugin_dir_path}/*octkforandroid*${CMAKE_ANDROID_ARCH_ABI}.so")
        file(GLOB plugin_dir_files LIST_DIRECTORIES FALSE "${glob_expression}")
        if(NOT plugin_dir_files)
            message(SEND_ERROR
                "Detected octk installation does not contain octkforandroid_${CMAKE_ANDROID_ARCH_ABI}.so in the following dir:\n"
                "${android_plugin_dir_path}\n"
                "This is most likely due to the installation not being a octk for Android build. "
                "Please recheck your build configuration.")
            return()
        else()
            list(GET plugin_dir_files 0 android_platform_plugin_path)
            message(STATUS "Found android platform plugin at: ${android_platform_plugin_path}")
        endif()
    endif()

    set(abi_records "")
    get_target_property(octk_android_abis ${target} _octk_android_abis)
    if(NOT octk_android_abis)
        set(octk_android_abis "")
    endif()
    foreach(abi IN LISTS octk_android_abis)
        _octk_internal_get_android_abi_path(octk_abi_path ${abi})
        file(TO_CMAKE_PATH "${octk_abi_path}" octk_android_install_dir_native)
        list(APPEND abi_records "\"${abi}\": \"${octk_android_install_dir_native}\"")
    endforeach()

    # Required to build unit tests in developer build
    if(OCTK_BUILD_INTERNALS_RELOCATABLE_INSTALL_PREFIX)
        set(octk_android_install_dir "${OCTK_BUILD_INTERNALS_RELOCATABLE_INSTALL_PREFIX}")
    else()
        set(octk_android_install_dir "${OCTK_INSTALL_PREFIX}")
    endif()
    file(TO_CMAKE_PATH "${octk_android_install_dir}" octk_android_install_dir_native)
    list(APPEND abi_records "\"${CMAKE_ANDROID_ARCH_ABI}\": \"${octk_android_install_dir_native}\"")

    list(JOIN abi_records "," octk_android_install_dir_records)
    set(octk_android_install_dir_records "{${octk_android_install_dir_records}}")

    string(APPEND file_contents
        "   \"octk\": ${octk_android_install_dir_records},\n")

    # Android SDK path
    file(TO_CMAKE_PATH "${ANDROID_SDK_ROOT}" android_sdk_root_native)
    string(APPEND file_contents
        "   \"sdk\": \"${android_sdk_root_native}\",\n")

    # Android SDK Build Tools Revision
    _octk_internal_android_get_sdk_build_tools_revision(android_sdk_build_tools)
    set(android_sdk_build_tools_genex "")
    string(APPEND android_sdk_build_tools_genex
        "$<IF:$<BOOL:$<TARGET_PROPERTY:${target},OCTK_ANDROID_SDK_BUILD_TOOLS_REVISION>>,"
        "$<TARGET_PROPERTY:${target},OCTK_ANDROID_SDK_BUILD_TOOLS_REVISION>,"
        "${android_sdk_build_tools}"
        ">"
        )
    string(APPEND file_contents
        "   \"sdkBuildToolsRevision\": \"${android_sdk_build_tools_genex}\",\n")

    # Android NDK
    file(TO_CMAKE_PATH "${CMAKE_ANDROID_NDK}" android_ndk_root_native)
    string(APPEND file_contents
        "   \"ndk\": \"${android_ndk_root_native}\",\n")

    # Setup LLVM toolchain
    string(APPEND file_contents
        "   \"toolchain-prefix\": \"llvm\",\n")
    string(APPEND file_contents
        "   \"tool-prefix\": \"llvm\",\n")
    string(APPEND file_contents
        "   \"useLLVM\": true,\n")

    # NDK Toolchain Version
    string(APPEND file_contents
        "   \"toolchain-version\": \"${CMAKE_ANDROID_NDK_TOOLCHAIN_VERSION}\",\n")

    # NDK Host
    string(APPEND file_contents
        "   \"ndk-host\": \"${ANDROID_NDK_HOST_SYSTEM_NAME}\",\n")

    set(architecture_record_list "")
    foreach(abi IN LISTS octk_android_abis CMAKE_ANDROID_ARCH_ABI)
        if(abi STREQUAL "x86")
            set(arch_value "i686-linux-android")
        elseif(abi STREQUAL "x86_64")
            set(arch_value "x86_64-linux-android")
        elseif(abi STREQUAL "arm64-v8a")
            set(arch_value "aarch64-linux-android")
        elseif(abi)
            set(arch_value "arm-linux-androideabi")
        endif()
        list(APPEND architecture_record_list "\"${abi}\":\"${arch_value}\"")
    endforeach()

    list(JOIN architecture_record_list "," architecture_records)
    # Architecture
    string(APPEND file_contents
        "   \"architectures\": { ${architecture_records} },\n")

    # deployment dependencies
    _octk_internal_add_android_deployment_multi_value_property(file_contents "dependencies"
        ${target} "OCTK_ANDROID_DEPLOYMENT_DEPENDENCIES")

    # Extra plugins
    _octk_internal_add_android_deployment_multi_value_property(file_contents "android-extra-plugins"
        ${target} "_octk_android_native_extra_plugins")

    # Extra libs
    _octk_internal_add_android_deployment_multi_value_property(file_contents "android-extra-libs"
        ${target} "_octk_android_native_extra_libs")

    # Alternative path to octk libraries on target device
    _octk_internal_add_android_deployment_property(file_contents "android-system-libs-prefix"
        ${target} "OCTK_ANDROID_SYSTEM_LIBS_PREFIX")

    # package source dir
    _octk_internal_add_android_deployment_property(file_contents "android-package-source-directory"
        ${target} "_octk_android_native_package_source_dir")

    # version code
    _octk_internal_add_android_deployment_property(file_contents "android-version-code"
        ${target} "OCTK_ANDROID_VERSION_CODE")

    # version name
    _octk_internal_add_android_deployment_property(file_contents "android-version-name"
        ${target} "OCTK_ANDROID_VERSION_NAME")

    # minimum SDK version
    _octk_internal_add_android_deployment_property(file_contents "android-min-sdk-version"
        ${target} "OCTK_ANDROID_MIN_SDK_VERSION")

    # target SDK version
    _octk_internal_add_android_deployment_property(file_contents "android-target-sdk-version"
        ${target} "OCTK_ANDROID_TARGET_SDK_VERSION")

    # should octk shared libs be excluded from deployment
    _octk_internal_add_android_deployment_property(file_contents "android-no-deploy-octk-libs"
        ${target} "OCTK_ANDROID_NO_DEPLOY_OCTK_LIBS")

    # App binary
    string(APPEND file_contents
        "   \"application-binary\": \"${target_output_name}\",\n")

    # App command-line arguments
    if(OCTK_ANDROID_APPLICATION_ARGUMENTS)
        string(APPEND file_contents
            "   \"android-application-arguments\": \"${OCTK_ANDROID_APPLICATION_ARGUMENTS}\",\n")
    endif()

    if(COMMAND _octk_internal_generate_android_qml_deployment_settings)
        _octk_internal_generate_android_qml_deployment_settings(file_contents ${target})
    else()
        string(APPEND file_contents
            "   \"qml-skip-import-scanning\": true,\n")
    endif()

    # Override rcc binary path
    _octk_internal_add_tool_to_android_deployment_settings(file_contents rcc "rcc-binary" "${target}")

    # Extra prefix paths
    foreach(prefix IN LISTS CMAKE_FIND_ROOT_PATH)
        if(NOT "${prefix}" STREQUAL "${octk_android_install_dir_native}"
            AND NOT "${prefix}" STREQUAL "${android_ndk_root_native}")
            file(TO_CMAKE_PATH "${prefix}" prefix)
            list(APPEND extra_prefix_list "\"${prefix}\"")
        endif()
    endforeach()
    string(REPLACE ";" "," extra_prefix_list "${extra_prefix_list}")
    string(APPEND file_contents
        "   \"extraPrefixDirs\" : [ ${extra_prefix_list} ],\n")

    # Create an empty target for the cases when we need to generate deployment setting but
    # octk_finalize_project is never called.
    if(NOT TARGET _octk_internal_apk_dependencies AND NOT OCTK_NO_COLLECT_BUILD_TREE_APK_DEPS)
        add_custom_target(_octk_internal_apk_dependencies)
    endif()

    # Extra library paths that could be used as a dependency lookup path by androiddeployoctk.
    #
    # Unlike 'extraPrefixDirs', the 'extraLibraryDirs' key doesn't expect the 'lib' subfolder
    # when looking for dependencies.
    # TODO: add a public target property accessible from user space
    _octk_internal_add_android_deployment_list_property(file_contents "extraLibraryDirs"
        ${target} "_octk_android_extra_library_dirs"
        _octk_internal_apk_dependencies "_octk_android_extra_library_dirs"
        )

    if(OCTK_FEATURE_zstd)
        set(is_zstd_enabled "true")
    else()
        set(is_zstd_enabled "false")
    endif()
    string(APPEND file_contents
        "   \"zstdCompression\": ${is_zstd_enabled},\n")

    # Last item in json file

    # base location of stdlibc++, will be suffixed by androiddeploy octk
    # Sysroot is set by Android toolchain file and is composed of ANDROID_TOOLCHAIN_ROOT.
    set(android_ndk_stdlib_base_path "${CMAKE_SYSROOT}/usr/lib/")
    string(APPEND file_contents
        "   \"stdcpp-path\": \"${android_ndk_stdlib_base_path}\"\n")

    # content end
    string(APPEND file_contents "}\n")

    file(GENERATE OUTPUT ${deploy_file} CONTENT ${file_contents})

    set_target_properties(${target}
        PROPERTIES
        OCTK_ANDROID_DEPLOYMENT_SETTINGS_FILE ${deploy_file}
        )
endfunction()


# Add custom target to package the APK
function(octk_android_add_apk_target target)
    # Avoid calling octk6_android_add_apk_target twice
    get_property(apk_targets GLOBAL PROPERTY _octk_apk_targets)
    if("${target}" IN_LIST apk_targets)
        return()
    endif()

    get_target_property(deployment_file ${target} OCTK_ANDROID_DEPLOYMENT_SETTINGS_FILE)
    if(NOT deployment_file)
        message(FATAL_ERROR "Target ${target} is not a valid android executable target\n")
    endif()
    # Use genex to get path to the deployment settings, the above check only to confirm that
    # octk6_android_add_apk_target is called on an android executable target.
    set(deployment_file "$<TARGET_PROPERTY:${target},OCTK_ANDROID_DEPLOYMENT_SETTINGS_FILE>")

    # Make global apk and aab targets depend on the current apk target.
    if(TARGET aab)
        add_dependencies(aab ${target}_make_aab)
    endif()
    if(TARGET apk)
        add_dependencies(apk ${target}_make_apk)
        _octk_internal_create_global_apk_all_target_if_needed()
    endif()

    set(deployment_tool "${OCTK_HOST_PATH}/${OCTK6_HOST_INFO_BINDIR}/androiddeployoctk")
    # No need to use genex for the BINARY_DIR since it's read-only.
    get_target_property(target_binary_dir ${target} BINARY_DIR)
    set(apk_final_dir "${target_binary_dir}/android-build")
    set(apk_file_name "${target}.apk")
    set(dep_file_name "${target}.d")
    set(apk_final_file_path "${apk_final_dir}/${apk_file_name}")
    set(dep_file_path "${apk_final_dir}/${dep_file_name}")
    set(target_file_copy_relative_path
        "libs/${CMAKE_ANDROID_ARCH_ABI}/$<TARGET_FILE_NAME:${target}>")

    set(extra_deps "")

    # Plugins still might be added after creating the deployment targets.
    if(NOT TARGET octk_internal_plugins)
        add_custom_target(octk_internal_plugins)
    endif()
    # Before running androiddeployoctk, we need to make sure all plugins are built.
    list(APPEND extra_deps octk_internal_plugins)

    # This target is used by octk Creator's Android support and by the ${target}_make_apk target
    # in case DEPFILEs are not supported.
    # Also the target is used to copy the library that belongs to ${target} when building multi-abi
    # apk to the abi-specific directory.
    _octk_internal_copy_file_if_different_command(copy_command
        "$<TARGET_FILE:${target}>"
        "${apk_final_dir}/${target_file_copy_relative_path}"
        )
    add_custom_target(${target}_prepare_apk_dir ALL
        DEPENDS ${target} ${extra_deps}
        COMMAND ${copy_command}
        COMMENT "Copying ${target} binary to apk folder"
        )

    set(sign_apk "")
    if(OCTK_ANDROID_SIGN_APK)
        set(sign_apk "--sign")
    endif()
    set(sign_aab "")
    if(OCTK_ANDROID_SIGN_AAB)
        set(sign_aab "--sign")
    endif()

    set(extra_args "")
    if(OCTK_INTERNAL_NO_ANDROID_RCC_BUNDLE_CLEANUP)
        list(APPEND extra_args "--no-rcc-bundle-cleanup")
    endif()
    if(OCTK_ENABLE_VERBOSE_DEPLOYMENT)
        list(APPEND extra_args "--verbose")
    endif()

    _octk_internal_check_depfile_support(has_depfile_support)

    if(has_depfile_support)
        cmake_policy(PUSH)
        if(POLICY CMP0116)
            # Without explicitly setting this policy to NEW, we get a warning
            # even though we ensure there's actually no problem here.
            # See https://gitlab.kitware.com/cmake/cmake/-/issues/21959
            cmake_policy(SET CMP0116 NEW)
            set(relative_to_dir ${CMAKE_CURRENT_BINARY_DIR})
        else()
            set(relative_to_dir ${CMAKE_BINARY_DIR})
        endif()

        # Add custom command that creates the apk and triggers rebuild if files listed in
        # ${dep_file_path} are changed.
        add_custom_command(OUTPUT "${apk_final_file_path}"
            COMMAND ${CMAKE_COMMAND}
            -E copy "$<TARGET_FILE:${target}>"
            "${apk_final_dir}/${target_file_copy_relative_path}"
            COMMAND "${deployment_tool}"
            --input "${deployment_file}"
            --output "${apk_final_dir}"
            --apk "${apk_final_file_path}"
            --depfile "${dep_file_path}"
            --builddir "${relative_to_dir}"
            ${extra_args}
            ${sign_apk}
            COMMENT "Creating APK for ${target}"
            DEPENDS "${target}" "${deployment_file}" ${extra_deps}
            DEPFILE "${dep_file_path}"
            VERBATIM
            )
        cmake_policy(POP)

        # Create a ${target}_make_apk target to trigger the apk build.
        add_custom_target(${target}_make_apk DEPENDS "${apk_final_file_path}")
    else()
        add_custom_target(${target}_make_apk
            DEPENDS ${target}_prepare_apk_dir
            COMMAND ${deployment_tool}
            --input ${deployment_file}
            --output ${apk_final_dir}
            --apk ${apk_final_file_path}
            ${extra_args}
            ${sign_apk}
            COMMENT "Creating APK for ${target}"
            VERBATIM
            )
    endif()

    # Add target triggering AAB creation. Since the _make_aab target is not added to the ALL
    # set, we may avoid dependency check for it and admit that the target is "always out
    # of date".
    add_custom_target(${target}_make_aab
        DEPENDS ${target}_prepare_apk_dir
        COMMAND ${deployment_tool}
        --input ${deployment_file}
        --output ${apk_final_dir}
        --apk ${apk_final_file_path}
        --aab
        ${sign_aab}
        ${extra_args}
        COMMENT "Creating AAB for ${target}"
        )

    if(OCTK_IS_ANDROID_MULTI_ABI_EXTERNAL_PROJECT)
        # When building per-ABI external projects we only need to copy ABI-specific libraries and
        # resources to the "main" ABI android build folder.

        if("${OCTK_INTERNAL_ANDROID_MULTI_ABI_BINARY_DIR}" STREQUAL "")
            message(FATAL_ERROR "OCTK_INTERNAL_ANDROID_MULTI_ABI_BINARY_DIR is not set when building"
                " ABI specific external project. This should not happen and might mean an issue"
                " in OpenCTK. Please report a bug with CMake traces attached.")
        endif()
        # Assume that external project mirrors build structure of the top-level ABI project and
        # replace the build root when specifying the output directory of androiddeployoctk.
        file(RELATIVE_PATH androiddeployoctk_output_path "${CMAKE_BINARY_DIR}" "${apk_final_dir}")
        set(androiddeployoctk_output_path
            "${OCTK_INTERNAL_ANDROID_MULTI_ABI_BINARY_DIR}/${androiddeployoctk_output_path}")
        _octk_internal_copy_file_if_different_command(copy_command
            "$<TARGET_FILE:${target}>"
            "${androiddeployoctk_output_path}/${target_file_copy_relative_path}"
            )
        if(has_depfile_support)
            set(deploy_android_deps_dir "${apk_final_dir}/${target}_deploy_android")
            set(timestamp_file "${deploy_android_deps_dir}/timestamp")
            set(dep_file "${deploy_android_deps_dir}/${target}.d")
            add_custom_command(OUTPUT "${timestamp_file}"
                DEPENDS ${target} ${extra_deps}
                COMMAND ${CMAKE_COMMAND} -E make_directory "${deploy_android_deps_dir}"
                COMMAND ${CMAKE_COMMAND} -E touch "${timestamp_file}"
                COMMAND ${copy_command}
                COMMAND ${deployment_tool}
                --input ${deployment_file}
                --output ${androiddeployoctk_output_path}
                --copy-dependencies-only
                ${extra_args}
                --depfile "${dep_file}"
                --builddir "${CMAKE_BINARY_DIR}"
                COMMENT "Resolving ${CMAKE_ANDROID_ARCH_ABI} dependencies for the ${target} APK"
                DEPFILE "${dep_file}"
                VERBATIM
                )
            add_custom_target(octk_internal_${target}_copy_apk_dependencies
                DEPENDS "${timestamp_file}")
        else()
            add_custom_target(octk_internal_${target}_copy_apk_dependencies
                DEPENDS ${target} ${extra_deps}
                COMMAND ${copy_command}
                COMMAND ${deployment_tool}
                --input ${deployment_file}
                --output ${androiddeployoctk_output_path}
                --copy-dependencies-only
                ${extra_args}
                COMMENT "Resolving ${CMAKE_ANDROID_ARCH_ABI} dependencies for the ${target} APK"
                )
        endif()
    endif()

    set_property(GLOBAL APPEND PROPERTY _octk_apk_targets ${target})
    _octk_internal_collect_apk_dependencies_defer()
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
# The wrapper function that contains routines that need to be called to produce a valid Android package for the
# executable 'target'. The function is added to the finalizer list of the Core module and is executed implicitly when
# configuring user projects.
#-----------------------------------------------------------------------------------------------------------------------
function(octk_internal_android_executable_finalizer target)
    octk_internal_configure_android_multiabi_target("${target}")
    octk_android_generate_deployment_settings("${target}")
    octk_android_add_apk_target("${target}")
endfunction()
