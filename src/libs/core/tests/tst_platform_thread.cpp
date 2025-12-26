/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <octk_platform_thread.hpp>
#include <octk_task_event.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace octk;

TEST(PlatformThreadTest, DefaultConstructedIsEmpty)
{
    PlatformThread thread;
    EXPECT_EQ(thread.handle(), utils::nullopt);
    EXPECT_TRUE(thread.empty());
}

TEST(PlatformThreadTest, StartFinalize)
{
    PlatformThread thread = PlatformThread::SpawnJoinable([] {}, "1");
    EXPECT_NE(thread.handle(), utils::nullopt);
    EXPECT_FALSE(thread.empty());
    thread.Finalize();
    EXPECT_TRUE(thread.empty());
    Event done;
    thread = PlatformThread::SpawnDetached([&] { done.Set(); }, "2");
    EXPECT_FALSE(thread.empty());
    thread.Finalize();
    EXPECT_TRUE(thread.empty());
    done.Wait(TimeDelta::Seconds(30));
}

TEST(PlatformThreadTest, MovesEmpty)
{
    PlatformThread thread1;
    PlatformThread thread2 = std::move(thread1);
    EXPECT_TRUE(thread1.empty());
    EXPECT_TRUE(thread2.empty());
}

TEST(PlatformThreadTest, MovesHandles)
{
    PlatformThread thread1 = PlatformThread::SpawnJoinable([] {}, "1");
    PlatformThread thread2 = std::move(thread1);
    EXPECT_TRUE(thread1.empty());
    EXPECT_FALSE(thread2.empty());
    Event done;
    thread1 = PlatformThread::SpawnDetached([&] { done.Set(); }, "2");
    thread2 = std::move(thread1);
    EXPECT_TRUE(thread1.empty());
    EXPECT_FALSE(thread2.empty());
    done.Wait(TimeDelta::Seconds(30));
}

TEST(PlatformThreadTest, TwoThreadHandlesAreDifferentWhenStartedAndEqualWhenJoined)
{
    PlatformThread thread1 = PlatformThread();
    PlatformThread thread2 = PlatformThread();
    EXPECT_EQ(thread1.handle(), thread2.handle());
    thread1 = PlatformThread::SpawnJoinable([] {}, "1");
    thread2 = PlatformThread::SpawnJoinable([] {}, "2");
    EXPECT_NE(thread1.handle(), thread2.handle());
    thread1.Finalize();
    EXPECT_NE(thread1.handle(), thread2.handle());
    thread2.Finalize();
    EXPECT_EQ(thread1.handle(), thread2.handle());
}

TEST(PlatformThreadTest, RunFunctionIsCalled)
{
    bool flag = false;
    PlatformThread::SpawnJoinable([&] { flag = true; }, "T");
    EXPECT_TRUE(flag);
}

TEST(PlatformThreadTest, JoinsThread)
{
    // This test flakes if there are problems with the join implementation.
    Event event;
    PlatformThread::SpawnJoinable([&] { event.Set(); }, "T");
    EXPECT_TRUE(event.Wait(/*give_up_after=*/TimeDelta::Zero()));
}

TEST(PlatformThreadTest, StopsBeforeDetachedThreadExits)
{
    // This test flakes if there are problems with the detached thread
    // implementation.
    bool flag = false;
    Event thread_started;
    Event thread_continue;
    Event thread_exiting;
    PlatformThread::SpawnDetached(
        [&] {
            thread_started.Set();
            thread_continue.Wait(Event::foreverDuration());
            flag = true;
            thread_exiting.Set();
        },
        "T");
    thread_started.Wait(Event::foreverDuration());
    EXPECT_FALSE(flag);
    thread_continue.Set();
    thread_exiting.Wait(Event::foreverDuration());
    EXPECT_TRUE(flag);
}
