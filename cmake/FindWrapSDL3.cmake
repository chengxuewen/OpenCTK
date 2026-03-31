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
if(TARGET OpenCTKWrapSDL3::WrapSDL3)
    set(OpenCTKWrapSDL3_FOUND ON)
    return()
endif()

set(OpenCTKWrapSDL3_NAME "SDL3-3.2.18")
set(OpenCTKWrapSDL3_PKG_NAME "${OpenCTKWrapSDL3_NAME}.tar.gz")
set(OpenCTKWrapSDL3_DIR_NAME "${OpenCTKWrapSDL3_NAME}-${OCTK_LOWER_BUILD_TYPE}")
set(OpenCTKWrapSDL3_URL_PATH "${PROJECT_SOURCE_DIR}/3rdparty/${OpenCTKWrapSDL3_PKG_NAME}")
set(OpenCTKWrapSDL3_ROOT_DIR "${PROJECT_BINARY_DIR}/3rdparty/${OpenCTKWrapSDL3_DIR_NAME}")
set(OpenCTKWrapSDL3_BUILD_DIR "${OpenCTKWrapSDL3_ROOT_DIR}/build" CACHE INTERNAL "" FORCE)
set(OpenCTKWrapSDL3_SOURCE_DIR "${OpenCTKWrapSDL3_ROOT_DIR}/source" CACHE INTERNAL "" FORCE)
set(OpenCTKWrapSDL3_INSTALL_DIR "${OpenCTKWrapSDL3_ROOT_DIR}/install" CACHE INTERNAL "" FORCE)
octk_stamp_file_info(OpenCTKWrapSDL3 OUTPUT_DIR "${OpenCTKWrapSDL3_ROOT_DIR}")
octk_fetch_3rdparty(OpenCTKWrapSDL3 URL "${OpenCTKWrapSDL3_URL_PATH}" OUTPUT_NAME "${OpenCTKWrapSDL3_DIR_NAME}")
if(NOT EXISTS "${OpenCTKWrapSDL3_STAMP_FILE_PATH}")
    if(NOT EXISTS ${OpenCTKWrapSDL3_SOURCE_DIR})
        message(FATAL_ERROR "${OpenCTKWrapSDL3_NAME} FetchContent failed.")
    endif()
    octk_reset_dir(${OpenCTKWrapSDL3_BUILD_DIR})

    message(STATUS "Configure ${OpenCTKWrapSDL3_NAME} lib...")
    execute_process(
        COMMAND ${CMAKE_COMMAND}
        -G ${CMAKE_GENERATOR}
        -DSDL_TESTS=OFF
        -DSDL_STATIC=ON
        -DSDL_SHARED=OFF
        -DSDL_STATIC_PIC=ON
        -DCMAKE_BUILD_TYPE=Release
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON
        -DCMAKE_INSTALL_PREFIX=${OpenCTKWrapSDL3_INSTALL_DIR}
        ${OpenCTKWrapSDL3_SOURCE_DIR}
        WORKING_DIRECTORY "${OpenCTKWrapSDL3_BUILD_DIR}"
        RESULT_VARIABLE CONFIGURE_RESULT)
    if(NOT CONFIGURE_RESULT MATCHES 0)
        message(FATAL_ERROR "${OpenCTKWrapSDL3_NAME} configure failed.")
    endif()
    message(STATUS "${OpenCTKWrapSDL3_NAME} configure success")
    
    execute_process(
        COMMAND ${CMAKE_COMMAND} --build ./ --parallel ${OCTK_NUMBER_OF_ASYNC_JOBS} --config Release --target install
        WORKING_DIRECTORY "${OpenCTKWrapSDL3_BUILD_DIR}"
        RESULT_VARIABLE BUILD_RESULT)
    if(NOT BUILD_RESULT MATCHES 0)
        message(FATAL_ERROR "${OpenCTKWrapSDL3_NAME} build failed.")
    endif()       
    message(STATUS "${OpenCTKWrapSDL3_NAME} build success")
            
    execute_process(
        COMMAND ${CMAKE_COMMAND} --install ./
        WORKING_DIRECTORY "${OpenCTKWrapSDL3_BUILD_DIR}"
        RESULT_VARIABLE INSTALL_RESULT)
    if(NOT INSTALL_RESULT MATCHES 0)
        message(FATAL_ERROR "${OpenCTKWrapSDL3_NAME} install failed.")
    endif()
    message(STATUS "${OpenCTKWrapSDL3_NAME} install success")
    octk_make_stamp_file("${OpenCTKWrapSDL3_STAMP_FILE_PATH}")
endif()
# wrap lib
if (NOT TARGET SDL3::SDL3-static)
    set(SDL3_DIR ${OpenCTKWrapSDL3_INSTALL_DIR}/lib/cmake/SDL3)
    find_package(SDL3 PATHS ${OpenCTKWrapSDL3_INSTALL_DIR} NO_DEFAULT_PATH REQUIRED)
endif()
add_library(OpenCTKWrapSDL3::WrapSDL3 INTERFACE IMPORTED)
target_link_libraries(OpenCTKWrapSDL3::WrapSDL3 INTERFACE SDL3::SDL3-static)
set(OpenCTKWrapSDL3_FOUND ON)
