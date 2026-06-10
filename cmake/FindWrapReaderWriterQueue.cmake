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
if(TARGET OpenCTKWrapReaderWriterQueue::WrapReaderWriterQueue)
    set(OpenCTKWrapReaderWriterQueue_FOUND ON)
    return()
endif()

set(OpenCTKWrapReaderWriterQueue_NAME "readerwriterqueue-1.0.7")
set(OpenCTKWrapReaderWriterQueue_PKG_NAME "${OpenCTKWrapReaderWriterQueue_NAME}.tar.gz")
set(OpenCTKWrapReaderWriterQueue_DIR_NAME "${OpenCTKWrapReaderWriterQueue_NAME}-${OCTK_LOWER_BUILD_TYPE}")
set(OpenCTKWrapReaderWriterQueue_URL_PATH "${PROJECT_SOURCE_DIR}/3rdparty/${OpenCTKWrapReaderWriterQueue_PKG_NAME}")
set(OpenCTKWrapReaderWriterQueue_ROOT_DIR "${PROJECT_BINARY_DIR}/3rdparty/${OpenCTKWrapReaderWriterQueue_DIR_NAME}")
set(OpenCTKWrapReaderWriterQueue_BUILD_DIR "${OpenCTKWrapReaderWriterQueue_ROOT_DIR}/build" CACHE INTERNAL "" FORCE)
set(OpenCTKWrapReaderWriterQueue_SOURCE_DIR "${OpenCTKWrapReaderWriterQueue_ROOT_DIR}/source" CACHE INTERNAL "" FORCE)
set(OpenCTKWrapReaderWriterQueue_INSTALL_DIR "${OpenCTKWrapReaderWriterQueue_ROOT_DIR}/install" CACHE INTERNAL "" FORCE)
octk_stamp_file_info(OpenCTKWrapReaderWriterQueue OUTPUT_DIR "${OpenCTKWrapReaderWriterQueue_ROOT_DIR}")
octk_fetch_3rdparty(OpenCTKWrapReaderWriterQueue URL "${OpenCTKWrapReaderWriterQueue_URL_PATH}" OUTPUT_NAME "${OpenCTKWrapReaderWriterQueue_DIR_NAME}")
if(NOT EXISTS "${OpenCTKWrapReaderWriterQueue_STAMP_FILE_PATH}")
    if(NOT EXISTS ${OpenCTKWrapReaderWriterQueue_SOURCE_DIR})
        message(FATAL_ERROR "${OpenCTKWrapReaderWriterQueue_NAME} FetchContent failed.")
    endif()
    octk_reset_dir(${OpenCTKWrapReaderWriterQueue_BUILD_DIR})

    message(STATUS "Configure ${OpenCTKWrapReaderWriterQueue_NAME} lib...")
    execute_process(
        COMMAND ${CMAKE_COMMAND}
        -Wno-deprecated
        --no-warn-unused-cli
        -G ${CMAKE_GENERATOR}
        -DCMAKE_INSTALL_PREFIX=${OpenCTKWrapReaderWriterQueue_INSTALL_DIR}
        ${OpenCTKWrapReaderWriterQueue_SOURCE_DIR}
        WORKING_DIRECTORY "${OpenCTKWrapReaderWriterQueue_BUILD_DIR}"
        RESULT_VARIABLE CONFIGURE_RESULT)
    if(NOT CONFIGURE_RESULT MATCHES 0)
        message(FATAL_ERROR "${OpenCTKWrapReaderWriterQueue_NAME} configure failed.")
    endif()
    message(STATUS "${OpenCTKWrapReaderWriterQueue_NAME} configure success")

    execute_process(
        COMMAND ${CMAKE_COMMAND} --build ./ --parallel ${OCTK_NUMBER_OF_ASYNC_JOBS} --config Release --target install
        WORKING_DIRECTORY "${OpenCTKWrapReaderWriterQueue_BUILD_DIR}"
        RESULT_VARIABLE BUILD_RESULT)
    if(NOT BUILD_RESULT MATCHES 0)
        message(FATAL_ERROR "${OpenCTKWrapReaderWriterQueue_NAME} build failed.")
    endif()
    message(STATUS "${OpenCTKWrapReaderWriterQueue_NAME} build success")

    execute_process(
        COMMAND ${CMAKE_COMMAND} --install ./
        WORKING_DIRECTORY "${OpenCTKWrapReaderWriterQueue_BUILD_DIR}"
        RESULT_VARIABLE INSTALL_RESULT)
    if(NOT INSTALL_RESULT MATCHES 0)
        message(FATAL_ERROR "${OpenCTKWrapReaderWriterQueue_NAME} install failed.")
    endif()
    message(STATUS "${OpenCTKWrapReaderWriterQueue_NAME} install success")
    octk_make_stamp_file("${OpenCTKWrapReaderWriterQueue_STAMP_FILE_PATH}")
endif()
# wrap lib
add_library(OpenCTKWrapReaderWriterQueue::WrapReaderWriterQueue INTERFACE IMPORTED)
find_package(readerwriterqueue PATHS ${OpenCTKWrapReaderWriterQueue_INSTALL_DIR} NO_DEFAULT_PATH REQUIRED)
target_link_libraries(OpenCTKWrapReaderWriterQueue::WrapReaderWriterQueue INTERFACE readerwriterqueue::readerwriterqueue)
set(OpenCTKWrapReaderWriterQueue_FOUND ON)