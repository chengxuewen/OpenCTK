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
if(TARGET OCTK3rdparty::WrapLibdatachannel)
	set(OCTKWrapLibdatachannel_FOUND ON)
	return()
endif()

octk_vcpkg_install_package(libdatachannel
	NOT_IMPORT
	TARGET
	OCTK3rdparty::WrapLibdatachannel
	PREFIX
	OCTKWrapLibdatachannel
	COMPONENTS
	srtp stdcall ws)
set(CMAKE_PREFIX_PATH_CACHE ${CMAKE_PREFIX_PATH})
set(CMAKE_PREFIX_PATH ${OCTKWrapLibdatachannel_INSTALL_DIR})
find_package(LibDataChannel REQUIRED)
target_link_libraries(OCTK3rdparty::WrapLibdatachannel INTERFACE LibDataChannel::LibDataChannel)
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH_CACHE})
set(OCTKWrapLibdatachannel_FOUND ON)
