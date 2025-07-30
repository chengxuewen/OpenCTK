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
if(TARGET OCTK3rdparty::Python)
    set(OCTKPython_FOUND ON)
    return()
endif()

if(WIN32)
    set(OCTKPython_NAME "python-3.8.10-embed-win32")
    set(OCTKPython_PKG_NAME "${OCTKPython_NAME}.zip")
    set(OCTKPython_URL_PATH "${PROJECT_SOURCE_DIR}/3rdparty/${OCTKPython_PKG_NAME}")
    set(OCTKPython_ROOT_DIR "${PROJECT_BINARY_DIR}/3rdparty/${OCTKPython_NAME}")
    set(OCTKPython_SOURCE_DIR "${OCTKPython_ROOT_DIR}/source")
    set(OCTKPython_INSTALL_DIR "${OCTKPython_ROOT_DIR}/install")
    octk_stamp_file_info(OCTKPython OUTPUT_DIR "${OCTKPython_ROOT_DIR}")
    octk_fetch_3rdparty(OCTKPython URL "${OCTKPython_URL_PATH}" OUTPUT_NAME "${OCTKPython_NAME}")
    set(OCTKPython_EXECUTABLE "${OCTKPython_SOURCE_DIR}/python.exe" CACHE INTERNAL "python executable path." FORCE)
else()
    message(STATUS "Find Python interpreter...")
    find_package(Python 3.8 REQUIRED COMPONENTS Interpreter)
    set(OCTKPython_EXECUTABLE "${Python_EXECUTABLE}" CACHE INTERNAL "python executable path." FORCE)
endif()
set(OCTKPython_FOUND ON)