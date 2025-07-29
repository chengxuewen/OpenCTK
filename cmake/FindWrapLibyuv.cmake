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
if(TARGET OCTK3rdparty::WrapLibyuv)
    set(OCTKWrapLibyuv_FOUND ON)
    return()
endif()

set(OCTKWrapLibyuv_PKG_NAME "libyuv.zip")
set(OCTKWrapLibyuv_DIR_NAME "libyuv-${OCTK_LOWER_BUILD_TYPE}")
set(OCTKWrapLibyuv_URL_PATH "${PROJECT_SOURCE_DIR}/3rdparty/${OCTKWrapLibyuv_PKG_NAME}")
set(OCTKWrapLibyuv_ROOT_DIR "${PROJECT_BINARY_DIR}/3rdparty/${OCTKWrapLibyuv_DIR_NAME}")
set(OCTKWrapLibyuv_BUILD_DIR "${OCTKWrapLibyuv_ROOT_DIR}/build" CACHE INTERNAL "" FORCE)
set(OCTKWrapLibyuv_SOURCE_DIR "${OCTKWrapLibyuv_ROOT_DIR}/source" CACHE INTERNAL "" FORCE)
set(OCTKWrapLibyuv_INSTALL_DIR "${OCTKWrapLibyuv_ROOT_DIR}/install" CACHE INTERNAL "" FORCE)
octk_stamp_file_info(OCTKWrapLibyuv OUTPUT_DIR "${OCTKWrapLibyuv_ROOT_DIR}")
octk_fetch_3rdparty(OCTKWrapLibyuv URL "${OCTKWrapLibyuv_URL_PATH}" OUTPUT_NAME "${OCTKWrapLibyuv_DIR_NAME}")
if(NOT EXISTS "${OCTKWrapLibyuv_STAMP_FILE_PATH}")
    if(NOT EXISTS ${OCTKWrapLibyuv_SOURCE_DIR})
        message(FATAL_ERROR "${OCTKWrapLibyuv_DIR_NAME} FetchContent failed.")
    endif()
    octk_reset_dir(PARENT_DIR ${OCTKWrapLibyuv_ROOT_DIR} TARGET_NAME build)

    message(STATUS "Configure ${OCTKWrapLibyuv_DIR_NAME} lib...")
    execute_process(
        COMMAND ${CMAKE_COMMAND}
        -G ${CMAKE_GENERATOR}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_CONFIGURATION_TYPES=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX=${OCTKWrapLibyuv_INSTALL_DIR}
        -DCMAKE_POLICY_VERSION_MINIMUM=3.5
        ${OCTKWrapLibyuv_SOURCE_DIR}
        WORKING_DIRECTORY "${OCTKWrapLibyuv_BUILD_DIR}"
        RESULT_VARIABLE CONFIGURE_RESULT)
    if(NOT CONFIGURE_RESULT MATCHES 0)
        message(FATAL_ERROR "${OCTKWrapLibyuv_DIR_NAME} configure failed.")
    endif()

    message(STATUS "${OCTKWrapLibyuv_DIR_NAME} configure success")
    execute_process(
        COMMAND ${CMAKE_COMMAND} --build ./ --parallel ${OCTK_NUMBER_OF_ASYNC_JOBS}
        --config ${CMAKE_BUILD_TYPE} --target install
        WORKING_DIRECTORY "${OCTKWrapLibyuv_BUILD_DIR}"
        RESULT_VARIABLE BUILD_RESULT)
    if(NOT BUILD_RESULT MATCHES 0)
        message(FATAL_ERROR "${OCTKWrapLibyuv_DIR_NAME} build failed.")
    endif()
    message(STATUS "${OCTKWrapLibyuv_DIR_NAME} build success")

    execute_process(
        COMMAND ${CMAKE_COMMAND} --install ./
        WORKING_DIRECTORY "${OCTKWrapLibyuv_BUILD_DIR}"
        RESULT_VARIABLE INSTALL_RESULT)
    if(NOT INSTALL_RESULT MATCHES 0)
        message(FATAL_ERROR "${OCTKWrapLibyuv_DIR_NAME} install failed.")
    endif()
    message(STATUS "${OCTKWrapLibyuv_DIR_NAME} install success")
    octk_make_stamp_file("${OCTKWrapLibyuv_STAMP_FILE_PATH}")
endif()
# wrap lib
add_library(OCTK3rdparty::WrapLibyuv STATIC IMPORTED)
if(WIN32)
    set(OCTKWrapLibyuv_LIBRARY yuv.lib)
else()
    set(OCTKWrapLibyuv_LIBRARY libyuv.a)
endif()
set_target_properties(OCTK3rdparty::WrapLibyuv PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES
    ${OCTKWrapLibyuv_INSTALL_DIR}/include
    IMPORTED_LOCATION
    ${OCTKWrapLibyuv_INSTALL_DIR}/lib/${OCTKWrapLibyuv_LIBRARY})
find_package(JPEG)
if(JPEG_FOUND)
    target_link_libraries(OCTK3rdparty::WrapLibyuv INTERFACE ${JPEG_LIBRARY})
endif()
if(NOT EXISTS "${OCTK_BUILD_DIR}/third_party/libyuv/include/libyuv")
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${OCTKWrapLibyuv_INSTALL_DIR}/include"
        "${OCTK_BUILD_DIR}/third_party/libyuv/include"
        WORKING_DIRECTORY "${OCTKWrapLibyuv_ROOT_DIR}"
        ERROR_QUIET)
endif()
set(OCTKWrapLibyuv_FOUND ON)