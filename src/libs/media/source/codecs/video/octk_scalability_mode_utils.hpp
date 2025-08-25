//
// Created by cxw on 25-8-15.
//

#ifndef _OCTK_SCALABILITY_MODE_UTILS_HPP
#define _OCTK_SCALABILITY_MODE_UTILS_HPP

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

#endif // _OCTK_SCALABILITY_MODE_UTILS_HPP
