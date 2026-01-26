/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
** Copyright (c) 2022 The WebRTC project authors. All Rights Reserved.
**
** License: MIT License
**
** Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
** documentation files (the "Software"), to deal in the Software without restriction, including without limitation
** the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
** and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in all copies or substantial portions
** of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
** TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
** CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
** IN THE SOFTWARE.
**
***********************************************************************************************************************/

#pragma once

#include <octk_string_view.hpp>
#include <octk_media_global.hpp>

#include <stddef.h>
#include <stdint.h>

OCTK_BEGIN_NAMESPACE

// Supported scalability modes. Most applications should use the
// PeerConnection-level apis where scalability mode is represented as a string.
// This list of currently recognized modes is intended for the api boundary
// between webrtc and injected encoders. Any application usage outside of
// injected encoders is strongly discouraged.
enum class ScalabilityMode : uint8_t
{
    kL1T1,
    kL1T2,
    kL1T3,
    kL2T1,
    kL2T1h,
    kL2T1_KEY,
    kL2T2,
    kL2T2h,
    kL2T2_KEY,
    kL2T2_KEY_SHIFT,
    kL2T3,
    kL2T3h,
    kL2T3_KEY,
    kL3T1,
    kL3T1h,
    kL3T1_KEY,
    kL3T2,
    kL3T2h,
    kL3T2_KEY,
    kL3T3,
    kL3T3h,
    kL3T3_KEY,
    kS2T1,
    kS2T1h,
    kS2T2,
    kS2T2h,
    kS2T3,
    kS2T3h,
    kS3T1,
    kS3T1h,
    kS3T2,
    kS3T2h,
    kS3T3,
    kS3T3h,
};

static const ScalabilityMode kAllScalabilityModes[] = {
    // clang-format off
    ScalabilityMode::kL1T1,
    ScalabilityMode::kL1T2,
    ScalabilityMode::kL1T3,
    ScalabilityMode::kL2T1,
    ScalabilityMode::kL2T1h,
    ScalabilityMode::kL2T1_KEY,
    ScalabilityMode::kL2T2,
    ScalabilityMode::kL2T2h,
    ScalabilityMode::kL2T2_KEY,
    ScalabilityMode::kL2T2_KEY_SHIFT,
    ScalabilityMode::kL2T3,
    ScalabilityMode::kL2T3h,
    ScalabilityMode::kL2T3_KEY,
    ScalabilityMode::kL3T1,
    ScalabilityMode::kL3T1h,
    ScalabilityMode::kL3T1_KEY,
    ScalabilityMode::kL3T2,
    ScalabilityMode::kL3T2h,
    ScalabilityMode::kL3T2_KEY,
    ScalabilityMode::kL3T3,
    ScalabilityMode::kL3T3h,
    ScalabilityMode::kL3T3_KEY,
    ScalabilityMode::kS2T1,
    ScalabilityMode::kS2T1h,
    ScalabilityMode::kS2T2,
    ScalabilityMode::kS2T2h,
    ScalabilityMode::kS2T3,
    ScalabilityMode::kS2T3h,
    ScalabilityMode::kS3T1,
    ScalabilityMode::kS3T1h,
    ScalabilityMode::kS3T2,
    ScalabilityMode::kS3T2h,
    ScalabilityMode::kS3T3,
    ScalabilityMode::kS3T3h,
    // clang-format on
};
static const size_t kScalabilityModeCount = sizeof(kAllScalabilityModes) / sizeof(ScalabilityMode);

OCTK_MEDIA_API StringView ScalabilityModeToString(ScalabilityMode scalability_mode);

OCTK_END_NAMESPACE
