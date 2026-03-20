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
set(OCTK_TEST_LINK_LIBRARIES OpenCTK::Media OpenCTK::MediaPrivate OCTK3rdparty::WrapGTest OCTK3rdparty::WrapBenchmark)


#-----------------------------------------------------------------------------------------------------------------------
# Add tests
#-----------------------------------------------------------------------------------------------------------------------
octk_add_test(OpenCTKMediaTstBalancedDegradationSettings
	SOURCES
	tst_balanced_degradation_settings.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
if(OCTK_FEATURE_ENABLE_CAPTURE_CAMERA)
	octk_add_test(OpenCTKMediaTstCameraCapture
		SOURCES
		tst_camera_capture.cpp
		INCLUDE_DIRECTORIES
		LIBRARIES
		${OCTK_TEST_LINK_LIBRARIES}
		OUTPUT_DIRECTORY
		${OCTK_TEST_OUTPUT_DIR})
endif()
octk_add_test(OpenCTKMediaTstChainDiffCalculator
	SOURCES
	tst_chain_diff_calculator.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKMediaTstColorSpace
	SOURCES
	tst_color_space.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKMediaTstFieldTrialList
	SOURCES
	tst_field_trial_list.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKMediaTstFieldTrialParser
	SOURCES
	tst_field_trial_parser.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKMediaTstFieldTrialUnits
	SOURCES
	tst_field_trial_units.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKMediaTstFieldTrials
	SOURCES
	tst_field_trials.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKMediaTstFrameDependenciesCalculator
	SOURCES
	tst_frame_dependencies_calculator.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKMediaTstFrameGenerator
	SOURCES
	tst_frame_generator.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKMediaTstFrameGeneratorCapturer
	SOURCES
	tst_frame_generator_capturer.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
if(OCTK_FEATURE_MEDIA_USE_H264)
	octk_add_test(OpenCTKMediaTstH264BitstreamParser
		SOURCES
		tst_h264_bitstream_parser.cpp
		INCLUDE_DIRECTORIES
		LIBRARIES
		${OCTK_TEST_LINK_LIBRARIES}
		OUTPUT_DIRECTORY
		${OCTK_TEST_OUTPUT_DIR})
	octk_add_test(OpenCTKMediaTstH264Codecs
		SOURCES
		tst_h264_codecs.cpp
		INCLUDE_DIRECTORIES
		LIBRARIES
		${OCTK_TEST_LINK_LIBRARIES}
		OUTPUT_DIRECTORY
		${OCTK_TEST_OUTPUT_DIR})
	octk_add_test(OpenCTKMediaTstH264Encoder
		SOURCES
		tst_h264_encoder.cpp
		INCLUDE_DIRECTORIES
		LIBRARIES
		${OCTK_TEST_LINK_LIBRARIES}
		OUTPUT_DIRECTORY
		${OCTK_TEST_OUTPUT_DIR})
	octk_add_test(OpenCTKMediaTstH264PpsParser
		SOURCES
		tst_h264_pps_parser.cpp
		INCLUDE_DIRECTORIES
		LIBRARIES
		${OCTK_TEST_LINK_LIBRARIES}
		OUTPUT_DIRECTORY
		${OCTK_TEST_OUTPUT_DIR})
	#octk_add_test(OpenCTKMediaTsth264_simulcast
	#	SOURCES
	#	tst_h264_simulcast.cpp
	#	INCLUDE_DIRECTORIES
	#	LIBRARIES
	#	${OCTK_TEST_LINK_LIBRARIES}
	#	OUTPUT_DIRECTORY
	#	${OCTK_TEST_OUTPUT_DIR})
	octk_add_test(OpenCTKMediaTstH264SpsParser
		SOURCES
		tst_h264_sps_parser.cpp
		INCLUDE_DIRECTORIES
		LIBRARIES
		${OCTK_TEST_LINK_LIBRARIES}
		OUTPUT_DIRECTORY
		${OCTK_TEST_OUTPUT_DIR})
	octk_add_test(OpenCTKMediaTstH264SpsVuiRewwriter
		SOURCES
		tst_h264_sps_vui_rewriter.cpp
		INCLUDE_DIRECTORIES
		LIBRARIES
		${OCTK_TEST_LINK_LIBRARIES}
		OUTPUT_DIRECTORY
		${OCTK_TEST_OUTPUT_DIR})
endif()
octk_add_test(OpenCTKMediaTstMediaContext
	SOURCES
	tst_media_context.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(OpenCTKMediaTstRateControlSettings
#	SOURCES
#	tst_rate_control_settings.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(OpenCTKMediaTstRtcContext
#	SOURCES
#	tst_rtc_context.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(OpenCTKMediaTstRtcStats
#	SOURCES
#	tst_rtc_stats.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(OpenCTKMediaTstRtcStatsReport
#	SOURCES
#	tst_rtc_stats_report.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(OpenCTKMediaTstRtpParameters
#	SOURCES
#	tst_rtp_parameters.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKMediaTstScalabilityModeUtils
	SOURCES
	tst_scalability_mode_utils.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	OCTK3rdparty::WrapLibyuv
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKMediaTstScalabilityStructure
	SOURCES
	tst_scalability_structure.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	OCTK3rdparty::WrapLibyuv
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKMediaTstScalabilityStructureFullSvc
	SOURCES
	tst_scalability_structure_full_svc.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	OCTK3rdparty::WrapLibyuv
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKMediaTstScalabilityStructureKeySvc
	SOURCES
	tst_scalability_structure_key_svc.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	OCTK3rdparty::WrapLibyuv
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKMediaTstScalabilityStructureL2T2KeyShift
	SOURCES
	tst_scalability_structure_l2t2_key_shift.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	OCTK3rdparty::WrapLibyuv
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKMediaTstSimulcastRateAllocator
	SOURCES
	tst_simulcast_rate_allocator.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	OCTK3rdparty::WrapLibyuv
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKMediaTstSimulcastToSvcConverter
	SOURCES
	tst_simulcast_to_svc_converter.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	OCTK3rdparty::WrapLibyuv
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(OpenCTKMediaTstStableTargetRateExperiment
#	SOURCES
#	tst_stable_target_rate_experiment.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	OCTK3rdparty::WrapLibyuv
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKMediaTstStructParametersParser
	SOURCES
	tst_struct_parameters_parser.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	OCTK3rdparty::WrapLibyuv
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKMediaTstSvcRateAllocator
	SOURCES
	tst_svc_rate_allocator.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	OCTK3rdparty::WrapLibyuv
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKMediaTstVideoAdaptationCounters
	SOURCES
	tst_video_adaptation_counters.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	OCTK3rdparty::WrapLibyuv
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKMediaTstVideoBitrateAllocation
	SOURCES
	tst_video_bitrate_allocation.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	OCTK3rdparty::WrapLibyuv
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
octk_add_test(OpenCTKMediaTstVideoFrameBufferPool
	SOURCES
	tst_video_frame_buffer_pool.cpp
	INCLUDE_DIRECTORIES
	LIBRARIES
	OCTK3rdparty::WrapLibyuv
	${OCTK_TEST_LINK_LIBRARIES}
	OUTPUT_DIRECTORY
	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(OpenCTKMediaTstVideoSourceRestrictions
#	SOURCES
#	tst_video_source_restrictions.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	OCTK3rdparty::WrapLibyuv
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(OpenCTKMediaTstVideoStreamEncoder
#	SOURCES
#	tst_video_stream_encoder.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})
#octk_add_test(OpenCTKMediaTstYuv
#	SOURCES
#	tst_yuv.cpp
#	INCLUDE_DIRECTORIES
#	LIBRARIES
#	${OCTK_TEST_LINK_LIBRARIES}
#	OUTPUT_DIRECTORY
#	${OCTK_TEST_OUTPUT_DIR})