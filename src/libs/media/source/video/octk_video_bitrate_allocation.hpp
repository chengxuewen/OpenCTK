/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
** Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
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

#include <octk_video_codec_constants.hpp>
#include <octk_media_global.hpp>
#include <octk_optional.hpp>

#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>
#include <vector>

OCTK_BEGIN_NAMESPACE

// Class that describes how video bitrate, in bps, is allocated across temporal
// and spatial layers. Not that bitrates are NOT cumulative. Depending on if
// layers are dependent or not, it is up to the user to aggregate.
// For each index, the bitrate can also both set and unset. This is used with a
// set bps = 0 to signal an explicit "turn off" signal.
class OCTK_MEDIA_API VideoBitrateAllocation
{
public:
    static constexpr uint32_t kMaxBitrateBps = std::numeric_limits<uint32_t>::max();
    VideoBitrateAllocation();

    bool SetBitrate(size_t spatial_index, size_t temporal_index, uint32_t bitrate_bps);

    bool HasBitrate(size_t spatial_index, size_t temporal_index) const;

    uint32_t GetBitrate(size_t spatial_index, size_t temporal_index) const;

    // Whether the specific spatial layers has the bitrate set in any of its
    // temporal layers.
    bool IsSpatialLayerUsed(size_t spatial_index) const;

    // Get the sum of all the temporal layer for a specific spatial layer.
    uint32_t GetSpatialLayerSum(size_t spatial_index) const;

    // Sum of bitrates of temporal layers, from layer 0 to `temporal_index`
    // inclusive, of specified spatial layer `spatial_index`. Bitrates of lower
    // spatial layers are not included.
    uint32_t GetTemporalLayerSum(size_t spatial_index, size_t temporal_index) const;

    // Returns a vector of the temporal layer bitrates for the specific spatial
    // layer. Length of the returned vector is cropped to the highest temporal
    // layer with a defined bitrate.
    std::vector<uint32_t> GetTemporalLayerAllocation(size_t spatial_index) const;

    // Returns one VideoBitrateAllocation for each spatial layer. This is used to
    // configure simulcast streams. Note that the length of the returned vector is
    // always kMaxSpatialLayers, the optional is unset for unused layers.
    std::vector<Optional<VideoBitrateAllocation>> GetSimulcastAllocations() const;

    uint32_t get_sum_bps() const { return sum_; }         // Sum of all bitrates.
    uint32_t get_sum_kbps() const { return sum_ / 1000; } // Round down to not exceed the allocated bitrate.

    bool operator==(const VideoBitrateAllocation &other) const;
    inline bool operator!=(const VideoBitrateAllocation &other) const { return !(*this == other); }

    std::string toString() const;

    // Indicates if the allocation has some layers/streams disabled due to
    // low available bandwidth.
    void set_bw_limited(bool limited) { is_bw_limited_ = limited; }
    bool is_bw_limited() const { return is_bw_limited_; }

private:
    uint32_t sum_;
    Optional<uint32_t> bitrates_[kMaxSpatialLayers][kMaxTemporalStreams];
    bool is_bw_limited_;
};

OCTK_END_NAMESPACE