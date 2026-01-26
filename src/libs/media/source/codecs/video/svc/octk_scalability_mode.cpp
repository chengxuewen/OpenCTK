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

#include <octk_scalability_mode.hpp>
#include <octk_checks.hpp>

OCTK_BEGIN_NAMESPACE

StringView ScalabilityModeToString(ScalabilityMode scalability_mode)
{
    switch (scalability_mode)
    {
        case ScalabilityMode::kL1T1: return "L1T1";
        case ScalabilityMode::kL1T2: return "L1T2";
        case ScalabilityMode::kL1T3: return "L1T3";
        case ScalabilityMode::kL2T1: return "L2T1";
        case ScalabilityMode::kL2T1h: return "L2T1h";
        case ScalabilityMode::kL2T1_KEY: return "L2T1_KEY";
        case ScalabilityMode::kL2T2: return "L2T2";
        case ScalabilityMode::kL2T2h: return "L2T2h";
        case ScalabilityMode::kL2T2_KEY: return "L2T2_KEY";
        case ScalabilityMode::kL2T2_KEY_SHIFT: return "L2T2_KEY_SHIFT";
        case ScalabilityMode::kL2T3: return "L2T3";
        case ScalabilityMode::kL2T3h: return "L2T3h";
        case ScalabilityMode::kL2T3_KEY: return "L2T3_KEY";
        case ScalabilityMode::kL3T1: return "L3T1";
        case ScalabilityMode::kL3T1h: return "L3T1h";
        case ScalabilityMode::kL3T1_KEY: return "L3T1_KEY";
        case ScalabilityMode::kL3T2: return "L3T2";
        case ScalabilityMode::kL3T2h: return "L3T2h";
        case ScalabilityMode::kL3T2_KEY: return "L3T2_KEY";
        case ScalabilityMode::kL3T3: return "L3T3";
        case ScalabilityMode::kL3T3h: return "L3T3h";
        case ScalabilityMode::kL3T3_KEY: return "L3T3_KEY";
        case ScalabilityMode::kS2T1: return "S2T1";
        case ScalabilityMode::kS2T1h: return "S2T1h";
        case ScalabilityMode::kS2T2: return "S2T2";
        case ScalabilityMode::kS2T2h: return "S2T2h";
        case ScalabilityMode::kS2T3: return "S2T3";
        case ScalabilityMode::kS2T3h: return "S2T3h";
        case ScalabilityMode::kS3T1: return "S3T1";
        case ScalabilityMode::kS3T1h: return "S3T1h";
        case ScalabilityMode::kS3T2: return "S3T2";
        case ScalabilityMode::kS3T2h: return "S3T2h";
        case ScalabilityMode::kS3T3: return "S3T3";
        case ScalabilityMode::kS3T3h: return "S3T3h";
    }
    OCTK_CHECK_NOTREACHED();
    return "";
}

OCTK_END_NAMESPACE
