/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
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

#include <octk_video_bitrate_allocation.hpp>
#include <octk_safe_conversions.hpp>
#include <octk_checks.hpp>

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

// #include "api/video/video_codec_constants.h"
// #include "rtc_base/checks.h"
// #include "rtc_base/numerics/safe_conversions.h"
// #include "rtc_base/strings/string_builder.h"

OCTK_BEGIN_NAMESPACE

VideoBitrateAllocation::VideoBitrateAllocation()
    : sum_(0), is_bw_limited_(false) {}

bool VideoBitrateAllocation::SetBitrate(size_t spatial_index,
                                        size_t temporal_index,
                                        uint32_t bitrate_bps)
{
    OCTK_CHECK_LT(spatial_index, kMaxSpatialLayers);
    OCTK_CHECK_LT(temporal_index, kMaxTemporalStreams);
    int64_t new_bitrate_sum_bps = sum_;
    Optional<uint32_t> &layer_bitrate = bitrates_[spatial_index][temporal_index];
    if (layer_bitrate)
    {
        OCTK_DCHECK_LE(*layer_bitrate, sum_);
        new_bitrate_sum_bps -= *layer_bitrate;
    }
    new_bitrate_sum_bps += bitrate_bps;
    if (new_bitrate_sum_bps > kMaxBitrateBps)
    {
        return false;
    }

    layer_bitrate = bitrate_bps;
    sum_ = utils::dchecked_cast<uint32_t>(new_bitrate_sum_bps);
    return true;
}

bool VideoBitrateAllocation::HasBitrate(size_t spatial_index,
                                        size_t temporal_index) const
{
    OCTK_CHECK_LT(spatial_index, kMaxSpatialLayers);
    OCTK_CHECK_LT(temporal_index, kMaxTemporalStreams);
    return bitrates_[spatial_index][temporal_index].has_value();
}

uint32_t VideoBitrateAllocation::GetBitrate(size_t spatial_index,
                                            size_t temporal_index) const
{
    OCTK_CHECK_LT(spatial_index, kMaxSpatialLayers);
    OCTK_CHECK_LT(temporal_index, kMaxTemporalStreams);
    return bitrates_[spatial_index][temporal_index].value_or(0);
}

// Whether the specific spatial layers has the bitrate set in any of its temporal layers.
bool VideoBitrateAllocation::IsSpatialLayerUsed(size_t spatial_index) const
{
    OCTK_CHECK_LT(spatial_index, kMaxSpatialLayers);
    for (size_t i = 0; i < kMaxTemporalStreams; ++i)
    {
        if (bitrates_[spatial_index][i].has_value())
        {
            return true;
        }
    }
    return false;
}

// Get the sum of all the temporal layer for a specific spatial layer.
uint32_t VideoBitrateAllocation::GetSpatialLayerSum(size_t spatial_index) const
{
    OCTK_CHECK_LT(spatial_index, kMaxSpatialLayers);
    return GetTemporalLayerSum(spatial_index, kMaxTemporalStreams - 1);
}

uint32_t VideoBitrateAllocation::GetTemporalLayerSum(size_t spatial_index,
                                                     size_t temporal_index) const
{
    OCTK_CHECK_LT(spatial_index, kMaxSpatialLayers);
    OCTK_CHECK_LT(temporal_index, kMaxTemporalStreams);
    uint32_t sum = 0;
    for (size_t i = 0; i <= temporal_index; ++i)
    {
        sum += bitrates_[spatial_index][i].value_or(0);
    }
    return sum;
}

std::vector<uint32_t> VideoBitrateAllocation::GetTemporalLayerAllocation(
    size_t spatial_index) const
{
    OCTK_CHECK_LT(spatial_index, kMaxSpatialLayers);
    std::vector<uint32_t> temporal_rates;

    // Find the highest temporal layer with a defined bitrate in order to
    // determine the size of the temporal layer allocation.
    for (size_t i = kMaxTemporalStreams; i > 0; --i)
    {
        if (bitrates_[spatial_index][i - 1].has_value())
        {
            temporal_rates.resize(i);
            break;
        }
    }

    for (size_t i = 0; i < temporal_rates.size(); ++i)
    {
        temporal_rates[i] = bitrates_[spatial_index][i].value_or(0);
    }

    return temporal_rates;
}

std::vector<Optional<VideoBitrateAllocation>> VideoBitrateAllocation::GetSimulcastAllocations() const
{
    std::vector<Optional<VideoBitrateAllocation>> bitrates;
    for (size_t si = 0; si < kMaxSpatialLayers; ++si)
    {
        Optional<VideoBitrateAllocation> layer_bitrate;
        if (IsSpatialLayerUsed(si))
        {
            layer_bitrate = VideoBitrateAllocation();
            for (int tl = 0; tl < kMaxTemporalStreams; ++tl)
            {
                if (HasBitrate(si, tl))
                {
                    layer_bitrate->SetBitrate(0, tl, GetBitrate(si, tl));
                }
            }
        }
        bitrates.push_back(layer_bitrate);
    }
    return bitrates;
}

bool VideoBitrateAllocation::operator==(const VideoBitrateAllocation &other) const
{
    for (size_t si = 0; si < kMaxSpatialLayers; ++si)
    {
        for (size_t ti = 0; ti < kMaxTemporalStreams; ++ti)
        {
            if (bitrates_[si][ti] != other.bitrates_[si][ti])
            {
                return false;
            }
        }
    }
    return true;
}

std::string VideoBitrateAllocation::toString() const
{
    if (sum_ == 0)
    {
        return "VideoBitrateAllocation [ [] ]";
    }

    // Max string length in practice is 260, but let's have some overhead and
    // round up to nearest power of two.
    // char string_buf[512];
    // rtc::SimpleStringBuilder ss(string_buf);
    std::stringstream ss;

    ss << "VideoBitrateAllocation [";
    uint32_t spatial_cumulator = 0;
    for (size_t si = 0; si < kMaxSpatialLayers; ++si)
    {
        OCTK_DCHECK_LE(spatial_cumulator, sum_);
        if (spatial_cumulator == sum_)
        {
            break;
        }

        const uint32_t layer_sum = GetSpatialLayerSum(si);
        if (layer_sum == sum_ && si == 0)
        {
            ss << " [";
        }
        else
        {
            if (si > 0)
            {
                ss << ",";
            }
            ss << '\n' << "  [";
        }
        spatial_cumulator += layer_sum;

        uint32_t temporal_cumulator = 0;
        for (size_t ti = 0; ti < kMaxTemporalStreams; ++ti)
        {
            OCTK_DCHECK_LE(temporal_cumulator, layer_sum);
            if (temporal_cumulator == layer_sum)
            {
                break;
            }

            if (ti > 0)
            {
                ss << ", ";
            }

            uint32_t bitrate = bitrates_[si][ti].value_or(0);
            ss << bitrate;
            temporal_cumulator += bitrate;
        }
        ss << "]";
    }

    OCTK_DCHECK_EQ(spatial_cumulator, sum_);
    ss << " ]";
    return ss.str();
}
OCTK_END_NAMESPACE