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
if(TARGET OpenCTKWrapLibsrtp::WrapLibsrtp)
    set(OpenCTKWrapLibsrtp_FOUND ON)
    return()
endif()

octk_find_package(WrapOpenSSL PROVIDED_TARGETS OpenCTKWrapOpenSSL::WrapOpenSSL)
set(OpenCTKWrapLibsrtp_NAME "libsrtp-2.7.0")
set(OpenCTKWrapLibsrtp_PKG_NAME "${OpenCTKWrapLibsrtp_NAME}.tar.gz")
set(OpenCTKWrapLibsrtp_DIR_NAME "${OpenCTKWrapLibsrtp_NAME}-${OCTK_LOWER_BUILD_TYPE}")
set(OpenCTKWrapLibsrtp_URL_PATH "${PROJECT_SOURCE_DIR}/3rdparty/${OpenCTKWrapLibsrtp_PKG_NAME}")
set(OpenCTKWrapLibsrtp_ROOT_DIR "${PROJECT_BINARY_DIR}/3rdparty/${OpenCTKWrapLibsrtp_DIR_NAME}")
set(OpenCTKWrapLibsrtp_BUILD_DIR "${OpenCTKWrapLibsrtp_ROOT_DIR}/build" CACHE INTERNAL "" FORCE)
set(OpenCTKWrapLibsrtp_SOURCE_DIR "${OpenCTKWrapLibsrtp_ROOT_DIR}/source" CACHE INTERNAL "" FORCE)
set(OpenCTKWrapLibsrtp_INSTALL_DIR "${OpenCTKWrapLibsrtp_ROOT_DIR}/install" CACHE INTERNAL "" FORCE)
octk_stamp_file_info(OpenCTKWrapLibsrtp OUTPUT_DIR "${OpenCTKWrapLibsrtp_ROOT_DIR}")
octk_fetch_3rdparty(OpenCTKWrapLibsrtp URL "${OpenCTKWrapLibsrtp_URL_PATH}" OUTPUT_NAME "${OpenCTKWrapLibsrtp_DIR_NAME}")
if(NOT EXISTS "${OpenCTKWrapLibsrtp_STAMP_FILE_PATH}")
    if(NOT EXISTS ${OpenCTKWrapLibsrtp_SOURCE_DIR})
        message(FATAL_ERROR "${OpenCTKWrapLibsrtp_DIR_NAME} FetchContent failed.")
    endif()
    octk_reset_dir(${OpenCTKWrapLibsrtp_BUILD_DIR})

    message(STATUS "Configure ${OpenCTKWrapLibsrtp_DIR_NAME} lib...")
    execute_process(
        COMMAND ${CMAKE_COMMAND}
        -G ${CMAKE_GENERATOR}
        -DENABLE_OPENSSL=ON
        -DOpenSSL_ROOT=${OpenCTKWrapOpenSSL_INSTALL_DIR}
        -DLIBSRTP_TEST_APPS=OFF
        -DBUILD_SHARED_LIBS=OFF
        -DCMAKE_POLICY_VERSION_MINIMUM=3.5
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_CONFIGURATION_TYPES=${CMAKE_BUILD_TYPE}
        -DCMAKE_PREFIX_PATH=${OpenCTKWrapOpenSSL_INSTALL_DIR}
        -DCMAKE_INSTALL_PREFIX=${OpenCTKWrapLibsrtp_INSTALL_DIR}
        ${OpenCTKWrapLibsrtp_SOURCE_DIR}
        WORKING_DIRECTORY "${OpenCTKWrapLibsrtp_BUILD_DIR}"
        RESULT_VARIABLE CONFIGURE_RESULT)
    if(NOT CONFIGURE_RESULT MATCHES 0)
        message(FATAL_ERROR "${OpenCTKWrapLibsrtp_DIR_NAME} configure failed.")
    endif()
    message(STATUS "${OpenCTKWrapLibsrtp_DIR_NAME} configure success")

    execute_process(
        COMMAND ${CMAKE_COMMAND} --build ./ --parallel ${OCTK_NUMBER_OF_ASYNC_JOBS}
        --config ${CMAKE_BUILD_TYPE} --target install
        WORKING_DIRECTORY "${OpenCTKWrapLibsrtp_BUILD_DIR}"
        RESULT_VARIABLE BUILD_RESULT)
    if(NOT BUILD_RESULT MATCHES 0)
        message(FATAL_ERROR "${OpenCTKWrapLibsrtp_DIR_NAME} build failed.")
    endif()
    message(STATUS "${OpenCTKWrapLibsrtp_DIR_NAME} build success")

    execute_process(
        COMMAND ${CMAKE_COMMAND} --install ./
        WORKING_DIRECTORY "${OpenCTKWrapLibsrtp_BUILD_DIR}"
        RESULT_VARIABLE INSTALL_RESULT)
    if(NOT INSTALL_RESULT MATCHES 0)
        message(FATAL_ERROR "${OpenCTKWrapLibsrtp_DIR_NAME} install failed.")
    endif()
    message(STATUS "${OpenCTKWrapLibsrtp_DIR_NAME} install success")
    octk_make_stamp_file("${OpenCTKWrapLibsrtp_STAMP_FILE_PATH}")
endif()
# wrap lib
add_library(OpenCTKWrapLibsrtp::WrapLibsrtp INTERFACE IMPORTED)
find_package(libSRTP PATHS ${OpenCTKWrapLibsrtp_INSTALL_DIR} NO_DEFAULT_PATH REQUIRED)
target_link_libraries(OpenCTKWrapLibsrtp::WrapLibsrtp INTERFACE libSRTP::srtp2)
set(OpenCTKWrapLibsrtp_FOUND ON)
