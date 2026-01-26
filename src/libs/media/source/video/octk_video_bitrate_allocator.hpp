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

#pragma once

#include <octk_video_bitrate_allocation.hpp>
#include <octk_data_rate.hpp>

OCTK_BEGIN_NAMESPACE

struct VideoBitrateAllocationParameters
{
    VideoBitrateAllocationParameters(uint32_t total_bitrate_bps, uint32_t framerate);
    VideoBitrateAllocationParameters(DataRate total_bitrate, double framerate);
    VideoBitrateAllocationParameters(DataRate total_bitrate, DataRate stable_bitrate, double framerate);
    ~VideoBitrateAllocationParameters();

    DataRate total_bitrate;
    DataRate stable_bitrate;
    double framerate;
};

class VideoBitrateAllocator
{
public:
    VideoBitrateAllocator() { }
    virtual ~VideoBitrateAllocator() { }

    virtual VideoBitrateAllocation GetAllocation(uint32_t total_bitrate_bps, uint32_t framerate);

    virtual VideoBitrateAllocation Allocate(VideoBitrateAllocationParameters parameters);

    // Deprecated: Only used to work around issues with the legacy conference
    // screenshare mode and shouldn't be needed by any subclasses.
    virtual void SetLegacyConferenceMode(bool enabled);
};

class VideoBitrateAllocationObserver
{
public:
    VideoBitrateAllocationObserver() { }
    virtual ~VideoBitrateAllocationObserver() { }

    virtual void OnBitrateAllocationUpdated(const VideoBitrateAllocation &allocation) = 0;
};

OCTK_END_NAMESPACE