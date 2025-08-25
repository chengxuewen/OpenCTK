/*
 *  Copyright (c) 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include <octk_video_codec_interface.hpp>

OCTK_BEGIN_NAMESPACE

CodecSpecificInfo::CodecSpecificInfo() : codecType(kVideoCodecGeneric)
{
    memset(&codecSpecific, 0, sizeof(codecSpecific));
}

CodecSpecificInfo::CodecSpecificInfo(const CodecSpecificInfo &) = default;
CodecSpecificInfo::~CodecSpecificInfo() = default;

OCTK_END_NAMESPACE
