/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
** Copyright 2016 The WebRTC Project Authors.
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

#include <octk_thread_pool.hpp>
#include <octk_logging.hpp>

#include <list>
#include <atomic>
#include <thread>
#include <memory>
#include <random>
#include <utility>

#include <gtest/gtest.h>

OCTK_BEGIN_NAMESPACE

namespace
{
static std::atomic<int> testFunctionCount;
static std::mutex *functionTestMutex{nullptr};

void emptyFunct() { }

void noSleepTestFunction() { ++testFunctionCount; }

void noSleepTestFunctionMutex()
{
    assert(functionTestMutex);
    functionTestMutex->lock();
    ++testFunctionCount;
    functionTestMutex->unlock();
}

void sleepTestFunctionMutex()
{
    assert(functionTestMutex);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    functionTestMutex->lock();
    ++testFunctionCount;
    functionTestMutex->unlock();
}
} // namespace

TEST(ThreadPoolTest, RunFunction)
{
    {
        ThreadPool manager;
        testFunctionCount = 0;
        manager.start(noSleepTestFunction);
    }
    EXPECT_EQ(testFunctionCount, 1);
}

TEST(ThreadPoolTest, RunFunctionLambda)
{
    int localCount = 0;
    {
        ThreadPool manager;
        manager.start([&]() { ++localCount; });
    }
    EXPECT_EQ(localCount, 1);
}

TEST(ThreadPoolTest, CreateThreadRunFunction)
{
    {
        ThreadPool manager;
        testFunctionCount = 0;
        manager.start(noSleepTestFunction);
    }
    EXPECT_EQ(testFunctionCount, 1);
}

TEST(ThreadPoolTest, RunMultiple)
{
    const int runs = 10;
    std::mutex mutex;
    functionTestMutex = &mutex;

    {
        ThreadPool manager;
        testFunctionCount = 0;
        for (int i = 0; i < runs; ++i)
        {
            manager.start(sleepTestFunctionMutex);
        }
    }
    EXPECT_EQ(testFunctionCount, runs);

    for (int j = 0; j < 100; ++j)
    {
        {
            ThreadPool manager;
            testFunctionCount = 0;
            for (int i = 0; i < runs; ++i)
            {
                manager.start(noSleepTestFunctionMutex);
            }
        }
        EXPECT_EQ(testFunctionCount, runs);
    }

    {
        ThreadPool manager;
        for (int i = 0; i < 500; ++i)
        {
            manager.start(emptyFunct);
        }
    }
}

TEST(ThreadPoolTest, waitcomplete)
{
    testFunctionCount = 0;
    const int runs = 500;
    for (int i = 0; i < 500; ++i)
    {
        ThreadPool pool;
        pool.start(noSleepTestFunction);
    }
    EXPECT_EQ(testFunctionCount, runs);
}

OCTK_END_NAMESPACE