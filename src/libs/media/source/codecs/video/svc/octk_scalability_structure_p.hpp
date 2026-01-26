/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
** Copyright (c) 2020 The WebRTC project authors. All Rights Reserved.
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
#include <private/octk_scalable_video_controller_p.hpp>
#include <octk_optional.hpp>

#include <memory>
#include <vector>

OCTK_BEGIN_NAMESPACE

// Creates a structure by name according to
// https://w3c.github.io/webrtc-svc/#scalabilitymodes*
// Returns nullptr for unknown name.
std::unique_ptr<ScalableVideoController> OCTK_MEDIA_API CreateScalabilityStructure(ScalabilityMode name);

// Returns description of the scalability structure identified by 'name',
// Return nullopt for unknown name.
Optional<ScalableVideoController::StreamLayersConfig> ScalabilityStructureConfig(ScalabilityMode name);

OCTK_END_NAMESPACE