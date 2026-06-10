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
if(TARGET OpenCTKWrapVariant::WrapVariant)
    set(OpenCTKWrapVariant_FOUND ON)
    return()
endif()

set(OpenCTKWrapVariant_NAME "variant-1.4.0")
set(OpenCTKWrapVariant_PKG_NAME "${OpenCTKWrapVariant_NAME}.tar.gz")
set(OpenCTKWrapVariant_DIR_NAME "${OpenCTKWrapVariant_NAME}-${OCTK_LOWER_BUILD_TYPE}")
set(OpenCTKWrapVariant_URL_PATH "${PROJECT_SOURCE_DIR}/3rdparty/${OpenCTKWrapVariant_PKG_NAME}")
set(OpenCTKWrapVariant_ROOT_DIR "${PROJECT_BINARY_DIR}/3rdparty/${OpenCTKWrapVariant_DIR_NAME}")
set(OpenCTKWrapVariant_BUILD_DIR "${OpenCTKWrapVariant_ROOT_DIR}/build" CACHE INTERNAL "" FORCE)
set(OpenCTKWrapVariant_SOURCE_DIR "${OpenCTKWrapVariant_ROOT_DIR}/source" CACHE INTERNAL "" FORCE)
set(OpenCTKWrapVariant_INSTALL_DIR "${OpenCTKWrapVariant_ROOT_DIR}/install" CACHE INTERNAL "" FORCE)
octk_stamp_file_info(OpenCTKWrapVariant OUTPUT_DIR "${OpenCTKWrapVariant_ROOT_DIR}")
octk_fetch_3rdparty(OpenCTKWrapVariant URL "${OpenCTKWrapVariant_URL_PATH}" OUTPUT_NAME "${OpenCTKWrapVariant_DIR_NAME}")
if(NOT EXISTS "${OpenCTKWrapVariant_STAMP_FILE_PATH}")
    if(NOT EXISTS ${OpenCTKWrapVariant_SOURCE_DIR})
        message(FATAL_ERROR "${OpenCTKWrapVariant_NAME} FetchContent failed.")
    endif()
    octk_reset_dir(${OpenCTKWrapVariant_BUILD_DIR})

    message(STATUS "Configure ${OpenCTKWrapVariant_NAME} lib...")
    execute_process(
        COMMAND ${CMAKE_COMMAND}
        -Wno-deprecated
        --no-warn-unused-cli
        -G ${CMAKE_GENERATOR}
        -DCMAKE_INSTALL_PREFIX=${OpenCTKWrapVariant_INSTALL_DIR}
        ${OpenCTKWrapVariant_SOURCE_DIR}
        WORKING_DIRECTORY "${OpenCTKWrapVariant_BUILD_DIR}"
        RESULT_VARIABLE CONFIGURE_RESULT)
    if(NOT CONFIGURE_RESULT MATCHES 0)
        message(FATAL_ERROR "${OpenCTKWrapVariant_NAME} configure failed.")
    endif()
    message(STATUS "${OpenCTKWrapVariant_NAME} configure success")

    execute_process(
        COMMAND ${CMAKE_COMMAND} --build ./ --parallel ${OCTK_NUMBER_OF_ASYNC_JOBS} --config Release --target install
        WORKING_DIRECTORY "${OpenCTKWrapVariant_BUILD_DIR}"
        RESULT_VARIABLE BUILD_RESULT)
    if(NOT BUILD_RESULT MATCHES 0)
        message(FATAL_ERROR "${OpenCTKWrapVariant_NAME} build failed.")
    endif()
    message(STATUS "${OpenCTKWrapVariant_NAME} build success")

    execute_process(
        COMMAND ${CMAKE_COMMAND} --install ./
        WORKING_DIRECTORY "${OpenCTKWrapVariant_BUILD_DIR}"
        RESULT_VARIABLE INSTALL_RESULT)
    if(NOT INSTALL_RESULT MATCHES 0)
        message(FATAL_ERROR "${OpenCTKWrapVariant_NAME} install failed.")
    endif()
    message(STATUS "${OpenCTKWrapVariant_NAME} install success")
    octk_make_stamp_file("${OpenCTKWrapVariant_STAMP_FILE_PATH}")
endif()
# wrap lib
add_library(OpenCTKWrapVariant::WrapVariant INTERFACE IMPORTED)
find_package(mpark_variant PATHS ${OpenCTKWrapVariant_INSTALL_DIR} REQUIRED)
target_link_libraries(OpenCTKWrapVariant::WrapVariant INTERFACE mpark_variant)
set(OpenCTKWrapVariant_FOUND ON)