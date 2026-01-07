/*
 *  Copyright 2020 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <octk_task_event.hpp>
#include <octk_time_delta.hpp>
#include <octk_memory.hpp>
#include <octk_mutex.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <benchmark/benchmark.h>

#include <type_traits>
#include <cstddef>
#include <utility>
#include <cstdint>
#include <atomic>
#include <memory>
#include <thread>
#include <vector>

#if 0
using namespace octk;

namespace
{

constexpr int kNumThreads = 16;

template <class MutexType> class OCTK_ATTRIBUTE_LOCKABLE RawMutexLocker
{
public:
    explicit RawMutexLocker(MutexType &mutex)
        : mMutex(mutex)
    {
    }
    void lock() OCTK_ATTRIBUTE_EXCLUSIVE_LOCK_FUNCTION() { mMutex.lock(); }
    void unlock() OCTK_ATTRIBUTE_UNLOCK_FUNCTION() { mMutex.unlock(); }

private:
    MutexType &mMutex;
};

class OCTK_ATTRIBUTE_LOCKABLE RawMutexTryLocker
{
public:
    explicit RawMutexTryLocker(Mutex &mutex)
        : mMutex(mutex)
    {
    }
    void lock() OCTK_ATTRIBUTE_EXCLUSIVE_LOCK_FUNCTION()
    {
        while (!mMutex.try_lock())
        {
            std::this_thread::yield();
        }
    }
    void unlock() OCTK_ATTRIBUTE_UNLOCK_FUNCTION() { mMutex.unlock(); }

private:
    Mutex &mMutex;
};

template <class MutexType, class MutexlockType> class Mutexlocklocker
{
public:
    explicit Mutexlocklocker(MutexType &mutex)
        : mMutex(mutex)
    {
    }
    void lock() { mlock = utils::make_unique<MutexlockType>(mMutex); }
    void unlock() { mlock = nullptr; }

private:
    MutexType &mMutex;
    std::unique_ptr<MutexlockType> mlock;
};

template <class MutexType, class Mutexlocker> class LockRunner
{
public:
    template <typename... Args>
    explicit LockRunner(Args... args)
        : mThreadsActive(0)
        , mStartEvent(true, false)
        , mDoneEvent(true, false)
        , mSharedValue(0)
        , mMutex(args...)
        , mlocker(mMutex)
    {
    }

    bool Run()
    {
        // Signal all threads to start.
       // mStartEvent.Set();

        // Wait for all threads to finish.
        return mDoneEvent.Wait(kLongTime());
    }

    void SetExpectedThreadCount(int count) { mThreadsActive = count; }

    int shared_value()
    {
        int shared_value;
        mlocker.lock();
        shared_value = mSharedValue;
        mlocker.unlock();
        return shared_value;
    }

    void Loop()
    {
        ASSERT_TRUE(mStartEvent.Wait(kLongTime()));
        mlocker.lock();

        EXPECT_EQ(0, mSharedValue);
        int old = mSharedValue;

        // Use a loop to increase the chance of race. If the `mlocker`
        // implementation is faulty, it would be improbable that the error slips
        // through.
        for (int i = 0; i < kOperationsToRun; ++i)
        {
            benchmark::DoNotOptimize(++mSharedValue);
        }
        EXPECT_EQ(old + kOperationsToRun, mSharedValue);
        mSharedValue = 0;

        mlocker.unlock();
        if (mThreadsActive.fetch_sub(1) == 1)
        {
            mDoneEvent.Set();
        }
    }

private:
    static TimeDelta kLongTime() { return TimeDelta::Seconds(10); }
    static constexpr int kOperationsToRun = 1000;

    std::atomic<int> mThreadsActive;
    //Event mStartEvent;
    //Event mDoneEvent;
    int mSharedValue;
    MutexType mMutex;
    Mutexlocker mlocker;
};

template <typename Runner> void StartThreads(std::vector<std::unique_ptr<std::thread>> &threads, Runner *handler)
{
    for (int i = 0; i < kNumThreads; ++i)
    {
        auto thread = utils::make_unique<std::thread>([handler, i]() { handler->Loop(); });
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
    for (auto &&thread : threads)
    {
        thread->join();
    }
}

TEST(MutexTest, ProtectsSharedResourceWithMutexAndRawMutexTryLocker)
{
    std::vector<std::unique_ptr<std::thread>> threads;
    LockRunner<Mutex, RawMutexTryLocker> runner;
    StartThreads(threads, &runner);
    runner.SetExpectedThreadCount(kNumThreads);
    EXPECT_TRUE(runner.Run());
    EXPECT_EQ(0, runner.shared_value());
    for (auto &&thread : threads)
    {
        thread->join();
    }
}

TEST(MutexTest, ProtectsSharedResourceWithMutexAndMutexlocker)
{
    std::vector<std::unique_ptr<std::thread>> threads;
    LockRunner<Mutex, Mutexlocklocker<Mutex, Mutex::Lock>> runner;
    StartThreads(threads, &runner);
    runner.SetExpectedThreadCount(kNumThreads);
    EXPECT_TRUE(runner.Run());
    EXPECT_EQ(0, runner.shared_value());
    for (auto &&thread : threads)
    {
        thread->join();
    }
}
} // namespace
#endif