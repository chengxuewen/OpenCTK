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

# We can't create the same interface imported target multiple times, CMake will complain if we do
# that. This can happen if the find_package call is done in multiple different subdirectories.
if(TARGET OCTK3rdparty::WrapBoostBeast)
    set(OCTKWrapBoostBeast_FOUND ON)
    return()
endif()

include(InstallVcpkg)
set(OCTKWrapBoostBeast_NAME "boost-beast")
set(OCTKWrapBoostBeast_ROOT_DIR "${PROJECT_BINARY_DIR}/3rdparty/${OCTKWrapBoostBeast_NAME}")
if(WIN32)
    set(OCTKWrapBoostBeast_VCPKG_TRIPLET ${OCTK_VCPKG_TRIPLET}-static-md)
else()
    set(OCTKWrapBoostBeast_VCPKG_TRIPLET ${OCTK_VCPKG_TRIPLET})
endif()
set(OCTKWrapBoostBeast_INSTALL_DIR "${OCTKWrapBoostBeast_ROOT_DIR}/installed/${OCTKWrapBoostBeast_VCPKG_TRIPLET}" CACHE INTERNAL "" FORCE)
if(NOT EXISTS "${OCTKWrapBoostBeast_INSTALL_DIR}")
    execute_process(
        COMMAND ${OCTKVcpkg_EXECUTABLE} list ${OCTKWrapBoostBeast_NAME}:${OCTKWrapBoostBeast_VCPKG_TRIPLET}
        WORKING_DIRECTORY "${OCTKVcpkg_ROOT_DIR}"
        OUTPUT_VARIABLE FIND_OUTPUT
        RESULT_VARIABLE FIND_RESULT)
    if("X${FIND_OUTPUT}" STREQUAL "X")
        message(STATUS "${OCTKWrapBoostBeast_NAME} not installed, start install...")
        set(OCTKWrapBoostBeast_VCPKG_CONFIGS ${OCTKWrapBoostBeast_NAME}:${OCTKWrapBoostBeast_VCPKG_TRIPLET})
        message(STATUS "${OCTKWrapBoostBeast_NAME} vcpkg install configs: ${OCTKWrapBoostBeast_VCPKG_CONFIGS}")
        execute_process(
            COMMAND "${OCTKVcpkg_EXECUTABLE}" install ${OCTKWrapBoostBeast_VCPKG_CONFIGS} --recurse
            WORKING_DIRECTORY "${OCTKVcpkg_ROOT_DIR}"
            RESULT_VARIABLE INSTALL_RESULT
            COMMAND_ECHO STDOUT)
        if(NOT (INSTALL_RESULT MATCHES 0))
            message(FATAL_ERROR "${OCTKWrapBoostBeast_NAME} install failed.")
        endif()
    endif()

    execute_process(
        COMMAND "${OCTKVcpkg_EXECUTABLE}" export ${OCTKWrapBoostBeast_NAME}:${OCTKWrapBoostBeast_VCPKG_TRIPLET}
        --raw --output=${OCTKWrapBoostBeast_NAME} --output-dir=${PROJECT_BINARY_DIR}/3rdparty
        WORKING_DIRECTORY "${OCTKVcpkg_ROOT_DIR}"
        RESULT_VARIABLE EXPORT_RESULT
        COMMAND_ECHO STDOUT)
    if(NOT (EXPORT_RESULT MATCHES 0))
        message(FATAL_ERROR "${OCTKWrapBoostBeast_NAME} export failed.")
    endif()
endif()
find_package(boost_beast PATHS ${OCTKWrapBoostBeast_INSTALL_DIR} NO_DEFAULT_PATH REQUIRED)
add_library(OCTK3rdparty::WrapBoostBeast INTERFACE IMPORTED)
target_link_libraries(OCTK3rdparty::WrapBoostBeast INTERFACE Boost::beast)
set(OCTKWrapBoostBeast_FOUND ON)
