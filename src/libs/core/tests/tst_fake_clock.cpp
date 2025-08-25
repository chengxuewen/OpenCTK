/*
 *  Copyright 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <octk_fake_clock.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace octk;

TEST(ScopedFakeClockTest, OverridesGlobalClock)
{
    const int64_t kFixedTimeUs = 100000;
    int64_t real_time_us = DateTime::TimeMicros();
    EXPECT_NE(real_time_us, 0);
    {
        ScopedFakeClock scoped;
        EXPECT_EQ(DateTime::TimeMicros(), 0);

        scoped.AdvanceTime(TimeDelta::Millis(1));
        EXPECT_EQ(DateTime::TimeMicros(), 1000);

        scoped.SetTime(Timestamp::Micros(kFixedTimeUs));
        EXPECT_EQ(DateTime::TimeMicros(), kFixedTimeUs);

        scoped.AdvanceTime(TimeDelta::Millis(1));
        EXPECT_EQ(DateTime::TimeMicros(), kFixedTimeUs + 1000);
    }

    EXPECT_NE(DateTime::TimeMicros(), kFixedTimeUs + 1000);
    EXPECT_GE(DateTime::TimeMicros(), real_time_us);
}
