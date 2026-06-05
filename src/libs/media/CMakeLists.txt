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
#octk_find_package(WrapOpenh264 PROVIDED_TARGETS OpenCTKWrapOpenh264::WrapOpenh264)
octk_find_package(WrapLibyuv PROVIDED_TARGETS OpenCTKWrapLibyuv::WrapLibyuv)


#-----------------------------------------------------------------------------------------------------------------------
# Add lib target
#-----------------------------------------------------------------------------------------------------------------------
octk_add_library(Media
	LIBRARY_INCLUDE_NAME "openctk/media"
	EXCEPTIONS
	SOURCES
#	source/codecs/video/backends/h264/video_decoder_openh264.cpp
#	source/codecs/video/backends/h264/video_decoder_openh264_p.hpp
#	source/codecs/video/backends/h264/video_encoder_openh264.cpp
#	source/codecs/video/backends/h264/video_encoder_openh264_p.hpp
#	source/codecs/video/formats/av1/av1_profile.cpp
#	source/codecs/video/formats/av1/av1_profile.hpp
#	source/codecs/video/formats/av1/av1_svc_config.cpp
#	source/codecs/video/formats/av1/av1_svc_config.hpp
#	source/codecs/video/formats/h264/h264_bitstream_parser.cpp
#	source/codecs/video/formats/h264/h264_bitstream_parser_p.hpp
#	source/codecs/video/formats/h264/h264_codecs.cpp
#	source/codecs/video/formats/h264/h264_codecs.hpp
#	source/codecs/video/formats/h264/h264_common.cpp
#	source/codecs/video/formats/h264/h264_common_p.hpp
#	source/codecs/video/formats/h264/h264_pps_parser.cpp
#	source/codecs/video/formats/h264/h264_pps_parser_p.hpp
#	source/codecs/video/formats/h264/h264_profile.cpp
#	source/codecs/video/formats/h264/h264_profile.hpp
#	source/codecs/video/formats/h264/h264_sps_parser.cpp
#	source/codecs/video/formats/h264/h264_sps_parser_p.hpp
#	source/codecs/video/formats/h264/h264_sps_vui_rewriter.cpp
#	source/codecs/video/formats/h264/h264_sps_vui_rewriter_p.hpp
#	source/codecs/video/formats/h264/h264_types.hpp
#	source/codecs/video/formats/vp8/vp8_constants.hpp
#	source/codecs/video/formats/vp8/vp8_header_parser.cpp
#	source/codecs/video/formats/vp8/vp8_header_parser.hpp
#	source/codecs/video/formats/vp8/vp8_types.hpp
#	source/codecs/video/formats/vp9/vp9_constants.hpp
#	source/codecs/video/formats/vp9/vp9_profile.cpp
#	source/codecs/video/formats/vp9/vp9_profile.hpp
#	source/codecs/video/formats/vp9/vp9_svc_config.cpp
#	source/codecs/video/formats/vp9/vp9_svc_config.hpp
#	source/codecs/video/formats/vp9/vp9_types.hpp
#	source/codecs/video/formats/vp9/vp9_uncompressed_header_parser.cpp
#	source/codecs/video/formats/vp9/vp9_uncompressed_header_parser_p.hpp
#	source/codecs/video/svc/scalability_mode.cpp
#	source/codecs/video/svc/scalability_mode.hpp
#	source/codecs/video/svc/scalability_mode_helper.cpp
#	source/codecs/video/svc/scalability_mode_helper_p.hpp
#	source/codecs/video/svc/scalability_mode_utils.cpp
#	source/codecs/video/svc/scalability_mode_utils_p.hpp
#	source/codecs/video/svc/scalability_structure.cpp
#	source/codecs/video/svc/scalability_structure_full_svc.cpp
#	source/codecs/video/svc/scalability_structure_full_svc_p.hpp
#	source/codecs/video/svc/scalability_structure_key_svc.cpp
#	source/codecs/video/svc/scalability_structure_key_svc_p.hpp
#	source/codecs/video/svc/scalability_structure_l2t2_key_shift.cpp
#	source/codecs/video/svc/scalability_structure_l2t2_key_shift_p.hpp
#	source/codecs/video/svc/scalability_structure_p.hpp
#	source/codecs/video/svc/scalability_structure_simulcast.cpp
#	source/codecs/video/svc/scalability_structure_simulcast_p.hpp
#	source/codecs/video/svc/scalable_video_controller_no_layering.cpp
#	source/codecs/video/svc/scalable_video_controller_no_layering_p.hpp
#	source/codecs/video/svc/scalable_video_controller_p.hpp
#	source/codecs/video/svc/simulcast_to_svc_converter.cpp
#	source/codecs/video/svc/simulcast_to_svc_converter_p.hpp
#	source/codecs/video/svc/svc_rate_allocator.cpp
#	source/codecs/video/svc/svc_rate_allocator_p.hpp
#	source/codecs/video/utility/simulcast_rate_allocator.cpp
#	source/codecs/video/utility/simulcast_rate_allocator.hpp
#	source/codecs/video/utility/simulcast_stream.cpp
#	source/codecs/video/utility/simulcast_stream.hpp
#	source/codecs/video/utility/simulcast_utility.cpp
#	source/codecs/video/utility/simulcast_utility_p.hpp
#	source/codecs/video/chain_diff_calculator.cpp
#	source/codecs/video/chain_diff_calculator_p.hpp
#	source/codecs/video/frame_dependencies_calculator.cpp
#	source/codecs/video/frame_dependencies_calculator_p.hpp
#	source/codecs/video/sdp_video_format.cpp
#	source/codecs/video/sdp_video_format.hpp
#	source/codecs/video/video_bitstream_parser_p.hpp
#	source/codecs/video/video_codec.cpp
#	source/codecs/video/video_codec.hpp
#	source/codecs/video/video_codec_constants.hpp
#	source/codecs/video/video_codes_error.cpp
#	source/codecs/video/video_codes_error.hpp
#	source/codecs/video/video_decoder.cpp
#	source/codecs/video/video_decoder.hpp
#	source/codecs/video/video_decoder_factory.hpp
#	source/codecs/video/video_encoder.cpp
#	source/codecs/video/video_encoder.hpp
#	source/codecs/video/video_encoder_factory.hpp
#	source/codecs/codec_specific_info.cpp
#	source/codecs/codec_specific_info.hpp
#	source/codecs/codecs_constants.hpp
#	source/codecs/dependency_descriptor.cpp
#	source/codecs/dependency_descriptor_p.hpp
#	source/event_log/media_event.cpp
#	source/event_log/media_event.hpp
#	source/event_log/media_event_log.cpp
#	source/event_log/media_event_log.hpp
#	source/event_log/media_event_log_factory.cpp
#	source/event_log/media_event_log_factory.hpp
#	source/experiments/balanced_degradation_settings.cpp
#	source/experiments/balanced_degradation_settings_p.hpp
#	source/experiments/field_trial_list.cpp
#	source/experiments/field_trial_list_p.hpp
#	source/experiments/field_trial_parser.cpp
#	source/experiments/field_trial_parser_p.hpp
#	source/experiments/field_trial_units.cpp
#	source/experiments/field_trial_units_p.hpp
#	source/experiments/field_trials.cpp
#	source/experiments/field_trials_p.hpp
#	source/experiments/field_trials_registry.cpp
#	source/experiments/field_trials_registry_p.hpp
#	source/experiments/field_trials_view.cpp
#	source/experiments/field_trials_view.hpp
#	source/experiments/rate_control_settings.cpp
#	source/experiments/rate_control_settings_p.hpp
#	source/experiments/stable_target_rate_experiment.cpp
#	source/experiments/stable_target_rate_experiment_p.hpp
#	source/experiments/struct_parameters_parser.cpp
#	source/experiments/struct_parameters_parser_p.hpp
#	source/media_context/media_context_factory.cpp
#	source/media_context/media_context_factory.hpp
#	source/media_context/media_context.hpp
#	source/protocols/rtp/rtp_headers.cpp
#	source/protocols/rtp/rtp_headers.hpp
#	source/protocols/rtp/rtp_packet_info.cpp
#	source/protocols/rtp/rtp_packet_info.hpp
#	source/protocols/rtp/rtp_packet_infos.hpp
#	source/protocols/rtp/rtp_video_header.cpp
#	source/protocols/rtp/rtp_video_header.hpp
	source/stream/media_stream_interface.cpp
	source/stream/media_stream_interface.hpp
	source/stream/media_stream_track.hpp
	source/video/color_space.cpp
	source/video/color_space.hpp
#	source/video/encoded_frame.cpp
#	source/video/encoded_frame.hpp
#	source/video/encoded_image.cpp
#	source/video/encoded_image.hpp
	source/video/frame_instrumentation_data.hpp
	source/video/frame_utils.cpp
	source/video/frame_utils.hpp
	source/video/framerate_controller.cpp
	source/video/framerate_controller.hpp
#	source/video/generic_frame_info.cpp
#	source/video/generic_frame_info.hpp
	source/video/hdr_metadata.cpp
	source/video/hdr_metadata.hpp
	source/video/i010_buffer.cpp
	source/video/i010_buffer.hpp
	source/video/i210_buffer.cpp
	source/video/i210_buffer.hpp
	source/video/i410_buffer.cpp
	source/video/i410_buffer.hpp
	source/video/i420_buffer.cpp
	source/video/i420_buffer.hpp
	source/video/i422_buffer.cpp
	source/video/i422_buffer.hpp
	source/video/i444_buffer.cpp
	source/video/i444_buffer.hpp
	source/video/nv12_buffer.cpp
	source/video/nv12_buffer.hpp
	source/video/recordable_encoded_frame.hpp
	source/video/render_resolution.hpp
	source/video/rgba_buffer.cpp
	source/video/rgba_buffer.hpp
	source/video/video_adaptation_counters.cpp
	source/video/video_adaptation_counters.hpp
	source/video/video_adaptation_reason.hpp
	source/video/video_adapter.cpp
	source/video/video_adapter.hpp
#	source/video/video_bitrate_allocation.cpp
#	source/video/video_bitrate_allocation.hpp
#	source/video/video_bitrate_allocator.cpp
#	source/video/video_bitrate_allocator.hpp
#	source/video/video_bitrate_allocator_factory.hpp
	source/video/video_broadcaster.cpp
	source/video/video_broadcaster.hpp
	source/video/video_codec_types.hpp
	source/video/video_content_type.cpp
	source/video/video_content_type.hpp
	source/video/video_filter_settings.hpp
	source/video/video_frame.cpp
	source/video/video_frame.hpp
	source/video/video_frame_buffer.cpp
	source/video/video_frame_buffer.hpp
	source/video/video_frame_buffer_pool.cpp
	source/video/video_frame_buffer_pool.hpp
#	source/video/video_frame_metadata.cpp
#	source/video/video_frame_metadata.hpp
	source/video/video_frame_type.hpp
	source/video/video_rotation.hpp
	source/video/video_sink_interface.hpp
	source/video/video_source_base.cpp
	source/video/video_source_base.hpp
	source/video/video_source_interface.cpp
	source/video/video_source_interface.hpp
	source/video/video_timing.cpp
	source/video/video_timing.hpp
	source/video/video_track_source.cpp
	source/video/video_track_source.hpp
	source/video/video_track_source_constraints.hpp
    source/video/video_type.cpp
	source/video/video_type.hpp
	source/video/yuv.cpp
	source/video/yuv.hpp
	source/media.cpp
	source/media.h
	source/media.hpp
	source/media_constants.cpp
	source/media_constants.hpp
	source/media_constants_p.hpp
	source/media_global.hpp
	source/media_p.hpp
	source/media_types.cpp
	source/media_types.hpp
	PRECOMPILED_HEADER
	source/media_pch.hpp
	LIBRARIES
	OpenCTKWrapLibyuv::WrapLibyuv
#	OpenCTKWrapOpenh264::WrapOpenh264
	${OCTK_LIB_LINK_LIBRARIES}
	PUBLIC_LIBRARIES
	OpenCTK::Network)
#if(OCTK_BUILD_TESTS)
#	add_subdirectory(source/test)
#	add_subdirectory(source/codecs/video/test)
#endif()
octk_add_subdirectory(source/capture/camera OCTK_FEATURE_MEDIA_ENABLE_CAPTURE_CAMERA)
octk_add_subdirectory(source/capture/portal OCTK_SYSTEM_LINUX)
add_subdirectory(source/capture/custom)
add_subdirectory(source/protocols/rtc)


#if(OCTK_FEATURE_MEDIA_USE_FFMPEG)
#	octk_find_package(WrapFFmpeg PROVIDED_TARGETS OpenCTKWrapFFmpeg::WrapFFmpeg)
#	octk_internal_extend_target(OpenCTKMedia
#		#		SOURCES
#		LIBRARIES
#		OpenCTKWrapFFmpeg::WrapFFmpeg)
#	octk_internal_extend_target(OpenCTKMedia # for static ffmpeg
#		CONDITION OCTK_SYSTEM_LINUX
#		LINK_OPTIONS "-Wl,-Bsymbolic")
#	octk_internal_extend_target(OpenCTKMedia # for static ffmpeg
#		CONDITION OCTK_SYSTEM_DARWIN
#		LINK_OPTIONS "-Wl,-w")
#endif()
#if(OCTK_FEATURE_MEDIA_USE_LIBSRTP)
#	octk_find_package(WrapLibsrtp PROVIDED_TARGETS OpenCTKWrapLibsrtp::WrapLibsrtp)
#	octk_internal_extend_target(OpenCTKMedia
#		LIBRARIES
#		OpenCTKWrapLibsrtp::WrapLibsrtp)
#endif()


#-----------------------------------------------------------------------------------------------------------------------
# Add lib examples and tests
#-----------------------------------------------------------------------------------------------------------------------
octk_add_subdirectory(examples OCTK_BUILD_EXAMPLES)
octk_add_subdirectory(tests OCTK_BUILD_TESTS)