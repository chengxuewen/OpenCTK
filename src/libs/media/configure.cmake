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

octk_configure_feature("USE_FFMPEG" PUBLIC
	LABEL "Enable this to build use ffmpeg"
	CONDITION OFF)
octk_configure_feature("USE_LIBSRTP" PUBLIC
	LABEL "Enable this to build use libsrtp"
	CONDITION OFF)
octk_configure_feature("USE_ZLMEDIAKIT" PUBLIC
	LABEL "Enable this to build use ZLMediaKit"
	CONDITION OFF)
octk_configure_feature("ENABLE_CAPTURE_CAMERA" PUBLIC
    LABEL "Enable this to build enable capture camera function"
    CONDITION OFF)
octk_configure_feature("ENABLE_CAPTURE_DESKTOP" PUBLIC
    LABEL "Enable this to build enable capture desktop function"
	CONDITION OFF)
octk_configure_feature("ENABLE_CAPTURE_VIDEO" PUBLIC
    LABEL "Enable this to build enable capture video function"
	CONDITION OFF)

find_package(PkgConfig REQUIRED)
pkg_check_modules(pipewire QUIET IMPORTED_TARGET libpipewire-0.3)
octk_configure_feature("ENABLE_CAPTURE_CAMERA_PIPEWIRE" PUBLIC
    LABEL "Enable this to build enable capture video libpipewire"
    DISABLE NOT TARGET PkgConfig::pipewire
    AUTODETECT ON)
