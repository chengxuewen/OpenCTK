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

#include <octk_scalability_mode.hpp>
#include <octk_string_view.hpp>
#include <octk_video_codec.hpp>
#include <octk_optional.hpp>

OCTK_BEGIN_NAMESPACE

enum class ScalabilityModeResolutionRatio
{
    kTwoToOne,   // The resolution ratio between spatial layers is 2:1.
    kThreeToTwo, // The resolution ratio between spatial layers is 1.5:1.
};

static constexpr char kDefaultScalabilityModeStr[] = "L1T2";

// Scalability mode to be used if falling back to default scalability mode is
// unsupported.
static constexpr char kNoLayeringScalabilityModeStr[] = "L1T1";

OCTK_MEDIA_API Optional<ScalabilityMode> MakeScalabilityMode(int num_spatial_layers,
                                                             int num_temporal_layers,
                                                             InterLayerPredMode inter_layer_pred,
                                                             Optional<ScalabilityModeResolutionRatio> ratio,
                                                             bool shift);

OCTK_MEDIA_API Optional<ScalabilityMode> ScalabilityModeFromString(StringView scalability_mode_string);

InterLayerPredMode ScalabilityModeToInterLayerPredMode(ScalabilityMode scalability_mode);

int ScalabilityModeToNumSpatialLayers(ScalabilityMode scalability_mode);

int ScalabilityModeToNumTemporalLayers(ScalabilityMode scalability_mode);

Optional<ScalabilityModeResolutionRatio> ScalabilityModeToResolutionRatio(ScalabilityMode scalability_mode);

bool ScalabilityModeIsShiftMode(ScalabilityMode scalability_mode);

ScalabilityMode LimitNumSpatialLayers(ScalabilityMode scalability_mode, int max_spatial_layers);

OCTK_END_NAMESPACE