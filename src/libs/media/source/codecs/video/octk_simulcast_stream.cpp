//
// Created by cxw on 25-8-15.
//

#include <octk_simulcast_stream.hpp>
#include <octk_checks.hpp>

OCTK_BEGIN_NAMESPACE

unsigned char SimulcastStream::GetNumberOfTemporalLayers() const { return numberOfTemporalLayers; }
void SimulcastStream::SetNumberOfTemporalLayers(unsigned char n)
{
    OCTK_DCHECK_GE(n, 1);
    OCTK_DCHECK_LE(n, 3);
    numberOfTemporalLayers = n;
}

Optional<ScalabilityMode> SimulcastStream::GetScalabilityMode() const
{
    static const ScalabilityMode scalability_modes[3] = {
        ScalabilityMode::kL1T1,
        ScalabilityMode::kL1T2,
        ScalabilityMode::kL1T3,
    };
    if (numberOfTemporalLayers < 1 || numberOfTemporalLayers > 3)
    {
        return utils::nullopt;
    }
    return scalability_modes[numberOfTemporalLayers - 1];
}

bool SimulcastStream::operator==(const SimulcastStream &other) const
{
    return (width == other.width && height == other.height && maxFramerate == other.maxFramerate &&
            numberOfTemporalLayers == other.numberOfTemporalLayers && maxBitrate == other.maxBitrate &&
            targetBitrate == other.targetBitrate && minBitrate == other.minBitrate && qpMax == other.qpMax &&
            active == other.active);
}


OCTK_END_NAMESPACE
