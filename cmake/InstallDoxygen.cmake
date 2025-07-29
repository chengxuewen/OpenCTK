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
    set(Doxygen_FOUND ON)
    return()
endif()

if(WIN32)
    find_program(Doxygen NAMES doxygen)
    if(NOT Doxygen_FOUND)
        message(STATUS "Install doxygen in windows platform.")
        set(Doxygen_DIR_NAME "doxygen-1.9.8-windows-x64")
        set(Doxygen_PKG_NAME "doxygen-1.9.8-windows-x64.zip")
        set(Doxygen_URL_PATH "${PROJECT_SOURCE_DIR}/3rdparty/${Doxygen_PKG_NAME}")
        set(Doxygen_ROOT_DIR "${PROJECT_BINARY_DIR}/3rdparty/${Doxygen_DIR_NAME}")
        set(Doxygen_INSTALL_DIR "${Doxygen_ROOT_DIR}/source" CACHE INTERNAL "" FORCE)
        octk_stamp_file_info(Doxygen OUTPUT_DIR "${Doxygen_ROOT_DIR}")
        octk_fetch_3rdparty(Doxygen URL "${Doxygen_URL_PATH}")
        message(STATUS "Set doxygen executable path.")
        find_program(Doxygen_PROGRAM NAMES doxygen PATHS ${Doxygen_INSTALL_DIR})
        set(Doxygen_EXECUTABLE "${Doxygen_PROGRAM}" CACHE INTERNAL "doxygen executable path." FORCE)
    endif()
else()
    find_program(Doxygen NAMES doxygen REQUIRED)
endif()
set(Doxygen_FOUND ON)