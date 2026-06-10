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
if(TARGET OpenCTKWrapFmt::WrapFmt)
    set(OpenCTKWrapFmt_FOUND ON)
    return()
endif()

set(OpenCTKWrapFmt_NAME "fmt-12.1.0")
set(OpenCTKWrapFmt_PKG_NAME "${OpenCTKWrapFmt_NAME}.zip")
set(OpenCTKWrapFmt_DIR_NAME "${OpenCTKWrapFmt_NAME}-${OCTK_LOWER_BUILD_TYPE}")
set(OpenCTKWrapFmt_URL_PATH "${PROJECT_SOURCE_DIR}/3rdparty/${OpenCTKWrapFmt_PKG_NAME}")
set(OpenCTKWrapFmt_ROOT_DIR "${PROJECT_BINARY_DIR}/3rdparty/${OpenCTKWrapFmt_DIR_NAME}")
set(OpenCTKWrapFmt_BUILD_DIR "${OpenCTKWrapFmt_ROOT_DIR}/build" CACHE INTERNAL "" FORCE)
set(OpenCTKWrapFmt_SOURCE_DIR "${OpenCTKWrapFmt_ROOT_DIR}/source" CACHE INTERNAL "" FORCE)
set(OpenCTKWrapFmt_INSTALL_DIR "${OpenCTKWrapFmt_ROOT_DIR}/install" CACHE INTERNAL "" FORCE)
octk_stamp_file_info(OpenCTKWrapFmt OUTPUT_DIR "${OpenCTKWrapFmt_ROOT_DIR}")
octk_fetch_3rdparty(OpenCTKWrapFmt URL "${OpenCTKWrapFmt_URL_PATH}" OUTPUT_NAME "${OpenCTKWrapFmt_DIR_NAME}")
if(NOT EXISTS "${OpenCTKWrapFmt_STAMP_FILE_PATH}")
    if(NOT EXISTS ${OpenCTKWrapFmt_SOURCE_DIR})
        message(FATAL_ERROR "${OpenCTKWrapFmt_NAME} FetchContent failed.")
    endif()
    octk_reset_dir(${OpenCTKWrapFmt_BUILD_DIR})

    message(STATUS "Configure ${OpenCTKWrapFmt_NAME} lib...")
    execute_process(
        COMMAND ${CMAKE_COMMAND}
        -Wno-deprecated
        --no-warn-unused-cli
        -G ${CMAKE_GENERATOR}
        -DFMT_DOC=OFF
        -DFMT_TEST=OFF
        -DFMT_CUDA_TEST=OFF
        -DFMT_INSTALL=ON
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON
        -DCMAKE_INSTALL_PREFIX=${OpenCTKWrapFmt_INSTALL_DIR}
        ${OpenCTKWrapFmt_SOURCE_DIR}
        WORKING_DIRECTORY "${OpenCTKWrapFmt_BUILD_DIR}"
        RESULT_VARIABLE CONFIGURE_RESULT)
    if(NOT CONFIGURE_RESULT MATCHES 0)
        message(FATAL_ERROR "${OpenCTKWrapFmt_NAME} configure failed.")
    endif()
    message(STATUS "${OpenCTKWrapFmt_NAME} configure success")

    execute_process(
        COMMAND ${CMAKE_COMMAND} --build ./ --parallel ${OCTK_NUMBER_OF_ASYNC_JOBS} --config Release --target install
        WORKING_DIRECTORY "${OpenCTKWrapFmt_BUILD_DIR}"
        RESULT_VARIABLE BUILD_RESULT)
    if(NOT BUILD_RESULT MATCHES 0)
        message(FATAL_ERROR "${OpenCTKWrapFmt_NAME} build failed.")
    endif()
    message(STATUS "${OpenCTKWrapFmt_NAME} build success")

    execute_process(
        COMMAND ${CMAKE_COMMAND} --install ./
        WORKING_DIRECTORY "${OpenCTKWrapFmt_BUILD_DIR}"
        RESULT_VARIABLE INSTALL_RESULT)
    if(NOT INSTALL_RESULT MATCHES 0)
        message(FATAL_ERROR "${OpenCTKWrapFmt_NAME} install failed.")
    endif()
    message(STATUS "${OpenCTKWrapFmt_NAME} install success")
    octk_make_stamp_file("${OpenCTKWrapFmt_STAMP_FILE_PATH}")
endif()
# wrap lib
add_library(OpenCTKWrapFmt::WrapFmt INTERFACE IMPORTED)
find_package(fmt PATHS ${OpenCTKWrapFmt_INSTALL_DIR} NO_DEFAULT_PATH REQUIRED)
target_link_libraries(OpenCTKWrapFmt::WrapFmt INTERFACE fmt::fmt)
set(OpenCTKWrapFmt_FOUND ON)