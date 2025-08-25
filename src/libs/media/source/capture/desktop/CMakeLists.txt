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
octk_internal_extend_target(octk_media
	SOURCES
	${ROOT_DIR}/octk_blank_detector_desktop_capturer_wrapper.cpp
	${ROOT_DIR}/octk_blank_detector_desktop_capturer_wrapper.hpp
	${ROOT_DIR}/octk_cropped_desktop_frame.cpp
	${ROOT_DIR}/octk_cropped_desktop_frame.hpp
	${ROOT_DIR}/octk_cropping_window_capturer.cpp
	${ROOT_DIR}/octk_cropping_window_capturer.hpp
	${ROOT_DIR}/octk_delegated_source_list_controller.hpp
	${ROOT_DIR}/octk_desktop_and_cursor_composer.cpp
	${ROOT_DIR}/octk_desktop_and_cursor_composer.hpp
	${ROOT_DIR}/octk_desktop_capture_metadata.hpp
	${ROOT_DIR}/octk_desktop_capture_metrics_helper.cpp
	${ROOT_DIR}/octk_desktop_capture_metrics_helper.hpp
	${ROOT_DIR}/octk_desktop_capture_options.cpp
	${ROOT_DIR}/octk_desktop_capture_options.hpp
	${ROOT_DIR}/octk_desktop_capture_source.cpp
	${ROOT_DIR}/octk_desktop_capture_source.hpp
	${ROOT_DIR}/octk_desktop_capture_types.hpp
	${ROOT_DIR}/octk_desktop_capturer.cpp
	${ROOT_DIR}/octk_desktop_capturer.hpp
	${ROOT_DIR}/octk_desktop_capturer_differ_wrapper.cpp
	${ROOT_DIR}/octk_desktop_capturer_differ_wrapper.hpp
	${ROOT_DIR}/octk_desktop_capturer_wrapper.cpp
	${ROOT_DIR}/octk_desktop_capturer_wrapper.hpp
	${ROOT_DIR}/octk_desktop_frame.cpp
	${ROOT_DIR}/octk_desktop_frame.hpp
	${ROOT_DIR}/octk_desktop_frame_generator.cpp
	${ROOT_DIR}/octk_desktop_frame_generator.hpp
	${ROOT_DIR}/octk_desktop_frame_rotation.cpp
	${ROOT_DIR}/octk_desktop_frame_rotation.hpp
	${ROOT_DIR}/octk_desktop_geometry.cpp
	${ROOT_DIR}/octk_desktop_geometry.hpp
	${ROOT_DIR}/octk_desktop_region.cpp
	${ROOT_DIR}/octk_desktop_region.hpp
	${ROOT_DIR}/octk_differ_block.cpp
	${ROOT_DIR}/octk_differ_block.hpp
	${ROOT_DIR}/octk_differ_vector_sse2.cpp
	${ROOT_DIR}/octk_differ_vector_sse2.hpp
	${ROOT_DIR}/octk_fake_desktop_capturer.cpp
	${ROOT_DIR}/octk_fake_desktop_capturer.hpp
	${ROOT_DIR}/octk_fallback_desktop_capturer_wrapper.cpp
	${ROOT_DIR}/octk_fallback_desktop_capturer_wrapper.hpp
	${ROOT_DIR}/octk_full_screen_application_handler.cpp
	${ROOT_DIR}/octk_full_screen_application_handler.hpp
	${ROOT_DIR}/octk_full_screen_window_detector.cpp
	${ROOT_DIR}/octk_full_screen_window_detector.hpp
	${ROOT_DIR}/octk_mouse_cursor.cpp
	${ROOT_DIR}/octk_mouse_cursor.hpp
	${ROOT_DIR}/octk_mouse_cursor_monitor.hpp
	${ROOT_DIR}/octk_resolution_tracker.cpp
	${ROOT_DIR}/octk_resolution_tracker.hpp
	${ROOT_DIR}/octk_rgba_color.cpp
	${ROOT_DIR}/octk_rgba_color.hpp
	${ROOT_DIR}/octk_screen_capture_frame_queue.hpp
	${ROOT_DIR}/octk_screen_capturer_helper.cpp
	${ROOT_DIR}/octk_screen_capturer_helper.hpp
	${ROOT_DIR}/octk_screen_drawer.cpp
	${ROOT_DIR}/octk_screen_drawer.hpp
	${ROOT_DIR}/octk_shared_desktop_frame.cpp
	${ROOT_DIR}/octk_shared_desktop_frame.hpp
	${ROOT_DIR}/octk_window_finder.cpp
	${ROOT_DIR}/octk_window_finder.hpp)
octk_internal_extend_target(octk_media
	SOURCES
	${ROOT_DIR}/mac/octk_desktop_configuration.hpp
	${ROOT_DIR}/mac/octk_desktop_configuration.mm
	${ROOT_DIR}/mac/octk_desktop_configuration_monitor.cpp
	${ROOT_DIR}/mac/octk_desktop_configuration_monitor.hpp
	${ROOT_DIR}/mac/octk_desktop_frame_cgimage.hpp
	${ROOT_DIR}/mac/octk_desktop_frame_cgimage.mm
	${ROOT_DIR}/mac/octk_desktop_frame_iosurface.hpp
	${ROOT_DIR}/mac/octk_desktop_frame_iosurface.mm
	${ROOT_DIR}/mac/octk_desktop_frame_provider.hpp
	${ROOT_DIR}/mac/octk_desktop_frame_provider.mm
	${ROOT_DIR}/mac/octk_desktop_frame_utils.cpp
	${ROOT_DIR}/mac/octk_desktop_frame_utils.hpp
	${ROOT_DIR}/mac/octk_full_screen_mac_application_handler.cpp
	${ROOT_DIR}/mac/octk_full_screen_mac_application_handler.hpp
	${ROOT_DIR}/mac/octk_screen_capturer_mac.hpp
	${ROOT_DIR}/mac/octk_screen_capturer_mac.mm
	${ROOT_DIR}/mac/octk_screen_capturer_sck.hpp
	${ROOT_DIR}/mac/octk_screen_capturer_sck.mm
	${ROOT_DIR}/mac/octk_window_finder_mac.hpp
	${ROOT_DIR}/mac/octk_window_finder_mac.mm
	${ROOT_DIR}/mac/octk_window_list_utils.cpp
	${ROOT_DIR}/mac/octk_window_list_utils.hpp
	${ROOT_DIR}/octk_mouse_cursor_monitor_mac.mm
	${ROOT_DIR}/octk_screen_capturer_darwin.mm
	${ROOT_DIR}/octk_screen_drawer_mac.cpp
	${ROOT_DIR}/octk_window_capturer_mac.mm
	LIBRARIES
	"-framework AppKit"
	"-framework CoreAudio"
	"-framework CoreVideo"
	"-framework CoreMedia"
	"-framework IOSurface"
	"-framework CoreServices"
	"-framework AVFoundation"
	"-framework AudioToolbox"
	"-framework ScreenCaptureKit"
	"-framework ApplicationServices"
	CONDITION OCTK_SYSTEM_MAC)
octk_internal_extend_target(octk_media
	SOURCES
	${ROOT_DIR}/octk_mouse_cursor_monitor_linux.cpp
	${ROOT_DIR}/octk_screen_capturer_linux.cpp
	${ROOT_DIR}/octk_screen_drawer_linux.cpp
	${ROOT_DIR}/octk_window_capturer_linux.cpp
	CONDITION OCTK_SYSTEM_LINUX)
octk_internal_extend_target(octk_media
	SOURCES
	${ROOT_DIR}/octk_screen_drawer_lock_posix.cpp
	${ROOT_DIR}/octk_screen_drawer_lock_posix.hpp
	CONDITION OCTK_SYSTEM_UNIX)
octk_internal_extend_target(octk_media
	SOURCES
	${ROOT_DIR}/octk_cropping_window_capturer_win.cpp
	${ROOT_DIR}/octk_mouse_cursor_monitor_win.cpp
	${ROOT_DIR}/octk_screen_capturer_win.cpp
	${ROOT_DIR}/octk_screen_drawer_win.cpp
	${ROOT_DIR}/octk_window_capturer_win.cpp
	CONDITION OCTK_SYSTEM_WIN)
octk_internal_extend_target(octk_media
	SOURCES
	${ROOT_DIR}/octk_cropping_window_capturer_win.cpp
	${ROOT_DIR}/octk_mouse_cursor_monitor_win.cpp
	${ROOT_DIR}/octk_screen_capturer_win.cpp
	${ROOT_DIR}/octk_screen_drawer_win.cpp
	${ROOT_DIR}/octk_window_capturer_win.cpp
	CONDITION OCTK_SYSTEM_WIN)
octk_internal_extend_target(octk_media
	SOURCES
	${ROOT_DIR}/octk_screen_capturer_null.cpp
	${ROOT_DIR}/octk_window_capturer_null.cpp
	${ROOT_DIR}/octk_mouse_cursor_monitor_null.cpp
	CONDITION ((NOT OCTK_SYSTEM_MAC) AND (NOT OCTK_SYSTEM_WIN) AND (NOT OCTK_SYSTEM_LINUX)))

