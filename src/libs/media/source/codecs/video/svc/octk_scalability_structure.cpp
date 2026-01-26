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

#include <private/octk_scalability_structure_p.hpp>
#include <octk_scalability_mode.hpp>
#include <private/octk_scalability_structure_full_svc_p.hpp>
#include <private/octk_scalability_structure_key_svc_p.hpp>
#include <private/octk_scalability_structure_l2t2_key_shift_p.hpp>
#include <private/octk_scalability_structure_simulcast_p.hpp>
#include <private/octk_scalable_video_controller_p.hpp>
#include <private/octk_scalable_video_controller_no_layering_p.hpp>
#include <octk_checks.hpp>

#include <memory>

OCTK_BEGIN_NAMESPACE

namespace
{

struct NamedStructureFactory
{
    ScalabilityMode name;
    // Use function pointer to make NamedStructureFactory trivally destructable.
    std::unique_ptr<ScalableVideoController> (*factory)();
    ScalableVideoController::StreamLayersConfig config;
};

// Wrap std::make_unique function to have correct return type.
template <typename T>
std::unique_ptr<ScalableVideoController> Create()
{
    return std::make_unique<T>();
}

template <typename T>
std::unique_ptr<ScalableVideoController> CreateH()
{
    // 1.5:1 scaling, see https://w3c.github.io/webrtc-svc/#scalabilitymodes*
    typename T::ScalingFactor factor;
    factor.num = 2;
    factor.den = 3;
    return std::make_unique<T>(factor);
}

constexpr ScalableVideoController::StreamLayersConfig kConfigL1T1 = {
    /*num_spatial_layers=*/1,
    /*num_temporal_layers=*/1,
    /*uses_reference_scaling=*/false};

constexpr ScalableVideoController::StreamLayersConfig kConfigL1T2 = {
    /*num_spatial_layers=*/1,
    /*num_temporal_layers=*/2,
    /*uses_reference_scaling=*/false};

constexpr ScalableVideoController::StreamLayersConfig kConfigL1T3 = {
    /*num_spatial_layers=*/1,
    /*num_temporal_layers=*/3,
    /*uses_reference_scaling=*/false};

constexpr ScalableVideoController::StreamLayersConfig kConfigL2T1 = {
    /*num_spatial_layers=*/2,
    /*num_temporal_layers=*/1,
    /*uses_reference_scaling=*/true,
    {1, 1},
    {2, 1}};

constexpr ScalableVideoController::StreamLayersConfig kConfigL2T1h = {
    /*num_spatial_layers=*/2,
    /*num_temporal_layers=*/1,
    /*uses_reference_scaling=*/true,
    {2, 1},
    {3, 1}};

constexpr ScalableVideoController::StreamLayersConfig kConfigL2T2 = {
    /*num_spatial_layers=*/2,
    /*num_temporal_layers=*/2,
    /*uses_reference_scaling=*/true,
    {1, 1},
    {2, 1}};

constexpr ScalableVideoController::StreamLayersConfig kConfigL2T2h = {
    /*num_spatial_layers=*/2,
    /*num_temporal_layers=*/2,
    /*uses_reference_scaling=*/true,
    {2, 1},
    {3, 1}};

constexpr ScalableVideoController::StreamLayersConfig kConfigL2T3 = {
    /*num_spatial_layers=*/2,
    /*num_temporal_layers=*/3,
    /*uses_reference_scaling=*/true,
    {1, 1},
    {2, 1}};

constexpr ScalableVideoController::StreamLayersConfig kConfigL2T3h = {
    /*num_spatial_layers=*/2,
    /*num_temporal_layers=*/3,
    /*uses_reference_scaling=*/true,
    {2, 1},
    {3, 1}};

constexpr ScalableVideoController::StreamLayersConfig kConfigL3T1 = {
    /*num_spatial_layers=*/3,
    /*num_temporal_layers=*/1,
    /*uses_reference_scaling=*/true,
    {1, 1, 1},
    {4, 2, 1}};

constexpr ScalableVideoController::StreamLayersConfig kConfigL3T1h = {
    /*num_spatial_layers=*/3,
    /*num_temporal_layers=*/1,
    /*uses_reference_scaling=*/true,
    {4, 2, 1},
    {9, 3, 1}};

constexpr ScalableVideoController::StreamLayersConfig kConfigL3T2 = {
    /*num_spatial_layers=*/3,
    /*num_temporal_layers=*/2,
    /*uses_reference_scaling=*/true,
    {1, 1, 1},
    {4, 2, 1}};

constexpr ScalableVideoController::StreamLayersConfig kConfigL3T2h = {
    /*num_spatial_layers=*/3,
    /*num_temporal_layers=*/2,
    /*uses_reference_scaling=*/true,
    {4, 2, 1},
    {9, 3, 1}};

constexpr ScalableVideoController::StreamLayersConfig kConfigL3T3 = {
    /*num_spatial_layers=*/3,
    /*num_temporal_layers=*/3,
    /*uses_reference_scaling=*/true,
    {1, 1, 1},
    {4, 2, 1}};

constexpr ScalableVideoController::StreamLayersConfig kConfigL3T3h = {
    /*num_spatial_layers=*/3,
    /*num_temporal_layers=*/3,
    /*uses_reference_scaling=*/true,
    {4, 2, 1},
    {9, 3, 1}};

constexpr ScalableVideoController::StreamLayersConfig kConfigS2T1 = {
    /*num_spatial_layers=*/2,
    /*num_temporal_layers=*/1,
    /*uses_reference_scaling=*/false,
    {1, 1},
    {2, 1}};

constexpr ScalableVideoController::StreamLayersConfig kConfigS2T1h = {
    /*num_spatial_layers=*/2,
    /*num_temporal_layers=*/1,
    /*uses_reference_scaling=*/false,
    {2, 1},
    {3, 1}};

constexpr ScalableVideoController::StreamLayersConfig kConfigS2T2 = {
    /*num_spatial_layers=*/2,
    /*num_temporal_layers=*/2,
    /*uses_reference_scaling=*/false,
    {1, 1},
    {2, 1}};

constexpr ScalableVideoController::StreamLayersConfig kConfigS2T2h = {
    /*num_spatial_layers=*/2,
    /*num_temporal_layers=*/2,
    /*uses_reference_scaling=*/false,
    {2, 1},
    {3, 1}};

constexpr ScalableVideoController::StreamLayersConfig kConfigS2T3 = {
    /*num_spatial_layers=*/2,
    /*num_temporal_layers=*/3,
    /*uses_reference_scaling=*/false,
    {1, 1},
    {2, 1}};

constexpr ScalableVideoController::StreamLayersConfig kConfigS2T3h = {
    /*num_spatial_layers=*/2,
    /*num_temporal_layers=*/3,
    /*uses_reference_scaling=*/false,
    {2, 1},
    {3, 1}};

constexpr ScalableVideoController::StreamLayersConfig kConfigS3T1 = {
    /*num_spatial_layers=*/3,
    /*num_temporal_layers=*/1,
    /*uses_reference_scaling=*/false,
    {1, 1, 1},
    {4, 2, 1}};

constexpr ScalableVideoController::StreamLayersConfig kConfigS3T1h = {
    /*num_spatial_layers=*/3,
    /*num_temporal_layers=*/1,
    /*uses_reference_scaling=*/false,
    {4, 2, 1},
    {9, 3, 1}};

constexpr ScalableVideoController::StreamLayersConfig kConfigS3T2 = {
    /*num_spatial_layers=*/3,
    /*num_temporal_layers=*/2,
    /*uses_reference_scaling=*/false,
    {1, 1, 1},
    {4, 2, 1}};

constexpr ScalableVideoController::StreamLayersConfig kConfigS3T2h = {
    /*num_spatial_layers=*/3,
    /*num_temporal_layers=*/2,
    /*uses_reference_scaling=*/false,
    {4, 2, 1},
    {9, 3, 1}};

constexpr ScalableVideoController::StreamLayersConfig kConfigS3T3 = {
    /*num_spatial_layers=*/3,
    /*num_temporal_layers=*/3,
    /*uses_reference_scaling=*/false,
    {1, 1, 1},
    {4, 2, 1}};

constexpr ScalableVideoController::StreamLayersConfig kConfigS3T3h = {
    /*num_spatial_layers=*/3,
    /*num_temporal_layers=*/3,
    /*uses_reference_scaling=*/false,
    {4, 2, 1},
    {9, 3, 1}};

constexpr NamedStructureFactory kFactories[] = {
    {ScalabilityMode::kL1T1, Create<ScalableVideoControllerNoLayering>, kConfigL1T1},
    {ScalabilityMode::kL1T2, Create<ScalabilityStructureL1T2>, kConfigL1T2},
    {ScalabilityMode::kL1T3, Create<ScalabilityStructureL1T3>, kConfigL1T3},
    {ScalabilityMode::kL2T1, Create<ScalabilityStructureL2T1>, kConfigL2T1},
    {ScalabilityMode::kL2T1h, CreateH<ScalabilityStructureL2T1>, kConfigL2T1h},
    {ScalabilityMode::kL2T1_KEY, Create<ScalabilityStructureL2T1Key>, kConfigL2T1},
    {ScalabilityMode::kL2T2, Create<ScalabilityStructureL2T2>, kConfigL2T2},
    {ScalabilityMode::kL2T2h, CreateH<ScalabilityStructureL2T2>, kConfigL2T2h},
    {ScalabilityMode::kL2T2_KEY, Create<ScalabilityStructureL2T2Key>, kConfigL2T2},
    {ScalabilityMode::kL2T2_KEY_SHIFT, Create<ScalabilityStructureL2T2KeyShift>, kConfigL2T2},
    {ScalabilityMode::kL2T3, Create<ScalabilityStructureL2T3>, kConfigL2T3},
    {ScalabilityMode::kL2T3h, CreateH<ScalabilityStructureL2T3>, kConfigL2T3h},
    {ScalabilityMode::kL2T3_KEY, Create<ScalabilityStructureL2T3Key>, kConfigL2T3},
    {ScalabilityMode::kL3T1, Create<ScalabilityStructureL3T1>, kConfigL3T1},
    {ScalabilityMode::kL3T1h, CreateH<ScalabilityStructureL3T1>, kConfigL3T1h},
    {ScalabilityMode::kL3T1_KEY, Create<ScalabilityStructureL3T1Key>, kConfigL3T1},
    {ScalabilityMode::kL3T2, Create<ScalabilityStructureL3T2>, kConfigL3T2},
    {ScalabilityMode::kL3T2h, CreateH<ScalabilityStructureL3T2>, kConfigL3T2h},
    {ScalabilityMode::kL3T2_KEY, Create<ScalabilityStructureL3T2Key>, kConfigL3T2},
    {ScalabilityMode::kL3T3, Create<ScalabilityStructureL3T3>, kConfigL3T3},
    {ScalabilityMode::kL3T3h, CreateH<ScalabilityStructureL3T3>, kConfigL3T3h},
    {ScalabilityMode::kL3T3_KEY, Create<ScalabilityStructureL3T3Key>, kConfigL3T3},
    {ScalabilityMode::kS2T1, Create<ScalabilityStructureS2T1>, kConfigS2T1},
    {ScalabilityMode::kS2T1h, CreateH<ScalabilityStructureS2T1>, kConfigS2T1h},
    {ScalabilityMode::kS2T2, Create<ScalabilityStructureS2T2>, kConfigS2T2},
    {ScalabilityMode::kS2T2h, CreateH<ScalabilityStructureS2T2>, kConfigS2T2h},
    {ScalabilityMode::kS2T3, Create<ScalabilityStructureS2T3>, kConfigS2T3},
    {ScalabilityMode::kS2T3h, CreateH<ScalabilityStructureS2T3>, kConfigS2T3h},
    {ScalabilityMode::kS3T1, Create<ScalabilityStructureS3T1>, kConfigS3T1},
    {ScalabilityMode::kS3T1h, CreateH<ScalabilityStructureS3T1>, kConfigS3T1h},
    {ScalabilityMode::kS3T2, Create<ScalabilityStructureS3T2>, kConfigS3T2},
    {ScalabilityMode::kS3T2h, CreateH<ScalabilityStructureS3T2>, kConfigS3T2h},
    {ScalabilityMode::kS3T3, Create<ScalabilityStructureS3T3>, kConfigS3T3},
    {ScalabilityMode::kS3T3h, CreateH<ScalabilityStructureS3T3>, kConfigS3T3h},
};

} // namespace

std::unique_ptr<ScalableVideoController> CreateScalabilityStructure(ScalabilityMode name)
{
    for (const auto &entry : kFactories)
    {
        if (entry.name == name)
        {
            return entry.factory();
        }
    }
    return nullptr;
}

Optional<ScalableVideoController::StreamLayersConfig> ScalabilityStructureConfig(ScalabilityMode name)
{
    for (const auto &entry : kFactories)
    {
        if (entry.name == name)
        {
            return entry.config;
        }
    }
    return utils::nullopt;
}

OCTK_END_NAMESPACE