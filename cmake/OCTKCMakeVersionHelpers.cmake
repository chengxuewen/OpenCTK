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

# Returns the oldest CMake version for which NEW policies should be enabled.
# It can be older than the minimum supported or computed CMake version, as it
# is only used for policy settings. The currently running CMake must not be
# older than this version though (doing so will result in an error).
function(octk_internal_get_min_new_policy_cmake_version out_var)
    # OCTK_MIN_NEW_POLICY_CMAKE_VERSION is set either in .cmake.conf or in
    # OCTKBuildInternalsExtras.cmake when building a child repo.
    set(lower_version "${OCTK_MIN_NEW_POLICY_CMAKE_VERSION}")
    set(${out_var} "${lower_version}" PARENT_SCOPE)
endfunction()


# Returns the latest CMake version for which NEW policies should be enabled.
# This cannot be less than the minimum CMake policy version or we will end up
# specifying a version range with the max less than the min.
function(octk_internal_get_max_new_policy_cmake_version out_var)
    # OCTK_MAX_NEW_POLICY_CMAKE_VERSION is set either in .cmake.conf or in
    # OCTKBuildInternalsExtras.cmake when building a child repo.
    set(upper_version "${OCTK_MAX_NEW_POLICY_CMAKE_VERSION}")
    octk_internal_get_min_new_policy_cmake_version(lower_version)
    if(upper_version VERSION_LESS lower_version)
        set(upper_version ${lower_version})
    endif()
    set(${out_var} "${upper_version}" PARENT_SCOPE)
endfunction()
