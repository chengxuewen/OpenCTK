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
if(TARGET OCTK3rdparty::WrapZLMediaKit)
    set(OCTKWrapZLMediaKit_FOUND ON)
    return()
endif()

octk_find_package(WrapOpenSSL PROVIDED_TARGETS OCTK3rdparty::WrapOpenSSL)
octk_find_package(WrapLibsrtp PROVIDED_TARGETS OCTK3rdparty::WrapLibsrtp)
set(OCTKWrapZLMediaKit_NAME "ZLMediaKit-8.0")
set(OCTKWrapZLMediaKit_PKG_NAME "${OCTKWrapZLMediaKit_NAME}.7z")
set(OCTKWrapZLMediaKit_DIR_NAME "${OCTKWrapZLMediaKit_NAME}-${OCTK_LOWER_BUILD_TYPE}")
set(OCTKWrapZLMediaKit_URL_PATH "${PROJECT_SOURCE_DIR}/3rdparty/${OCTKWrapZLMediaKit_PKG_NAME}")
set(OCTKWrapZLMediaKit_ROOT_DIR "${PROJECT_BINARY_DIR}/3rdparty/${OCTKWrapZLMediaKit_DIR_NAME}")
set(OCTKWrapZLMediaKit_BUILD_DIR "${OCTKWrapZLMediaKit_ROOT_DIR}/build" CACHE INTERNAL "" FORCE)
set(OCTKWrapZLMediaKit_SOURCE_DIR "${OCTKWrapZLMediaKit_ROOT_DIR}/source" CACHE INTERNAL "" FORCE)
set(OCTKWrapZLMediaKit_INSTALL_DIR "${OCTKWrapZLMediaKit_ROOT_DIR}/install" CACHE INTERNAL "" FORCE)
octk_stamp_file_info(OCTKWrapZLMediaKit OUTPUT_DIR "${OCTKWrapZLMediaKit_ROOT_DIR}")
octk_fetch_3rdparty(OCTKWrapZLMediaKit URL "${OCTKWrapZLMediaKit_URL_PATH}" OUTPUT_NAME "${OCTKWrapZLMediaKit_DIR_NAME}")
if(NOT EXISTS "${OCTKWrapZLMediaKit_STAMP_FILE_PATH}")
    if(NOT EXISTS ${OCTKWrapZLMediaKit_SOURCE_DIR})
        message(FATAL_ERROR "${OCTKWrapZLMediaKit_DIR_NAME} FetchContent failed.")
    endif()
    octk_reset_dir(PARENT_DIR ${OCTKWrapZLMediaKit_ROOT_DIR} TARGET_NAME build)
    message(OCTKWrapLibsrtp_INSTALL_DIR=${OCTKWrapLibsrtp_INSTALL_DIR})
    message(STATUS "Configure ${OCTKWrapZLMediaKit_DIR_NAME} lib...")
    execute_process(
        COMMAND ${CMAKE_COMMAND}
        -G ${CMAKE_GENERATOR}
        -DENABLE_WEBRTC=ON
        -DSRTP_PREFIX=${OCTKWrapLibsrtp_INSTALL_DIR}
        -DOPENSSL_ROOT_DIR=${OCTKWrapOpenSSL_INSTALL_DIR}
        -DCMAKE_POLICY_VERSION_MINIMUM=3.5
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_CONFIGURATION_TYPES=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX=${OCTKWrapZLMediaKit_INSTALL_DIR}
        ${OCTKWrapZLMediaKit_SOURCE_DIR}
        WORKING_DIRECTORY "${OCTKWrapZLMediaKit_BUILD_DIR}"
        RESULT_VARIABLE CONFIGURE_RESULT)
    if(NOT CONFIGURE_RESULT MATCHES 0)
        message(FATAL_ERROR "${OCTKWrapZLMediaKit_DIR_NAME} configure failed.")
    endif()
    message(STATUS "${OCTKWrapZLMediaKit_DIR_NAME} configure success")

#    return()
    execute_process(
        COMMAND ${CMAKE_COMMAND} --build ./ --parallel ${OCTK_NUMBER_OF_ASYNC_JOBS}
        --config ${CMAKE_BUILD_TYPE} --target install
        WORKING_DIRECTORY "${OCTKWrapZLMediaKit_BUILD_DIR}"
        RESULT_VARIABLE BUILD_RESULT)
    if(NOT BUILD_RESULT MATCHES 0)
        message(FATAL_ERROR "${OCTKWrapZLMediaKit_DIR_NAME} build failed.")
    endif()
    message(STATUS "${OCTKWrapZLMediaKit_DIR_NAME} build success")

    execute_process(
        COMMAND ${CMAKE_COMMAND} --install ./
        WORKING_DIRECTORY "${OCTKWrapZLMediaKit_BUILD_DIR}"
        RESULT_VARIABLE INSTALL_RESULT)
    if(NOT INSTALL_RESULT MATCHES 0)
        message(FATAL_ERROR "${OCTKWrapZLMediaKit_DIR_NAME} install failed.")
    endif()
    message(STATUS "${OCTKWrapZLMediaKit_DIR_NAME} install success")
    octk_make_stamp_file("${OCTKWrapZLMediaKit_STAMP_FILE_PATH}")
endif()
# wrap lib
add_library(OCTK3rdparty::WrapZLMediaKit INTERFACE IMPORTED)
#find_package(cppzmq PATHS ${OCTKWrapZLMediaKit_INSTALL_DIR} NO_DEFAULT_PATH REQUIRED)
#target_link_libraries(OCTK3rdparty::WrapZLMediaKit INTERFACE cppzmq-static)
set(OCTKWrapZLMediaKit_FOUND ON)
