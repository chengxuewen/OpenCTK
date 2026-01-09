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
octk_find_package(WrapLibyuv PROVIDED_TARGETS OCTK3rdparty::WrapLibyuv)


#-----------------------------------------------------------------------------------------------------------------------
# Add lib target
#-----------------------------------------------------------------------------------------------------------------------
octk_add_library(media
	EXCEPTIONS
	SOURCES
	source/codecs/video/octk_av1_profile.cpp
	source/codecs/video/octk_av1_profile.hpp
	source/codecs/video/octk_codecs_h264_types.hpp
	source/codecs/video/octk_codecs_vp8_types.hpp
	source/codecs/video/octk_codecs_vp9_types.hpp
	source/codecs/video/octk_h264_profile.cpp
	source/codecs/video/octk_h264_profile.hpp
	source/codecs/video/octk_scalability_mode.cpp
	source/codecs/video/octk_scalability_mode.hpp
	source/codecs/video/octk_scalability_mode_helper.cpp
	source/codecs/video/octk_scalability_mode_helper.hpp
	source/codecs/video/octk_scalability_mode_utils.cpp
	source/codecs/video/octk_scalability_mode_utils.hpp
	source/codecs/video/octk_sdp_video_format.cpp
	source/codecs/video/octk_sdp_video_format.hpp
	source/codecs/video/octk_simulcast_stream.cpp
	source/codecs/video/octk_simulcast_stream.hpp
	source/codecs/video/octk_video_codec.cpp
	source/codecs/video/octk_video_codec.hpp
	source/codecs/video/octk_video_codec_constants.hpp
	source/codecs/video/octk_video_codec_interface.cpp
	source/codecs/video/octk_video_codec_interface.hpp
	source/codecs/video/octk_video_codes_error.cpp
	source/codecs/video/octk_video_codes_error.hpp
	source/codecs/video/octk_video_encoder.cpp
	source/codecs/video/octk_video_encoder.hpp
	source/codecs/video/octk_video_encoder_factory.hpp
	source/codecs/video/octk_vp9_profile.cpp
	source/codecs/video/octk_vp9_profile.hpp
	source/codecs/octk_codecs_constants.hpp
	source/experiments/octk_balanced_degradation_settings.cpp
	source/experiments/octk_balanced_degradation_settings.hpp
	source/experiments/octk_field_trial_list.cpp
	source/experiments/octk_field_trial_list.hpp
	source/experiments/octk_field_trial_parser.cpp
	source/experiments/octk_field_trial_parser.hpp
	source/experiments/octk_field_trials.cpp
	source/experiments/octk_field_trials.hpp
	source/experiments/octk_field_trials_registry.cpp
	source/experiments/octk_field_trials_registry.hpp
	source/experiments/octk_field_trials_view.hpp
	source/rtc/adaptation/octk_adaptation_constraint.cpp
	source/rtc/adaptation/octk_adaptation_constraint.hpp
	source/rtc/adaptation/octk_broadcast_resource_listener.cpp
	source/rtc/adaptation/octk_broadcast_resource_listener.hpp
	source/rtc/adaptation/octk_resource.cpp
	source/rtc/adaptation/octk_resource.hpp
	source/rtc/adaptation/octk_video_source_restrictions.cpp
	source/rtc/adaptation/octk_video_source_restrictions.hpp
	source/rtc/adaptation/octk_video_stream_input_state.cpp
	source/rtc/adaptation/octk_video_stream_input_state.hpp
	source/rtc/octk_dtls_transport_interface.cpp
	source/rtc/octk_dtls_transport_interface.hpp
	source/rtc/stats/octk_rtc_stats.cpp
	source/rtc/stats/octk_rtc_stats.hpp
	source/rtc/stats/octk_rtc_stats_collector.hpp
	source/rtc/stats/octk_rtc_stats_objects.cpp
	source/rtc/stats/octk_rtc_stats_objects.hpp
	source/rtc/stats/octk_rtc_stats_report.cpp
	source/rtc/stats/octk_rtc_stats_report.hpp
	source/rtc/octk_rtc_context.cpp
	source/rtc/octk_rtc_context.hpp
	source/rtc/octk_rtc_error.cpp
	source/rtc/octk_rtc_error.hpp
	source/rtc/octk_rtc_event.cpp
	source/rtc/octk_rtc_event.hpp
	source/rtc/octk_rtc_event_log.hpp
	source/rtc/octk_rtc_event_log_output.hpp
	source/rtc/octk_rtc_offer_answer_options.hpp
	source/rtc/octk_rtc_peer_connection.cpp
	source/rtc/octk_rtc_peer_connection.hpp
	source/rtc/octk_rtc_rtp_transceiver.cpp
	source/rtc/octk_rtc_rtp_transceiver.hpp
	source/rtp/octk_dependency_descriptor.cpp
	source/rtp/octk_dependency_descriptor.cpp
	source/rtp/octk_rtp_headers.cpp
	source/rtp/octk_rtp_headers.hpp
	source/rtp/octk_rtp_packet_info.cpp
	source/rtp/octk_rtp_packet_info.hpp
	source/rtp/octk_rtp_packet_infos.hpp
	source/rtp/octk_rtp_parameters.cpp
	source/rtp/octk_rtp_parameters.hpp
	source/rtp/octk_rtp_receiver_interface.cpp
	source/rtp/octk_rtp_receiver_interface.hpp
	source/rtp/octk_rtp_sender_interface.cpp
	source/rtp/octk_rtp_sender_interface.hpp
	source/rtp/octk_rtp_source.hpp
	source/rtp/octk_rtp_transceiver_direction.hpp
	source/rtp/octk_rtp_transceiver_interface.cpp
	source/rtp/octk_rtp_transceiver_interface.hpp
	source/rtp/octk_rtp_video_header.cpp
	source/rtp/octk_rtp_video_header.hpp
#	source/test/octk_simulated_task_queue.cpp
#	source/test/octk_simulated_task_queue.hpp
#	source/test/octk_simulated_thread.cpp
#	source/test/octk_simulated_thread.hpp
#	source/test/octk_simulated_time_controller.cpp
#	source/test/octk_simulated_time_controller.hpp
	source/video/octk_color_space.cpp
	source/video/octk_color_space.hpp
	source/video/octk_encoded_frame.cpp
	source/video/octk_encoded_frame.hpp
	source/video/octk_encoded_image.cpp
	source/video/octk_encoded_image.hpp
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
	source/video/octk_video_frame_metadata.cpp
	source/video/octk_video_frame_metadata.hpp
	source/video/octk_video_frame_type.hpp
	source/video/octk_video_resolution.hpp
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
	source/octk_media_global.hpp
	source/octk_media_p.hpp
	source/octk_media_stream_interface.cpp
	source/octk_media_stream_interface.hpp
	source/octk_media_stream_track.hpp
	source/octk_media_types.cpp
	source/octk_media_types.hpp
	PRECOMPILED_HEADER
	source/octk_media_pch.hpp
	LIBRARIES
	OCTK3rdparty::WrapLibyuv
	${OCTK_LIB_LINK_LIBRARIES}
	PUBLIC_LIBRARIES
	octk::network)
add_subdirectory(source/capture/custom)
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
