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
if(TARGET OCTK3rdparty::WrapStringViewLite)
    set(OCTKWrapStringviewLite_FOUND ON)
    return()
endif()

set(OCTKWrapStringviewLite_NAME "string-view-lite-1.8.0")
set(OCTKWrapStringviewLite_PKG_NAME "${OCTKWrapStringviewLite_NAME}.tar.gz")
set(OCTKWrapStringviewLite_DIR_NAME "${OCTKWrapStringviewLite_NAME}-${OCTK_LOWER_BUILD_TYPE}")
set(OCTKWrapStringviewLite_URL_PATH "${PROJECT_SOURCE_DIR}/3rdparty/${OCTKWrapStringviewLite_PKG_NAME}")
set(OCTKWrapStringviewLite_ROOT_DIR "${PROJECT_BINARY_DIR}/3rdparty/${OCTKWrapStringviewLite_DIR_NAME}")
set(OCTKWrapStringviewLite_BUILD_DIR "${OCTKWrapStringviewLite_ROOT_DIR}/build" CACHE INTERNAL "" FORCE)
set(OCTKWrapStringviewLite_SOURCE_DIR "${OCTKWrapStringviewLite_ROOT_DIR}/source" CACHE INTERNAL "" FORCE)
set(OCTKWrapStringviewLite_INSTALL_DIR "${OCTKWrapStringviewLite_ROOT_DIR}/install" CACHE INTERNAL "" FORCE)
octk_stamp_file_info(OCTKWrapStringviewLite OUTPUT_DIR "${OCTKWrapStringviewLite_ROOT_DIR}")
octk_fetch_3rdparty(OCTKWrapStringviewLite URL "${OCTKWrapStringviewLite_URL_PATH}" OUTPUT_NAME "${OCTKWrapStringviewLite_DIR_NAME}")
if(NOT EXISTS "${OCTKWrapStringviewLite_STAMP_FILE_PATH}")
    if(NOT EXISTS ${OCTKWrapStringviewLite_SOURCE_DIR})
        message(FATAL_ERROR "${OCTKWrapStringviewLite_NAME} FetchContent failed.")
    endif()
    octk_reset_dir(PARENT_DIR ${OCTKWrapStringviewLite_ROOT_DIR} TARGET_NAME build)

    message(STATUS "Configure ${OCTKWrapStringviewLite_NAME} lib...")
    execute_process(
        COMMAND ${CMAKE_COMMAND}
        -G ${CMAKE_GENERATOR}
        -DSTRING_VIEW_LITE_OPT_BUILD_TESTS=OFF
        -DCMAKE_INSTALL_PREFIX=${OCTKWrapStringviewLite_INSTALL_DIR}
        ${OCTKWrapStringviewLite_SOURCE_DIR}
        WORKING_DIRECTORY "${OCTKWrapStringviewLite_BUILD_DIR}"
        RESULT_VARIABLE CONFIGURE_RESULT)
    if(NOT CONFIGURE_RESULT MATCHES 0)
        message(FATAL_ERROR "${OCTKWrapStringviewLite_NAME} configure failed.")
    endif()
    message(STATUS "${OCTKWrapStringviewLite_NAME} configure success")

    execute_process(
        COMMAND ${CMAKE_COMMAND} --build ./ --parallel ${OCTK_NUMBER_OF_ASYNC_JOBS} --config Release --target install
        WORKING_DIRECTORY "${OCTKWrapStringviewLite_BUILD_DIR}"
        RESULT_VARIABLE BUILD_RESULT)
    if(NOT BUILD_RESULT MATCHES 0)
        message(FATAL_ERROR "${OCTKWrapStringviewLite_NAME} build failed.")
    endif()
    message(STATUS "${OCTKWrapStringviewLite_NAME} build success")

    execute_process(
        COMMAND ${CMAKE_COMMAND} --install ./
        WORKING_DIRECTORY "${OCTKWrapStringviewLite_BUILD_DIR}"
        RESULT_VARIABLE INSTALL_RESULT)
    if(NOT INSTALL_RESULT MATCHES 0)
        message(FATAL_ERROR "${OCTKWrapStringviewLite_NAME} install failed.")
    endif()
    message(STATUS "${OCTKWrapStringviewLite_NAME} install success")
    octk_make_stamp_file("${OCTKWrapStringviewLite_STAMP_FILE_PATH}")
endif()
# wrap lib
add_library(OCTK3rdparty::WrapStringViewLite INTERFACE IMPORTED)
find_package(string-view-lite PATHS ${OCTKWrapStringviewLite_INSTALL_DIR} REQUIRED)
target_link_libraries(OCTK3rdparty::WrapStringViewLite INTERFACE nonstd::string-view-lite)
set(OCTKWrapStringviewLite_FOUND ON)