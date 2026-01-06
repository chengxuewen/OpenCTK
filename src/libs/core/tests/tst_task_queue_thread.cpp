/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
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

#include <octk_task_queue_thread.hpp>
#include <octk_repeating_task.hpp>
#include <octk_elapsed_timer.hpp>
#include <octk_semaphore.hpp>
#include <octk_logging.hpp>
#include <octk_utility.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <utility>
#include <vector>

OCTK_BEGIN_NAMESPACE

namespace
{
using ::testing::AtLeast;
using ::testing::Invoke;
using ::testing::MockFunction;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::WithArg;

class MockClosure
{
public:
    MOCK_METHOD(TimeDelta, Call, ());
    MOCK_METHOD(void, Delete, ());
};

// NOTE: Since this utility class holds a raw pointer to a variable that likely
// lives on the stack, it's important that any repeating tasks that use this
// class be explicitly stopped when the test criteria have been met. If the
// task is not stopped, an instance of this class can be deleted when the
// pointed-to MockClosure has been deleted and we end up trying to call a
// virtual method on a deleted object in the dtor.
class MoveOnlyClosure
{
public:
    explicit MoveOnlyClosure(MockClosure *mock)
        : mMock(mock)
    {
    }
    MoveOnlyClosure(const MoveOnlyClosure &) = delete;
    MoveOnlyClosure(MoveOnlyClosure &&other)
        : mMock(other.mMock)
    {
        other.mMock = nullptr;
    }
    ~MoveOnlyClosure()
    {
        if (mMock)
        {
            mMock->Delete();
        }
    }
    TimeDelta operator()() { return mMock->Call(); }

private:
    MockClosure *mMock;
};

OCTK_CXX14_CONSTEXPR TimeDelta kTimeout = TimeDelta::Millis(1000);
} // namespace

TEST(TaskQueueThreadTest, PostDelayedTask)
{
    std::mutex mutex;
    ElapsedTimer timer;
    std::condition_variable condition;
    auto taskQueueThread = TaskQueueThread::makeShared();
    timer.start();
    taskQueueThread->postDelayedTask(
        [taskQueueThread, &condition]()
        {
            EXPECT_TRUE(taskQueueThread->isCurrent());
            condition.notify_one();
        },
        TimeDelta::Millis(3));
    std::unique_lock<std::mutex> lock(mutex);
    EXPECT_EQ(std::cv_status::no_timeout, condition.wait_for(lock, std::chrono::seconds(1)));
    const auto elapsed = timer.elapsed();
    OCTK_DEBUG("TaskQueueThreadTest::PostDelayedTask: elapsed %dms", elapsed);
    EXPECT_GE(elapsed, 3);
}

TEST(RepeatingTaskTest, CancelDelayedTaskBeforeItRuns)
{
    Semaphore done;
    MockClosure mock;
    EXPECT_CALL(mock, Call).Times(0);
    EXPECT_CALL(mock, Delete).WillOnce(Invoke([&done] { done.release(); }));
    auto taskQueueThread = TaskQueueThread::makeShared();
    auto handle = RepeatingTaskHandle::delayedStart(taskQueueThread.get(),
                                                    TimeDelta::Millis(100),
                                                    MoveOnlyClosure(&mock));
    {
        auto handleMove = utils::makeMoveWrapper(std::move(handle));
        taskQueueThread->postTask([handleMove]() mutable { handleMove.move().stop(); });
    }
    EXPECT_TRUE(done.tryAcquireFor(1, std::chrono::microseconds(kTimeout.us())));
}

TEST(RepeatingTaskTest, CancelTaskAfterItRuns)
{
    Semaphore done;
    MockClosure mock;
    EXPECT_CALL(mock, Call).WillOnce(Return(TimeDelta::Millis(100)));
    EXPECT_CALL(mock, Delete).WillOnce(Invoke([&done] { done.release(); }));
    auto taskQueueThread = TaskQueueThread::makeShared();
    auto handle = RepeatingTaskHandle::start(taskQueueThread.get(), MoveOnlyClosure(&mock));
    {
        auto handleMove = utils::makeMoveWrapper(std::move(handle));
        taskQueueThread->postTask([handleMove]() mutable { handleMove.move().stop(); });
    }
    EXPECT_TRUE(done.tryAcquireFor(1, std::chrono::microseconds(kTimeout.us())));
}

TEST(RepeatingTaskTest, ZeroReturnValueRepostsTheTask)
{
    NiceMock<MockClosure> closure;
    Semaphore done;
    ElapsedTimer timer;
    EXPECT_CALL(closure, Call())
        .WillOnce(Return(TimeDelta::Zero()))
        .WillOnce(Invoke(
            [&]
            {
                done.release();
                return TimeDelta::PlusInfinity();
            }));
    auto taskQueueThread = TaskQueueThread::makeShared();
    timer.start();
    RepeatingTaskHandle::start(taskQueueThread.get(), MoveOnlyClosure(&closure));
    EXPECT_TRUE(done.tryAcquireFor(1, std::chrono::microseconds(kTimeout.us()))) << "elapsed:" << timer.elapsed();
}

TEST(RepeatingTaskTest, StartPeriodicTask)
{
    MockFunction<TimeDelta()> closure;
    Semaphore done;
    EXPECT_CALL(closure, Call())
        .WillOnce(Return(TimeDelta::Millis(20)))
        .WillOnce(Return(TimeDelta::Millis(20)))
        .WillOnce(Invoke(
            [&]
            {
                done.release();
                return TimeDelta::PlusInfinity();
            }));
    auto taskQueueThread = TaskQueueThread::makeShared();
    RepeatingTaskHandle::start(taskQueueThread.get(), closure.AsStdFunction());
    EXPECT_TRUE(done.tryAcquireFor(1, std::chrono::microseconds(kTimeout.us())));
}

TEST(RepeatingTaskTest, Example)
{
    class ObjectOnTaskQueue
    {
    public:
        void DoPeriodicTask() { }
        TimeDelta TimeUntilNextRun() { return TimeDelta::Millis(100); }
        void StartPeriodicTask(RepeatingTaskHandle *handle, TaskQueueBase *taskQueueThread)
        {
            *handle = RepeatingTaskHandle::start(taskQueueThread,
                                                 [this]
                                                 {
                                                     DoPeriodicTask();
                                                     return TimeUntilNextRun();
                                                 });
        }
    };
    auto taskQueueThread = TaskQueueThread::makeShared();
    auto object = utils::make_unique<ObjectOnTaskQueue>();
    // Create and start the periodic task.
    RepeatingTaskHandle handle;
    object->StartPeriodicTask(&handle, taskQueueThread.get());
    // Restart the task
    {
        auto handleMove = utils::makeMoveWrapper(std::move(handle));
        taskQueueThread->postTask([handleMove]() mutable { handleMove.move().stop(); });
    }
    object->StartPeriodicTask(&handle, taskQueueThread.get());
    {
        auto handleMove = utils::makeMoveWrapper(std::move(handle));
        taskQueueThread->postTask([handleMove]() mutable { handleMove.move().stop(); });
    }
    struct Destructor
    {
        void operator()() { object.reset(); }
        std::unique_ptr<ObjectOnTaskQueue> object;
    };
    taskQueueThread->postTask(Destructor{std::move(object)});
    // Do not wait for the destructor closure in order to create a race between
    // task queue destruction and running the desctructor closure.
}

OCTK_END_NAMESPACE