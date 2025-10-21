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
if(TARGET OCTK3rdparty::WrapLibcurl)
    set(OCTKWrapLibcurl_FOUND ON)
    return()
endif()

octk_find_package(WrapMbedTLS PROVIDED_TARGETS OCTK3rdparty::WrapMbedTLS)
set(OCTKWrapLibcurl_NAME "curl-8.16.0")
set(OCTKWrapLibcurl_PKG_NAME "${OCTKWrapLibcurl_NAME}.tar.xz")
set(OCTKWrapLibcurl_DIR_NAME "${OCTKWrapLibcurl_NAME}-${OCTK_LOWER_BUILD_TYPE}")
set(OCTKWrapLibcurl_URL_PATH "${PROJECT_SOURCE_DIR}/3rdparty/${OCTKWrapLibcurl_PKG_NAME}")
set(OCTKWrapLibcurl_ROOT_DIR "${PROJECT_BINARY_DIR}/3rdparty/${OCTKWrapLibcurl_DIR_NAME}")
set(OCTKWrapLibcurl_BUILD_DIR "${OCTKWrapLibcurl_ROOT_DIR}/build" CACHE INTERNAL "" FORCE)
set(OCTKWrapLibcurl_SOURCE_DIR "${OCTKWrapLibcurl_ROOT_DIR}/source" CACHE INTERNAL "" FORCE)
set(OCTKWrapLibcurl_INSTALL_DIR "${OCTKWrapLibcurl_ROOT_DIR}/install" CACHE INTERNAL "" FORCE)
octk_stamp_file_info(OCTKWrapLibcurl OUTPUT_DIR "${OCTKWrapLibcurl_ROOT_DIR}")
octk_fetch_3rdparty(OCTKWrapLibcurl URL "${OCTKWrapLibcurl_URL_PATH}" OUTPUT_NAME "${OCTKWrapLibcurl_DIR_NAME}")
if(NOT EXISTS "${OCTKWrapLibcurl_STAMP_FILE_PATH}")
    if(NOT EXISTS ${OCTKWrapLibcurl_SOURCE_DIR})
        message(FATAL_ERROR "${OCTKWrapLibcurl_NAME} FetchContent failed.")
    endif()
    octk_reset_dir(PARENT_DIR ${OCTKWrapLibcurl_ROOT_DIR} TARGET_NAME build)

    message(STATUS "Configure ${OCTKWrapLibcurl_NAME} lib...")
    execute_process(
        COMMAND ${CMAKE_COMMAND}
        -G ${CMAKE_GENERATOR}
        -DCURL_USE_MBEDTLS=ON
        -DCURL_USE_LIBPSL=OFF
        -DBUILD_STATIC_CURL=ON
        -DBUILD_SHARED_LIBS=OFF
        -DBUILD_FOR_MT=${OCTK_MSVC_STATIC_RUNTIME}
        -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_PREFIX_PATH=${OCTKWrapMbedTLS_INSTALL_DIR}
        -DCMAKE_INSTALL_PREFIX=${OCTKWrapLibcurl_INSTALL_DIR}
        ${OCTKWrapLibcurl_SOURCE_DIR}
        WORKING_DIRECTORY "${OCTKWrapLibcurl_BUILD_DIR}"
        RESULT_VARIABLE CONFIGURE_RESULT)
    if(NOT CONFIGURE_RESULT MATCHES 0)
        message(FATAL_ERROR "${OCTKWrapLibcurl_NAME} configure failed.")
    endif()
    message(STATUS "${OCTKWrapLibcurl_NAME} configure success")

    execute_process(
        COMMAND ${CMAKE_COMMAND} --build ./ --parallel ${OCTK_NUMBER_OF_ASYNC_JOBS} --config Release --target install
        WORKING_DIRECTORY "${OCTKWrapLibcurl_BUILD_DIR}"
        RESULT_VARIABLE BUILD_RESULT)
    if(NOT BUILD_RESULT MATCHES 0)
        message(FATAL_ERROR "${OCTKWrapLibcurl_NAME} build failed.")
    endif()
    message(STATUS "${OCTKWrapLibcurl_NAME} build success")

    execute_process(
        COMMAND ${CMAKE_COMMAND} --install ./
        WORKING_DIRECTORY "${OCTKWrapLibcurl_BUILD_DIR}"
        RESULT_VARIABLE INSTALL_RESULT)
    if(NOT INSTALL_RESULT MATCHES 0)
        message(FATAL_ERROR "${OCTKWrapLibcurl_NAME} install failed.")
    endif()
    message(STATUS "${OCTKWrapLibcurl_NAME} install success")
    octk_make_stamp_file("${OCTKWrapLibcurl_STAMP_FILE_PATH}")
endif()
# wrap lib
add_library(OCTK3rdparty::WrapLibcurl INTERFACE IMPORTED)
find_package(CURL PATHS ${OCTKWrapLibcurl_INSTALL_DIR} NO_DEFAULT_PATH REQUIRED)
target_link_libraries(OCTK3rdparty::WrapLibcurl INTERFACE CURL::libcurl_static)
set(OCTKWrapLibcurl_FOUND ON)