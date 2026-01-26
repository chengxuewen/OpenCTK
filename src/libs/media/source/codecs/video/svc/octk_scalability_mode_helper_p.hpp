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