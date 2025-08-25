/*
 *  Copyright 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <octk_task_queue_for_test.hpp>
#include <octk_repeating_task.hpp>
#include <octk_task_queue.hpp>
#include <octk_time_delta.hpp>
#include <octk_task_event.hpp>
#include <octk_timestamp.hpp>
#include <octk_optional.hpp>
#include <octk_memory.hpp>
#include <octk_clock.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <atomic>
#include <memory>
#include <optional>

// #include "absl/functional/any_invocable.h"
// #include "api/task_queue/test/mock_task_queue_base.h"

// NOTE: Since these tests rely on real time behavior, they will be flaky
// if run on heavily loaded systems.
using namespace octk;

namespace
{
using ::testing::AtLeast;
using ::testing::Invoke;
using ::testing::MockFunction;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::WithArg;

OCTK_CXX14_CONSTEXPR TimeDelta kTimeout = TimeDelta::Millis(1000);

class MockClosure
{
public:
    MOCK_METHOD(TimeDelta, Call, ());
    MOCK_METHOD(void, Delete, ());
};

class MockTaskQueue : public TaskQueue
{
public:
    using TaskQueue::PostDelayedTaskTraits;
    using TaskQueue::PostTaskTraits;

public:
    MockTaskQueue() : task_queue_setter_(this) {}
    MOCK_METHOD(void, Delete, (), (override));
    MOCK_METHOD(void,
                PostTaskImpl,
                (TaskQueue::Task, const PostTaskTraits&, const SourceLocation&),
                (override));
    MOCK_METHOD(void,
                PostDelayedTaskImpl,
                (TaskQueue::Task, TimeDelta, const PostDelayedTaskTraits&, const SourceLocation&),
                (override));
private:
    CurrentTaskQueueSetter task_queue_setter_;
};

class FakeTaskQueue : public TaskQueue
{
public:
    explicit FakeTaskQueue(SimulatedClock *clock)
        : task_queue_setter_(this), clock_(clock) {}

    void Delete() override {}

    void PostTaskImpl(TaskQueue::Task task,
                      const PostTaskTraits & /*traits*/,
                      const SourceLocation & /*location*/) override
    {
        last_task_ = std::move(task);
        last_precision_ = utils::nullopt;
        last_delay_ = TimeDelta::Zero();
    }

    void PostDelayedTaskImpl(TaskQueue::Task task,
                             TimeDelta delay,
                             const PostDelayedTaskTraits &traits,
                             const SourceLocation & /*location*/) override
    {
        last_task_ = std::move(task);
        last_precision_ = traits.high_precision
                          ? TaskQueue::DelayPrecision::kHigh
                          : TaskQueue::DelayPrecision::kLow;
        last_delay_ = delay;
    }

    bool AdvanceTimeAndRunLastTask()
    {
        EXPECT_TRUE(last_task_);
        EXPECT_TRUE(last_delay_.IsFinite());
        clock_->AdvanceTime(last_delay_);
        last_delay_ = TimeDelta::MinusInfinity();
        auto task = std::move(last_task_);
        std::move(task)();
        return last_task_ == nullptr;
    }

    bool IsTaskQueued() { return !!last_task_; }

    TimeDelta last_delay() const
    {
        EXPECT_TRUE(last_delay_.IsFinite());
        return last_delay_;
    }

    Optional<TaskQueue::DelayPrecision> last_precision() const
    {
        return last_precision_;
    }

private:
    CurrentTaskQueueSetter task_queue_setter_;
    SimulatedClock *clock_;
    TaskQueue::Task last_task_;
    TimeDelta last_delay_ = TimeDelta::MinusInfinity();
    Optional<TaskQueue::DelayPrecision> last_precision_;
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
    explicit MoveOnlyClosure(MockClosure *mock) : mock_(mock) {}
    MoveOnlyClosure(const MoveOnlyClosure &) = delete;
    MoveOnlyClosure(MoveOnlyClosure &&other) : mock_(other.mock_)
    {
        other.mock_ = nullptr;
    }
    ~MoveOnlyClosure()
    {
        if (mock_)
        {
            mock_->Delete();
        }
    }
    TimeDelta operator()() { return mock_->Call(); }

private:
    MockClosure *mock_;
};
}  // namespace

TEST(RepeatingTaskTest, TaskIsStoppedOnStop)
{
    const TimeDelta kShortInterval = TimeDelta::Millis(50);

    SimulatedClock clock(Timestamp::Zero());
    FakeTaskQueue task_queue(&clock);
    std::atomic_int counter(0);
    auto handle = RepeatingTaskHandle::Start(
        &task_queue,
        [&] {
            counter++;
            return kShortInterval;
        },
        TaskQueue::DelayPrecision::kLow, &clock);
    EXPECT_EQ(task_queue.last_delay(), TimeDelta::Zero());
    EXPECT_FALSE(task_queue.AdvanceTimeAndRunLastTask());
    EXPECT_EQ(counter.load(), 1);

// The handle reposted at the short interval.
    EXPECT_EQ(task_queue.last_delay(), kShortInterval);

// Stop the handle. This prevernts the counter from incrementing.
    handle.Stop();
    EXPECT_TRUE(task_queue.AdvanceTimeAndRunLastTask());
    EXPECT_EQ(counter.load(), 1);
}

TEST(RepeatingTaskTest, CompensatesForLongRunTime)
{
    const TimeDelta kRepeatInterval = TimeDelta::Millis(2);
// Sleeping inside the task for longer than the repeat interval once, should
// be compensated for by repeating the task faster to catch up.
    const TimeDelta kSleepDuration = TimeDelta::Millis(20);

    std::atomic_int counter(0);
    SimulatedClock clock(Timestamp::Zero());
    FakeTaskQueue task_queue(&clock);
    RepeatingTaskHandle::Start(&task_queue,
                               [&] {
                                   ++counter;
// Task takes longer than the repeat duration.
                                   clock.AdvanceTime(kSleepDuration);
                                   return kRepeatInterval;
                               },
                               TaskQueue::DelayPrecision::kLow, &clock);

    EXPECT_EQ(task_queue.last_delay(), TimeDelta::Zero());
    EXPECT_FALSE(task_queue.AdvanceTimeAndRunLastTask());

// Task is posted right away since it took longer to run then the repeat
// interval.
    EXPECT_EQ(task_queue.last_delay(), TimeDelta::Zero());
    EXPECT_EQ(counter.load(), 1);
}

TEST(RepeatingTaskTest, CompensatesForShortRunTime)
{
    SimulatedClock clock(Timestamp::Zero());
    FakeTaskQueue task_queue(&clock);
    std::atomic_int counter(0);
    RepeatingTaskHandle::Start(&task_queue,
                               [&] {
// Simulate the task taking 100ms, which should be compensated for.
                                   counter++;
                                   clock.AdvanceTime(TimeDelta::Millis(100));
                                   return TimeDelta::Millis(300);
                               },
                               TaskQueue::DelayPrecision::kLow, &clock);

// Expect instant post task.
    EXPECT_EQ(task_queue.last_delay(), TimeDelta::Zero());
// Task should be retained by the handler since it is not cancelled.
    EXPECT_FALSE(task_queue.AdvanceTimeAndRunLastTask());
// New delay should be 200ms since repeat delay was 300ms but task took 100ms.
    EXPECT_EQ(task_queue.last_delay(), TimeDelta::Millis(200));
}

TEST(RepeatingTaskTest, CancelDelayedTaskBeforeItRuns)
{
    Event done;
    MockClosure mock;
    EXPECT_CALL(mock, Call).Times(0);
    EXPECT_CALL(mock, Delete).WillOnce(Invoke([&done] { done.Set(); }));
    TaskQueueForTest task_queue("queue");
    auto handle = RepeatingTaskHandle::DelayedStart(task_queue.Get(), TimeDelta::Millis(100), MoveOnlyClosure(&mock));
    {
        auto handleMove = utils::makeMoveWrapper(std::move(handle));
        task_queue.PostTask([handleMove]() mutable { handleMove.move().Stop(); });
    }
    EXPECT_TRUE(done.Wait(kTimeout));
}

TEST(RepeatingTaskTest, CancelTaskAfterItRuns)
{
    Event done;
    MockClosure mock;
    EXPECT_CALL(mock, Call).WillOnce(Return(TimeDelta::Millis(100)));
    EXPECT_CALL(mock, Delete).WillOnce(Invoke([&done] { done.Set(); }));
    TaskQueueForTest task_queue("queue");
    auto handle = RepeatingTaskHandle::Start(task_queue.Get(), MoveOnlyClosure(&mock));
    {
        auto handleMove = utils::makeMoveWrapper(std::move(handle));
        task_queue.PostTask([handleMove]() mutable { handleMove.move().Stop(); });
    }
    EXPECT_TRUE(done.Wait(kTimeout));
}

TEST(RepeatingTaskTest, TaskCanStopItself)
{
    std::atomic_int counter(0);
    SimulatedClock clock(Timestamp::Zero());
    FakeTaskQueue task_queue(&clock);
    RepeatingTaskHandle handle = RepeatingTaskHandle::Start(&task_queue, [&] {
        ++counter;
        handle.Stop();
        return TimeDelta::Millis(2);
    });
    EXPECT_EQ(task_queue.last_delay(), TimeDelta::Zero());
// Task cancelled itself so wants to be released.
    EXPECT_TRUE(task_queue.AdvanceTimeAndRunLastTask());
    EXPECT_EQ(counter.load(), 1);
}

TEST(RepeatingTaskTest, TaskCanStopItselfByReturningInfinity)
{
    std::atomic_int counter(0);
    SimulatedClock clock(Timestamp::Zero());
    FakeTaskQueue task_queue(&clock);
    RepeatingTaskHandle handle = RepeatingTaskHandle::Start(&task_queue, [&] {
        ++counter;
        return TimeDelta::PlusInfinity();
    });
    EXPECT_EQ(task_queue.last_delay(), TimeDelta::Zero());
// Task cancelled itself so wants to be released.
    EXPECT_TRUE(task_queue.AdvanceTimeAndRunLastTask());
    EXPECT_EQ(counter.load(), 1);
}

TEST(RepeatingTaskTest, ZeroReturnValueRepostsTheTask)
{
    NiceMock<MockClosure> closure;
    Event done;
    EXPECT_CALL(closure, Call())
        .WillOnce(Return(TimeDelta::Zero()))
        .WillOnce(Invoke([&] {
            done.Set();
            return TimeDelta::PlusInfinity();
        }));
    TaskQueueForTest task_queue("queue");
    RepeatingTaskHandle::Start(task_queue.Get(), MoveOnlyClosure(&closure));
    EXPECT_TRUE(done.Wait(kTimeout));
}

TEST(RepeatingTaskTest, StartPeriodicTask)
{
    MockFunction<TimeDelta()> closure;
    Event done;
    EXPECT_CALL(closure, Call())
        .WillOnce(Return(TimeDelta::Millis(20)))
        .WillOnce(Return(TimeDelta::Millis(20)))
        .WillOnce(Invoke([&] {
            done.Set();
            return TimeDelta::PlusInfinity();
        }));
    TaskQueueForTest task_queue("queue");
    RepeatingTaskHandle::Start(task_queue.Get(), closure.AsStdFunction());
    EXPECT_TRUE(done.Wait(kTimeout));
}

TEST(RepeatingTaskTest, Example)
{
    class ObjectOnTaskQueue
    {
    public:
        void DoPeriodicTask() {}
        TimeDelta TimeUntilNextRun() { return TimeDelta::Millis(100); }
        void StartPeriodicTask(RepeatingTaskHandle *handle,
                               TaskQueue *task_queue)
        {
            *handle = RepeatingTaskHandle::Start(task_queue, [this] {
                DoPeriodicTask();
                return TimeUntilNextRun();
            });
        }
    };
    TaskQueueForTest task_queue("queue");
    auto object = utils::make_unique<ObjectOnTaskQueue>();
    // Create and start the periodic task.
    RepeatingTaskHandle handle;
    object->StartPeriodicTask(&handle, task_queue.Get());
    // Restart the task
    {
        auto handleMove = utils::makeMoveWrapper(std::move(handle));
        task_queue.PostTask([handleMove]() mutable { handleMove.move().Stop(); });
    }
    object->StartPeriodicTask(&handle, task_queue.Get());
    {
        auto handleMove = utils::makeMoveWrapper(std::move(handle));
        task_queue.PostTask([handleMove]() mutable { handleMove.move().Stop(); });
    }
    struct Destructor
    {
        void operator()() { object.reset(); }
        std::unique_ptr<ObjectOnTaskQueue> object;
    };
    task_queue.PostTask(Destructor{std::move(object)});
// Do not wait for the destructor closure in order to create a race between
// task queue destruction and running the desctructor closure.
}

TEST(RepeatingTaskTest, ClockIntegration)
{
    TaskQueue::Task delayed_task;
    TimeDelta expected_delay = TimeDelta::Zero();
    SimulatedClock clock(Timestamp::Zero());

    NiceMock<MockTaskQueue> task_queue;
    ON_CALL(task_queue, PostDelayedTaskImpl)
        .WillByDefault([&](TaskQueue::Task task, TimeDelta delay,
                           const MockTaskQueue::PostDelayedTaskTraits &,
                           const SourceLocation &) {
            EXPECT_EQ(delay, expected_delay);
            delayed_task = std::move(task);
        });

    expected_delay = TimeDelta::Millis(100);
    RepeatingTaskHandle handle = RepeatingTaskHandle::DelayedStart(
        &task_queue, TimeDelta::Millis(100),
        [&clock]() {
            EXPECT_EQ(Timestamp::Millis(100), clock.CurrentTime());
            // Simulate work happening for 10ms.
            clock.AdvanceTimeMilliseconds(10);
            return TimeDelta::Millis(100);
        },
        TaskQueue::DelayPrecision::kLow, &clock);

    clock.AdvanceTimeMilliseconds(100);
    TaskQueue::Task task_to_run = std::move(delayed_task);
    expected_delay = TimeDelta::Millis(90);
    std::move(task_to_run)();
    EXPECT_NE(delayed_task, nullptr);
    handle.Stop();
}

TEST(RepeatingTaskTest, CanBeStoppedAfterTaskQueueDeletedTheRepeatingTask)
{
    TaskQueue::Task repeating_task;

    MockTaskQueue task_queue;
    EXPECT_CALL(task_queue, PostDelayedTaskImpl)
        .WillOnce(WithArg<0>([&](TaskQueue::Task task) {
            repeating_task = std::move(task);
        }));

    RepeatingTaskHandle handle =
        RepeatingTaskHandle::DelayedStart(&task_queue, TimeDelta::Millis(100),
                                          [] { return TimeDelta::Millis(100); });

// shutdown task queue: delete all pending tasks and run 'regular' task.
    repeating_task = nullptr;
    handle.Stop();
}

TEST(RepeatingTaskTest, DefaultPrecisionIsLow)
{
    SimulatedClock clock(Timestamp::Zero());
    FakeTaskQueue task_queue(&clock);
// Closure that repeats twice.
    MockFunction<TimeDelta()> closure;
    EXPECT_CALL(closure, Call())
        .WillOnce(Return(TimeDelta::Millis(1)))
        .WillOnce(Return(TimeDelta::PlusInfinity()));
    RepeatingTaskHandle::Start(&task_queue, closure.AsStdFunction());
// Initial task is a PostTask().
    EXPECT_FALSE(task_queue.last_precision().has_value());
    EXPECT_FALSE(task_queue.AdvanceTimeAndRunLastTask());
// Repeated task is a delayed task with the default precision: low.
    EXPECT_TRUE(task_queue.last_precision().has_value());
    EXPECT_EQ(task_queue.last_precision().value(),
              TaskQueue::DelayPrecision::kLow);
// No more tasks.
    EXPECT_TRUE(task_queue.AdvanceTimeAndRunLastTask());
}

TEST(RepeatingTaskTest, CanSpecifyToPostTasksWithLowPrecision)
{
    SimulatedClock clock(Timestamp::Zero());
    FakeTaskQueue task_queue(&clock);
// Closure that repeats twice.
    MockFunction<TimeDelta()> closure;
    EXPECT_CALL(closure, Call())
        .WillOnce(Return(TimeDelta::Millis(1)))
        .WillOnce(Return(TimeDelta::PlusInfinity()));
    RepeatingTaskHandle::Start(&task_queue, closure.AsStdFunction(),
                               TaskQueue::DelayPrecision::kLow);
// Initial task is a PostTask().
    EXPECT_FALSE(task_queue.last_precision().has_value());
    EXPECT_FALSE(task_queue.AdvanceTimeAndRunLastTask());
// Repeated task is a delayed task with the specified precision.
    EXPECT_TRUE(task_queue.last_precision().has_value());
    EXPECT_EQ(task_queue.last_precision().value(),
              TaskQueue::DelayPrecision::kLow);
// No more tasks.
    EXPECT_TRUE(task_queue.AdvanceTimeAndRunLastTask());
}

TEST(RepeatingTaskTest, CanSpecifyToPostTasksWithHighPrecision)
{
    SimulatedClock clock(Timestamp::Zero());
    FakeTaskQueue task_queue(&clock);
// Closure that repeats twice.
    MockFunction<TimeDelta()> closure;
    EXPECT_CALL(closure, Call())
        .WillOnce(Return(TimeDelta::Millis(1)))
        .WillOnce(Return(TimeDelta::PlusInfinity()));
    RepeatingTaskHandle::Start(&task_queue, closure.AsStdFunction(),
                               TaskQueue::DelayPrecision::kHigh);
// Initial task is a PostTask().
    EXPECT_FALSE(task_queue.last_precision().has_value());
    EXPECT_FALSE(task_queue.AdvanceTimeAndRunLastTask());
// Repeated task is a delayed task with the specified precision.
    EXPECT_TRUE(task_queue.last_precision().has_value());
    EXPECT_EQ(task_queue.last_precision().value(),
              TaskQueue::DelayPrecision::kHigh);
// No more tasks.
    EXPECT_TRUE(task_queue.AdvanceTimeAndRunLastTask());
}
