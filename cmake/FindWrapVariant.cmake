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
if(TARGET OCTK3rdparty::WrapVariant)
    set(OCTKWrapVariant_FOUND ON)
    return()
endif()

set(OCTKWrapVariant_NAME "variant-1.4.0")
set(OCTKWrapVariant_PKG_NAME "${OCTKWrapVariant_NAME}.tar.gz")
set(OCTKWrapVariant_DIR_NAME "${OCTKWrapVariant_NAME}-${OCTK_LOWER_BUILD_TYPE}")
set(OCTKWrapVariant_URL_PATH "${PROJECT_SOURCE_DIR}/3rdparty/${OCTKWrapVariant_PKG_NAME}")
set(OCTKWrapVariant_ROOT_DIR "${PROJECT_BINARY_DIR}/3rdparty/${OCTKWrapVariant_DIR_NAME}")
set(OCTKWrapVariant_BUILD_DIR "${OCTKWrapVariant_ROOT_DIR}/build" CACHE INTERNAL "" FORCE)
set(OCTKWrapVariant_SOURCE_DIR "${OCTKWrapVariant_ROOT_DIR}/source" CACHE INTERNAL "" FORCE)
set(OCTKWrapVariant_INSTALL_DIR "${OCTKWrapVariant_ROOT_DIR}/install" CACHE INTERNAL "" FORCE)
octk_stamp_file_info(OCTKWrapVariant OUTPUT_DIR "${OCTKWrapVariant_ROOT_DIR}")
octk_fetch_3rdparty(OCTKWrapVariant URL "${OCTKWrapVariant_URL_PATH}" OUTPUT_NAME "${OCTKWrapVariant_DIR_NAME}")
if(NOT EXISTS "${OCTKWrapVariant_STAMP_FILE_PATH}")
    if(NOT EXISTS ${OCTKWrapVariant_SOURCE_DIR})
        message(FATAL_ERROR "${OCTKWrapVariant_NAME} FetchContent failed.")
    endif()
    octk_reset_dir(${OCTKWrapVariant_ROOT_DIR})

    message(STATUS "Configure ${OCTKWrapVariant_NAME} lib...")
    execute_process(
        COMMAND ${CMAKE_COMMAND}
        -G ${CMAKE_GENERATOR}
        -DCMAKE_INSTALL_PREFIX=${OCTKWrapVariant_INSTALL_DIR}
        ${OCTKWrapVariant_SOURCE_DIR}
        WORKING_DIRECTORY "${OCTKWrapVariant_BUILD_DIR}"
        RESULT_VARIABLE CONFIGURE_RESULT)
    if(NOT CONFIGURE_RESULT MATCHES 0)
        message(FATAL_ERROR "${OCTKWrapVariant_NAME} configure failed.")
    endif()
    message(STATUS "${OCTKWrapVariant_NAME} configure success")

    execute_process(
        COMMAND ${CMAKE_COMMAND} --build ./ --parallel ${OCTK_NUMBER_OF_ASYNC_JOBS} --config Release --target install
        WORKING_DIRECTORY "${OCTKWrapVariant_BUILD_DIR}"
        RESULT_VARIABLE BUILD_RESULT)
    if(NOT BUILD_RESULT MATCHES 0)
        message(FATAL_ERROR "${OCTKWrapVariant_NAME} build failed.")
    endif()
    message(STATUS "${OCTKWrapVariant_NAME} build success")

    execute_process(
        COMMAND ${CMAKE_COMMAND} --install ./
        WORKING_DIRECTORY "${OCTKWrapVariant_BUILD_DIR}"
        RESULT_VARIABLE INSTALL_RESULT)
    if(NOT INSTALL_RESULT MATCHES 0)
        message(FATAL_ERROR "${OCTKWrapVariant_NAME} install failed.")
    endif()
    message(STATUS "${OCTKWrapVariant_NAME} install success")
    octk_make_stamp_file("${OCTKWrapVariant_STAMP_FILE_PATH}")
endif()
# wrap lib
add_library(OCTK3rdparty::WrapVariant INTERFACE IMPORTED)
find_package(mpark_variant PATHS ${OCTKWrapVariant_INSTALL_DIR} REQUIRED)
target_link_libraries(OCTK3rdparty::WrapVariant INTERFACE mpark_variant)
set(OCTKWrapVariant_FOUND ON)