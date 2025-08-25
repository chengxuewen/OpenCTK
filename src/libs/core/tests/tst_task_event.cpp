/*
 *  Copyright 2004 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <octk_platform_thread.hpp>
#include <octk_task_event.hpp>
#include <octk_time_delta.hpp>
#include <octk_date_time.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace octk;

TEST(EventTest, InitiallySignaled)
{
    Event event(false, true);
    ASSERT_TRUE(event.Wait(TimeDelta::Zero()));
}

TEST(EventTest, ManualReset)
{
    Event event(true, false);
    ASSERT_FALSE(event.Wait(TimeDelta::Zero()));

    event.Set();
    ASSERT_TRUE(event.Wait(TimeDelta::Zero()));
    ASSERT_TRUE(event.Wait(TimeDelta::Zero()));

    event.Reset();
    ASSERT_FALSE(event.Wait(TimeDelta::Zero()));
}

TEST(EventTest, AutoReset)
{
    Event event;
    ASSERT_FALSE(event.Wait(TimeDelta::Zero()));

    event.Set();
    ASSERT_TRUE(event.Wait(TimeDelta::Zero()));
    ASSERT_FALSE(event.Wait(TimeDelta::Zero()));
}

class SignalerThread
{
public:
    void Start(Event *writer, Event *reader)
    {
        writer_ = writer;
        reader_ = reader;
        thread_ = PlatformThread::SpawnJoinable(
            [this] {
                while (!stop_event_.Wait(TimeDelta::Zero()))
                {
                    writer_->Set();
                    reader_->Wait(Event::foreverDuration());
                }
            },
            "EventPerf");
    }
    void Stop()
    {
        stop_event_.Set();
        thread_.Finalize();
    }
    Event stop_event_;
    Event *writer_;
    Event *reader_;
    PlatformThread thread_;
};

TEST(EventTest, UnsignaledWaitDoesNotReturnBeforeTimeout)
{
    OCTK_CXX14_CONSTEXPR TimeDelta kDuration = TimeDelta::Micros(10499);
    Event event;
    auto begin = DateTime::TimeMicros();
    EXPECT_FALSE(event.Wait(kDuration));
    EXPECT_GE(DateTime::TimeMicros(), begin + kDuration.us());
}

// These tests are disabled by default and only intended to be run manually.
TEST(EventTest, PerformanceSingleThread) // DISABLED_
{
    static const int kNumIterations = 10000000;
    Event event;
    for (int i = 0; i < kNumIterations; ++i)
    {
        event.Set();
        event.Wait(TimeDelta::Zero());
    }
}

TEST(EventTest, PerformanceMultiThread) // DISABLED_
{
    static const int kNumIterations = 10000;
    Event read;
    Event write;
    SignalerThread thread;
    thread.Start(&read, &write);

    for (int i = 0; i < kNumIterations; ++i)
    {
        write.Set();
        read.Wait(Event::foreverDuration());
    }
    write.Set();

    thread.Stop();
}

#if OCTK_DCHECK_IS_ON && GTEST_HAS_DEATH_TEST && !defined(OCTK_OS_ANDROID)
// Tests that we crash if we attempt to call Event::Wait while we're
// not allowed to (as per `OCTK_DISALLOW_WAIT()`).
TEST(EventTestDeathTest, DisallowEventWait)
{
    Event event;
    OCTK_DISALLOW_WAIT();
    EXPECT_DEATH(event.Wait(Event::foreverDuration()), "");
}
#endif  // OCTK_DCHECK_IS_ON && GTEST_HAS_DEATH_TEST && !defined(OCTK_OS_ANDROID)
