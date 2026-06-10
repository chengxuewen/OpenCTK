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
if(TARGET OpenCTKWrapLibyuv::WrapLibyuv)
    set(OpenCTKWrapLibyuv_FOUND ON)
    return()
endif()

# in arm64/aarch64-linux, need gcc10!
set(OpenCTKWrapLibyuv_NAME "libyuv")
set(OpenCTKWrapLibyuv_PKG_NAME "${OpenCTKWrapLibyuv_NAME}.7z")
set(OpenCTKWrapLibyuv_DIR_NAME "${OpenCTKWrapLibyuv_NAME}-${OCTK_LOWER_BUILD_TYPE}")
set(OpenCTKWrapLibyuv_URL_PATH "${PROJECT_SOURCE_DIR}/3rdparty/${OpenCTKWrapLibyuv_PKG_NAME}")
set(OpenCTKWrapLibyuv_ROOT_DIR "${PROJECT_BINARY_DIR}/3rdparty/${OpenCTKWrapLibyuv_DIR_NAME}")
set(OpenCTKWrapLibyuv_BUILD_DIR "${OpenCTKWrapLibyuv_ROOT_DIR}/build" CACHE INTERNAL "" FORCE)
set(OpenCTKWrapLibyuv_SOURCE_DIR "${OpenCTKWrapLibyuv_ROOT_DIR}/source" CACHE INTERNAL "" FORCE)
set(OpenCTKWrapLibyuv_INSTALL_DIR "${OpenCTKWrapLibyuv_ROOT_DIR}/install" CACHE INTERNAL "" FORCE)
octk_stamp_file_info(OpenCTKWrapLibyuv OUTPUT_DIR "${OpenCTKWrapLibyuv_ROOT_DIR}")
octk_fetch_3rdparty(OpenCTKWrapLibyuv URL "${OpenCTKWrapLibyuv_URL_PATH}" OUTPUT_NAME "${OpenCTKWrapLibyuv_DIR_NAME}")
if(NOT EXISTS "${OpenCTKWrapLibyuv_STAMP_FILE_PATH}")
    if(NOT EXISTS ${OpenCTKWrapLibyuv_SOURCE_DIR})
        message(FATAL_ERROR "${OpenCTKWrapLibyuv_NAME} FetchContent failed.")
    endif()
    octk_reset_dir(${OpenCTKWrapLibyuv_BUILD_DIR})

    message(STATUS "Configure ${OpenCTKWrapLibyuv_NAME} lib...")
    execute_process(
        COMMAND ${CMAKE_COMMAND}
        -Wno-deprecated
        --no-warn-unused-cli
        -G ${CMAKE_GENERATOR}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX=${OpenCTKWrapLibyuv_INSTALL_DIR}
        -DCMAKE_POLICY_VERSION_MINIMUM=3.5
        ${OpenCTKWrapLibyuv_SOURCE_DIR}
        WORKING_DIRECTORY "${OpenCTKWrapLibyuv_BUILD_DIR}"
        RESULT_VARIABLE CONFIGURE_RESULT)
    if(NOT CONFIGURE_RESULT MATCHES 0)
        message(FATAL_ERROR "${OpenCTKWrapLibyuv_NAME} configure failed.")
    endif()

    message(STATUS "${OpenCTKWrapLibyuv_NAME} configure success")
    execute_process(
        COMMAND ${CMAKE_COMMAND} --build ./ --parallel ${OCTK_NUMBER_OF_ASYNC_JOBS}
        --config ${CMAKE_BUILD_TYPE} --target install
        WORKING_DIRECTORY "${OpenCTKWrapLibyuv_BUILD_DIR}"
        RESULT_VARIABLE BUILD_RESULT)
    if(NOT BUILD_RESULT MATCHES 0)
        message(FATAL_ERROR "${OpenCTKWrapLibyuv_NAME} build failed.")
    endif()
    message(STATUS "${OpenCTKWrapLibyuv_NAME} build success")

    execute_process(
        COMMAND ${CMAKE_COMMAND} --install ./
        WORKING_DIRECTORY "${OpenCTKWrapLibyuv_BUILD_DIR}"
        RESULT_VARIABLE INSTALL_RESULT)
    if(NOT INSTALL_RESULT MATCHES 0)
        message(FATAL_ERROR "${OpenCTKWrapLibyuv_NAME} install failed.")
    endif()
    message(STATUS "${OpenCTKWrapLibyuv_NAME} install success")
    octk_make_stamp_file("${OpenCTKWrapLibyuv_STAMP_FILE_PATH}")
endif()
# wrap lib
add_library(OpenCTKWrapLibyuv::WrapLibyuv STATIC IMPORTED)
if(WIN32)
    set(OpenCTKWrapLibyuv_LIBRARY yuv.lib)
else()
    set(OpenCTKWrapLibyuv_LIBRARY libyuv.a)
endif()
set_target_properties(OpenCTKWrapLibyuv::WrapLibyuv PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES
    ${OpenCTKWrapLibyuv_INSTALL_DIR}/include
    IMPORTED_LOCATION
    ${OpenCTKWrapLibyuv_INSTALL_DIR}/lib/${OpenCTKWrapLibyuv_LIBRARY})
find_package(JPEG)
if(JPEG_FOUND)
    target_link_libraries(OpenCTKWrapLibyuv::WrapLibyuv INTERFACE ${JPEG_LIBRARY})
endif()
if(NOT EXISTS "${OCTK_BUILD_DIR}/third_party/libyuv/include/libyuv")
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${OpenCTKWrapLibyuv_INSTALL_DIR}/include"
        "${OCTK_BUILD_DIR}/third_party/libyuv/include"
        WORKING_DIRECTORY "${OpenCTKWrapLibyuv_ROOT_DIR}"
        ERROR_QUIET)
endif()
set(OpenCTKWrapLibyuv_FOUND ON)