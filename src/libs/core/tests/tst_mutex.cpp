/*
 *  Copyright 2020 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <octk_mutex.hpp>
#include <octk_event.hpp>
#include <octk_thread.hpp>
#include <octk_time_delta.hpp>

#include <benchmark/benchmark.h>
#include <gtest/gtest.h>

#include <stddef.h>
#include <stdint.h>

#include <atomic>
#include <memory>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

namespace octk
{
namespace
{

constexpr int kNumThreads = 16;

template <class MutexType>
class OCTK_LOCKABLE RawMutexLocker
{
public:
    explicit RawMutexLocker(MutexType &mutex) : mMutex(mutex) {}
    void Lock() OCTK_EXCLUSIVE_LOCK_FUNCTION() { mMutex.Lock(); }
    void Unlock() OCTK_UNLOCK_FUNCTION() { mMutex.Unlock(); }

private:
    MutexType &mMutex;
};

class OCTK_LOCKABLE RawMutexTryLocker
{
public:
    explicit RawMutexTryLocker(Mutex &mutex) : mMutex(mutex) {}
    void Lock() OCTK_EXCLUSIVE_LOCK_FUNCTION()
    {
        while (!mMutex.TryLock())
        {
            Thread::Yield();
        }
    }
    void Unlock() OCTK_UNLOCK_FUNCTION() { mMutex.Unlock(); }

private:
    Mutex &mMutex;
};

template <class MutexType, class MutexLockType>
class MutexLockLocker
{
public:
    explicit MutexLockLocker(MutexType &mutex) : mMutex(mutex) {}
    void Lock() { mLock = std::make_unique<MutexLockType>(&mMutex); }
    void Unlock() { mLock = nullptr; }

private:
    MutexType &mMutex;
    std::unique_ptr<MutexLockType> mLock;
};

template <class MutexType, class MutexLocker>
class LockRunner
{
public:
    template <typename... Args>
    explicit LockRunner(Args... args)
        : mThreadsActive(0)
        , mStartEvent(true, false)
        , mDoneEvent(true, false)
        , mSharedValue(0), mMutex(args...)
        , mLocker(mMutex) {}

    bool Run()
    {
        // Signal all threads to start.
        mStartEvent.Set();

        // Wait for all threads to finish.
        return mDoneEvent.Wait(kLongTime);
    }

    void SetExpectedThreadCount(int count) { mThreadsActive = count; }

    int shared_value()
    {
        int shared_value;
        mLocker.Lock();
        shared_value = mSharedValue;
        mLocker.Unlock();
        return shared_value;
    }

    void Loop()
    {
        ASSERT_TRUE(mStartEvent.Wait(kLongTime));
        mLocker.Lock();

        EXPECT_EQ(0, mSharedValue);
        int old = mSharedValue;

        // Use a loop to increase the chance of race. If the `mLocker`
        // implementation is faulty, it would be improbable that the error slips
        // through.
        for (int i = 0; i < kOperationsToRun; ++i)
        {
            benchmark::DoNotOptimize(++mSharedValue);
        }
        EXPECT_EQ(old + kOperationsToRun, mSharedValue);
        mSharedValue = 0;

        mLocker.Unlock();
        if (mThreadsActive.fetch_sub(1) == 1)
        {
            mDoneEvent.Set();
        }
    }

private:
    static constexpr TimeDelta kLongTime = TimeDelta::Seconds(10);
    static constexpr int kOperationsToRun = 1000;

    std::atomic<int> mThreadsActive;
    Event mStartEvent;
    Event mDoneEvent;
    int mSharedValue;
    MutexType mMutex;
    MutexLocker mLocker;
};

template <typename Runner>
void StartThreads(std::vector<std::unique_ptr<std::thread>> &threads, Runner *handler)
{
    for (int i = 0; i < kNumThreads; ++i)
    {
        auto thread = std::make_unique<std::thread>([handler] { handler->Loop(); });
        threads.push_back(std::move(thread));
    }
}

TEST(MutexTest, ProtectsSharedResourceWithMutexAndRawMutexLocker)
{
    std::vector<std::unique_ptr<std::thread>> threads;
    LockRunner<Mutex, RawMutexLocker<Mutex>> runner;
    StartThreads(threads, &runner);
    runner.SetExpectedThreadCount(kNumThreads);
    EXPECT_TRUE(runner.Run());
    EXPECT_EQ(0, runner.shared_value());
}

TEST(MutexTest, ProtectsSharedResourceWithMutexAndRawMutexTryLocker)
{
    std::vector<std::unique_ptr<std::thread>> threads;
    LockRunner<Mutex, RawMutexTryLocker> runner;
    StartThreads(threads, &runner);
    runner.SetExpectedThreadCount(kNumThreads);
    EXPECT_TRUE(runner.Run());
    EXPECT_EQ(0, runner.shared_value());
}

TEST(MutexTest, ProtectsSharedResourceWithMutexAndMutexLocker)
{
    std::vector<std::unique_ptr<std::thread>> threads;
    LockRunner<Mutex, MutexLockLocker<Mutex, MutexLock>> runner;
    StartThreads(threads, &runner);
    runner.SetExpectedThreadCount(kNumThreads);
    EXPECT_TRUE(runner.Run());
    EXPECT_EQ(0, runner.shared_value());
}

}  // namespace
}  // namespace webrtc
