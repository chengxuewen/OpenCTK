/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2026~Present ChengXueWen.
** Copyright 2019 The WebRTC project authors. All Rights Reserved.
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

#include <octk_field_trials_view.hpp>
#include <private/octk_field_trial_parser_p.hpp>

OCTK_BEGIN_NAMESPACE

class StableTargetRateExperiment
{
public:
    explicit StableTargetRateExperiment(const FieldTrialsView &field_trials);
    StableTargetRateExperiment(const StableTargetRateExperiment &);
    StableTargetRateExperiment(StableTargetRateExperiment &&);

    bool IsEnabled() const;
    double GetVideoHysteresisFactor() const;
    double GetScreenshareHysteresisFactor() const;

private:
    FieldTrialParameter<bool> enabled_;
    FieldTrialParameter<double> video_hysteresis_factor_;
    FieldTrialParameter<double> screenshare_hysteresis_factor_;
};

OCTK_END_NAMESPACE
