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
if(TARGET OCTK3rdparty::WrapSpdlog)
    set(OCTKWrapSpdlog_FOUND ON)
    return()
endif()

set(OCTKWrapSpdlog_NAME "spdlog-1.15.3")
set(OCTKWrapSpdlog_PKG_NAME "${OCTKWrapSpdlog_NAME}.tar.gz")
set(OCTKWrapSpdlog_DIR_NAME "${OCTKWrapSpdlog_NAME}-${OCTK_LOWER_BUILD_TYPE}")
set(OCTKWrapSpdlog_URL_PATH "${PROJECT_SOURCE_DIR}/3rdparty/${OCTKWrapSpdlog_PKG_NAME}")
set(OCTKWrapSpdlog_ROOT_DIR "${PROJECT_BINARY_DIR}/3rdparty/${OCTKWrapSpdlog_DIR_NAME}")
set(OCTKWrapSpdlog_BUILD_DIR "${OCTKWrapSpdlog_ROOT_DIR}/build" CACHE INTERNAL "" FORCE)
set(OCTKWrapSpdlog_SOURCE_DIR "${OCTKWrapSpdlog_ROOT_DIR}/source" CACHE INTERNAL "" FORCE)
set(OCTKWrapSpdlog_INSTALL_DIR "${OCTKWrapSpdlog_ROOT_DIR}/install" CACHE INTERNAL "" FORCE)
octk_stamp_file_info(OCTKWrapSpdlog OUTPUT_DIR "${OCTKWrapSpdlog_ROOT_DIR}")
octk_fetch_3rdparty(OCTKWrapSpdlog URL "${OCTKWrapSpdlog_URL_PATH}" OUTPUT_NAME "${OCTKWrapSpdlog_DIR_NAME}")
if(NOT EXISTS "${OCTKWrapSpdlog_STAMP_FILE_PATH}")
    if(NOT EXISTS ${OCTKWrapSpdlog_SOURCE_DIR})
        message(FATAL_ERROR "${OCTKWrapSpdlog_DIR_NAME} FetchContent failed.")
    endif()
    octk_reset_dir(${OCTKWrapSpdlog_BUILD_DIR})

    message(STATUS "Configure ${OCTKWrapSpdlog_DIR_NAME} lib...")
    execute_process(
        COMMAND ${CMAKE_COMMAND}
        -G ${CMAKE_GENERATOR}
        -DSPDLOG_BUILD_PIC=ON
        -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_CONFIGURATION_TYPES=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX=${OCTKWrapSpdlog_INSTALL_DIR}
        ${OCTKWrapSpdlog_SOURCE_DIR}
        WORKING_DIRECTORY "${OCTKWrapSpdlog_BUILD_DIR}"
        RESULT_VARIABLE CONFIGURE_RESULT)
    if(NOT CONFIGURE_RESULT MATCHES 0)
        message(FATAL_ERROR "${OCTKWrapSpdlog_DIR_NAME} configure failed.")
    endif()
    message(STATUS "${OCTKWrapSpdlog_DIR_NAME} configure success")
    
    execute_process(
        COMMAND ${CMAKE_COMMAND} --build ./ --parallel ${OCTK_NUMBER_OF_ASYNC_JOBS} 
        --config ${CMAKE_BUILD_TYPE} --target install
        WORKING_DIRECTORY "${OCTKWrapSpdlog_BUILD_DIR}"
        RESULT_VARIABLE BUILD_RESULT)
    if(NOT BUILD_RESULT MATCHES 0)
        message(FATAL_ERROR "${OCTKWrapSpdlog_DIR_NAME} build failed.")
    endif()
    message(STATUS "${OCTKWrapSpdlog_DIR_NAME} build success")
    
    execute_process(
        COMMAND ${CMAKE_COMMAND} --install ./
        WORKING_DIRECTORY "${OCTKWrapSpdlog_BUILD_DIR}"
        RESULT_VARIABLE INSTALL_RESULT)
    if(NOT INSTALL_RESULT MATCHES 0)
        message(FATAL_ERROR "${OCTKWrapSpdlog_DIR_NAME} install failed.")
    endif()
    message(STATUS "${OCTKWrapSpdlog_DIR_NAME} install success")
    octk_make_stamp_file("${OCTKWrapSpdlog_STAMP_FILE_PATH}")
endif()
# wrap lib
add_library(OCTK3rdparty::WrapSpdlog INTERFACE IMPORTED)
find_package(spdlog PATHS ${OCTKWrapSpdlog_INSTALL_DIR} REQUIRED)
target_link_libraries(OCTK3rdparty::WrapSpdlog INTERFACE spdlog::spdlog_header_only)
set(OCTKWrapSpdlog_FOUND ON)