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
	${ROOT_DIR}/octk_rtc_audio_device.hpp
	${ROOT_DIR}/octk_rtc_audio_frame.hpp
	${ROOT_DIR}/octk_rtc_audio_processor.hpp
	${ROOT_DIR}/octk_rtc_audio_track.hpp
	${ROOT_DIR}/octk_rtc_configuration.hpp
	${ROOT_DIR}/octk_rtc_data_channel.hpp
	${ROOT_DIR}/octk_rtc_dtls_transport.hpp
	${ROOT_DIR}/octk_rtc_dtmf_sender.hpp
	${ROOT_DIR}/octk_rtc_engine.cpp
	${ROOT_DIR}/octk_rtc_engine.hpp
	${ROOT_DIR}/octk_rtc_ice_candidate.hpp
	${ROOT_DIR}/octk_rtc_ice_transport.hpp
	${ROOT_DIR}/octk_rtc_media_constraints.cpp
	${ROOT_DIR}/octk_rtc_media_constraints.hpp
	${ROOT_DIR}/octk_rtc_media_stream.hpp
	${ROOT_DIR}/octk_rtc_media_track.hpp
	${ROOT_DIR}/octk_rtc_peerconnection.cpp
	${ROOT_DIR}/octk_rtc_peerconnection.hpp
	${ROOT_DIR}/octk_rtc_peerconnection_factory.hpp
	${ROOT_DIR}/octk_rtc_rtp_capabilities.hpp
	${ROOT_DIR}/octk_rtc_rtp_parameters.hpp
	${ROOT_DIR}/octk_rtc_rtp_receiver.hpp
	${ROOT_DIR}/octk_rtc_rtp_sender.hpp
	${ROOT_DIR}/octk_rtc_rtp_transceiver.hpp
	${ROOT_DIR}/octk_rtc_session_description.cpp
	${ROOT_DIR}/octk_rtc_session_description.hpp
	${ROOT_DIR}/octk_rtc_stats.hpp
	${ROOT_DIR}/octk_rtc_stats_report.cpp
	${ROOT_DIR}/octk_rtc_stats_report.hpp
	${ROOT_DIR}/octk_rtc_types.hpp
	${ROOT_DIR}/octk_rtc_video_codec_factory.hpp
	${ROOT_DIR}/octk_rtc_video_decoder.hpp
	${ROOT_DIR}/octk_rtc_video_device.hpp
	${ROOT_DIR}/octk_rtc_video_encoder.cpp
	${ROOT_DIR}/octk_rtc_video_encoder.hpp
	${ROOT_DIR}/octk_rtc_video_frame.cpp
	${ROOT_DIR}/octk_rtc_video_frame.hpp
	${ROOT_DIR}/octk_rtc_video_track.hpp)
octk_add_subdirectory(${ROOT_DIR}/backends/webrtc OCTK_FEATURE_MEDIA_USE_WEBRTC)