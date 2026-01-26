/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2026~Present ChengXueWen.
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

#include <octk_video_bitrate_allocation.hpp>
#include <octk_optional.hpp>

#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

OCTK_BEGIN_NAMESPACE

TEST(VideoBitrateAllocation, SimulcastTargetBitrate)
{
    VideoBitrateAllocation bitrate;
    bitrate.SetBitrate(0, 0, 10000);
    bitrate.SetBitrate(0, 1, 20000);
    bitrate.SetBitrate(1, 0, 40000);
    bitrate.SetBitrate(1, 1, 80000);

    VideoBitrateAllocation layer0_bitrate;
    layer0_bitrate.SetBitrate(0, 0, 10000);
    layer0_bitrate.SetBitrate(0, 1, 20000);

    VideoBitrateAllocation layer1_bitrate;
    layer1_bitrate.SetBitrate(0, 0, 40000);
    layer1_bitrate.SetBitrate(0, 1, 80000);

    std::vector<Optional<VideoBitrateAllocation>> layer_allocations = bitrate.GetSimulcastAllocations();

    EXPECT_EQ(layer0_bitrate, layer_allocations[0]);
    EXPECT_EQ(layer1_bitrate, layer_allocations[1]);
}

TEST(VideoBitrateAllocation, SimulcastTargetBitrateWithInactiveStream)
{
    // Create bitrate allocation with bitrate only for the first and third stream.
    VideoBitrateAllocation bitrate;
    bitrate.SetBitrate(0, 0, 10000);
    bitrate.SetBitrate(0, 1, 20000);
    bitrate.SetBitrate(2, 0, 40000);
    bitrate.SetBitrate(2, 1, 80000);

    VideoBitrateAllocation layer0_bitrate;
    layer0_bitrate.SetBitrate(0, 0, 10000);
    layer0_bitrate.SetBitrate(0, 1, 20000);

    VideoBitrateAllocation layer2_bitrate;
    layer2_bitrate.SetBitrate(0, 0, 40000);
    layer2_bitrate.SetBitrate(0, 1, 80000);

    std::vector<Optional<VideoBitrateAllocation>> layer_allocations = bitrate.GetSimulcastAllocations();

    EXPECT_EQ(layer0_bitrate, layer_allocations[0]);
    EXPECT_FALSE(layer_allocations[1]);
    EXPECT_EQ(layer2_bitrate, layer_allocations[2]);
}

OCTK_END_NAMESPACE
