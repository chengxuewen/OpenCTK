/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
** Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
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

#include <octk_video_bitrate_allocator.hpp>

OCTK_BEGIN_NAMESPACE

VideoBitrateAllocationParameters::VideoBitrateAllocationParameters(uint32_t total_bitrate_bps, uint32_t framerate)
    : total_bitrate(DataRate::BitsPerSec(total_bitrate_bps))
    , stable_bitrate(DataRate::BitsPerSec(total_bitrate_bps))
    , framerate(static_cast<double>(framerate))
{
}

VideoBitrateAllocationParameters::VideoBitrateAllocationParameters(DataRate total_bitrate, double framerate)
    : total_bitrate(total_bitrate)
    , stable_bitrate(total_bitrate)
    , framerate(framerate)
{
}

VideoBitrateAllocationParameters::VideoBitrateAllocationParameters(DataRate total_bitrate,
                                                                   DataRate stable_bitrate,
                                                                   double framerate)
    : total_bitrate(total_bitrate)
    , stable_bitrate(stable_bitrate)
    , framerate(framerate)
{
}

VideoBitrateAllocationParameters::~VideoBitrateAllocationParameters() = default;

VideoBitrateAllocation VideoBitrateAllocator::GetAllocation(uint32_t total_bitrate_bps, uint32_t framerate)
{
    return Allocate({DataRate::BitsPerSec(total_bitrate_bps),
                     DataRate::BitsPerSec(total_bitrate_bps),
                     static_cast<double>(framerate)});
}

VideoBitrateAllocation VideoBitrateAllocator::Allocate(VideoBitrateAllocationParameters parameters)
{
    return GetAllocation(parameters.total_bitrate.bps(), parameters.framerate);
}

void VideoBitrateAllocator::SetLegacyConferenceMode(bool /* enabled */)
{
}

OCTK_END_NAMESPACE
