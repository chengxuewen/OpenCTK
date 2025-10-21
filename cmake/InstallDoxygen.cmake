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
if(TARGET OCTK3rdparty::Doxygen)
    set(OCTKDoxygen_FOUND ON)
    return()
endif()

if(WIN32)
    find_program(Doxygen NAMES doxygen)
    if(NOT Doxygen_FOUND)
        message(STATUS "Install doxygen in windows platform.")
        set(OCTKDoxygen_NAME "doxygen-1.9.8-windows-x64")
        set(OCTKDoxygen_PKG_NAME "${OCTKDoxygen_NAME}.zip")
        set(OCTKDoxygen_URL_PATH "${PROJECT_SOURCE_DIR}/3rdparty/${OCTKDoxygen_PKG_NAME}")
        set(OCTKDoxygen_ROOT_DIR "${PROJECT_BINARY_DIR}/3rdparty/${OCTKDoxygen_NAME}")
        set(OCTKDoxygen_INSTALL_DIR "${OCTKDoxygen_ROOT_DIR}/source")
        octk_stamp_file_info(OCTKDoxygen OUTPUT_DIR "${OCTKDoxygen_ROOT_DIR}")
        octk_fetch_3rdparty(OCTKDoxygen URL "${OCTKDoxygen_URL_PATH}")
        message(STATUS "Set doxygen executable path.")
        find_program(OCTKDoxygen_PROGRAM NAMES doxygen PATHS ${OCTKDoxygen_INSTALL_DIR})
        set(OCTKDoxygen_EXECUTABLE "${OCTKDoxygen_PROGRAM}" CACHE INTERNAL "doxygen executable path." FORCE)
    endif()
else()
    find_program(Doxygen NAMES doxygen REQUIRED)
    set(OCTKDoxygen_EXECUTABLE "${Doxygen_EXECUTABLE}" CACHE INTERNAL "doxygen executable path." FORCE)
endif()
set(OCTKDoxygen_FOUND ON)