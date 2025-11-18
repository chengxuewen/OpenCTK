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

#-----------------------------------------------------------------------------------------------------------------------
# Add lib link libraries
#-----------------------------------------------------------------------------------------------------------------------
#octk_find_package(WrapOpenSSL PROVIDED_TARGETS OCTK3rdparty::WrapOpenSSL)


#-----------------------------------------------------------------------------------------------------------------------
# Add lib target
#-----------------------------------------------------------------------------------------------------------------------
octk_add_library(network
	EXCEPTIONS
	SOURCES
	source/ssl/octk_ssl_certificate.cpp
	source/ssl/octk_ssl_certificate.hpp
	source/ssl/octk_ssl_fingerprint.cpp
	source/ssl/octk_ssl_fingerprint.hpp
	source/ssl/octk_ssl_identity.cpp
	source/ssl/octk_ssl_identity.hpp
	source/octk_network.cpp
	source/octk_network.h
	source/octk_network_global.hpp
	PRECOMPILED_HEADER
	source/octk_network_pch.hpp
	LIBRARIES
#	OCTK3rdparty::WrapOpenSSL
	octk::core_private
	PUBLIC_LIBRARIES
	octk::core)
if(OCTK_FEATURE_USE_BOOST_BACKEND)
	octk_find_package(WrapBoostAsio PROVIDED_TARGETS OCTK3rdparty::WrapBoostAsio)
	octk_find_package(WrapBoostBeast PROVIDED_TARGETS OCTK3rdparty::WrapBoostBeast)
	octk_internal_extend_target(octk_network
		SOURCES
		LIBRARIES
		OCTK3rdparty::WrapBoostAsio
		OCTK3rdparty::WrapBoostBeast)
else()
	octk_find_package(WrapLibcurl PROVIDED_TARGETS OCTK3rdparty::WrapLibcurl)
	octk_internal_extend_target(octk_network
		SOURCES
		LIBRARIES
		OCTK3rdparty::WrapLibcurl)
endif()
if(OCTK_FEATURE_USE_LIBCPR)
	octk_find_package(WrapLibcpr PROVIDED_TARGETS OCTK3rdparty::WrapLibcpr)
	octk_internal_extend_target(octk_network
		LIBRARIES
		OCTK3rdparty::WrapLibcpr)
endif()


#-----------------------------------------------------------------------------------------------------------------------
# Add lib examples and tests
#-----------------------------------------------------------------------------------------------------------------------
octk_add_subdirectory(examples OCTK_BUILD_EXAMPLES)
octk_add_subdirectory(tests OCTK_BUILD_TESTS)
