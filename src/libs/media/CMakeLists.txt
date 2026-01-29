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
octk_find_package(WrapOpenh264 PROVIDED_TARGETS OCTK3rdparty::WrapOpenh264)
octk_find_package(WrapLibyuv PROVIDED_TARGETS OCTK3rdparty::WrapLibyuv)


#-----------------------------------------------------------------------------------------------------------------------
# Add lib target
#-----------------------------------------------------------------------------------------------------------------------
octk_add_library(media
	EXCEPTIONS
	SOURCES
	source/codecs/video/backends/h264/octk_video_decoder_openh264.cpp
	source/codecs/video/backends/h264/octk_video_decoder_openh264_p.hpp
	source/codecs/video/backends/h264/octk_video_encoder_openh264.cpp
	source/codecs/video/backends/h264/octk_video_encoder_openh264_p.hpp
	source/codecs/video/formats/av1/octk_av1_profile.cpp
	source/codecs/video/formats/av1/octk_av1_profile.hpp
	source/codecs/video/formats/av1/octk_av1_svc_config.cpp
	source/codecs/video/formats/av1/octk_av1_svc_config.hpp
	source/codecs/video/formats/h264/octk_h264_bitstream_parser.cpp
	source/codecs/video/formats/h264/octk_h264_bitstream_parser_p.hpp
	source/codecs/video/formats/h264/octk_h264_codecs.cpp
	source/codecs/video/formats/h264/octk_h264_codecs.hpp
	source/codecs/video/formats/h264/octk_h264_common.cpp
	source/codecs/video/formats/h264/octk_h264_common_p.hpp
	source/codecs/video/formats/h264/octk_h264_pps_parser.cpp
	source/codecs/video/formats/h264/octk_h264_pps_parser_p.hpp
	source/codecs/video/formats/h264/octk_h264_profile.cpp
	source/codecs/video/formats/h264/octk_h264_profile.hpp
	source/codecs/video/formats/h264/octk_h264_sps_parser.cpp
	source/codecs/video/formats/h264/octk_h264_sps_parser_p.hpp
	source/codecs/video/formats/h264/octk_h264_sps_vui_rewriter.cpp
	source/codecs/video/formats/h264/octk_h264_sps_vui_rewriter_p.hpp
	source/codecs/video/formats/h264/octk_h264_types.hpp
	source/codecs/video/formats/vp8/octk_vp8_constants.hpp
	source/codecs/video/formats/vp8/octk_vp8_header_parser.cpp
	source/codecs/video/formats/vp8/octk_vp8_header_parser.hpp
	source/codecs/video/formats/vp8/octk_vp8_types.hpp
	source/codecs/video/formats/vp9/octk_vp9_constants.hpp
	source/codecs/video/formats/vp9/octk_vp9_profile.cpp
	source/codecs/video/formats/vp9/octk_vp9_profile.hpp
	source/codecs/video/formats/vp9/octk_vp9_svc_config.cpp
	source/codecs/video/formats/vp9/octk_vp9_svc_config.hpp
	source/codecs/video/formats/vp9/octk_vp9_types.hpp
	source/codecs/video/formats/vp9/octk_vp9_uncompressed_header_parser.cpp
	source/codecs/video/formats/vp9/octk_vp9_uncompressed_header_parser_p.hpp
	source/codecs/video/svc/octk_scalability_mode.cpp
	source/codecs/video/svc/octk_scalability_mode.hpp
	source/codecs/video/svc/octk_scalability_mode_helper.cpp
	source/codecs/video/svc/octk_scalability_mode_helper_p.hpp
	source/codecs/video/svc/octk_scalability_mode_utils.cpp
	source/codecs/video/svc/octk_scalability_mode_utils_p.hpp
	source/codecs/video/svc/octk_scalability_structure.cpp
	source/codecs/video/svc/octk_scalability_structure_full_svc.cpp
	source/codecs/video/svc/octk_scalability_structure_full_svc_p.hpp
	source/codecs/video/svc/octk_scalability_structure_key_svc.cpp
	source/codecs/video/svc/octk_scalability_structure_key_svc_p.hpp
	source/codecs/video/svc/octk_scalability_structure_l2t2_key_shift.cpp
	source/codecs/video/svc/octk_scalability_structure_l2t2_key_shift_p.hpp
	source/codecs/video/svc/octk_scalability_structure_p.hpp
	source/codecs/video/svc/octk_scalability_structure_simulcast.cpp
	source/codecs/video/svc/octk_scalability_structure_simulcast_p.hpp
	source/codecs/video/svc/octk_scalable_video_controller_no_layering.cpp
	source/codecs/video/svc/octk_scalable_video_controller_no_layering_p.hpp
	source/codecs/video/svc/octk_scalable_video_controller_p.hpp
	source/codecs/video/svc/octk_simulcast_to_svc_converter.cpp
	source/codecs/video/svc/octk_simulcast_to_svc_converter_p.hpp
	source/codecs/video/svc/octk_svc_rate_allocator.cpp
	source/codecs/video/svc/octk_svc_rate_allocator_p.hpp
#	source/codecs/video/utility/octk_ivf_defines_p.hpp
#	source/codecs/video/utility/octk_ivf_file_reader.cpp
#	source/codecs/video/utility/octk_ivf_file_reader_p.hpp
#	source/codecs/video/utility/octk_ivf_file_writer.cpp
#	source/codecs/video/utility/octk_ivf_file_writer_p.hpp
	source/codecs/video/utility/octk_simulcast_rate_allocator.cpp
	source/codecs/video/utility/octk_simulcast_rate_allocator.hpp
	source/codecs/video/utility/octk_simulcast_stream.cpp
	source/codecs/video/utility/octk_simulcast_stream.hpp
	source/codecs/video/utility/octk_simulcast_utility.cpp
	source/codecs/video/utility/octk_simulcast_utility_p.hpp
	source/codecs/video/octk_chain_diff_calculator.cpp
	source/codecs/video/octk_chain_diff_calculator_p.hpp
	source/codecs/video/octk_frame_dependencies_calculator.cpp
	source/codecs/video/octk_frame_dependencies_calculator_p.hpp
	source/codecs/video/octk_sdp_video_format.cpp
	source/codecs/video/octk_sdp_video_format.hpp
	source/codecs/video/octk_video_bitstream_parser_p.hpp
	source/codecs/video/octk_video_codec.cpp
	source/codecs/video/octk_video_codec.hpp
	source/codecs/video/octk_video_codec_constants.hpp
	source/codecs/video/octk_video_codes_error.cpp
	source/codecs/video/octk_video_codes_error.hpp
	source/codecs/video/octk_video_decoder.cpp
	source/codecs/video/octk_video_decoder.hpp
	source/codecs/video/octk_video_decoder_factory.hpp
	source/codecs/video/octk_video_encoder.cpp
	source/codecs/video/octk_video_encoder.hpp
	source/codecs/video/octk_video_encoder_factory.hpp
	source/codecs/octk_codec_specific_info.cpp
	source/codecs/octk_codec_specific_info.hpp
	source/codecs/octk_codecs_constants.hpp
	source/codecs/octk_dependency_descriptor.cpp
	source/codecs/octk_dependency_descriptor_p.hpp
	source/event_log/octk_media_event.cpp
	source/event_log/octk_media_event.hpp
	source/event_log/octk_media_event_log.cpp
	source/event_log/octk_media_event_log.hpp
	source/event_log/octk_media_event_log_factory.cpp
	source/event_log/octk_media_event_log_factory.hpp
	source/experiments/octk_balanced_degradation_settings.cpp
	source/experiments/octk_balanced_degradation_settings_p.hpp
	source/experiments/octk_field_trial_list.cpp
	source/experiments/octk_field_trial_list_p.hpp
	source/experiments/octk_field_trial_parser.cpp
	source/experiments/octk_field_trial_parser_p.hpp
	source/experiments/octk_field_trial_units.cpp
	source/experiments/octk_field_trial_units_p.hpp
	source/experiments/octk_field_trials.cpp
	source/experiments/octk_field_trials_p.hpp
	source/experiments/octk_field_trials_registry.cpp
	source/experiments/octk_field_trials_registry_p.hpp
	source/experiments/octk_field_trials_view.cpp
	source/experiments/octk_field_trials_view.hpp
	source/experiments/octk_rate_control_settings.cpp
	source/experiments/octk_rate_control_settings_p.hpp
	source/experiments/octk_stable_target_rate_experiment.cpp
	source/experiments/octk_stable_target_rate_experiment_p.hpp
	source/experiments/octk_struct_parameters_parser.cpp
	source/experiments/octk_struct_parameters_parser_p.hpp
	source/media_context/octk_media_context_factory.cpp
	source/media_context/octk_media_context_factory.hpp
	source/media_context/octk_media_context.hpp
	source/protocols/rtc/octk_rtc_audio_device.hpp
	source/protocols/rtc/octk_rtc_audio_frame.hpp
	source/protocols/rtc/octk_rtc_audio_source.hpp
	source/protocols/rtc/octk_rtc_audio_track.hpp
	source/protocols/rtc/octk_rtc_data_channel.hpp
	source/protocols/rtc/octk_rtc_dtls_transport.hpp
	source/protocols/rtc/octk_rtc_dtmf_sender.hpp
	source/protocols/rtc/octk_rtc_ice_candidate.hpp
	source/protocols/rtc/octk_rtc_ice_transport.hpp
	source/protocols/rtc/octk_rtc_media_constraints.cpp
	source/protocols/rtc/octk_rtc_media_constraints.hpp
	source/protocols/rtc/octk_rtc_media_stream.hpp
	source/protocols/rtc/octk_rtc_media_track.hpp
	source/protocols/rtc/octk_rtc_peerconnection.hpp
	source/protocols/rtc/octk_rtc_peerconnection_factory.hpp
	source/protocols/rtc/octk_rtc_rtp_capabilities.hpp
	source/protocols/rtc/octk_rtc_rtp_parameters.hpp
	source/protocols/rtc/octk_rtc_rtp_receiver.hpp
	source/protocols/rtc/octk_rtc_rtp_sender.hpp
	source/protocols/rtc/octk_rtc_rtp_transceiver.hpp
	source/protocols/rtc/octk_rtc_session_description.hpp
	source/protocols/rtc/octk_rtc_stats.hpp
	source/protocols/rtc/octk_rtc_stats_report.cpp
	source/protocols/rtc/octk_rtc_stats_report.hpp
	source/protocols/rtc/octk_rtc_types.hpp
	source/protocols/rtc/octk_rtc_video_codec_factory.hpp
	source/protocols/rtc/octk_rtc_video_decoder.hpp
	source/protocols/rtc/octk_rtc_video_device.hpp
	source/protocols/rtc/octk_rtc_video_encoder.cpp
	source/protocols/rtc/octk_rtc_video_encoder.hpp
	source/protocols/rtc/octk_rtc_video_frame.hpp
	source/protocols/rtc/octk_rtc_video_sink.hpp
	source/protocols/rtc/octk_rtc_video_source.hpp
	source/protocols/rtc/octk_rtc_video_track.hpp
	source/protocols/rtp/octk_rtp_headers.cpp
	source/protocols/rtp/octk_rtp_headers.hpp
	source/protocols/rtp/octk_rtp_packet_info.cpp
	source/protocols/rtp/octk_rtp_packet_info.hpp
	source/protocols/rtp/octk_rtp_packet_infos.hpp
	source/protocols/rtp/octk_rtp_video_header.cpp
	source/protocols/rtp/octk_rtp_video_header.hpp
	#	source/rtc/adaptation/octk_adaptation_constraint.cpp
#	source/rtc/adaptation/octk_adaptation_constraint.hpp
#	source/rtc/adaptation/octk_broadcast_resource_listener.cpp
#	source/rtc/adaptation/octk_broadcast_resource_listener.hpp
#	source/rtc/adaptation/octk_resource.cpp
#	source/rtc/adaptation/octk_resource.hpp
#	source/rtc/adaptation/octk_video_source_restrictions.cpp
#	source/rtc/adaptation/octk_video_source_restrictions.hpp
#	source/rtc/adaptation/octk_video_stream_input_state.cpp
#	source/rtc/adaptation/octk_video_stream_input_state.hpp
#	source/rtc/octk_dtls_transport_interface.cpp
#	source/rtc/octk_dtls_transport_interface.hpp
#	source/rtc/stats/octk_rtc_stats.cpp
#	source/rtc/stats/octk_rtc_stats.hpp
#	source/rtc/stats/octk_rtc_stats_collector.hpp
#	source/rtc/stats/octk_rtc_stats_objects.cpp
#	source/rtc/stats/octk_rtc_stats_objects.hpp
#	source/rtc/stats/octk_rtc_stats_report.cpp
#	source/rtc/stats/octk_rtc_stats_report.hpp
#	source/rtc/octk_rtc_error.cpp
#	source/rtc/octk_rtc_error.hpp
#	source/rtc/octk_rtc_offer_answer_options.hpp
#	source/rtc/octk_rtc_peer_connection.cpp
#	source/rtc/octk_rtc_peer_connection.hpp
#	source/rtc/octk_rtc_rtp_transceiver.cpp
#	source/rtc/octk_rtc_rtp_transceiver.hpp
#	source/rtp/octk_dependency_descriptor.cpp
#	source/rtp/octk_dependency_descriptor.cpp
#	source/rtp/octk_rtp_headers.cpp
#	source/rtp/octk_rtp_headers.hpp
#	source/rtp/octk_rtp_packet_info.cpp
#	source/rtp/octk_rtp_packet_info.hpp
#	source/rtp/octk_rtp_packet_infos.hpp
#	source/rtp/octk_rtp_parameters.cpp
#	source/rtp/octk_rtp_parameters.hpp
#	source/rtp/octk_rtp_receiver_interface.cpp
#	source/rtp/octk_rtp_receiver_interface.hpp
#	source/rtp/octk_rtp_sender_interface.cpp
#	source/rtp/octk_rtp_sender_interface.hpp
#	source/rtp/octk_rtp_source.hpp
#	source/rtp/octk_rtp_transceiver_direction.hpp
#	source/rtp/octk_rtp_transceiver_interface.cpp
#	source/rtp/octk_rtp_transceiver_interface.hpp
#	source/rtp/octk_rtp_video_header.cpp
#	source/rtp/octk_rtp_video_header.hpp
	source/stream/octk_media_stream_interface.cpp
	source/stream/octk_media_stream_interface.hpp
	source/stream/octk_media_stream_track.hpp
	source/video/octk_color_space.cpp
	source/video/octk_color_space.hpp
	source/video/octk_encoded_frame.cpp
	source/video/octk_encoded_frame.hpp
	source/video/octk_encoded_image.cpp
	source/video/octk_encoded_image.hpp
	source/video/octk_frame_instrumentation_data.hpp
	source/video/octk_frame_utils.cpp
	source/video/octk_frame_utils.hpp
	source/video/octk_framerate_controller.cpp
	source/video/octk_framerate_controller.hpp
	source/video/octk_generic_frame_info.cpp
	source/video/octk_generic_frame_info.hpp
	source/video/octk_hdr_metadata.cpp
	source/video/octk_hdr_metadata.hpp
	source/video/octk_i010_buffer.cpp
	source/video/octk_i010_buffer.hpp
	source/video/octk_i210_buffer.cpp
	source/video/octk_i210_buffer.hpp
	source/video/octk_i410_buffer.cpp
	source/video/octk_i410_buffer.hpp
	source/video/octk_i420_buffer.cpp
	source/video/octk_i420_buffer.hpp
	source/video/octk_i422_buffer.cpp
	source/video/octk_i422_buffer.hpp
	source/video/octk_i444_buffer.cpp
	source/video/octk_i444_buffer.hpp
	source/video/octk_nv12_buffer.cpp
	source/video/octk_nv12_buffer.hpp
	source/video/octk_recordable_encoded_frame.hpp
	source/video/octk_render_resolution.hpp
	source/video/octk_rgba_buffer.cpp
	source/video/octk_rgba_buffer.hpp
	source/video/octk_video_adaptation_counters.cpp
	source/video/octk_video_adaptation_counters.hpp
	source/video/octk_video_adaptation_reason.hpp
	source/video/octk_video_adapter.cpp
	source/video/octk_video_adapter.hpp
	source/video/octk_video_bitrate_allocation.cpp
	source/video/octk_video_bitrate_allocation.hpp
	source/video/octk_video_bitrate_allocator.cpp
	source/video/octk_video_bitrate_allocator.hpp
	source/video/octk_video_bitrate_allocator_factory.hpp
	source/video/octk_video_broadcaster.cpp
	source/video/octk_video_broadcaster.hpp
	source/video/octk_video_codec_types.hpp
	source/video/octk_video_content_type.cpp
	source/video/octk_video_content_type.hpp
	source/video/octk_video_filter_settings.hpp
	source/video/octk_video_frame.cpp
	source/video/octk_video_frame.hpp
	source/video/octk_video_frame_buffer.cpp
	source/video/octk_video_frame_buffer.hpp
	source/video/octk_video_frame_buffer_pool.cpp
	source/video/octk_video_frame_buffer_pool.hpp
	source/video/octk_video_frame_metadata.cpp
	source/video/octk_video_frame_metadata.hpp
	source/video/octk_video_frame_type.hpp
	source/video/octk_video_rotation.hpp
	source/video/octk_video_sink_interface.hpp
	source/video/octk_video_source_base.cpp
	source/video/octk_video_source_base.hpp
	source/video/octk_video_source_interface.cpp
	source/video/octk_video_source_interface.hpp
	source/video/octk_video_timing.cpp
	source/video/octk_video_timing.hpp
	source/video/octk_video_track_source.cpp
	source/video/octk_video_track_source.hpp
	source/video/octk_video_track_source_constraints.hpp
    source/video/octk_video_type.cpp
	source/video/octk_video_type.hpp
	source/video/octk_yuv.cpp
	source/video/octk_yuv.hpp
	source/octk_media.cpp
	source/octk_media.h
	source/octk_media.hpp
	source/octk_media_constants.cpp
	source/octk_media_constants.hpp
	source/octk_media_constants_p.hpp
	source/octk_media_global.hpp
	source/octk_media_p.hpp
	source/octk_media_types.cpp
	source/octk_media_types.hpp
	PRECOMPILED_HEADER
	source/octk_media_pch.hpp
	LIBRARIES
	OCTK3rdparty::WrapLibyuv
	OCTK3rdparty::WrapOpenh264
	${OCTK_LIB_LINK_LIBRARIES}
	PUBLIC_LIBRARIES
	octk::network)
add_subdirectory(source/capture/custom)
if(OCTK_BUILD_TESTS)
	add_subdirectory(source/test)
	add_subdirectory(source/codecs/video/test)
endif()
if(OCTK_FEATURE_ENABLE_CAPTURE_DESKTOP)
	add_subdirectory(source/capture/desktop)
endif()
if(OCTK_FEATURE_ENABLE_CAPTURE_CAMERA)
    add_subdirectory(source/capture/camera)
endif()
if(OCTK_FEATURE_ENABLE_CAPTURE_VIDEO)
	add_subdirectory(source/capture/video)
endif()

if(OCTK_FEATURE_USE_FFMPEG)
	octk_find_package(WrapFFmpeg PROVIDED_TARGETS OCTK3rdparty::WrapFFmpeg)
	octk_internal_extend_target(octk_media
		#		SOURCES
		LIBRARIES
		OCTK3rdparty::WrapFFmpeg)
	octk_internal_extend_target(octk_media # for static ffmpeg
		CONDITION OCTK_SYSTEM_LINUX
		LINK_OPTIONS "-Wl,-Bsymbolic")
	octk_internal_extend_target(octk_media # for static ffmpeg
		CONDITION OCTK_SYSTEM_DARWIN
		LINK_OPTIONS "-Wl,-w")
endif()
if(OCTK_FEATURE_USE_ZLMEDIAKIT)
	octk_find_package(WrapZLMediaKit PROVIDED_TARGETS OCTK3rdparty::WrapZLMediaKit)
endif()
if(OCTK_FEATURE_USE_LIBSRTP)
	octk_find_package(WrapZLMediaKit PROVIDED_TARGETS OCTK3rdparty::WrapLibsrtp)
	octk_internal_extend_target(octk_media
		LIBRARIES
		OCTK3rdparty::WrapLibsrtp)
endif()


#-----------------------------------------------------------------------------------------------------------------------
# Add lib examples and tests
#-----------------------------------------------------------------------------------------------------------------------
octk_add_subdirectory(examples OCTK_BUILD_EXAMPLES)
octk_add_subdirectory(tests OCTK_BUILD_TESTS)