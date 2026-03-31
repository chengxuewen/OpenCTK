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
if(TARGET OpenCTKWrapYamlCpp::WrapYamlCpp)
    set(OpenCTKWrapYamlCpp_FOUND ON)
    return()
endif()

set(OpenCTKWrapYamlCpp_NAME "yaml-cpp-0.9.0")
set(OpenCTKWrapYamlCpp_PKG_NAME "${OpenCTKWrapYamlCpp_NAME}.zip")
set(OpenCTKWrapYamlCpp_DIR_NAME "${OpenCTKWrapYamlCpp_NAME}-${OCTK_LOWER_BUILD_TYPE}")
set(OpenCTKWrapYamlCpp_URL_PATH "${PROJECT_SOURCE_DIR}/3rdparty/${OpenCTKWrapYamlCpp_PKG_NAME}")
set(OpenCTKWrapYamlCpp_ROOT_DIR "${PROJECT_BINARY_DIR}/3rdparty/${OpenCTKWrapYamlCpp_DIR_NAME}")
set(OpenCTKWrapYamlCpp_BUILD_DIR "${OpenCTKWrapYamlCpp_ROOT_DIR}/build" CACHE INTERNAL "" FORCE)
set(OpenCTKWrapYamlCpp_SOURCE_DIR "${OpenCTKWrapYamlCpp_ROOT_DIR}/source" CACHE INTERNAL "" FORCE)
set(OpenCTKWrapYamlCpp_INSTALL_DIR "${OpenCTKWrapYamlCpp_ROOT_DIR}/install" CACHE INTERNAL "" FORCE)
octk_stamp_file_info(OpenCTKWrapYamlCpp OUTPUT_DIR "${OpenCTKWrapYamlCpp_ROOT_DIR}")
octk_fetch_3rdparty(OpenCTKWrapYamlCpp URL "${OpenCTKWrapYamlCpp_URL_PATH}" OUTPUT_NAME "${OpenCTKWrapYamlCpp_DIR_NAME}")
if(NOT EXISTS "${OpenCTKWrapYamlCpp_STAMP_FILE_PATH}")
    if(NOT EXISTS ${OpenCTKWrapYamlCpp_SOURCE_DIR})
        message(FATAL_ERROR "${OpenCTKWrapYamlCpp_NAME} FetchContent failed.")
    endif()
    octk_reset_dir(${OpenCTKWrapYamlCpp_BUILD_DIR})

    message(STATUS "Configure ${OpenCTKWrapYamlCpp_NAME} lib...")
    execute_process(
        COMMAND ${CMAKE_COMMAND}
        -G ${CMAKE_GENERATOR}
        -DYAML_BUILD_SHARED_LIBS=OFF
        -DYAML_CPP_BUILD_TESTS=OFF
        -DYAML_CPP_INSTALL=ON
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON
        -DCMAKE_INSTALL_PREFIX=${OpenCTKWrapYamlCpp_INSTALL_DIR}
        ${OpenCTKWrapYamlCpp_SOURCE_DIR}
        WORKING_DIRECTORY "${OpenCTKWrapYamlCpp_BUILD_DIR}"
        RESULT_VARIABLE CONFIGURE_RESULT)
    if(NOT CONFIGURE_RESULT MATCHES 0)
        message(FATAL_ERROR "${OpenCTKWrapYamlCpp_NAME} configure failed.")
    endif()
    message(STATUS "${OpenCTKWrapYamlCpp_NAME} configure success")

    execute_process(
        COMMAND ${CMAKE_COMMAND} --build ./ --parallel ${OCTK_NUMBER_OF_ASYNC_JOBS} --config Release --target install
        WORKING_DIRECTORY "${OpenCTKWrapYamlCpp_BUILD_DIR}"
        RESULT_VARIABLE BUILD_RESULT)
    if(NOT BUILD_RESULT MATCHES 0)
        message(FATAL_ERROR "${OpenCTKWrapYamlCpp_NAME} build failed.")
    endif()
    message(STATUS "${OpenCTKWrapYamlCpp_NAME} build success")

    execute_process(
        COMMAND ${CMAKE_COMMAND} --install ./
        WORKING_DIRECTORY "${OpenCTKWrapYamlCpp_BUILD_DIR}"
        RESULT_VARIABLE INSTALL_RESULT)
    if(NOT INSTALL_RESULT MATCHES 0)
        message(FATAL_ERROR "${OpenCTKWrapYamlCpp_NAME} install failed.")
    endif()
    message(STATUS "${OpenCTKWrapYamlCpp_NAME} install success")
    octk_make_stamp_file("${OpenCTKWrapYamlCpp_STAMP_FILE_PATH}")
endif()
# wrap lib
add_library(OpenCTKWrapYamlCpp::WrapYamlCpp INTERFACE IMPORTED)
find_package(yaml-cpp PATHS ${OpenCTKWrapYamlCpp_INSTALL_DIR} NO_DEFAULT_PATH REQUIRED)
target_link_libraries(OpenCTKWrapYamlCpp::WrapYamlCpp INTERFACE yaml-cpp::yaml-cpp)
set(OpenCTKWrapYamlCpp_FOUND ON)