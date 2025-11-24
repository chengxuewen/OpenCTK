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
if(TARGET OCTK3rdparty::WrapFunction2)
    set(OCTKWrapFunction2_FOUND ON)
    return()
endif()

set(OCTKWrapFunction2_NAME "function2-4.2.5")
set(OCTKWrapFunction2_PKG_NAME "${OCTKWrapFunction2_NAME}.tar.gz")
set(OCTKWrapFunction2_DIR_NAME "${OCTKWrapFunction2_NAME}-${OCTK_LOWER_BUILD_TYPE}")
set(OCTKWrapFunction2_URL_PATH "${PROJECT_SOURCE_DIR}/3rdparty/${OCTKWrapFunction2_PKG_NAME}")
set(OCTKWrapFunction2_ROOT_DIR "${PROJECT_BINARY_DIR}/3rdparty/${OCTKWrapFunction2_DIR_NAME}")
set(OCTKWrapFunction2_BUILD_DIR "${OCTKWrapFunction2_ROOT_DIR}/build" CACHE INTERNAL "" FORCE)
set(OCTKWrapFunction2_SOURCE_DIR "${OCTKWrapFunction2_ROOT_DIR}/source" CACHE INTERNAL "" FORCE)
set(OCTKWrapFunction2_INSTALL_DIR "${OCTKWrapFunction2_ROOT_DIR}/install" CACHE INTERNAL "" FORCE)
octk_stamp_file_info(OCTKWrapFunction2 OUTPUT_DIR "${OCTKWrapFunction2_ROOT_DIR}")
octk_fetch_3rdparty(OCTKWrapFunction2 URL "${OCTKWrapFunction2_URL_PATH}" OUTPUT_NAME "${OCTKWrapFunction2_DIR_NAME}")
if(NOT EXISTS "${OCTKWrapFunction2_STAMP_FILE_PATH}")
    if(NOT EXISTS ${OCTKWrapFunction2_SOURCE_DIR})
        message(FATAL_ERROR "${OCTKWrapFunction2_NAME} FetchContent failed.")
    endif()
    octk_reset_dir(${OCTKWrapFunction2_BUILD_DIR})

    message(STATUS "Configure ${OCTKWrapFunction2_NAME} lib...")
    execute_process(
        COMMAND ${CMAKE_COMMAND}
        -G ${CMAKE_GENERATOR}
        -DBUILD_TESTING=OFF
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_CONFIGURATION_TYPES=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX=${OCTKWrapFunction2_INSTALL_DIR}
        ${OCTKWrapFunction2_SOURCE_DIR}
        WORKING_DIRECTORY "${OCTKWrapFunction2_BUILD_DIR}"
        RESULT_VARIABLE CONFIGURE_RESULT)
    if(NOT CONFIGURE_RESULT MATCHES 0)
        message(FATAL_ERROR "${OCTKWrapFunction2_NAME} configure failed.")
    endif()
    message(STATUS "${OCTKWrapFunction2_NAME} configure success")

    execute_process(
        COMMAND ${CMAKE_COMMAND} --build ./ --parallel ${OCTK_NUMBER_OF_ASYNC_JOBS} --config
        ${CMAKE_BUILD_TYPE} --target install
        WORKING_DIRECTORY "${OCTKWrapFunction2_BUILD_DIR}"
        RESULT_VARIABLE BUILD_RESULT)
    if(NOT BUILD_RESULT MATCHES 0)
        message(FATAL_ERROR "${OCTKWrapFunction2_NAME} build failed.")
    endif()
    message(STATUS "${OCTKWrapFunction2_NAME} build success")

    execute_process(
        COMMAND ${CMAKE_COMMAND} --install ./
        WORKING_DIRECTORY "${OCTKWrapFunction2_BUILD_DIR}"
        RESULT_VARIABLE INSTALL_RESULT)
    if(NOT INSTALL_RESULT MATCHES 0)
        message(FATAL_ERROR "${OCTKWrapFunction2_NAME} install failed.")
    endif()
    message(STATUS "${OCTKWrapFunction2_NAME} install success")
    octk_make_stamp_file("${OCTKWrapFunction2_STAMP_FILE_PATH}")
endif()
# wrap lib
add_library(OCTK3rdparty::WrapFunction2 INTERFACE IMPORTED)
set(function2_DIR "${OCTKWrapFunction2_INSTALL_DIR}/lib/cmake/function2")
find_package(function2 PATHS "${OCTKWrapFunction2_INSTALL_DIR}/lib" NO_DEFAULT_PATH REQUIRED)
target_link_libraries(OCTK3rdparty::WrapFunction2 INTERFACE function2::function2)
set(OCTKWrapFunction2_FOUND ON)
