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

#include <private/octk_scalability_mode_utils_p.hpp>
#include <octk_iterator.hpp>
#include <octk_checks.hpp>

OCTK_BEGIN_NAMESPACE

namespace
{

struct ScalabilityModeParameters
{
    OCTK_CXX14_CONSTEXPR ScalabilityModeParameters(ScalabilityMode smode,
                                                   StringView n,
                                                   int nsl,
                                                   int ntl,
                                                   InterLayerPredMode ipl,
                                                   Optional<ScalabilityModeResolutionRatio> r,
                                                   bool s)
        : scalability_mode(smode)
        , name(n)
        , num_spatial_layers(nsl)
        , num_temporal_layers(ntl)
        , inter_layer_pred(ipl)
        , ratio(r)
        , shift(s)
    {
    }
    const ScalabilityMode scalability_mode;
    const StringView name;
    const int num_spatial_layers;
    const int num_temporal_layers;
    const InterLayerPredMode inter_layer_pred;
    const Optional<ScalabilityModeResolutionRatio> ratio = ScalabilityModeResolutionRatio::kTwoToOne;
    const bool shift = false;
};

constexpr size_t kNumScalabilityModes = static_cast<size_t>(ScalabilityMode::kS3T3h) + 1;

OCTK_CXX14_CONSTEXPR ScalabilityModeParameters kScalabilityModeParams[] = {
    {ScalabilityMode::kL1T1, "L1T1", 1, 1, InterLayerPredMode::kOff, utils::nullopt, false},
    {ScalabilityMode::kL1T2, "L1T2", 1, 2, InterLayerPredMode::kOff, utils::nullopt, false},
    {ScalabilityMode::kL1T3, "L1T3", 1, 3, InterLayerPredMode::kOff, utils::nullopt, false},
    {ScalabilityMode::kL2T1, "L2T1", 2, 1, InterLayerPredMode::kOn, utils::nullopt, false},
    {ScalabilityMode::kL2T1h,
     "L2T1h",
     2,
     1,
     InterLayerPredMode::kOn,
     ScalabilityModeResolutionRatio::kThreeToTwo,
     false},
    {ScalabilityMode::kL2T1_KEY, "L2T1_KEY", 2, 1, InterLayerPredMode::kOnKeyPic, utils::nullopt, false},
    {ScalabilityMode::kL2T2, "L2T2", 2, 2, InterLayerPredMode::kOn, utils::nullopt, false},
    {ScalabilityMode::kL2T2h,
     "L2T2h",
     2,
     2,
     InterLayerPredMode::kOn,
     ScalabilityModeResolutionRatio::kThreeToTwo,
     false},
    {ScalabilityMode::kL2T2_KEY, "L2T2_KEY", 2, 2, InterLayerPredMode::kOnKeyPic, utils::nullopt, false},
    {ScalabilityMode::kL2T2_KEY_SHIFT, "L2T2_KEY_SHIFT", 2, 2, InterLayerPredMode::kOnKeyPic, utils::nullopt, true},
    {ScalabilityMode::kL2T3, "L2T3", 2, 3, InterLayerPredMode::kOn, utils::nullopt, false},
    {ScalabilityMode::kL2T3h,
     "L2T3h",
     2,
     3,
     InterLayerPredMode::kOn,
     ScalabilityModeResolutionRatio::kThreeToTwo,
     false},
    {ScalabilityMode::kL2T3_KEY, "L2T3_KEY", 2, 3, InterLayerPredMode::kOnKeyPic, utils::nullopt, false},
    {ScalabilityMode::kL3T1, "L3T1", 3, 1, InterLayerPredMode::kOn, utils::nullopt, false},
    {ScalabilityMode::kL3T1h,
     "L3T1h",
     3,
     1,
     InterLayerPredMode::kOn,
     ScalabilityModeResolutionRatio::kThreeToTwo,
     false},
    {ScalabilityMode::kL3T1_KEY, "L3T1_KEY", 3, 1, InterLayerPredMode::kOnKeyPic, utils::nullopt, false},
    {ScalabilityMode::kL3T2, "L3T2", 3, 2, InterLayerPredMode::kOn, utils::nullopt, false},
    {ScalabilityMode::kL3T2h,
     "L3T2h",
     3,
     2,
     InterLayerPredMode::kOn,
     ScalabilityModeResolutionRatio::kThreeToTwo,
     false},
    {ScalabilityMode::kL3T2_KEY, "L3T2_KEY", 3, 2, InterLayerPredMode::kOnKeyPic, utils::nullopt, false},
    {ScalabilityMode::kL3T3, "L3T3", 3, 3, InterLayerPredMode::kOn, utils::nullopt, false},
    {ScalabilityMode::kL3T3h,
     "L3T3h",
     3,
     3,
     InterLayerPredMode::kOn,
     ScalabilityModeResolutionRatio::kThreeToTwo,
     false},
    {ScalabilityMode::kL3T3_KEY, "L3T3_KEY", 3, 3, InterLayerPredMode::kOnKeyPic, utils::nullopt, false},
    {ScalabilityMode::kS2T1, "S2T1", 2, 1, InterLayerPredMode::kOff, utils::nullopt, false},
    {ScalabilityMode::kS2T1h,
     "S2T1h",
     2,
     1,
     InterLayerPredMode::kOff,
     ScalabilityModeResolutionRatio::kThreeToTwo,
     false},
    {ScalabilityMode::kS2T2, "S2T2", 2, 2, InterLayerPredMode::kOff, utils::nullopt, false},
    {ScalabilityMode::kS2T2h,
     "S2T2h",
     2,
     2,
     InterLayerPredMode::kOff,
     ScalabilityModeResolutionRatio::kThreeToTwo,
     false},
    {ScalabilityMode::kS2T3, "S2T3", 2, 3, InterLayerPredMode::kOff, utils::nullopt, false},
    {ScalabilityMode::kS2T3h,
     "S2T3h",
     2,
     3,
     InterLayerPredMode::kOff,
     ScalabilityModeResolutionRatio::kThreeToTwo,
     false},
    {ScalabilityMode::kS3T1, "S3T1", 3, 1, InterLayerPredMode::kOff, utils::nullopt, false},
    {ScalabilityMode::kS3T1h,
     "S3T1h",
     3,
     1,
     InterLayerPredMode::kOff,
     ScalabilityModeResolutionRatio::kThreeToTwo,
     false},
    {ScalabilityMode::kS3T2, "S3T2", 3, 2, InterLayerPredMode::kOff, utils::nullopt, false},
    {ScalabilityMode::kS3T2h,
     "S3T2h",
     3,
     2,
     InterLayerPredMode::kOff,
     ScalabilityModeResolutionRatio::kThreeToTwo,
     false},
    {ScalabilityMode::kS3T3, "S3T3", 3, 3, InterLayerPredMode::kOff, utils::nullopt, false},
    {ScalabilityMode::kS3T3h,
     "S3T3h",
     3,
     3,
     InterLayerPredMode::kOff,
     ScalabilityModeResolutionRatio::kThreeToTwo,
     false},
};

// This could be replaced with std::all_of in c++20.
OCTK_CXX14_CONSTEXPR bool CheckScalabilityModeParams()
{
    static_assert(utils::size(kScalabilityModeParams) == kNumScalabilityModes, "");
    for (size_t s = 0; s < kNumScalabilityModes; ++s)
    {
        if (kScalabilityModeParams[s].scalability_mode != static_cast<ScalabilityMode>(s))
        {
            return false;
        }
    }
    return true;
}

OCTK_CXX14_CONSTEXPR std::underlying_type<ScalabilityMode>::type Idx(ScalabilityMode s)
{
    OCTK_CXX14_CONSTEXPR_ASSERT_X(CheckScalabilityModeParams(), "There is a scalability mode mismatch in the array!");
    const auto index = static_cast<std::underlying_type<ScalabilityMode>::type>(s);
    OCTK_CHECK_LT(index, kNumScalabilityModes);
    return index;
}

} // namespace

Optional<ScalabilityMode> MakeScalabilityMode(int num_spatial_layers,
                                              int num_temporal_layers,
                                              InterLayerPredMode inter_layer_pred,
                                              Optional<ScalabilityModeResolutionRatio> ratio,
                                              bool shift)
{
    for (const auto &candidate_mode : kScalabilityModeParams)
    {
        if (candidate_mode.num_spatial_layers == num_spatial_layers &&
            candidate_mode.num_temporal_layers == num_temporal_layers)
        {
            if (num_spatial_layers == 1 || (candidate_mode.inter_layer_pred == inter_layer_pred &&
                                            candidate_mode.ratio == ratio && candidate_mode.shift == shift))
            {
                return candidate_mode.scalability_mode;
            }
        }
    }
    return utils::nullopt;
}

Optional<ScalabilityMode> ScalabilityModeFromString(StringView mode_string)
{
    for (size_t i = 0; i < OCTK_ARRAY_SIZE(kScalabilityModeParams); ++i)
    {
        if (mode_string == kScalabilityModeParams[i].name)
        {
            return kScalabilityModeParams[i].scalability_mode;
        }
    }
    // const auto it = std::find_if(kScalabilityModeParams,
    //                              [&](const ScalabilityModeParameters &candidate_mode)
    //                              { return candidate_mode.name == mode_string; });
    // if (it != std::end(kScalabilityModeParams))
    // {
    //     return it->scalability_mode;
    // }
    return utils::nullopt;
}

InterLayerPredMode ScalabilityModeToInterLayerPredMode(ScalabilityMode scalability_mode)
{
    return kScalabilityModeParams[Idx(scalability_mode)].inter_layer_pred;
}

int ScalabilityModeToNumSpatialLayers(ScalabilityMode scalability_mode)
{
    return kScalabilityModeParams[Idx(scalability_mode)].num_spatial_layers;
}

int ScalabilityModeToNumTemporalLayers(ScalabilityMode scalability_mode)
{
    return kScalabilityModeParams[Idx(scalability_mode)].num_temporal_layers;
}

Optional<ScalabilityModeResolutionRatio> ScalabilityModeToResolutionRatio(ScalabilityMode scalability_mode)
{
    return kScalabilityModeParams[Idx(scalability_mode)].ratio;
}

ScalabilityMode LimitNumSpatialLayers(ScalabilityMode scalability_mode, int max_spatial_layers)
{
    int num_spatial_layers = ScalabilityModeToNumSpatialLayers(scalability_mode);
    if (max_spatial_layers >= num_spatial_layers)
    {
        return scalability_mode;
    }

    switch (scalability_mode)
    {
        case ScalabilityMode::kL1T1: return ScalabilityMode::kL1T1;
        case ScalabilityMode::kL1T2: return ScalabilityMode::kL1T2;
        case ScalabilityMode::kL1T3: return ScalabilityMode::kL1T3;
        case ScalabilityMode::kL2T1: return ScalabilityMode::kL1T1;
        case ScalabilityMode::kL2T1h: return ScalabilityMode::kL1T1;
        case ScalabilityMode::kL2T1_KEY: return ScalabilityMode::kL1T1;
        case ScalabilityMode::kL2T2: return ScalabilityMode::kL1T2;
        case ScalabilityMode::kL2T2h: return ScalabilityMode::kL1T2;
        case ScalabilityMode::kL2T2_KEY: return ScalabilityMode::kL1T2;
        case ScalabilityMode::kL2T2_KEY_SHIFT: return ScalabilityMode::kL1T2;
        case ScalabilityMode::kL2T3: return ScalabilityMode::kL1T3;
        case ScalabilityMode::kL2T3h: return ScalabilityMode::kL1T3;
        case ScalabilityMode::kL2T3_KEY: return ScalabilityMode::kL1T3;
        case ScalabilityMode::kL3T1: return max_spatial_layers == 2 ? ScalabilityMode::kL2T1 : ScalabilityMode::kL1T1;
        case ScalabilityMode::kL3T1h: return max_spatial_layers == 2 ? ScalabilityMode::kL2T1h : ScalabilityMode::kL1T1;
        case ScalabilityMode::kL3T1_KEY:
            return max_spatial_layers == 2 ? ScalabilityMode::kL2T1_KEY : ScalabilityMode::kL1T1;
        case ScalabilityMode::kL3T2: return max_spatial_layers == 2 ? ScalabilityMode::kL2T2 : ScalabilityMode::kL1T2;
        case ScalabilityMode::kL3T2h: return max_spatial_layers == 2 ? ScalabilityMode::kL2T2h : ScalabilityMode::kL1T2;
        case ScalabilityMode::kL3T2_KEY:
            return max_spatial_layers == 2 ? ScalabilityMode::kL2T2_KEY : ScalabilityMode::kL1T2;
        case ScalabilityMode::kL3T3: return max_spatial_layers == 2 ? ScalabilityMode::kL2T3 : ScalabilityMode::kL1T3;
        case ScalabilityMode::kL3T3h: return max_spatial_layers == 2 ? ScalabilityMode::kL2T3h : ScalabilityMode::kL1T3;
        case ScalabilityMode::kL3T3_KEY:
            return max_spatial_layers == 2 ? ScalabilityMode::kL2T3_KEY : ScalabilityMode::kL1T3;
        case ScalabilityMode::kS2T1: return ScalabilityMode::kL1T1;
        case ScalabilityMode::kS2T1h: return ScalabilityMode::kL1T1;
        case ScalabilityMode::kS2T2: return ScalabilityMode::kL1T2;
        case ScalabilityMode::kS2T2h: return ScalabilityMode::kL1T2;
        case ScalabilityMode::kS2T3: return ScalabilityMode::kL1T3;
        case ScalabilityMode::kS2T3h: return ScalabilityMode::kL1T3;
        case ScalabilityMode::kS3T1: return max_spatial_layers == 2 ? ScalabilityMode::kS2T1 : ScalabilityMode::kL1T1;
        case ScalabilityMode::kS3T1h: return max_spatial_layers == 2 ? ScalabilityMode::kS2T1h : ScalabilityMode::kL1T1;
        case ScalabilityMode::kS3T2: return max_spatial_layers == 2 ? ScalabilityMode::kS2T2 : ScalabilityMode::kL1T2;
        case ScalabilityMode::kS3T2h: return max_spatial_layers == 2 ? ScalabilityMode::kS2T2h : ScalabilityMode::kL1T2;
        case ScalabilityMode::kS3T3: return max_spatial_layers == 2 ? ScalabilityMode::kS2T3 : ScalabilityMode::kL1T3;
        case ScalabilityMode::kS3T3h: return max_spatial_layers == 2 ? ScalabilityMode::kS2T3h : ScalabilityMode::kL1T3;
    }
    OCTK_CHECK_NOTREACHED();
    return ScalabilityMode::kL1T1;
}

bool ScalabilityModeIsShiftMode(ScalabilityMode scalability_mode)
{
    return kScalabilityModeParams[Idx(scalability_mode)].shift;
}

OCTK_END_NAMESPACE
