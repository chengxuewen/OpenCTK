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
if(TARGET OCTK3rdparty::WrapBoostAsio)
    set(OCTKWrapBoostAsio_FOUND ON)
    return()
endif()

include(InstallVcpkg)
set(OCTKWrapBoostAsio_NAME "boost-asio")
set(OCTKWrapBoostAsio_ROOT_DIR "${PROJECT_BINARY_DIR}/3rdparty/${OCTKWrapBoostAsio_NAME}")
if(WIN32)
    set(OCTKWrapBoostAsio_VCPKG_TRIPLET ${OCTK_VCPKG_TRIPLET}-static-md)
else()
    set(OCTKWrapBoostAsio_VCPKG_TRIPLET ${OCTK_VCPKG_TRIPLET})
endif()
set(OCTKWrapBoostAsio_INSTALL_DIR "${OCTKWrapBoostAsio_ROOT_DIR}/installed/${OCTKWrapBoostAsio_VCPKG_TRIPLET}" CACHE INTERNAL "" FORCE)
if(NOT EXISTS "${OCTKWrapBoostAsio_INSTALL_DIR}")
    execute_process(
        COMMAND ${OCTKVcpkg_EXECUTABLE} list ${OCTKWrapBoostAsio_NAME}:${OCTKWrapBoostAsio_VCPKG_TRIPLET}
        WORKING_DIRECTORY "${OCTKVcpkg_ROOT_DIR}"
        OUTPUT_VARIABLE FIND_OUTPUT
        RESULT_VARIABLE FIND_RESULT)
    if("X${FIND_OUTPUT}" STREQUAL "X")
        message(STATUS "${OCTKWrapBoostAsio_NAME} not installed, start install...")
        set(OCTKWrapBoostAsio_VCPKG_CONFIGS ${OCTKWrapBoostAsio_NAME}:${OCTKWrapBoostAsio_VCPKG_TRIPLET})
        message(STATUS "${OCTKWrapBoostAsio_NAME} vcpkg install configs: ${OCTKWrapBoostAsio_VCPKG_CONFIGS}")
        execute_process(
            COMMAND "${OCTKVcpkg_EXECUTABLE}" install ${OCTKWrapBoostAsio_VCPKG_CONFIGS} --recurse
            WORKING_DIRECTORY "${OCTKVcpkg_ROOT_DIR}"
            RESULT_VARIABLE INSTALL_RESULT
            COMMAND_ECHO STDOUT)
        if(NOT (INSTALL_RESULT MATCHES 0))
            message(FATAL_ERROR "${OCTKWrapBoostAsio_NAME} install failed.")
        endif()
    endif()

    execute_process(
        COMMAND "${OCTKVcpkg_EXECUTABLE}" export ${OCTKWrapBoostAsio_NAME}:${OCTKWrapBoostAsio_VCPKG_TRIPLET}
        --raw --output=${OCTKWrapBoostAsio_NAME} --output-dir=${PROJECT_BINARY_DIR}/3rdparty
        WORKING_DIRECTORY "${OCTKVcpkg_ROOT_DIR}"
        RESULT_VARIABLE EXPORT_RESULT
        COMMAND_ECHO STDOUT)
    if(NOT (EXPORT_RESULT MATCHES 0))
        message(FATAL_ERROR "${OCTKWrapBoostAsio_NAME} export failed.")
    endif()
endif()
find_package(boost_asio PATHS ${OCTKWrapBoostAsio_INSTALL_DIR} NO_DEFAULT_PATH REQUIRED)
add_library(OCTK3rdparty::WrapBoostAsio INTERFACE IMPORTED)
target_link_libraries(OCTK3rdparty::WrapBoostAsio INTERFACE Boost::asio)
set(OCTKWrapBoostAsio_FOUND ON)