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
#octk_find_package(WrapFFmpeg PROVIDED_TARGETS OCTK3rdparty::WrapFFmpeg)
octk_find_package(WrapLibyuv PROVIDED_TARGETS OCTK3rdparty::WrapLibyuv)


#-----------------------------------------------------------------------------------------------------------------------
# Add lib target
#-----------------------------------------------------------------------------------------------------------------------
octk_add_library(media
    EXCEPTIONS
    SOURCES
    source/octk_media_p.hpp
    source/octk_media.cpp
    source/octk_media.hpp
    source/octk_media.h
    source/octk_media_global.hpp
    PRECOMPILED_HEADER
    source/octk_media_pch.hpp
    LIBRARIES
    OCTK3rdparty::WrapLibyuv
    #	OCTK3rdparty::WrapFFmpeg
    octk::core_private
    PUBLIC_LIBRARIES
    octk::core)
#octk_internal_extend_target(octk_media # for static ffmpeg
#	CONDITION OCTK_SYSTEM_LINUX
#	LINK_OPTIONS "-Wl,-Bsymbolic")


#-----------------------------------------------------------------------------------------------------------------------
# Add lib examples and tests
#-----------------------------------------------------------------------------------------------------------------------
octk_add_subdirectory(examples OCTK_BUILD_EXAMPLES)
octk_add_subdirectory(tests OCTK_BUILD_TESTS)
