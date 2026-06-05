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
if(TARGET OpenCTKWrapCLI11::WrapCLI11)
    set(OpenCTKWrapCLI11_FOUND ON)
    return()
endif()

set(OpenCTKWrapCLI11_NAME "CLI11-2.6.2")
set(OpenCTKWrapCLI11_PKG_NAME "${OpenCTKWrapCLI11_NAME}.tar.gz")
set(OpenCTKWrapCLI11_DIR_NAME "${OpenCTKWrapCLI11_NAME}-${OCTK_LOWER_BUILD_TYPE}")
set(OpenCTKWrapCLI11_URL_PATH "${PROJECT_SOURCE_DIR}/3rdparty/${OpenCTKWrapCLI11_PKG_NAME}")
set(OpenCTKWrapCLI11_ROOT_DIR "${PROJECT_BINARY_DIR}/3rdparty/${OpenCTKWrapCLI11_DIR_NAME}")
set(OpenCTKWrapCLI11_BUILD_DIR "${OpenCTKWrapCLI11_ROOT_DIR}/build" CACHE INTERNAL "" FORCE)
set(OpenCTKWrapCLI11_SOURCE_DIR "${OpenCTKWrapCLI11_ROOT_DIR}/source" CACHE INTERNAL "" FORCE)
set(OpenCTKWrapCLI11_INSTALL_DIR "${OpenCTKWrapCLI11_ROOT_DIR}/install" CACHE INTERNAL "" FORCE)
octk_stamp_file_info(OpenCTKWrapCLI11 OUTPUT_DIR "${OpenCTKWrapCLI11_ROOT_DIR}")
octk_fetch_3rdparty(OpenCTKWrapCLI11 URL "${OpenCTKWrapCLI11_URL_PATH}" OUTPUT_NAME "${OpenCTKWrapCLI11_DIR_NAME}")
if(NOT EXISTS "${OpenCTKWrapCLI11_STAMP_FILE_PATH}")
    if(NOT EXISTS ${OpenCTKWrapCLI11_SOURCE_DIR})
        message(FATAL_ERROR "${OpenCTKWrapCLI11_NAME} FetchContent failed.")
    endif()
    octk_reset_dir(${OpenCTKWrapCLI11_BUILD_DIR})

    message(STATUS "Configure ${OpenCTKWrapCLI11_NAME} lib...")
    execute_process(
        COMMAND ${CMAKE_COMMAND}
        -G ${CMAKE_GENERATOR}
        -DCLI11_BUILD_DOCS=OFF
        -DCLI11_BUILD_TESTS=OFF
        -DCLI11_BUILD_EXAMPLES=OFF
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON
        -DCMAKE_INSTALL_PREFIX=${OpenCTKWrapCLI11_INSTALL_DIR}
        ${OpenCTKWrapCLI11_SOURCE_DIR}
        WORKING_DIRECTORY "${OpenCTKWrapCLI11_BUILD_DIR}"
        RESULT_VARIABLE CONFIGURE_RESULT)
    if(NOT CONFIGURE_RESULT MATCHES 0)
        message(FATAL_ERROR "${OpenCTKWrapCLI11_NAME} configure failed.")
    endif()
    message(STATUS "${OpenCTKWrapCLI11_NAME} configure success")

    execute_process(
        COMMAND ${CMAKE_COMMAND} --build ./ --parallel ${OCTK_NUMBER_OF_ASYNC_JOBS} --config Release --target install
        WORKING_DIRECTORY "${OpenCTKWrapCLI11_BUILD_DIR}"
        RESULT_VARIABLE BUILD_RESULT)
    if(NOT BUILD_RESULT MATCHES 0)
        message(FATAL_ERROR "${OpenCTKWrapCLI11_NAME} build failed.")
    endif()
    message(STATUS "${OpenCTKWrapCLI11_NAME} build success")

    execute_process(
        COMMAND ${CMAKE_COMMAND} --install ./
        WORKING_DIRECTORY "${OpenCTKWrapCLI11_BUILD_DIR}"
        RESULT_VARIABLE INSTALL_RESULT)
    if(NOT INSTALL_RESULT MATCHES 0)
        message(FATAL_ERROR "${OpenCTKWrapCLI11_NAME} install failed.")
    endif()
    message(STATUS "${OpenCTKWrapCLI11_NAME} install success")
    octk_make_stamp_file("${OpenCTKWrapCLI11_STAMP_FILE_PATH}")
endif()
# wrap lib
add_library(OpenCTKWrapCLI11::WrapCLI11 INTERFACE IMPORTED)
find_package(CLI11 PATHS ${OpenCTKWrapCLI11_INSTALL_DIR} NO_DEFAULT_PATH REQUIRED)
target_link_libraries(OpenCTKWrapCLI11::WrapCLI11 INTERFACE CLI11::CLI11)
set(OpenCTKWrapCLI11_FOUND ON)