/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
** Copyright 2018 The WebRTC project authors. All Rights Reserved.
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

#include <private/octk_field_trial_units_p.hpp>
#include <private/octk_field_trial_parser_p.hpp>

#include <string>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

OCTK_BEGIN_NAMESPACE

namespace
{
struct DummyExperiment
{
    FieldTrialParameter<DataRate> target_rate = FieldTrialParameter<DataRate>("t", DataRate::KilobitsPerSec(100));
    FieldTrialParameter<TimeDelta> period = FieldTrialParameter<TimeDelta>("p", TimeDelta::Millis(100));
    FieldTrialOptional<DataSize> max_buffer = FieldTrialOptional<DataSize>("b", utils::nullopt);

    explicit DummyExperiment(StringView field_trial)
    {
        ParseFieldTrial({&target_rate, &max_buffer, &period}, field_trial);
    }
};
} // namespace

TEST(FieldTrialParserUnitsTest, FallsBackToDefaults)
{
    DummyExperiment exp("");
    EXPECT_EQ(exp.target_rate.Get(), DataRate::KilobitsPerSec(100));
    EXPECT_FALSE(exp.max_buffer.GetOptional().has_value());
    EXPECT_EQ(exp.period.Get(), TimeDelta::Millis(100));
}
TEST(FieldTrialParserUnitsTest, ParsesUnitParameters)
{
    DummyExperiment exp("t:300kbps,b:5bytes,p:300ms");
    EXPECT_EQ(exp.target_rate.Get(), DataRate::KilobitsPerSec(300));
    EXPECT_EQ(*exp.max_buffer.GetOptional(), DataSize::Bytes(5));
    EXPECT_EQ(exp.period.Get(), TimeDelta::Millis(300));
}
TEST(FieldTrialParserUnitsTest, ParsesDefaultUnitParameters)
{
    DummyExperiment exp("t:300,b:5,p:300");
    EXPECT_EQ(exp.target_rate.Get(), DataRate::KilobitsPerSec(300));
    EXPECT_EQ(*exp.max_buffer.GetOptional(), DataSize::Bytes(5));
    EXPECT_EQ(exp.period.Get(), TimeDelta::Millis(300));
}
TEST(FieldTrialParserUnitsTest, ParsesInfinityParameter)
{
    DummyExperiment exp("t:inf,p:inf");
    EXPECT_EQ(exp.target_rate.Get(), DataRate::Infinity());
    EXPECT_EQ(exp.period.Get(), TimeDelta::PlusInfinity());
}
TEST(FieldTrialParserUnitsTest, ParsesOtherUnitParameters)
{
    DummyExperiment exp("t:300bps,p:0.3 seconds,b:8 bytes");
    EXPECT_EQ(exp.target_rate.Get(), DataRate::BitsPerSec(300));
    EXPECT_EQ(*exp.max_buffer.GetOptional(), DataSize::Bytes(8));
    EXPECT_EQ(exp.period.Get(), TimeDelta::Millis(300));
}
TEST(FieldTrialParserUnitsTest, IgnoresOutOfRange)
{
    FieldTrialConstrained<DataRate> rate("r",
                                         DataRate::KilobitsPerSec(30),
                                         DataRate::KilobitsPerSec(10),
                                         DataRate::KilobitsPerSec(100));
    FieldTrialConstrained<TimeDelta> delta("d", TimeDelta::Millis(30), TimeDelta::Millis(10), TimeDelta::Millis(100));
    FieldTrialConstrained<DataSize> size("s", DataSize::Bytes(30), DataSize::Bytes(10), DataSize::Bytes(100));
    ParseFieldTrial({&rate, &delta, &size}, "r:0,d:0,s:0");
    EXPECT_EQ(rate->kbps(), 30);
    EXPECT_EQ(delta->ms(), 30);
    EXPECT_EQ(size->bytes(), 30);
    ParseFieldTrial({&rate, &delta, &size}, "r:300,d:300,s:300");
    EXPECT_EQ(rate->kbps(), 30);
    EXPECT_EQ(delta->ms(), 30);
    EXPECT_EQ(size->bytes(), 30);
    ParseFieldTrial({&rate, &delta, &size}, "r:50,d:50,s:50");
    EXPECT_EQ(rate->kbps(), 50);
    EXPECT_EQ(delta->ms(), 50);
    EXPECT_EQ(size->bytes(), 50);
}

OCTK_END_NAMESPACE
