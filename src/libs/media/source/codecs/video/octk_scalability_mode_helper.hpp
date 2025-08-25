/*
 *  Copyright (c) 2022 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef _OCTK_SCALABILITY_MODE_HELPER_HPP
#define _OCTK_SCALABILITY_MODE_HELPER_HPP

#include <octk_scalability_mode.hpp>
#include <octk_string_view.hpp>
#include <octk_optional.hpp>
#include <octk_video_codec.hpp>

OCTK_BEGIN_NAMESPACE

// Returns the number of spatial layers from the `scalability_mode_string`
// or utils::nullopt if the given mode is unknown.
OCTK_MEDIA_API Optional<int> ScalabilityModeStringToNumSpatialLayers(StringView scalability_mode_string);

// Returns the number of temporal layers from the `scalability_mode_string`
// or utils::nullopt if the given mode is unknown.
OCTK_MEDIA_API Optional<int> ScalabilityModeStringToNumTemporalLayers(StringView scalability_mode_string);

// Convert the `scalability_mode_string` to the scalability mode enum value
// or utils::nullopt if the given mode is unknown.
OCTK_MEDIA_API Optional<ScalabilityMode> ScalabilityModeStringToEnum(StringView scalability_mode_string);

OCTK_END_NAMESPACE

#endif // _OCTK_SCALABILITY_MODE_HELPER_HPP
