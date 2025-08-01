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
if(TARGET OCTK3rdparty::WrapFTXUI)
    set(OCTKWrapFTXUI_FOUND ON)
    return()
endif()

set(OCTKWrapFTXUI_DIR_NAME "FTXUI-5.0.0")
set(OCTKWrapFTXUI_PKG_NAME "FTXUI-5.0.0.zip")
set(OCTKWrapFTXUI_URL_PATH "${PROJECT_SOURCE_DIR}/3rdparty/${OCTKWrapFTXUI_PKG_NAME}")
set(OCTKWrapFTXUI_ROOT_DIR "${PROJECT_BINARY_DIR}/3rdparty/${OCTKWrapFTXUI_DIR_NAME}")
set(OCTKWrapFTXUI_BUILD_DIR "${OCTKWrapFTXUI_ROOT_DIR}/build" CACHE INTERNAL "" FORCE)
set(OCTKWrapFTXUI_SOURCE_DIR "${OCTKWrapFTXUI_ROOT_DIR}/source" CACHE INTERNAL "" FORCE)
set(OCTKWrapFTXUI_INSTALL_DIR "${OCTKWrapFTXUI_ROOT_DIR}/install" CACHE INTERNAL "" FORCE)
octk_stamp_file_info(OCTKWrapFTXUI OUTPUT_DIR "${OCTKWrapFTXUI_ROOT_DIR}")
octk_fetch_3rdparty(OCTKWrapFTXUI URL "${OCTKWrapFTXUI_URL_PATH}")
if(NOT EXISTS "${OCTKWrapFTXUI_STAMP_FILE_PATH}")
    if(NOT EXISTS ${OCTKWrapFTXUI_SOURCE_DIR})
        message(FATAL_ERROR "${OCTKWrapFTXUI_DIR_NAME} FetchContent failed.")
    endif()
    octk_reset_dir(PARENT_DIR ${OCTKWrapFTXUI_ROOT_DIR} TARGET_NAME build)

    message(STATUS "Configure ${OCTKWrapFTXUI_DIR_NAME} lib...")
    execute_process(
        COMMAND ${CMAKE_COMMAND}
        -G ${CMAKE_GENERATOR}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_CONFIGURATION_TYPES=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX=${OCTKWrapFTXUI_INSTALL_DIR}
        ${OCTKWrapFTXUI_SOURCE_DIR}
        WORKING_DIRECTORY "${OCTKWrapFTXUI_BUILD_DIR}"
        RESULT_VARIABLE CONFIGURE_RESULT)
    if(NOT CONFIGURE_RESULT MATCHES 0)
        message(FATAL_ERROR "${OCTKWrapFTXUI_DIR_NAME} configure failed.")
    endif()
    message(STATUS "${OCTKWrapFTXUI_DIR_NAME} configure success")

    execute_process(
        COMMAND ${CMAKE_COMMAND} --build ./ --parallel ${OCTK_NUMBER_OF_ASYNC_JOBS} --config 
        ${CMAKE_BUILD_TYPE} --target install
        WORKING_DIRECTORY "${OCTKWrapFTXUI_BUILD_DIR}"
        RESULT_VARIABLE BUILD_RESULT)
    if(NOT BUILD_RESULT MATCHES 0)
        message(FATAL_ERROR "${OCTKWrapFTXUI_DIR_NAME} build failed.")
    endif()
    message(STATUS "${OCTKWrapFTXUI_DIR_NAME} build success")

    execute_process(
        COMMAND ${CMAKE_COMMAND} --install ./
        WORKING_DIRECTORY "${OCTKWrapFTXUI_BUILD_DIR}"
        RESULT_VARIABLE INSTALL_RESULT)
    if(NOT INSTALL_RESULT MATCHES 0)
        message(FATAL_ERROR "${OCTKWrapFTXUI_DIR_NAME} install failed.")
    endif()
    message(STATUS "${OCTKWrapFTXUI_DIR_NAME} install success")
    octk_make_stamp_file("${OCTKWrapFTXUI_STAMP_FILE_PATH}")
endif()
# wrap lib
add_library(OCTK3rdparty::WrapFTXUI INTERFACE IMPORTED)
find_package(ftxui PATHS ${OCTKWrapFTXUI_INSTALL_DIR} NO_DEFAULT_PATH REQUIRED)
target_link_libraries(OCTK3rdparty::WrapFTXUI INTERFACE ftxui::dom ftxui::screen ftxui::component)
set(OCTKWrapFTXUI_FOUND ON)