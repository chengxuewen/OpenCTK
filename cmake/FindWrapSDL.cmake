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
if(TARGET OCTK3rdparty::WrapSDL)
    set(OCTKWrapSDL_FOUND ON)
    return()
endif()


set(OCTKWrapSDL_NAME "SDL3-3.2.18")
set(OCTKWrapSDL_PKG_NAME "${OCTKWrapSDL_NAME}.tar.gz")
set(OCTKWrapSDL_DIR_NAME "${OCTKWrapSDL_NAME}-${OCTK_LOWER_BUILD_TYPE}")
set(OCTKWrapSDL_URL_PATH "${PROJECT_SOURCE_DIR}/3rdparty/${OCTKWrapSDL_PKG_NAME}")
set(OCTKWrapSDL_ROOT_DIR "${PROJECT_BINARY_DIR}/3rdparty/${OCTKWrapSDL_DIR_NAME}")
set(OCTKWrapSDL_BUILD_DIR "${OCTKWrapSDL_ROOT_DIR}/build" CACHE INTERNAL "" FORCE)
set(OCTKWrapSDL_SOURCE_DIR "${OCTKWrapSDL_ROOT_DIR}/source" CACHE INTERNAL "" FORCE)
set(OCTKWrapSDL_INSTALL_DIR "${OCTKWrapSDL_ROOT_DIR}/install" CACHE INTERNAL "" FORCE)
octk_stamp_file_info(OCTKWrapSDL OUTPUT_DIR "${OCTKWrapSDL_ROOT_DIR}")
octk_fetch_3rdparty(OCTKWrapSDL URL "${OCTKWrapSDL_URL_PATH}" OUTPUT_NAME "${OCTKWrapSDL_DIR_NAME}")
if(NOT EXISTS "${OCTKWrapSDL_STAMP_FILE_PATH}")
    if(NOT EXISTS ${OCTKWrapSDL_SOURCE_DIR})
        message(FATAL_ERROR "${OCTKWrapSDL_NAME} FetchContent failed.")
    endif()
    octk_reset_dir(${OCTKWrapSDL_ROOT_DIR})

    message(STATUS "Configure ${OCTKWrapSDL_NAME} lib...")
    execute_process(
        COMMAND ${CMAKE_COMMAND}
        -G ${CMAKE_GENERATOR}
        -DSDL_TESTS=OFF
        -DSDL_STATIC=ON
        -DSDL_SHARED=OFF
        -DSDL_STATIC_PIC=ON
        -DCMAKE_BUILD_TYPE=Release
        -DCMAKE_INSTALL_PREFIX=${OCTKWrapSDL_INSTALL_DIR}
        ${OCTKWrapSDL_SOURCE_DIR}
        WORKING_DIRECTORY "${OCTKWrapSDL_BUILD_DIR}"
        RESULT_VARIABLE CONFIGURE_RESULT)
    if(NOT CONFIGURE_RESULT MATCHES 0)
        message(FATAL_ERROR "${OCTKWrapSDL_NAME} configure failed.")
    endif()
    message(STATUS "${OCTKWrapSDL_NAME} configure success")
    
    execute_process(
        COMMAND ${CMAKE_COMMAND} --build ./ --parallel ${OCTK_NUMBER_OF_ASYNC_JOBS} --config Release --target install
        WORKING_DIRECTORY "${OCTKWrapSDL_BUILD_DIR}"
        RESULT_VARIABLE BUILD_RESULT)
    if(NOT BUILD_RESULT MATCHES 0)
        message(FATAL_ERROR "${OCTKWrapSDL_NAME} build failed.")
    endif()       
    message(STATUS "${OCTKWrapSDL_NAME} build success")
            
    execute_process(
        COMMAND ${CMAKE_COMMAND} --install ./
        WORKING_DIRECTORY "${OCTKWrapSDL_BUILD_DIR}"
        RESULT_VARIABLE INSTALL_RESULT)
    if(NOT INSTALL_RESULT MATCHES 0)
        message(FATAL_ERROR "${OCTKWrapSDL_NAME} install failed.")
    endif()
    message(STATUS "${OCTKWrapSDL_NAME} install success")
    octk_make_stamp_file("${OCTKWrapSDL_STAMP_FILE_PATH}")
endif()
# wrap lib
if (NOT TARGET SDL3::SDL3-static)
    set(SDL3_DIR ${OCTKWrapSDL_INSTALL_DIR}/lib/cmake/SDL3)
    find_package(SDL3 PATHS ${OCTKWrapSDL_INSTALL_DIR} NO_DEFAULT_PATH REQUIRED)
endif()
add_library(OCTK3rdparty::WrapSDL INTERFACE IMPORTED)
target_link_libraries(OCTK3rdparty::WrapSDL INTERFACE SDL3::SDL3-static)
set(OCTKWrapSDL_FOUND ON)