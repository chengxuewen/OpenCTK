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
# Resolve gio-unix-2.0 via pixi env (consistent with the rest of the build).
# Let pkgconf discover it through CMAKE_PREFIX_PATH → pixi lib/pkgconfig.
# Resolve full lib paths with find_library so CMake generates correct RPATH
# and avoids system/pixi library shadowing warnings.
octk_pkgconf_check_modules(GIOUnix QUIET
    IMPORTED_TARGET gio-unix-2.0)
if(NOT GIOUnix_FOUND)
    message(FATAL_ERROR
        "Camera portal capture requires gio-unix-2.0 (from pixi environment).\n"
        "Ensure pixi environment is set up: ./pixi.sh\n"
        "Verify: ls \${CMAKE_PREFIX_PATH}/lib/pkgconfig/gio-unix-2.0.pc")
endif()
find_library(_GIOUnix_GIO_LIB gio-2.0 REQUIRED)
find_library(_GIOUnix_GOBJECT_LIB gobject-2.0 REQUIRED)
find_library(_GIOUnix_GLIB_LIB glib-2.0 REQUIRED)
octk_internal_extend_target(OpenCTKMedia
    SOURCES
    ${ROOT_DIR}/octk_camera_portal.cpp
    ${ROOT_DIR}/octk_camera_portal_p.hpp
    ${ROOT_DIR}/octk_portal_request_response_p.hpp
    ${ROOT_DIR}/octk_scoped_glib.cpp
    ${ROOT_DIR}/octk_scoped_glib_p.hpp
    ${ROOT_DIR}/octk_xdg_desktop_portal_utils.cpp
    ${ROOT_DIR}/octk_xdg_desktop_portal_utils_p.hpp
    ${ROOT_DIR}/octk_xdg_session_detail_p.hpp
    INCLUDE_DIRECTORIES
    ${GIOUnix_INCLUDE_DIRS}
    COMPILE_OPTIONS
    ${GIOUnix_CFLAGS_OTHER}
    LINK_OPTIONS
    ${GIOUnix_LDFLAGS_OTHER}
    LIBRARIES
    ${_GIOUnix_GIO_LIB}
    ${_GIOUnix_GOBJECT_LIB}
    ${_GIOUnix_GLIB_LIB})
if (OCTK_FEATURE_MEDIA_ENABLE_CAPTURE_CAMERA_PIPEWIRE)
	octk_internal_extend_target(OpenCTKMedia
		SOURCES
		octk_pipewire_utils.cpp
		octk_pipewire_utils_p.hpp
		LIBRARIES
		PkgConfig::PipeWire)
endif ()
