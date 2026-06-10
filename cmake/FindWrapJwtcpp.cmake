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
if(TARGET OpenCTKWrapJwtcpp::WrapJwtcpp)
    set(OpenCTKWrapJwtcpp_FOUND ON)
    return()
endif()

octk_find_package(WrapWolfSSL PROVIDED_TARGETS OpenCTKWrapWolfSSL::WrapWolfSSL)
set(OpenCTKWrapJwtcpp_NAME "jwt-cpp-v0.7.2")
set(OpenCTKWrapJwtcpp_PKG_NAME "${OpenCTKWrapJwtcpp_NAME}.7z")
set(OpenCTKWrapJwtcpp_DIR_NAME "${OpenCTKWrapJwtcpp_NAME}-${OCTK_LOWER_BUILD_TYPE}")
set(OpenCTKWrapJwtcpp_URL_PATH "${PROJECT_SOURCE_DIR}/3rdparty/${OpenCTKWrapJwtcpp_PKG_NAME}")
set(OpenCTKWrapJwtcpp_ROOT_DIR "${PROJECT_BINARY_DIR}/3rdparty/${OpenCTKWrapJwtcpp_DIR_NAME}")
set(OpenCTKWrapJwtcpp_BUILD_DIR "${OpenCTKWrapJwtcpp_ROOT_DIR}/build" CACHE INTERNAL "" FORCE)
set(OpenCTKWrapJwtcpp_SOURCE_DIR "${OpenCTKWrapJwtcpp_ROOT_DIR}/source" CACHE INTERNAL "" FORCE)
set(OpenCTKWrapJwtcpp_INSTALL_DIR "${OpenCTKWrapJwtcpp_ROOT_DIR}/install" CACHE INTERNAL "" FORCE)
octk_stamp_file_info(OpenCTKWrapJwtcpp OUTPUT_DIR "${OpenCTKWrapJwtcpp_ROOT_DIR}")
octk_fetch_3rdparty(OpenCTKWrapJwtcpp URL "${OpenCTKWrapJwtcpp_URL_PATH}" OUTPUT_NAME "${OpenCTKWrapJwtcpp_DIR_NAME}")
if(NOT EXISTS "${OpenCTKWrapJwtcpp_STAMP_FILE_PATH}")
    if(NOT EXISTS ${OpenCTKWrapJwtcpp_SOURCE_DIR})
        message(FATAL_ERROR "${OpenCTKWrapJwtcpp_DIR_NAME} FetchContent failed.")
    endif()
    octk_reset_dir(${OpenCTKWrapJwtcpp_BUILD_DIR})

    message(STATUS "Configure ${OpenCTKWrapJwtcpp_DIR_NAME} lib...")
    execute_process(
        COMMAND ${CMAKE_COMMAND}
        -Wno-deprecated
        --no-warn-unused-cli
        -G ${CMAKE_GENERATOR}
        -DPKG_CONFIG_EXECUTABLE=${OpenCTKPkgconf_EXECUTABLE}
        -DJWT_SSL_LIBRARY=wolfSSL
        -DJWT_BUILD_EXAMPLES=OFF
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_PREFIX_PATH=${OpenCTKWrapWolfSSL_INSTALL_DIR}
        -DCMAKE_INSTALL_PREFIX=${OpenCTKWrapJwtcpp_INSTALL_DIR}
        ${OpenCTKWrapJwtcpp_SOURCE_DIR}
        WORKING_DIRECTORY "${OpenCTKWrapJwtcpp_BUILD_DIR}"
        RESULT_VARIABLE CONFIGURE_RESULT)
    if(NOT CONFIGURE_RESULT MATCHES 0)
        message(FATAL_ERROR "${OpenCTKWrapJwtcpp_DIR_NAME} configure failed.")
    endif()
    message(STATUS "${OpenCTKWrapJwtcpp_DIR_NAME} configure success")
    
    execute_process(
        COMMAND ${CMAKE_COMMAND} --build ./ --parallel ${OCTK_NUMBER_OF_ASYNC_JOBS} --config 
        ${CMAKE_BUILD_TYPE} --target install
        WORKING_DIRECTORY "${OpenCTKWrapJwtcpp_BUILD_DIR}"
        RESULT_VARIABLE BUILD_RESULT)
    if(NOT BUILD_RESULT MATCHES 0)
        message(FATAL_ERROR "${OpenCTKWrapJwtcpp_DIR_NAME} build failed.")
    endif()
    message(STATUS "${OpenCTKWrapJwtcpp_DIR_NAME} build success")
    
    execute_process(
        COMMAND ${CMAKE_COMMAND} --install ./
        WORKING_DIRECTORY "${OpenCTKWrapJwtcpp_BUILD_DIR}"
        RESULT_VARIABLE INSTALL_RESULT)
    if(NOT INSTALL_RESULT MATCHES 0)
        message(FATAL_ERROR "${OpenCTKWrapJwtcpp_DIR_NAME} install failed.")
    endif()        
    message(STATUS "${OpenCTKWrapJwtcpp_DIR_NAME} install success")
    octk_make_stamp_file("${OpenCTKWrapJwtcpp_STAMP_FILE_PATH}")
endif()
# wrap lib
add_library(OpenCTKWrapJwtcpp::WrapJwtcpp INTERFACE IMPORTED)
# jwt-cpp-config.cmake calls pkg_check_modules(wolfssl ...), need PKG_CONFIG_PATH
# to include wolfssl install's pkgconfig dir.
set(_jwtcpp_saved_pkg_config_path "$ENV{PKG_CONFIG_PATH}")
set(ENV{PKG_CONFIG_PATH} "${OpenCTKWrapWolfSSL_INSTALL_DIR}/lib/pkgconfig")
find_package(jwt-cpp PATHS "${OpenCTKWrapJwtcpp_INSTALL_DIR}" NO_DEFAULT_PATH REQUIRED)
set(ENV{PKG_CONFIG_PATH} "${_jwtcpp_saved_pkg_config_path}")
target_link_libraries(OpenCTKWrapJwtcpp::WrapJwtcpp INTERFACE jwt-cpp::jwt-cpp)
set(OpenCTKWrapJwtcpp_FOUND ON)