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
if(TARGET OpenCTKPython::Python)
    set(OpenCTKPython_FOUND ON)
    return()
endif()

if(WIN32)
    set(OpenCTKPython_NAME "python-3.8.10-embed-win32")
    set(OpenCTKPython_PKG_NAME "${OpenCTKPython_NAME}.zip")
    set(OpenCTKPython_URL_PATH "${PROJECT_SOURCE_DIR}/3rdparty/${OpenCTKPython_PKG_NAME}")
    set(OpenCTKPython_ROOT_DIR "${PROJECT_BINARY_DIR}/3rdparty/${OpenCTKPython_NAME}")
    set(OpenCTKPython_SOURCE_DIR "${OpenCTKPython_ROOT_DIR}/source")
    set(OpenCTKPython_INSTALL_DIR "${OpenCTKPython_ROOT_DIR}/install")
    octk_stamp_file_info(OpenCTKPython OUTPUT_DIR "${OpenCTKPython_ROOT_DIR}")
    octk_fetch_3rdparty(OpenCTKPython URL "${OpenCTKPython_URL_PATH}" OUTPUT_NAME "${OpenCTKPython_NAME}")
    set(OpenCTKPython_EXECUTABLE "${OpenCTKPython_SOURCE_DIR}/python.exe" CACHE INTERNAL "python executable path." FORCE)
else()
    message(STATUS "Find Python interpreter...")
    find_package(Python 3.8 REQUIRED COMPONENTS Interpreter)
    set(OpenCTKPython_EXECUTABLE "${Python_EXECUTABLE}" CACHE INTERNAL "python executable path." FORCE)
endif()
set(OCTKMeson_FILE "${PROJECT_SOURCE_DIR}/3rdparty/meson.pyz" CACHE INTERNAL "meson pyz file path." FORCE)
set(OpenCTKPython_FOUND ON)