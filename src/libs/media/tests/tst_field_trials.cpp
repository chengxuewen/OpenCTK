/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
** Copyright 2014 The WebRTC project authors. All Rights Reserved.
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

#include <private/octk_field_trials_p.hpp>
#include <octk_string_encode.hpp>
#include <octk_logging.hpp>
#include <octk_checks.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <utility>
#include <string>
#include <map>

OCTK_BEGIN_NAMESPACE

namespace
{

using ::testing::NotNull;

TEST(FieldTrialsTest, EmptyStringHasNoEffect)
{
    FieldTrials f("");
    f.RegisterKeysForTesting({"MyCoolTrial"});

    EXPECT_FALSE(f.IsEnabled("MyCoolTrial"));
    EXPECT_FALSE(f.IsDisabled("MyCoolTrial"));
}

TEST(FieldTrialsTest, EnabledDisabledMustBeFirstInValue)
{
    FieldTrials f("MyCoolTrial/EnabledFoo/"
                  "MyUncoolTrial/DisabledBar/"
                  "AnotherTrial/BazEnabled/");
    f.RegisterKeysForTesting({"MyCoolTrial", "MyUncoolTrial", "AnotherTrial"});

    EXPECT_TRUE(f.IsEnabled("MyCoolTrial"));
    EXPECT_TRUE(f.IsDisabled("MyUncoolTrial"));
    EXPECT_FALSE(f.IsEnabled("AnotherTrial"));
}

TEST(FieldTrialsTest, FieldTrialsDoesNotReadGlobalString)
{
    FieldTrials f("");
    f.RegisterKeysForTesting({"MyCoolTrial", "MyUncoolTrial"});

    EXPECT_FALSE(f.IsEnabled("MyCoolTrial"));
    EXPECT_FALSE(f.IsDisabled("MyUncoolTrial"));
}

TEST(FieldTrialsTest, FieldTrialsSupportsSeparateInstances)
{
    {
        FieldTrials f("SomeString/Enabled/");
    }
    {
        FieldTrials f("SomeOtherString/Enabled/");
    }
}

TEST(FieldTrialsTest, FieldTrialsInstanceIsIsolated)
{
    std::unique_ptr<FieldTrials> f = FieldTrials::CreateNoGlobal("SomeString/Enabled/");
    ASSERT_THAT(f, NotNull());
    f->RegisterKeysForTesting({"SomeString"});

    EXPECT_TRUE(f->IsEnabled("SomeString"));
}

#if GTEST_HAS_DEATH_TEST && !defined(OCTK_OS_ANDROID)
TEST(FieldTrialsTest, FieldTrialsSupportsSimultaneousInstances)
{
    FieldTrials f1("SomeString/Enabled/");
    FieldTrials f2("SomeOtherString/Enabled/");

    EXPECT_TRUE(f1.IsEnabled("SomeString"));
    EXPECT_FALSE(f1.IsEnabled("SomeOtherString"));

    EXPECT_TRUE(f2.IsEnabled("SomeOtherString"));
    EXPECT_FALSE(f2.IsEnabled("SomeString"));
}
#endif // GTEST_HAS_DEATH_TEST && !defined(OCTK_OS_ANDROID)

} // namespace

OCTK_END_NAMESPACE
