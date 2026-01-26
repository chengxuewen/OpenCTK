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
# Set tests output path
#-----------------------------------------------------------------------------------------------------------------------
set(OCTK_TEST_OUTPUT_DIR ${OCTK_BUILD_DIR}/${OCTK_INSTALL_TESTSDIR})


#-----------------------------------------------------------------------------------------------------------------------
# Add tests link libraries
#-----------------------------------------------------------------------------------------------------------------------
octk_find_package(WrapLibyuv PROVIDED_TARGETS OCTK3rdparty::WrapLibyuv)
set(OCTK_TEST_LINK_LIBRARIES octk::media octk::media_private OCTK3rdparty::WrapGTest OCTK3rdparty::WrapBenchmark)


#-----------------------------------------------------------------------------------------------------------------------
# Add tests
#-----------------------------------------------------------------------------------------------------------------------
octk_add_test(octk_media_tst_balanced_degradation_settings
	SOURCES
	tst_balanced_degradation_settings.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(octk_media_tst_broadcast_resource_listener
	SOURCES
	tst_broadcast_resource_listener.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
if(OCTK_FEATURE_ENABLE_CAPTURE_CAMERA)
	octk_add_test(octk_media_tst_camera_capture
		SOURCES
		tst_camera_capture.cpp
		INCLUDE_DIRECTORIES
		LIBRARIES
		${OCTK_TEST_LINK_LIBRARIES}
		OUTPUT_DIRECTORY
		${OCTK_TEST_OUTPUT_DIR})
endif()
octk_add_test(octk_media_tst_chain_diff_calculator
	SOURCES
	tst_chain_diff_calculator.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(octk_media_tst_color_space
	SOURCES
	tst_color_space.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(octk_media_tst_field_trial_list
	SOURCES
	tst_field_trial_list.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(octk_media_tst_field_trial_parser
	SOURCES
	tst_field_trial_parser.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(octk_media_tst_field_trial_units
	SOURCES
	tst_field_trial_units.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(octk_media_tst_field_trials
	SOURCES
	tst_field_trials.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(octk_media_tst_frame_dependencies_calculator
	SOURCES
	tst_frame_dependencies_calculator.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(octk_media_tst_frame_generator
	SOURCES
	tst_frame_generator.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(octk_media_tst_frame_generator_capturer
	SOURCES
	tst_frame_generator_capturer.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(octk_media_tst_h264_bitstream_parser
	SOURCES
	tst_h264_bitstream_parser.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(octk_media_tst_h264_codecs
	SOURCES
	tst_h264_codecs.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(octk_media_tst_h264_encoder
	SOURCES
	tst_h264_encoder.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(octk_media_tst_h264_pps_parser
	SOURCES
	tst_h264_pps_parser.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(octk_media_tst_h264_simulcast
	SOURCES
	tst_h264_simulcast.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(octk_media_tst_h264_sps_parser
	SOURCES
	tst_h264_sps_parser.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(octk_media_tst_h264_sps_vui_rewriter
	SOURCES
	tst_h264_sps_vui_rewriter.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(octk_media_tst_media_context
	SOURCES
	tst_media_context.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(octk_media_tst_rate_control_settings
#	SOURCES
#	tst_rate_control_settings.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(octk_media_tst_rtc_context
#	SOURCES
#	tst_rtc_context.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(octk_media_tst_rtc_stats
#	SOURCES
#	tst_rtc_stats.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(octk_media_tst_rtc_stats_report
#	SOURCES
#	tst_rtc_stats_report.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(octk_media_tst_rtp_parameters
#	SOURCES
#	tst_rtp_parameters.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(octk_media_tst_scalability_mode_utils
	SOURCES
	tst_scalability_mode_utils.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	OCTK3rdparty::WrapLibyuv
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(octk_media_tst_scalability_structure
	SOURCES
	tst_scalability_structure.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	OCTK3rdparty::WrapLibyuv
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(octk_media_tst_scalability_structure_full_svc
	SOURCES
	tst_scalability_structure_full_svc.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	OCTK3rdparty::WrapLibyuv
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(octk_media_tst_scalability_structure_key_svc
	SOURCES
	tst_scalability_structure_key_svc.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	OCTK3rdparty::WrapLibyuv
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(octk_media_tst_scalability_structure_l2t2_key_shift
	SOURCES
	tst_scalability_structure_l2t2_key_shift.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	OCTK3rdparty::WrapLibyuv
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(octk_media_tst_simulated_time_controller
	SOURCES
	tst_simulated_time_controller.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	OCTK3rdparty::WrapLibyuv
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(octk_media_tst_simulcast_rate_allocator
	SOURCES
	tst_simulcast_rate_allocator.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	OCTK3rdparty::WrapLibyuv
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(octk_media_tst_simulcast_to_svc_converter
	SOURCES
	tst_simulcast_to_svc_converter.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	OCTK3rdparty::WrapLibyuv
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(octk_media_tst_stable_target_rate_experiment
#	SOURCES
#	tst_stable_target_rate_experiment.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	OCTK3rdparty::WrapLibyuv
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(octk_media_tst_struct_parameters_parser
	SOURCES
	tst_struct_parameters_parser.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	OCTK3rdparty::WrapLibyuv
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(octk_media_tst_svc_rate_allocator
	SOURCES
	tst_svc_rate_allocator.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	OCTK3rdparty::WrapLibyuv
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(octk_media_tst_video_adaptation_counters
	SOURCES
	tst_video_adaptation_counters.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	OCTK3rdparty::WrapLibyuv
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(octk_media_tst_video_bitrate_allocation
	SOURCES
	tst_video_bitrate_allocation.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	OCTK3rdparty::WrapLibyuv
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(octk_media_tst_video_frame_buffer_pool
	SOURCES
	tst_video_frame_buffer_pool.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	OCTK3rdparty::WrapLibyuv
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(octk_media_tst_video_source_restrictions
	SOURCES
	tst_video_source_restrictions.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	OCTK3rdparty::WrapLibyuv
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(octk_media_tst_video_stream_encoder
#	SOURCES
#	tst_video_stream_encoder.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(octk_media_tst_yuv
#	SOURCES
#	tst_yuv.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})