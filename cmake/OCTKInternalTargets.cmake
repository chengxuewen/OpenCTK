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
function(octk_internal_set_warnings_are_errors_flags target target_scope)
    set(flags "")
    if(CLANG AND NOT MSVC)
        list(APPEND flags -Werror -Wno-error=\#warnings -Wno-error=deprecated-declarations)
        if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang") # as in: not AppleClang
            if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "10.0.0")
                # We do mixed enum arithmetic all over the place:
                list(APPEND flags -Wno-error=deprecated-enum-enum-conversion)
            endif()
            if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "14.0.0")
                # Clang 14 introduced these two but we are not clean for it.
                list(APPEND flags -Wno-error=deprecated-copy-with-user-provided-copy)
                list(APPEND flags -Wno-error=unused-but-set-variable)
            endif()
        endif()
    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        # using GCC
        list(APPEND flags -Werror -Wno-error=cpp -Wno-error=deprecated-declarations)

        # GCC prints this bogus warning, after it has inlined a lot of code
        # error: assuming signed overflow does not occur when assuming that (X + c) < X is always false
        list(APPEND flags -Wno-error=strict-overflow)

        # GCC 7 includes -Wimplicit-fallthrough in -Wextra, but OpenCTK is not yet free of implicit fallthroughs.
        if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "7.0.0")
            list(APPEND flags -Wno-error=implicit-fallthrough)
        endif()

        if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "9.0.0")
            # GCC 9 introduced these but we are not clean for it.
            list(APPEND flags -Wno-error=deprecated-copy -Wno-error=redundant-move -Wno-error=init-list-lifetime)
            # GCC 9 introduced -Wformat-overflow in -Wall, but it is buggy:
            list(APPEND flags -Wno-error=format-overflow)
        endif()

        if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "10.0.0")
            # GCC 10 has a number of bugs in -Wstringop-overflow. Do not make them an error.
            # https://gcc.gnu.org/bugzilla/show_bug.cgi?id=92955
            # https://gcc.gnu.org/bugzilla/show_bug.cgi?id=94335
            # https://gcc.gnu.org/bugzilla/show_bug.cgi?id=101134
            list(APPEND flags -Wno-error=stringop-overflow)
        endif()

        if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "11.0.0")
            # We do mixed enum arithmetic all over the place:
            list(APPEND flags -Wno-error=deprecated-enum-enum-conversion -Wno-error=deprecated-enum-float-conversion)
        endif()

        if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "11.0.0" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "11.2.0")
            # GCC 11.1 has a regression in the integrated preprocessor, so disable it as a workaround (OCTKBUG-93360)
            # https://gcc.gnu.org/bugzilla/show_bug.cgi?id=100796
            # This in turn triggers a fallthrough warning in cborparser.c, so we disable this warning.
            list(APPEND flags -no-integrated-cpp -Wno-implicit-fallthrough)
        endif()

        # Work-around for bug https://code.google.com/p/android/issues/detail?id=58135
        if(ANDROID)
            list(APPEND flags -Wno-error=literal-suffix)
        endif()
    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        # Only enable for versions of MSVC that are known to work
        # 1939 is Visual Studio 2022 version 17.0
        if(MSVC_VERSION LESS_EQUAL 1939)
            list(APPEND flags /WX)
        endif()
    endif()
    set(warnings_are_errors_enabled_genex
        "$<NOT:$<BOOL:$<TARGET_PROPERTY:OCTK_SKIP_WARNINGS_ARE_ERRORS>>>")

    # Apparently qmake only adds -Werror to CXX and OBJCXX files, not C files. We have to do the
    # same otherwise MinGW builds break when building 3rdparty\md4c\md4c.c (and probably on other
    # platforms too).
    set(cxx_only_genex "$<OR:$<COMPILE_LANGUAGE:CXX>,$<COMPILE_LANGUAGE:OBJCXX>>")
    set(final_condition_genex "$<AND:${warnings_are_errors_enabled_genex},${cxx_only_genex}>")
    set(flags_generator_expression "$<${final_condition_genex}:${flags}>")

    target_compile_options("${target}" ${target_scope} "${flags_generator_expression}")
endfunction()

# The function adds a global 'definition' to the platform internal targets and the target
# property-based switch to disable the definition.
# Arguments:
#     VALUE optional value that the definition will take.
#     SCOPE the list of scopes the definition needs to be set for. If the SCOPE is not specified the
#        definition is added to PlatformCommonInternal target.
#        Possible values:
#            MODULE - set the definition for all OpenCTK modules
#            PLUGIN - set the definition for all OpenCTK plugins
#            TOOL - set the definition for all OpenCTK tools
#            APP - set the definition for all OpenCTK applications
#        TODO: Add a tests specific platform target and the definition scope for it.
function(octk_internal_add_global_definition definition)
    set(optional_args)
    set(single_value_args VALUE)
    set(multi_value_args SCOPE)
    cmake_parse_arguments(args
        "${optional_args}"
        "${single_value_args}"
        "${multi_value_args}"
        ${ARGN})

    set(scope_LIBRARY PlatformLibraryInternal)
    set(scope_PLUGIN PlatformPluginInternal)
    set(scope_TOOL PlatformToolInternal)
    set(scope_APP PlatformAppInternal)

    set(undef_property_name "OCTK_INTERNAL_UNDEF_${definition}")

    if(DEFINED arg_VALUE)
        set(definition "${definition}=${arg_VALUE}")
    endif()

    set(definition_genex "$<$<NOT:$<BOOL:$<TARGET_PROPERTY:${undef_property_name}>>>:${definition}>")

    if(NOT DEFINED arg_SCOPE)
        target_compile_definitions(PlatformCommonInternal INTERFACE "${definition_genex}")
    else()
        foreach(scope IN LISTS arg_SCOPE)
            if(NOT DEFINED scope_${scope})
                message(FATAL_ERROR "Unknown scope ${scope}.")
            endif()
            target_compile_definitions("${scope_${scope}}" INTERFACE "${definition_genex}")
        endforeach()
    endif()
endfunction()

#add_library(PlatformCommonInternal INTERFACE)
#octk_internal_add_target_aliases(PlatformCommonInternal)
#target_link_libraries(PlatformCommonInternal INTERFACE platform)
#
#add_library(PlatformLibraryInternal INTERFACE)
#octk_internal_add_target_aliases(PlatformLibraryInternal)
#target_link_libraries(PlatformLibraryInternal INTERFACE PlatformLibraryInternal)
#
#add_library(PlatformPluginInternal INTERFACE)
#octk_internal_add_target_aliases(PlatformPluginInternal)
#target_link_libraries(PlatformPluginInternal INTERFACE PlatformPluginInternal)
#
#add_library(PlatformAppInternal INTERFACE)
#octk_internal_add_target_aliases(PlatformAppInternal)
#target_link_libraries(PlatformAppInternal INTERFACE PlatformCommonInternal)
#
#add_library(PlatformToolInternal INTERFACE)
#octk_internal_add_target_aliases(PlatformToolInternal)
#target_link_libraries(PlatformToolInternal INTERFACE PlatformAppInternal)

#octk_internal_add_global_definition(OCTK_NO_JAVA_STYLE_ITERATORS)
#octk_internal_add_global_definition(OCTK_NO_NARROWING_CONVERSIONS_IN_CONNECT)

#if(WARNINGS_ARE_ERRORS)
#    octk_internal_set_warnings_are_errors_flags(PlatformLibraryInternal INTERFACE)
#    octk_internal_set_warnings_are_errors_flags(PlatformPluginInternal INTERFACE)
#    octk_internal_set_warnings_are_errors_flags(PlatformAppInternal INTERFACE)
#endif()
#if(WIN32)
#    # Needed for M_PI define. Same as mkspecs/features/octk_module.prf.
#    # It's set for every module being built, but it's not propagated to user apps.
#    target_compile_definitions(PlatformLibraryInternal INTERFACE _USE_MATH_DEFINES)
#endif()
#if(FEATURE_largefile AND UNIX)
#    target_compile_definitions(PlatformCommonInternal
#        INTERFACE "_LARGEFILE64_SOURCE;_LARGEFILE_SOURCE")
#endif()
#
#if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND CMAKE_SYSTEM_NAME STREQUAL "Windows")
#    # Clang will otherwise show error about inline method conflicting with dllimport class attribute in tools
#    # (this was tested with Clang 10)
#    #    error: 'string::operator[]' redeclared inline; 'dllimport' attribute ignored [-Werror,-Wignored-attributes]
#    target_compile_options(PlatformCommonInternal INTERFACE -Wno-ignored-attributes)
#endif()
#
#target_compile_definitions(PlatformCommonInternal INTERFACE OCTK_NO_NARROWING_CONVERSIONS_IN_CONNECT)
#target_compile_definitions(PlatformCommonInternal INTERFACE $<$<NOT:$<CONFIG:Debug>>:OCTK_NO_DEBUG>)
#
#if(FEATURE_developer_build)
#    # This causes an ABI break on Windows, so we cannot unconditionally
#    # enable it. Keep it for developer builds only for now.
#    ### OpenCTK 7: remove the if.
#    target_compile_definitions(PlatformCommonInternal INTERFACE OCTK_STRICT_QLIST_ITERATORS)
#endif()

function(octk_internal_apply_bitcode_flags target)
    # See mkspecs/features/uikit/bitcode.prf
    set(release_flags "-fembed-bitcode")
    set(debug_flags "-fembed-bitcode-marker")

    set(is_release_genex "$<NOT:$<CONFIG:Debug>>")
    set(flags_genex "$<IF:${is_release_genex},${release_flags},${debug_flags}>")
    set(is_enabled_genex "$<NOT:$<BOOL:$<TARGET_PROPERTY:OCTK_NO_BITCODE>>>")

    set(bitcode_flags "$<${is_enabled_genex}:${flags_genex}>")

    target_compile_options("${target}" INTERFACE ${bitcode_flags})
endfunction()

#TODO:del?
# Apple deprecated the entire OpenGL API in favor of Metal, which
# we are aware of, so silence the deprecation warnings in code.
# This does not apply to user-code, which will need to silence
# their own warnings if they use the deprecated APIs explicitly.
#if(MACOS)
#    target_compile_definitions(PlatformCommonInternal INTERFACE GL_SILENCE_DEPRECATION)
#elseif(UIKIT)
#    target_compile_definitions(PlatformCommonInternal INTERFACE GLES_SILENCE_DEPRECATION)
#endif()

#if(MSVC)
#    target_compile_definitions(PlatformCommonInternal INTERFACE
#        "_CRT_SECURE_NO_WARNINGS"
#        "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:_WINDLL>")
#endif()

#if(UIKIT)
#    # Do what mkspecs/features/uikit/default_pre.prf does, aka enable sse2 for
#    # simulator_and_device_builds.
#    if(FEATURE_simulator_and_device)
#        # Setting the definition on PlatformCommonInternal behaves slightly differently from what
#        # is done in qmake land. This way the define is not propagated to tests, examples, or
#        # user projects built with qmake, but only modules, plugins and tools.
#        # TODO: Figure out if this ok or not (sounds ok to me).
#        target_compile_definitions(PlatformCommonInternal INTERFACE OCTK_COMPILER_SUPPORTS_SSE2)
#    endif()
#endif()
#
#if(WASM AND OCTK_FEATURE_sse2)
#    target_compile_definitions(PlatformCommonInternal INTERFACE OCTK_COMPILER_SUPPORTS_SSE2)
#endif()

# Taken from mkspecs/common/msvc-version.conf and mkspecs/common/msvc-desktop.conf
#if(MSVC)
#    if(MSVC_VERSION GREATER_EQUAL 1799)
#        target_compile_options(PlatformCommonInternal INTERFACE -FS -Zc:rvalueCast -Zc:inline)
#    endif()
#    if(MSVC_VERSION GREATER_EQUAL 1899)
#        target_compile_options(PlatformCommonInternal INTERFACE -Zc:strictStrings)
#        if(NOT CLANG)
#            target_compile_options(PlatformCommonInternal INTERFACE -Zc:throwingNew)
#        endif()
#    endif()
#    if(MSVC_VERSION GREATER_EQUAL 1909 AND NOT CLANG)
#        target_compile_options(PlatformCommonInternal INTERFACE -Zc:referenceBinding)
#    endif()
#    if(MSVC_VERSION GREATER_EQUAL 1919 AND NOT CLANG)
#        target_compile_options(PlatformCommonInternal INTERFACE -Zc:externConstexpr)
#    endif()
#
#    target_compile_options(PlatformCommonInternal INTERFACE -Zc:wchar_t -bigobj)
#
#    target_compile_options(PlatformCommonInternal INTERFACE $<$<NOT:$<CONFIG:Debug>>:-guard:cf -Gw>)
#
#    target_link_options(PlatformCommonInternal INTERFACE
#        -DYNAMICBASE -NXCOMPAT -LARGEADDRESSAWARE
#        $<$<NOT:$<CONFIG:Debug>>:-OPT:REF -OPT:ICF -GUARD:CF>)
#endif()
#
#if(MINGW)
#    target_compile_options(PlatformCommonInternal INTERFACE -Wa,-mbig-obj)
#endif()
#
#if(GCC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "9.2")
#    target_compile_options(PlatformCommonInternal INTERFACE $<$<COMPILE_LANGUAGE:CXX>:-Wsuggest-override>)
#endif()
#
#if(OCTK_FEATURE_INTELCET)
#    if(MSVC)
#        target_link_options(PlatformCommonInternal INTERFACE -CETCOMPAT)
#    else()
#        target_compile_options(PlatformCommonInternal INTERFACE -fcf-protection)
#    endif()
#endif()
#
#if(OCTK_FEATURE_FORCE_ASSERTS)
#    target_compile_definitions(PlatformCommonInternal INTERFACE OCTK_FORCE_ASSERTS)
#endif()
#
#if(DEFINED OCTK_EXTRA_DEFINES)
#    target_compile_definitions(PlatformCommonInternal INTERFACE ${OCTK_EXTRA_DEFINES})
#endif()
#
#if(DEFINED OCTK_EXTRA_INCLUDEPATHS)
#    target_include_directories(PlatformCommonInternal INTERFACE ${OCTK_EXTRA_INCLUDEPATHS})
#endif()
#
#if(DEFINED OCTK_EXTRA_LIBDIRS)
#    target_link_directories(PlatformCommonInternal INTERFACE ${OCTK_EXTRA_LIBDIRS})
#endif()
#
#if(DEFINED OCTK_EXTRA_FRAMEWORKPATHS AND APPLE)
#    list(TRANSFORM OCTK_EXTRA_FRAMEWORKPATHS PREPEND "-F" OUTPUT_VARIABLE __octk_fw_flags)
#    target_compile_options(PlatformCommonInternal INTERFACE ${__octk_fw_flags})
#    target_link_options(PlatformCommonInternal INTERFACE ${__octk_fw_flags})
#    unset(__octk_fw_flags)
#endif()
#
#octk_internal_get_active_linker_flags(__octk_internal_active_linker_flags)
#if(__octk_internal_active_linker_flags)
#    target_link_options(PlatformCommonInternal INTERFACE "${__octk_internal_active_linker_flags}")
#endif()
#unset(__octk_internal_active_linker_flags)
#
#if(OCTK_FEATURE_ENABLE_GDB_INDEX)
#    target_link_options(PlatformCommonInternal INTERFACE "-Wl,--gdb-index")
#endif()
#
#if(OCTK_FEATURE_ENABLE_NEW_DTAGS)
#    target_link_options(PlatformCommonInternal INTERFACE "-Wl,--enable-new-dtags")
#endif()


#-----------------------------------------------------------------------------------------------------------------------
#-----------------------------------------------------------------------------------------------------------------------
function(octk_get_implicit_sse2_genex_condition out_var)
    set(is_shared_lib "$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>")
    set(is_static_lib "$<STREQUAL:$<TARGET_PROPERTY:TYPE>,STATIC_LIBRARY>")
    set(is_static_octk_build "$<NOT:$<BOOL:${OCTK_BUILD_SHARED_LIBS}>>")
    set(is_staitc_lib_during_static_octk_build "$<AND:${is_static_octk_build},${is_static_lib}>")
    set(enable_sse2_condition "$<OR:${is_shared_lib},${is_staitc_lib_during_static_octk_build}>")
    set(${out_var} "${enable_sse2_condition}" PARENT_SCOPE)
endfunction()


#-----------------------------------------------------------------------------------------------------------------------
#-----------------------------------------------------------------------------------------------------------------------
function(octk_auto_detect_implicit_sse2)
    # sse2 configuration adjustment in octk_module.prf
    # If the compiler supports SSE2, enable it unconditionally in all of OpenCTK shared libraries
    # (and only the libraries). This is not expected to be a problem because:
    # - on Windows, sharing of libraries is uncommon
    # - on Mac OS X, all x86 CPUs already have SSE2 support (we won't even reach here)
    # - on Linux, the dynamic loader can find the libraries on LIBDIR/sse2/
    # The last guarantee does not apply to executables and plugins, so we can't enable for them.
    set(__implicit_sse2_for_octk_modules_enabled FALSE PARENT_SCOPE)
    if(TEST_subarch_sse2 AND NOT TEST_arch_${TEST_architecture_arch}_subarch_sse2)
        octk_get_implicit_sse2_genex_condition(enable_sse2_condition)
        set(enable_sse2_genex "$<${enable_sse2_condition}:${OCTK_CFLAGS_SSE2}>")
        target_compile_options(PlatformLibraryInternal INTERFACE ${enable_sse2_genex})
        set(__implicit_sse2_for_octk_modules_enabled TRUE PARENT_SCOPE)
    endif()
endfunction()
octk_auto_detect_implicit_sse2()


#-----------------------------------------------------------------------------------------------------------------------
#-----------------------------------------------------------------------------------------------------------------------
function(octk_auto_detect_fpmath)
    # fpmath configuration adjustment in octk_module.prf
    set(fpmath_supported FALSE)
    if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang|GNU|IntelLLVM")
        set(fpmath_supported TRUE)
    endif()
    if(fpmath_supported AND TEST_architecture_arch STREQUAL "i386" AND __implicit_sse2_for_octk_modules_enabled)
        octk_get_implicit_sse2_genex_condition(enable_sse2_condition)
        set(enable_fpmath_genex "$<${enable_sse2_condition}:-mfpmath=sse>")
        target_compile_options(PlatformLibraryInternal INTERFACE ${enable_fpmath_genex})
    endif()
endfunction()
octk_auto_detect_fpmath()


#-----------------------------------------------------------------------------------------------------------------------
#-----------------------------------------------------------------------------------------------------------------------
# function(octk_handle_apple_app_extension_api_only)
#     if(APPLE)
#         # Build OpenCTK libraries with -fapplication-extension. Needed to avoid linker warnings
#         # transformed into errors on darwin platforms.
#         set(flags "-fapplication-extension")

#         # The flags should only be applied to internal OpenCTK libraries like modules and plugins.
#         # The reason why we use a custom property to apply the flags is because there's no other
#         # way to prevent the link options spilling out into user projects if the target that links
#         # against PlatformXInternal is a static library.
#         # The exported static library's INTERFACE_LINK_LIBRARIES property would contain
#         # $<LINK_ONLY:platformXinternal> and platformXinternal's INTERFACE_LINK_OPTIONS would be
#         # applied to a user project target.
#         # So to contain the spilling out of the flags, we ensure the link options are only added
#         # to internal OpenCTK libraries that are marked with the property.
#         set(not_disabled "$<NOT:$<BOOL:$<TARGET_PROPERTY:OCTK_NO_APP_EXTENSION_ONLY_API>>>")
#         set(is_octk_internal_library "$<BOOL:$<TARGET_PROPERTY:_octk_is_internal_library>>")

#         set(condition "$<AND:${not_disabled},${is_octk_internal_library}>")

#         set(flags "$<${condition}:${flags}>")
#         target_compile_options(PlatformLibraryInternal INTERFACE ${flags})
#         target_link_options(PlatformLibraryInternal INTERFACE ${flags})
#         target_compile_options(PlatformPluginInternal INTERFACE ${flags})
#         target_link_options(PlatformPluginInternal INTERFACE ${flags})
#     endif()
# endfunction()
# octk_handle_apple_app_extension_api_only()