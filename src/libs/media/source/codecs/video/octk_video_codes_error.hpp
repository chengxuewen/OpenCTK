/*
 *  Copyright (c) 2011 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef _OCTK_VIDEO_CODES_ERROR_HPP
#define _OCTK_VIDEO_CODES_ERROR_HPP

#include <octk_media_global.hpp>

#define WEBRTC_VIDEO_CODEC_TARGET_BITRATE_OVERSHOOT 5
#define WEBRTC_VIDEO_CODEC_OK_REQUEST_KEYFRAME 4
#define WEBRTC_VIDEO_CODEC_NO_OUTPUT 1
#define WEBRTC_VIDEO_CODEC_OK 0
#define WEBRTC_VIDEO_CODEC_ERROR -1
#define WEBRTC_VIDEO_CODEC_MEMORY -3
#define WEBRTC_VIDEO_CODEC_ERR_PARAMETER -4
#define WEBRTC_VIDEO_CODEC_TIMEOUT -6
#define WEBRTC_VIDEO_CODEC_UNINITIALIZED -7
#define WEBRTC_VIDEO_CODEC_FALLBACK_SOFTWARE -13
#define WEBRTC_VIDEO_CODEC_ERR_SIMULCAST_PARAMETERS_NOT_SUPPORTED -15
#define WEBRTC_VIDEO_CODEC_ENCODER_FAILURE -16

OCTK_BEGIN_NAMESPACE

const char *WebRtcVideoCodecErrorToString(int32_t error_code);

OCTK_END_NAMESPACE

#endif  // _OCTK_VIDEO_CODES_ERROR_HPP
