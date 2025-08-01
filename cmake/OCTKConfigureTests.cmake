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

include(CheckCSourceCompiles)

function(octk_run_config_test_architecture)
    set(OCTK_BASE_CONFIGURE_TESTS_VARS_TO_EXPORT "" CACHE INTERNAL "Test variables that should be exported" FORCE)

    # Compile test to find the target architecture and sub-architectures.
    set(flags "")
    octk_get_platform_try_compile_vars(platform_try_compile_vars)
    list(APPEND flags ${platform_try_compile_vars})

    try_compile(
        _arch_result
        "${CMAKE_CURRENT_BINARY_DIR}/cmake/config.tests/arch"
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.tests/arch"
        arch
        CMAKE_FLAGS ${flags}
    )

    if(NOT _arch_result)
        message(FATAL_ERROR "Failed to compile architecture detection file.")
    endif()

    set(_arch_file_suffix "${CMAKE_EXECUTABLE_SUFFIX}")

    # With emscripten the application entry point is a .js file (to be run with node for example),
    # but the real "data" is in the .wasm file, so that's where we need to look for the ABI, etc.
    # information.
    if(EMSCRIPTEN)
        set(_arch_file_suffix ".wasm")
    endif()

    set(arch_test_location "cmake/config.tests/arch")

    if(OCTK_MULTI_CONFIG_FIRST_CONFIG)
        string(APPEND arch_test_location "/${OCTK_MULTI_CONFIG_FIRST_CONFIG}")
    endif()

    set(_arch_file "${CMAKE_CURRENT_BINARY_DIR}/${arch_test_location}/architecture_test${_arch_file_suffix}")

    if(NOT EXISTS "${_arch_file}")
        message(FATAL_ERROR "Failed to find compiled architecture detection executable at ${_arch_file}.")
    endif()

    message(STATUS "Extracting architecture info from ${_arch_file}.")

    file(STRINGS "${_arch_file}" _arch_lines LENGTH_MINIMUM 16 LENGTH_MAXIMUM 1024 ENCODING UTF-8)

    foreach(_line ${_arch_lines})
        string(LENGTH "${_line}" lineLength)
        string(FIND "${_line}" "==OpenCTK=magic=OpenCTK== Architecture:" _pos)

        if(_pos GREATER -1)
            math(EXPR _pos "${_pos}+29")
            string(SUBSTRING "${_line}" ${_pos} -1 _architecture)
        endif()

        string(FIND "${_line}" "==OpenCTK=magic=OpenCTK== Sub-architecture:" _pos)

        if(_pos GREATER -1 AND ${lineLength} GREATER 33)
            math(EXPR _pos "${_pos}+34")
            string(SUBSTRING "${_line}" ${_pos} -1 _sub_architecture)
            string(REPLACE " " ";" _sub_architecture "${_sub_architecture}")
        endif()

        string(FIND "${_line}" "==OpenCTK=magic=OpenCTK== Build-ABI:" _pos)

        if(_pos GREATER -1)
            math(EXPR _pos "${_pos}+26")
            string(SUBSTRING "${_line}" ${_pos} -1 _build_abi)
        endif()
    endforeach()

    if(NOT _architecture OR NOT _build_abi)
        message(FATAL_ERROR "Failed to extract architecture data from file.")
    endif()

    set(TEST_architecture 1 CACHE INTERNAL "Ran the architecture test")
    set(TEST_architecture_arch "${_architecture}" CACHE INTERNAL "Target machine architecture")
    list(APPEND OCTK_BASE_CONFIGURE_TESTS_VARS_TO_EXPORT TEST_architecture_arch)
    set(TEST_subarch 1 CACHE INTERNAL "Ran machine subArchitecture test")
    set(TEST_subarch_result "${_sub_architecture}" CACHE INTERNAL "Target sub-architectures")
    list(APPEND OCTK_BASE_CONFIGURE_TESTS_VARS_TO_EXPORT TEST_subarch_result)

    foreach(it ${_sub_architecture})
        set(TEST_arch_${TEST_architecture_arch}_subarch_${it} 1 CACHE INTERNAL "Target sub architecture result")
        list(APPEND OCTK_BASE_CONFIGURE_TESTS_VARS_TO_EXPORT TEST_arch_${TEST_architecture_arch}_subarch_${it})
    endforeach()

    set(TEST_buildAbi "${_build_abi}" CACHE INTERNAL "Target machine buildAbi")
    list(APPEND OCTK_BASE_CONFIGURE_TESTS_VARS_TO_EXPORT TEST_buildAbi)
    set(OCTK_BASE_CONFIGURE_TESTS_VARS_TO_EXPORT ${OCTK_BASE_CONFIGURE_TESTS_VARS_TO_EXPORT} CACHE INTERNAL "Test variables that should be exported")

    list(JOIN _sub_architecture " " subarch_summary)
    set_property(GLOBAL PROPERTY octk_configure_subarch_summary "${subarch_summary}")
endfunction()

function(octk_run_linker_version_script_support)
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/version_flag.map" "VERS_1 { global: sym; };
        VERS_2 { global: sym; }
        VERS_1;
        ")

    if(DEFINED CMAKE_REQUIRED_FLAGS)
        set(CMAKE_REQUIRED_FLAGS_SAVE ${CMAKE_REQUIRED_FLAGS})
    else()
        set(CMAKE_REQUIRED_FLAGS "")
    endif()

    set(CMAKE_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS} "-Wl,--version-script=\"${CMAKE_CURRENT_BINARY_DIR}/version_flag.map\"")
    check_c_source_compiles("int main(void){return 0;}" HAVE_LD_VERSION_SCRIPT)

    if(DEFINED CMAKE_REQUIRED_FLAGS_SAVE)
        set(CMAKE_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS_SAVE})
    endif()

    file(REMOVE "${CMAKE_CURRENT_BINARY_DIR}/conftest.map")

    # For some reason the linker command line written by the XCode generator, which is
    # subsequently executed by xcodebuild, ignores the linker flag, and thus the test
    # seemingly succeeds. Explicitly disable the version script test on darwin platforms.
    if(APPLE)
        set(HAVE_LD_VERSION_SCRIPT OFF)
    endif()

    set(TEST_ld_version_script "${HAVE_LD_VERSION_SCRIPT}" CACHE INTERNAL "linker version script support")
endfunction()

function(octk_run_octkbase_config_tests)
    octk_run_config_test_architecture()
    octk_run_linker_version_script_support()
endfunction()

# The cmake build of android does not perform the right architecture tests and
# forcefully disables sse4 on android x86. We have to mimic this behavior
# for now
if(CMAKE_ANDROID_ARCH_ABI STREQUAL x86)
    set(OCTK_FEATURE_sse4_1 OFF CACHE BOOL INTERNAL FORCE)
    set(OCTK_FEATURE_sse4_2 OFF CACHE BOOL INTERNAL FORCE)
    set(TEST_subarch_sse4_1 FALSE CACHE BOOL INTERNAL FORCE)
    set(TEST_subarch_sse4_2 FALSE CACHE BOOL INTERNAL FORCE)
endif()

octk_run_octkbase_config_tests()

function(octk_internal_print_cmake_darwin_info)
    if(APPLE)
        if(NOT CMAKE_OSX_ARCHITECTURES)
            set(default_osx_arch " (defaults to ${CMAKE_SYSTEM_PROCESSOR})")
        endif()

        message(STATUS "CMAKE_OSX_ARCHITECTURES: \"${CMAKE_OSX_ARCHITECTURES}\"${default_osx_arch}")
        message(STATUS "CMAKE_OSX_SYSROOT: \"${CMAKE_OSX_SYSROOT}\"")
        message(STATUS "CMAKE_OSX_DEPLOYMENT_TARGET: \"${CMAKE_OSX_DEPLOYMENT_TARGET}\"")
        message(STATUS "OCTK_MAC_SDK_VERSION: \"${OCTK_MAC_SDK_VERSION}\"")
        message(STATUS "OCTK_MAC_XCODE_VERSION: \"${OCTK_MAC_XCODE_VERSION}\"")

        if(OCTK_UIKIT_SDK)
            message(STATUS "OCTK_UIKIT_SDK: \"${OCTK_UIKIT_SDK}\"")
        endif()
    endif()
endfunction()

octk_internal_print_cmake_darwin_info()

function(octk_internal_print_cmake_host_and_target_info)
    message(STATUS "CMAKE_VERSION: \"${CMAKE_VERSION}\"")
    message(STATUS "CMAKE_HOST_SYSTEM: \"${CMAKE_HOST_SYSTEM}\"")
    message(STATUS "CMAKE_HOST_SYSTEM_NAME: \"${CMAKE_HOST_SYSTEM_NAME}\"")
    message(STATUS "CMAKE_HOST_SYSTEM_VERSION: \"${CMAKE_HOST_SYSTEM_VERSION}\"")
    message(STATUS "CMAKE_HOST_SYSTEM_PROCESSOR: \"${CMAKE_HOST_SYSTEM_PROCESSOR}\"")

    message(STATUS "CMAKE_SYSTEM: \"${CMAKE_SYSTEM_NAME}\"")
    message(STATUS "CMAKE_SYSTEM_NAME: \"${CMAKE_SYSTEM_NAME}\"")
    message(STATUS "CMAKE_SYSTEM_VERSION: \"${CMAKE_SYSTEM_VERSION}\"")
    message(STATUS "CMAKE_SYSTEM_PROCESSOR: \"${CMAKE_SYSTEM_PROCESSOR}\"")

    message(STATUS "CMAKE_CROSSCOMPILING: \"${CMAKE_CROSSCOMPILING}\"")
endfunction()

octk_internal_print_cmake_host_and_target_info()

function(octk_internal_print_cmake_compiler_info)
    message(STATUS "CMAKE_C_COMPILER: \"${CMAKE_C_COMPILER}\" (${CMAKE_C_COMPILER_VERSION})")

    if(CMAKE_OBJC_COMPILER)
        message(STATUS "CMAKE_OBJC_COMPILER: \"${CMAKE_OBJC_COMPILER}\" (${CMAKE_OBJC_COMPILER_VERSION})")
    endif()

endfunction()

octk_internal_print_cmake_compiler_info()

function(octk_internal_print_cmake_windows_info)
    if(MSVC_VERSION)
        message(STATUS "MSVC_VERSION: \"${MSVC_VERSION}\"")
    endif()

    if(MSVC_TOOLSET_VERSION)
        message(STATUS "MSVC_TOOLSET_VERSION: \"${MSVC_TOOLSET_VERSION}\"")
    endif()
endfunction()

octk_internal_print_cmake_windows_info()

function(octk_internal_print_cmake_android_info)
    if(ANDROID)
        message(STATUS "ANDROID_TOOLCHAIN: \"${ANDROID_TOOLCHAIN}\"")
        message(STATUS "ANDROID_NDK: \"${ANDROID_NDK}\"")
        message(STATUS "ANDROID_ABI: \"${ANDROID_ABI}\"")
        message(STATUS "ANDROID_PLATFORM: \"${ANDROID_PLATFORM}\"")
        message(STATUS "ANDROID_STL: \"${ANDROID_STL}\"")
        message(STATUS "ANDROID_PIE: \"${ANDROID_PIE}\"")
        message(STATUS "ANDROID_CPP_FEATURES: \"${ANDROID_CPP_FEATURES}\"")
        message(STATUS "ANDROID_ALLOW_UNDEFINED_SYMBOLS: \"${ANDROID_ALLOW_UNDEFINED_SYMBOLS}\"")
        message(STATUS "ANDROID_ARM_MODE: \"${ANDROID_ARM_MODE}\"")
        message(STATUS "ANDROID_ARM_NEON: \"${ANDROID_ARM_NEON}\"")
        message(STATUS "ANDROID_DISABLE_FORMAT_STRING_CHECKS: \"${ANDROID_DISABLE_FORMAT_STRING_CHECKS}\"")
        message(STATUS "ANDROID_NATIVE_API_LEVEL: \"${ANDROID_NATIVE_API_LEVEL}\"")
        message(STATUS "ANDROID_LLVM_TRIPLE: \"${ANDROID_LLVM_TRIPLE}\"")
    endif()
endfunction()

octk_internal_print_cmake_android_info()
