/*
 *  Copyright (c) 2022 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <octk_scalability_mode_helper.hpp>
#include <octk_scalability_mode_utils.hpp>

OCTK_BEGIN_NAMESPACE

Optional<int> ScalabilityModeStringToNumSpatialLayers(StringView scalability_mode_string)
{
    Optional<ScalabilityMode> scalability_mode = ScalabilityModeFromString(scalability_mode_string);
    if (!scalability_mode.has_value())
    {
        return utils::nullopt;
    }
    return ScalabilityModeToNumSpatialLayers(*scalability_mode);
}

Optional<int> ScalabilityModeStringToNumTemporalLayers(StringView scalability_mode_string)
{
    Optional<ScalabilityMode> scalability_mode = ScalabilityModeFromString(scalability_mode_string);
    if (!scalability_mode.has_value())
    {
        return utils::nullopt;
    }
    return ScalabilityModeToNumTemporalLayers(*scalability_mode);
}

Optional<ScalabilityMode> ScalabilityModeStringToEnum(StringView scalability_mode_string)
{
    return ScalabilityModeFromString(scalability_mode_string);
}

OCTK_END_NAMESPACE
