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
if(TARGET OCTK3rdparty::WrapLibcpr)
    set(OCTK3WrapLibcpr_FOUND ON)
    return()
endif()

octk_vcpkg_install_package(cpr
    NOT_IMPORT
    TARGET
    OCTK3rdparty::WrapLibcpr
    PREFIX
	OCTK3WrapLibcpr
    COMPONENTS
    ssl)
set(CPR_ROOT_DIR ${OCTK3WrapLibcpr_INSTALL_DIR})
set(OPENSSL_ROOT_DIR ${OCTK3WrapLibcpr_INSTALL_DIR})
find_package(cpr PATHS ${OCTK3WrapLibcpr_INSTALL_DIR} NO_DEFAULT_PATH REQUIRED)
get_target_property(cpr_IMPORTED_LOCATION_RELEASE cpr::cpr IMPORTED_LOCATION_RELEASE)
set_target_properties(cpr::cpr PROPERTIES IMPORTED_LOCATION_MINSIZEREL ${cpr_IMPORTED_LOCATION_RELEASE})
set_target_properties(cpr::cpr PROPERTIES IMPORTED_LOCATION_RELWITHDEBINFO ${cpr_IMPORTED_LOCATION_RELEASE})
target_link_libraries(OCTK3rdparty::WrapLibcpr INTERFACE OpenSSL::SSL OpenSSL::Crypto cpr::cpr)
set(OCTK3WrapLibcpr_FOUND ON)