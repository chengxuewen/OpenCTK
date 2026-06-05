########################################################################################################################
#
# Library: OpenCTK
#
# Copyright (C) 2026~Present ChengXueWen.
#
# License: MIT License
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
# documentation files (the "Software"), to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
# and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
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
# Install third-party PUBLIC_LIBRARIES headers under the OpenCTK namespace
#
# This function is called AFTER octk_add_library(). It:
#   1. Creates build tree symlinks: ${OCTK_BUILD_DIR}/include/openctk/3rdparty/<shortname>/
#   2. Adds install rules for headers to include/openctk/3rdparty/<shortname>/
#   3. Adds install rules for .so files (if the library has them, e.g. yaml-cpp)
#   4. Overrides INTERFACE_INCLUDE_DIRECTORIES for the wrap target
#
# Usage:
#   octk_install_public_wrap_headers(OpenCTKCore
#       WRAPS
#       OpenCTKWrapFmt::WrapFmt;fmt
#       OpenCTKWrapYamlCpp::WrapYamlCpp;yaml-cpp
#       OpenCTKWrapJson::WrapJson;nlohmann
#       ...)
#
# ARG_WRAPS: semicolon-separated list of wrap_target;shortname pairs.
#-----------------------------------------------------------------------------------------------------------------------
function(octk_install_public_wrap_headers target)
    cmake_parse_arguments(arg "" "" "WRAPS" ${ARGN})

    foreach(pair ${arg_WRAPS})
        # Parse wrap_target|shortname (using | as separator to avoid CMake list splitting)
        string(REPLACE "|" ";" pair_list ${pair})
        list(GET pair_list 0 wrap_target)
        list(GET pair_list 1 shortname)

        # Get wrap's install directory variable name from the target name
        # e.g., OpenCTKWrapFmt::WrapFmt → variable: OpenCTKWrapFmt_INSTALL_DIR
        string(REGEX REPLACE "OpenCTKWrap([^:]+)::.*" "\\1" wrap_name ${wrap_target})
        set(install_dir_var "OpenCTKWrap${wrap_name}_INSTALL_DIR")
        if(NOT ${install_dir_var})
            message(WARNING "octk_install_public_wrap_headers: ${install_dir_var} not set for ${wrap_target}")
            continue()
        endif()

        set(wrap_install_dir "${${install_dir_var}}")
        set(wrap_install_dir "${${install_dir_var}}")
        set(wrap_include_dir "${wrap_install_dir}/include")
        set(install_dest_dir "include/openctk/3rdparty/${shortname}")
        set(install_lib_dir "lib")

        if(EXISTS "${wrap_include_dir}")
            file(COPY "${wrap_include_dir}/" DESTINATION "${OCTK_BUILD_DIR}/include/openctk/3rdparty")
        else()
            file(MAKE_DIRECTORY "${build_dir}")
        endif()

        # Install header files from wrap includes to OpenCTK namespace
        # We take the wrap's include/ directory and install all subdirectories
        # Install header files to the unified namespace parent directory
        # Source: <wrap>/install/include/<lib>/header.h
        # Dest:  <prefix>/include/openctk/3rdparty/<lib>/header.h
        install(DIRECTORY "${wrap_include_dir}/"
            DESTINATION "include/openctk/3rdparty"
            FILES_MATCHING
            PATTERN "*.hpp"
            PATTERN "*.h"
            PATTERN "*.hh"
        )

        # Use target_include_directories (works with IMPORTED targets, appends to existing)
        # to add OpenCTK-namespaced include paths for build and install interfaces.
        # This ensures consumers using find_package(OpenCTKCore) get correct include paths.
        # Include path points to the ROOT, so <openctk/3rdparty/fmt/os.h> resolves correctly
        # -I build/include → build/include/openctk/3rdparty/fmt/os.h ✅
        # -I include/openctk/3rdparty/fmt → WRONG (doubles the path)
        target_include_directories(${wrap_target} INTERFACE
            "$<BUILD_INTERFACE:${OCTK_BUILD_DIR}/include>"
            "$<INSTALL_INTERFACE:include>"
        )
        # Bake into main target for find_package export
        target_include_directories(${target} INTERFACE
            "$<BUILD_INTERFACE:${OCTK_BUILD_DIR}/include>"
            "$<INSTALL_INTERFACE:include>")
        # yaml-cpp is a static library (.a) linked into libopenctk_core.so
        # No separate .so distribution needed. Symbols are embedded in the .so.
    endforeach()
endfunction()
