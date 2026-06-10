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

set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
#-----------------------------------------------------------------------------------------------------------------------
# extend lib target
#-----------------------------------------------------------------------------------------------------------------------
string(FIND "${CMAKE_PREFIX_PATH}" ".pixi/envs" PIXI_ENV_FIND_POS)
if(EXISTS "${CMAKE_SOURCE_DIR}/pixi.toml" AND PIXI_ENV_FIND_POS GREATER -1)
    octk_pkgconf_check_modules(GIOUnix REQUIRED IMPORTED_TARGET gio-unix-2.0)
else()
    octk_pkgconf_check_modules(GIOUnix REQUIRED
	    PATH "/usr/lib/${CMAKE_LIBRARY_ARCHITECTURE}/pkgconfig"
	    IMPORTED_TARGET gio-unix-2.0)
endif()
octk_internal_extend_target(Media
	SOURCES
	${ROOT_DIR}/camera_portal.cpp
	${ROOT_DIR}/camera_portal_p.hpp
	${ROOT_DIR}/portal_request_response_p.hpp
	${ROOT_DIR}/scoped_glib.cpp
	${ROOT_DIR}/scoped_glib_p.hpp
	${ROOT_DIR}/xdg_desktop_portal_utils.cpp
	${ROOT_DIR}/xdg_desktop_portal_utils_p.hpp
	${ROOT_DIR}/xdg_session_detail_p.hpp
	LIBRARIES
	PkgConfig::GIOUnix)
if (OCTK_FEATURE_MEDIA_ENABLE_CAPTURE_CAMERA_PIPEWIRE)
	octk_internal_extend_target(Media
		SOURCES
		pipewire_utils.cpp
		pipewire_utils_p.hpp
		LIBRARIES
		PkgConfig::PipeWire)
endif ()
