/*
 *  Copyright (c) 2011 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef _OCTK_VIDEO_CAPTURE_MAIN_SOURCE_VIDEO_CAPTURE_CONFIG_H_
#define _OCTK_VIDEO_CAPTURE_MAIN_SOURCE_VIDEO_CAPTURE_CONFIG_H_

namespace webrtc {
namespace videocapturemodule {
enum { kDefaultWidth = 640 };     // start width
enum { kDefaultHeight = 480 };    // start heigt
enum { kDefaultFrameRate = 30 };  // start frame rate

enum { kMaxFrameRate = 60 };  // Max allowed frame rate of the start image

enum { kDefaultCaptureDelay = 120 };
enum {
  kMaxCaptureDelay = 270
};  // Max capture delay allowed in the precompiled capture delay values.

enum { kFrameRateCallbackInterval = 1000 };
enum { kFrameRateCountHistorySize = 90 };
enum { kFrameRateHistoryWindowMs = 2000 };
}  // namespace videocapturemodule
}  // namespace webrtc

#endif  // _OCTK_VIDEO_CAPTURE_MAIN_SOURCE_VIDEO_CAPTURE_CONFIG_H_
