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

# Applies necessary rpaths to a target upon target installation.
# No-op when targeting Windows, Android.
#
# Since abf72395411b135054b5820f64f93dfbcda430b8 rpaths are also applied in non-prefix builds,
# to address -rpath-link issues when cross-compiling, although this might not be needed anymore
# due to 606124c5cceba0dd4a406a9278588b58bb9f9800.
# See OCTKBUG-86533 for the whole saga.
#
# If no RELATIVE_RPATH option is given, embeds an absolute path rpath to
# ${CMAKE_INSTALL_PREFIX}/${OCTK_INSTALL_LIBDIR} into the target.

# If RELATIVE_RPATH is given, the INSTALL_PATH value is used to compute the relative path from
# ${CMAKE_INSTALL_PREFIX}/${OCTK_INSTALL_LIBDIR} to wherever the target will be installed
# (the value of INSTALL_PATH).
# INSTALL_PATH is expected to be a relative directory where the binary / library will be installed.

# RELATIVE_RPATH is the equivalent of qmake's relative_octk_rpath.
# INSTALL_PATH is used to implement the equivalent of qmake's $$octkRelativeRPathBase().
#
# OCTK_DISABLE_RPATH can be set to disable embedding any OpenCTK specific rpaths.
function(octk_apply_rpaths)
    # No rpath support for win32 and android.
    if(WIN32 OR ANDROID)
        return()
    endif()

    # Rpaths explicitly disabled (like for uikit), equivalent to qmake's no_octk_rpath.
    # Or feature was turned OFF.
    if(OCTK_DISABLE_RPATH OR NOT OCTK_FEATURE_rpath)
        return()
    endif()

    octk_parse_all_arguments(arg "octk_apply_rpaths" "RELATIVE_RPATH" "TARGET;INSTALL_PATH" "" ${ARGN})
    if(NOT arg_TARGET)
        message(FATAL_ERROR "No target given to octk_apply_rpaths.")
    else()
        set(target "${arg_TARGET}")
    endif()

    # If a target is not built (which can happen for tools when crosscompiling, we shouldn't try
    # to apply properties.
    if(NOT TARGET "${target}")
        return()
    endif()

    # Protect against interface libraries.
    get_target_property(target_type "${target}" TYPE)
    if(target_type STREQUAL "INTERFACE_LIBRARY")
        return()
    endif()

    if(NOT arg_INSTALL_PATH)
        message(FATAL_ERROR "No INSTALL_PATH given to octk_apply_rpaths.")
    endif()

    set(rpaths "")

    # Modify the install path to contain the nested structure of a framework.
    get_target_property(is_framework "${target}" FRAMEWORK)
    if(is_framework)
        octk_internal_get_framework_info(fw ${target})
        if(UIKIT)
            # Shallow framework
            string(APPEND arg_INSTALL_PATH "/${fw_dir}")
        else()
            # Full framework
            string(APPEND arg_INSTALL_PATH "/${fw_dir}/Versions/Current")
        endif()
    endif()

    # Same but for an app bundle.
    get_target_property(is_bundle "${target}" MACOSX_BUNDLE)
    if(is_bundle AND NOT is_framework)
        if(UIKIT)
            # Shallow bundle
            string(APPEND arg_INSTALL_PATH "/${target}.app")
        else()
            # Full bundle
            string(APPEND arg_INSTALL_PATH "/${target}.app/Contents/MacOS")
        endif()
    endif()

    octk_internal_get_relative_rpath_base_token(rpath_base_token)
    if(rpath_base_token STREQUAL "NO_KNOWN_RPATH_REL_BASE")
        set(relative_rpath_supported FALSE)
    else()
        set(relative_rpath_supported TRUE)
    endif()

    # Somewhat similar to mkspecs/features/octk.prf
    # Embed either an absolute path to the installed OpenCTK lib dir, or a relative one, based on
    # where ${target} is installed.
    # Don't embed relative rpaths if the platform does not support it.
    if(arg_RELATIVE_RPATH AND relative_rpath_supported)
        octk_compute_relative_rpath_base(
            "${_default_install_rpath}" "${arg_INSTALL_PATH}" relative_rpath)
        list(APPEND rpaths "${relative_rpath}")
    else()
        list(APPEND rpaths "${_default_install_rpath}")
    endif()

    # Somewhat similar to mkspecs/features/octk_build_extra.prf.
    foreach(rpath ${OCTK_EXTRA_RPATHS})
        if(IS_ABSOLUTE "${rpath}")
            list(APPEND rpaths "${rpath}")
        else()
            if(relative_rpath_supported)
                octk_compute_relative_rpath_base("${rpath}" "${arg_INSTALL_PATH}" relative_rpath)
                list(APPEND rpaths "${relative_rpath}")
            else()
                # Any extra relative rpaths on a platform that does not support relative rpaths,
                # need to be transformed into absolute ones.
                set(install_lib_dir_absolute "${CMAKE_INSTALL_PREFIX}/${OCTK_INSTALL_LIBDIR}")
                get_filename_component(rpath_absolute "${rpath}"
                    ABSOLUTE BASE_DIR "${install_lib_dir_absolute}")
                list(APPEND rpaths "${rpath_absolute}")
            endif()
        endif()
    endforeach()

    if(rpaths)
        list(REMOVE_DUPLICATES rpaths)
        if(OCTK_BUILD_INSTALL)
            set(prop_name "INSTALL_RPATH")
        else()
            set(prop_name "BUILD_RPATH")
        endif()
        set_property(TARGET "${target}" APPEND PROPERTY "${prop_name}" ${rpaths})
    endif()
endfunction()


# Overrides the CMAKE_STAGING_PREFIX in a subdirectory scope, to stop CMake from rewriting build
# rpaths to point into the original staging prefix, and thus breaking running executables from
# the build directory.
# See details at https://bugreports.octk.io/browse/OCTKBUG-102592
# and https://gitlab.kitware.com/cmake/cmake/-/issues/23421
#
# This macro is only meant to be called in functions like
# octk_internal_add_library / octk_add_tool to ensure the variable is set in the
# subdirectory scope of the calling function, and not in the actual function scope (where the
# variable assignment would have no effect).
#
# This is the best workaround we can currently do, but it comes with the disadvantage that calling
# subdirectory-scoped install targets does not work anymore.
# e.g. calling ninja src/gui/install will try to install to the fake prefix and fail.
# A regular ninja install call works fine.
#
# Usage of this macro assumes there are no binaries or libraries added in the root CMakeLists.txt
# of the project because that would mean the macro is called at root level scope, which would
# break installation.
#
# The implementation has to be a macro, so we can propagate the variable into the calling
# subdirectory scope. The implementation can't use return().
macro(octk_internal_apply_staging_prefix_build_rpath_workaround)
    set(__octk_internal_should_apply_staging_prefix_build_rpath_workaround TRUE)
    # Allow an opt out.
    if(OCTK_NO_STAGING_PREFIX_BUILD_RPATH_WORKAROUND)
        set(__octk_internal_should_apply_staging_prefix_build_rpath_workaround FALSE)
    endif()

    # No need for workaround if CMAKE_STAGING_PREFIX is not set.
    if(NOT CMAKE_STAGING_PREFIX)
        set(__octk_internal_should_apply_staging_prefix_build_rpath_workaround FALSE)
    endif()

    # No rpath support for win32, android, ios, so nothing to do.
    if(WIN32 OR ANDROID OR UIKIT)
        set(__octk_internal_should_apply_staging_prefix_build_rpath_workaround FALSE)
    endif()

    # Set the staging prefix to a non-existent directory, which is unlikely to have permissions
    # for installation.
    # The verbose directory name is chosen to attract the user's attention in case if they end up
    # calling a subdirectory-scope install file.
    if(__octk_internal_should_apply_staging_prefix_build_rpath_workaround)
        set_property(GLOBAL PROPERTY _octk_internal_staging_prefix_build_rpath_workaround TRUE)
        set(CMAKE_STAGING_PREFIX
            "/octk_fake_staging_prefix/check_octk_internal_apply_staging_prefix_build_rpath_workaround"
            PARENT_SCOPE)
    endif()
endmacro()
