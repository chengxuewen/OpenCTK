/*
*  Copyright (c) 2022 The WebRTC project authors. All Rights Reserved.
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source
*  tree. An additional intellectual property rights grant can be found
*  in the file PATENTS.  All contributing project authors may
*  be found in the AUTHORS file in the root of the source tree.
*/

#pragma once

#include <octk_scalability_mode.hpp>
#include <octk_optional.hpp>

OCTK_BEGIN_NAMESPACE

// TODO(bugs.webrtc.org/6883): Unify with struct VideoStream, part of
// VideoEncoderConfig.
struct OCTK_MEDIA_API SimulcastStream
{
    // Temporary utility methods for transition from numberOfTemporalLayers
    // setting to ScalabilityMode.
    unsigned char GetNumberOfTemporalLayers() const;
    Optional<ScalabilityMode> GetScalabilityMode() const;
    void SetNumberOfTemporalLayers(unsigned char n);

    bool operator==(const SimulcastStream &other) const;
    bool operator!=(const SimulcastStream &other) const { return !(*this == other); }

    int width = 0;
    int height = 0;
    float maxFramerate = 0; // fps.
    unsigned char numberOfTemporalLayers = 1;
    unsigned int maxBitrate = 0;    // kilobits/sec.
    unsigned int targetBitrate = 0; // kilobits/sec.
    unsigned int minBitrate = 0;    // kilobits/sec.
    unsigned int qpMax = 0;         // minimum quality
    bool active = false;            // encoded and sent.
};

OCTK_END_NAMESPACE