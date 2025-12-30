/*
 *  Copyright 2004 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <octk_task_queue_factory.hpp>
#include <octk_task_thread.hpp>
#include <octk_fake_clock.hpp>
#include <octk_date_time.hpp>
#include <octk_memory.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#if defined(OCTK_OS_WIN)
#   include <comdef.h>  // NOLINT
#endif

#define WAIT_(ex, timeout, res) \
    do { \
        int64_t start = DateTime::TimeMillis(); \
        res = (ex) && true; \
        while (!res && DateTime::TimeMillis() < start + (timeout)) { \
            TaskThread::Current()->ProcessMessages(0); \
            TaskThread::Current()->SleepMs(1); \
            res = (ex) && true; \
        } \
    } while (0)

#define EXPECT_TRUE_WAIT(ex, timeout) \
    GTEST_AMBIGUOUS_ELSE_BLOCKER_ \
    if (bool res = true) { \
        WAIT_(ex, timeout, res); \
        if (!res) \
            goto GTEST_CONCAT_TOKEN_(gunit_label_, __LINE__); \
        } else \
            GTEST_CONCAT_TOKEN_(gunit_label_, __LINE__) : EXPECT_TRUE(ex)

using namespace octk;

namespace
{

// using ::testing::ElementsAre;

#if 0
// Generates a sequence of numbers (collaboratively).
class TestGenerator
{
public:
    TestGenerator() : last(0), count(0) {}

    int Next(int prev)
    {
        int result = prev + last;
        last = result;
        count += 1;
        return result;
    }

    int last;
    int count;
};

// Receives messages and sends on a socket.
class MessageClient : public TestGenerator
{
public:
    MessageClient(TaskThread *pth, Socket *socket) : socket_(socket) {}

    ~MessageClient() { delete socket_; }

    void OnValue(int value)
    {
        int result = Next(value);
        EXPECT_GE(socket_->Send(&result, sizeof(result)), 0);
    }

private:
    Socket *socket_;
};

// Receives on a socket and sends by posting messages.
class SocketClient : public TestGenerator, public sigslot::has_slots<>
{
public:
    SocketClient(Socket *socket,
                 const SocketAddress &addr,
                 TaskThread *post_thread,
                 MessageClient *phandler)
        : socket_(AsyncUDPSocket::Create(socket, addr)), post_thread_(post_thread), post_handler_(phandler)
    {
        socket_->RegisterReceivedPacketCallback(
            [&](rtc::AsyncPacketSocket *socket, const rtc::ReceivedPacket &packet) {
                OnPacket(socket, packet);
            });
    }

    ~SocketClient() override { delete socket_; }

    SocketAddress address() const { return socket_->GetLocalAddress(); }

    void OnPacket(AsyncPacketSocket *socket, const rtc::ReceivedPacket &packet)
    {
        EXPECT_EQ(packet.payload().size(), sizeof(uint32_t));
        uint32_t prev =
            reinterpret_cast<const uint32_t *>(packet.payload().data())[0];
        uint32_t result = Next(prev);

        post_thread_->PostDelayedTask([post_handler_ = post_handler_,
                                          result] { post_handler_->OnValue(result); },
                                      TimeDelta::Millis(200));
    }

private:
    AsyncUDPSocket *socket_;
    TaskThread *post_thread_;
    MessageClient *post_handler_;
};
#endif

class CustomThread : public TaskThread
{
public:
    CustomThread() : TaskThread(std::unique_ptr<SocketServer>(new NullSocketServer())) {}
    ~CustomThread() override { Stop(); }
    bool Start() { return false; }

    bool WrapCurrent() { return TaskThread::WrapCurrent(); }
    void UnwrapCurrent() { TaskThread::UnwrapCurrent(); }
};

// A thread that does nothing when it runs and signals an event
// when it is destroyed.
class SignalWhenDestroyedThread : public TaskThread
{
public:
    SignalWhenDestroyedThread(Event *event)
        : TaskThread(std::unique_ptr<SocketServer>(new NullSocketServer())), event_(event) {}

    ~SignalWhenDestroyedThread() override
    {
        Stop();
        event_->Set();
    }

    void Run() override
    {
        // Do nothing.
    }

private:
    Event *event_;
};

#if 0
// See: https://code.google.com/p/webrtc/issues/detail?id=2409
TEST(ThreadTest, DISABLED_Main)
{
    AutoTaskThread main_thread;
    const SocketAddress addr("127.0.0.1", 0);

// Create the messaging client on its own thread.
    auto th1 = TaskThread::CreateWithSocketServer();
    Socket *socket = th1->socketserver()->CreateSocket(addr.family(), SOCK_DGRAM);
    MessageClient msg_client(th1.get(), socket);

// Create the socket client on its own thread.
    auto th2 = TaskThread::CreateWithSocketServer();
    Socket *asocket = th2->socketserver()->CreateSocket(addr.family(), SOCK_DGRAM);
    SocketClient sock_client(asocket, addr, th1.get(), &msg_client);

    socket->Connect(sock_client.address());

    th1->Start();
    th2->Start();

// Get the messages started.
    th1->PostDelayedTask([&msg_client] { msg_client.OnValue(1); },
                         TimeDelta::Millis(100));

// Give the clients a little while to run.
// Messages will be processed at 100, 300, 500, 700, 900.
    TaskThread *th_main = TaskThread::Current();
    th_main->ProcessMessages(1000);

// Stop the sending client. Give the receiver a bit longer to run, in case
// it is running on a machine that is under load (e.g. the build machine).
    th1->Stop();
    th_main->ProcessMessages(200);
    th2->Stop();

// Make sure the results were correct
    EXPECT_EQ(5, msg_client.count);
    EXPECT_EQ(34, msg_client.last);
    EXPECT_EQ(5, sock_client.count);
    EXPECT_EQ(55, sock_client.last);
}
#endif

TEST(ThreadTest, CountBlockingCalls)
{
    AutoTaskThread current;

// When the test runs, this will print out:
//   (thread_unittest.cc:262): Blocking TestBody: total=2 (actual=1, could=1)
//     OCTK_LOG_THREAD_BLOCK_COUNT();
#if OCTK_DCHECK_IS_ON
    TaskThread::ScopedCountBlockingCalls blocked_calls(
        [&](uint32_t actual_block, uint32_t could_block) {
            EXPECT_EQ(1u, actual_block);
            EXPECT_EQ(1u, could_block);
        });

    EXPECT_EQ(0u, blocked_calls.GetBlockingCallCount());
    EXPECT_EQ(0u, blocked_calls.GetCouldBeBlockingCallCount());
    EXPECT_EQ(0u, blocked_calls.GetTotalBlockedCallCount());

    // Test invoking on the current thread. This should not count as an 'actual'
    // invoke, but should still count as an invoke that could block since we
    // that the call to `BlockingCall` serves a purpose in some configurations
    // (and should not be used a general way to call methods on the same thread).
    current.BlockingCall([]() {});
    EXPECT_EQ(0u, blocked_calls.GetBlockingCallCount());
    EXPECT_EQ(1u, blocked_calls.GetCouldBeBlockingCallCount());
    EXPECT_EQ(1u, blocked_calls.GetTotalBlockedCallCount());

    // Create a new thread to invoke on.
    auto thread = TaskThread::CreateWithSocketServer();
    thread->Start();
    EXPECT_EQ(42, thread->BlockingCall([]() { return 42; }));
    EXPECT_EQ(1u, blocked_calls.GetBlockingCallCount());
    EXPECT_EQ(1u, blocked_calls.GetCouldBeBlockingCallCount());
    EXPECT_EQ(2u, blocked_calls.GetTotalBlockedCallCount());
    thread->Stop();
    // OCTK_DCHECK_BLOCK_COUNT_NO_MORE_THAN(2); //TODO
#else
    // OCTK_DCHECK_BLOCK_COUNT_NO_MORE_THAN(0);
    OCTK_INFO() << "Test not active in this config";
#endif
}

#if OCTK_DCHECK_IS_ON
TEST(ThreadTest, CountBlockingCallsOneCallback)
{
    AutoTaskThread current;
    bool was_called_back = false;
    {
        TaskThread::ScopedCountBlockingCalls blocked_calls(
            [&](uint32_t actual_block, uint32_t could_block) {
                was_called_back = true;
            });
        current.BlockingCall([]() {});
    }
    EXPECT_TRUE(was_called_back);
}

TEST(ThreadTest, CountBlockingCallsSkipCallback)
{
    AutoTaskThread current;
    bool was_called_back = false;
    {
        TaskThread::ScopedCountBlockingCalls blocked_calls(
            [&](uint32_t actual_block, uint32_t could_block) {
                was_called_back = true;
            });
        // Changed `blocked_calls` to not issue the callback if there are 1 or
        // fewer blocking calls (i.e. we set the minimum required number to 2).
        blocked_calls.set_minimum_call_count_for_callback(2);
        current.BlockingCall([]() {});
    }
    // We should not have gotten a call back.
    EXPECT_FALSE(was_called_back);
}
#endif

// Test that setting thread names doesn't cause a malfunction.
// There's no easy way to verify the name was set properly at this time.
TEST(ThreadTest, Names)
{
// Default name
    auto thread = TaskThread::CreateWithSocketServer();
    EXPECT_TRUE(thread->Start());
    thread->Stop();
// Name with no object parameter
    thread = TaskThread::CreateWithSocketServer();
    EXPECT_TRUE(thread->SetName("No object", nullptr));
    EXPECT_TRUE(thread->Start());
    thread->Stop();
// Really long name
    thread = TaskThread::CreateWithSocketServer();
    EXPECT_TRUE(thread->SetName("Abcdefghijklmnopqrstuvwxyz1234567890", this));
    EXPECT_TRUE(thread->Start());
    thread->Stop();
}

TEST(ThreadTest, Wrap)
{
    TaskThread *current_thread = TaskThread::Current();
    TaskThreadManager::Instance()->SetCurrentTaskThread(nullptr);

    {
        CustomThread cthread;
        EXPECT_TRUE(cthread.WrapCurrent());
        EXPECT_EQ(&cthread, TaskThread::Current());
        EXPECT_TRUE(cthread.RunningForTest());
        EXPECT_FALSE(cthread.IsOwned());
        cthread.UnwrapCurrent();
        EXPECT_FALSE(cthread.RunningForTest());
    }
    TaskThreadManager::Instance()->SetCurrentTaskThread(current_thread);
}

#if (!defined(NDEBUG) || OCTK_DCHECK_IS_ON)
TEST(ThreadTest, InvokeToTaskThreadAllowedReturnsTrueWithoutPolicies)
{
    AutoTaskThread main_thread;
// Create and start the thread.
    auto thread1 = TaskThread::CreateWithSocketServer();
    auto thread2 = TaskThread::CreateWithSocketServer();

    thread1->PostTask(
        [&]() { EXPECT_TRUE(thread1->IsInvokeToTaskThreadAllowed(thread2.get())); });
    main_thread.ProcessMessages(100);
}

TEST(ThreadTest, InvokeAllowedWhenThreadsAdded)
{
    AutoTaskThread main_thread;
// Create and start the thread.
    auto thread1 = TaskThread::CreateWithSocketServer();
    auto thread2 = TaskThread::CreateWithSocketServer();
    auto thread3 = TaskThread::CreateWithSocketServer();
    auto thread4 = TaskThread::CreateWithSocketServer();

    thread1->AllowInvokesToTaskThread(thread2.get());
    thread1->AllowInvokesToTaskThread(thread3.get());

    thread1->PostTask([&]() {
        EXPECT_TRUE(thread1->IsInvokeToTaskThreadAllowed(thread2.get()));
        EXPECT_TRUE(thread1->IsInvokeToTaskThreadAllowed(thread3.get()));
        EXPECT_FALSE(thread1->IsInvokeToTaskThreadAllowed(thread4.get()));
    });
    main_thread.ProcessMessages(100);
}

TEST(ThreadTest, InvokesDisallowedWhenDisallowAllInvokes)
{
    AutoTaskThread main_thread;
// Create and start the thread.
    auto thread1 = TaskThread::CreateWithSocketServer();
    auto thread2 = TaskThread::CreateWithSocketServer();

    thread1->DisallowAllInvokes();

    thread1->PostTask([&]() { EXPECT_FALSE(thread1->IsInvokeToTaskThreadAllowed(thread2.get())); });
    main_thread.ProcessMessages(100);
}
#endif  // (!defined(NDEBUG) || OCTK_DCHECK_IS_ON)

TEST(ThreadTest, InvokesAllowedByDefault)
{
    AutoTaskThread main_thread;
// Create and start the thread.
    auto thread1 = TaskThread::CreateWithSocketServer();
    auto thread2 = TaskThread::CreateWithSocketServer();

    thread1->PostTask([&]() { EXPECT_TRUE(thread1->IsInvokeToTaskThreadAllowed(thread2.get())); });
    main_thread.ProcessMessages(100);
}

TEST(ThreadTest, BlockingCall)
{
// Create and start the thread.
    auto thread = TaskThread::CreateWithSocketServer();
    thread->Start();
// Try calling functors.
    EXPECT_EQ(42, thread->BlockingCall([] { return 42; }));
    bool called = false;
    thread->BlockingCall([&] { called = true; });
    EXPECT_TRUE(called);

// Try calling bare functions.
    struct LocalFuncs
    {
        static int Func1() { return 999; }
        static void Func2() {}
    };
    EXPECT_EQ(999, thread->BlockingCall(&LocalFuncs::Func1));
    thread->BlockingCall(&LocalFuncs::Func2);
}

// Verifies that two threads calling Invoke on each other at the same time does
// not deadlock but crash.
#if OCTK_DCHECK_IS_ON && GTEST_HAS_DEATH_TEST && !defined(OCTK_OS_ANDROID)
TEST(ThreadTest, TwoThreadsInvokeDeathTest)
{
    GTEST_FLAG_SET(death_test_style, "threadsafe");
    AutoTaskThread thread;
    TaskThread *main_thread = TaskThread::Current();
    auto other_thread = TaskThread::CreateWithSocketServer();
    other_thread->Start();
    other_thread->BlockingCall([main_thread] { EXPECT_DEATH(main_thread->BlockingCall([] {}), ""); });
}

TEST(ThreadTest, ThreeThreadsInvokeDeathTest)
{
    GTEST_FLAG_SET(death_test_style, "threadsafe");
    AutoTaskThread thread;
    TaskThread *first = TaskThread::Current();

    auto second = TaskThread::Create();
    second->Start();
    auto third = TaskThread::Create();
    third->Start();

    second->BlockingCall([&] { third->BlockingCall([&] { EXPECT_DEATH(first->BlockingCall([] {}), ""); }); });
}

#endif

// Verifies that if thread A invokes a call on thread B and thread C is trying
// to invoke A at the same time, thread A does not handle C's invoke while
// invoking B.
TEST(ThreadTest, ThreeThreadsBlockingCall)
{
    AutoTaskThread thread;
    TaskThread *thread_a = TaskThread::Current();
    auto thread_b = TaskThread::CreateWithSocketServer();
    auto thread_c = TaskThread::CreateWithSocketServer();
    thread_b->Start();
    thread_c->Start();

    class LockedBool
    {
    public:
        explicit LockedBool(bool value) : value_(value) {}

        void Set(bool value)
        {
            Mutex::Lock locker(&mutex_);
            value_ = value;
        }

        bool Get()
        {
            Mutex::Lock locker(&mutex_);
            return value_;
        }

    private:
        Mutex mutex_;
        bool value_
        OCTK_ATTRIBUTE_GUARDED_BY(mutex_);
    };

    struct LocalFuncs
    {
        static void Set(LockedBool *out) { out->Set(true); }
        static void InvokeSet(TaskThread *thread, LockedBool *out)
        {
            thread->BlockingCall([out] { Set(out); });
        }

        // Set `out` true and call InvokeSet on `thread`.
        static void SetAndInvokeSet(LockedBool *out,
                                    TaskThread *thread,
                                    LockedBool *out_inner)
        {
            out->Set(true);
            InvokeSet(thread, out_inner);
        }

        // Asynchronously invoke SetAndInvokeSet on `thread1` and wait until
        // `thread1` starts the call.
        static void AsyncInvokeSetAndWait(TaskThread *thread1,
                                          TaskThread *thread2,
                                          LockedBool *out)
        {
            LockedBool async_invoked(false);

            thread1->PostTask([&async_invoked, thread2, out] {
                SetAndInvokeSet(&async_invoked, thread2, out);
            });

            EXPECT_TRUE_WAIT(async_invoked.Get(), 2000);
        }
    };

    LockedBool thread_a_called(false);

// Start the sequence A --(invoke)--> B --(async invoke)--> C --(invoke)--> A.
// TaskThread B returns when C receives the call and C should be blocked until A
// starts to process messages.
    TaskThread *thread_c_ptr = thread_c.get();
    thread_b->BlockingCall([thread_c_ptr, thread_a, &thread_a_called] {
        LocalFuncs::AsyncInvokeSetAndWait(thread_c_ptr, thread_a, &thread_a_called);
    });
    EXPECT_FALSE(thread_a_called.Get());

    EXPECT_TRUE_WAIT(thread_a_called.Get(), 2000);
}

#if 0
static void DelayedPostsWithIdenticalTimesAreProcessedInFifoOrder(FakeClock &clock,
                                                                  TaskThread &q)
{
    std::vector<int> run_order;

    Event done;
    int64_t now = DateTime::TimeMillis();
    q.PostDelayedTask([&] { run_order.push_back(3); }, TimeDelta::Millis(3));
    q.PostDelayedTask([&] { run_order.push_back(0); }, TimeDelta::Millis(1));
    q.PostDelayedTask([&] { run_order.push_back(1); }, TimeDelta::Millis(2));
    q.PostDelayedTask([&] { run_order.push_back(4); }, TimeDelta::Millis(3));
    q.PostDelayedTask([&] { run_order.push_back(2); }, TimeDelta::Millis(2));
    q.PostDelayedTask([&] { done.Set(); }, TimeDelta::Millis(4));
    // Validate time was frozen while tasks were posted.
    OCTK_DCHECK_EQ(DateTime::TimeMillis(), now);

    // Change time to make all tasks ready to run and wait for them.
    clock.AdvanceTime(TimeDelta::Millis(4));
    ASSERT_TRUE(done.Wait(TimeDelta::Seconds(1)));

    EXPECT_THAT(run_order, ElementsAre(0, 1, 2, 3, 4));
}

TEST(ThreadTest, DelayedPostsWithIdenticalTimesAreProcessedInFifoOrder)
{
    ScopedBaseFakeClock clock;
    TaskThread q(CreateDefaultSocketServer(), true);
    q.Start();
    DelayedPostsWithIdenticalTimesAreProcessedInFifoOrder(clock, q);

    NullSocketServer nullss;
    TaskThread q_nullss(&nullss, true);
    q_nullss.Start();
    DelayedPostsWithIdenticalTimesAreProcessedInFifoOrder(clock, q_nullss);
}
#endif

// Ensure that ProcessAllMessageQueues does its essential function; process
// all messages (both delayed and non delayed) up until the current time, on
// all registered message queues.
TEST(TaskThreadManager, ProcessAllMessageQueues)
{
    AutoTaskThread main_thread;
    Event entered_process_all_message_queues(true, false);
    auto a = TaskThread::CreateWithSocketServer();
    auto b = TaskThread::CreateWithSocketServer();
    a->Start();
    b->Start();

    std::atomic<int> messages_processed(0);
    auto incrementer = [&messages_processed,
        &entered_process_all_message_queues] {
        // Wait for event as a means to ensure Increment doesn't occur outside
        // of ProcessAllMessageQueues. The event is set by a message posted to
        // the main thread, which is guaranteed to be handled inside
        // ProcessAllMessageQueues.
        entered_process_all_message_queues.Wait(Event::foreverDuration());
        messages_processed.fetch_add(1);
    };
    auto event_signaler = [&entered_process_all_message_queues] {
        entered_process_all_message_queues.Set();
    };

// Post messages (both delayed and non delayed) to both threads.
    a->PostTask(incrementer);
    b->PostTask(incrementer);
    a->PostDelayedTask(incrementer, TimeDelta::Zero());
    b->PostDelayedTask(incrementer, TimeDelta::Zero());
    main_thread.PostTask(event_signaler);

    TaskThreadManager::ProcessAllMessageQueuesForTesting();
    EXPECT_EQ(4, messages_processed.load(std::memory_order_acquire));
}

// Test that ProcessAllMessageQueues doesn't hang if a thread is quitting.
TEST(TaskThreadManager, ProcessAllMessageQueuesWithQuittingThread)
{
    auto t = TaskThread::CreateWithSocketServer();
    t->Start();
    t->Quit();
    TaskThreadManager::ProcessAllMessageQueuesForTesting();
}

void WaitAndSetEvent(Event *wait_event, Event *set_event)
{
    wait_event->Wait(Event::foreverDuration());
    set_event->Set();
}

// A functor that keeps track of the number of copies and moves.
class LifeCycleFunctor
{
public:
    struct Stats
    {
        size_t copy_count = 0;
        size_t move_count = 0;
    };

    LifeCycleFunctor(Stats *stats, Event *event) : stats_(stats), event_(event) {}
    LifeCycleFunctor(const LifeCycleFunctor &other) { *this = other; }
    LifeCycleFunctor(LifeCycleFunctor &&other) { *this = std::forward<LifeCycleFunctor &&>(other); }

    LifeCycleFunctor &operator=(const LifeCycleFunctor &other)
    {
        stats_ = other.stats_;
        event_ = other.event_;
        ++stats_->copy_count;
        return *this;
    }

    LifeCycleFunctor &operator=(LifeCycleFunctor &&other)
    {
        stats_ = other.stats_;
        event_ = other.event_;
        ++stats_->move_count;
        return *this;
    }

    void operator()() { event_->Set(); }

private:
    Stats *stats_;
    Event *event_;
};

// A functor that verifies the thread it was destroyed on.
class DestructionFunctor
{
public:
    DestructionFunctor(TaskThread *thread, bool *thread_was_current, Event *event)
        : thread_(thread), thread_was_current_(thread_was_current), event_(event) {}
    ~DestructionFunctor()
    {
        // Only signal the event if this was the functor that was invoked to avoid
        // the event being signaled due to the destruction of temporary/moved
        // versions of this object.
        if (was_invoked_)
        {
            *thread_was_current_ = thread_->IsCurrent();
            event_->Set();
        }
    }

    void operator()() { was_invoked_ = true; }

private:
    TaskThread *thread_;
    bool *thread_was_current_;
    Event *event_;
    bool was_invoked_ = false;
};

TEST(ThreadPostTaskTest, InvokesWithLambda)
{
    std::unique_ptr<TaskThread> background_thread(TaskThread::Create());
    background_thread->Start();

    Event event;
    background_thread->PostTask([&event] { event.Set(); });
    event.Wait(Event::foreverDuration());
}

TEST(ThreadPostTaskTest, DISABLED_InvokesWithCopiedFunctor)
{
    std::unique_ptr<TaskThread> background_thread(TaskThread::Create());
    background_thread->Start();

    LifeCycleFunctor::Stats stats;
    Event event;
    LifeCycleFunctor functor(&stats, &event);
    background_thread->PostTask(functor);
    event.Wait(Event::foreverDuration());

    EXPECT_EQ(1u, stats.copy_count);
    EXPECT_EQ(0u, stats.move_count);
}

TEST(ThreadPostTaskTest, DISABLED_InvokesWithMovedFunctor)
{
    std::unique_ptr<TaskThread> background_thread(TaskThread::Create());
    background_thread->Start();

    LifeCycleFunctor::Stats stats;
    Event event;
    LifeCycleFunctor functor(&stats, &event);
    background_thread->PostTask(std::move(functor));
    event.Wait(Event::foreverDuration());

    EXPECT_EQ(0u, stats.copy_count);
    EXPECT_EQ(1u, stats.move_count);
}

TEST(ThreadPostTaskTest, DISABLED_InvokesWithReferencedFunctorShouldCopy)
{
    std::unique_ptr<TaskThread> background_thread(TaskThread::Create());
    background_thread->Start();

    LifeCycleFunctor::Stats stats;
    Event event;
    LifeCycleFunctor functor(&stats, &event);
    LifeCycleFunctor &functor_ref = functor;
    background_thread->PostTask(functor_ref);
    event.Wait(Event::foreverDuration());

    EXPECT_EQ(1u, stats.copy_count);
    EXPECT_EQ(0u, stats.move_count);
}

TEST(ThreadPostTaskTest, InvokesWithCopiedFunctorDestroyedOnTargetThread)
{
    std::unique_ptr<TaskThread> background_thread(TaskThread::Create());
    background_thread->Start();

    Event event;
    bool was_invoked_on_background_thread = false;
    DestructionFunctor functor(background_thread.get(),
                               &was_invoked_on_background_thread, &event);
    background_thread->PostTask(functor);
    event.Wait(Event::foreverDuration());

    EXPECT_TRUE(was_invoked_on_background_thread);
}

TEST(ThreadPostTaskTest, InvokesWithMovedFunctorDestroyedOnTargetThread)
{
    std::unique_ptr<TaskThread> background_thread(TaskThread::Create());
    background_thread->Start();

    Event event;
    bool was_invoked_on_background_thread = false;
    DestructionFunctor functor(background_thread.get(),
                               &was_invoked_on_background_thread, &event);
    background_thread->PostTask(std::move(functor));
    event.Wait(Event::foreverDuration());

    EXPECT_TRUE(was_invoked_on_background_thread);
}

TEST(ThreadPostTaskTest, InvokesWithReferencedFunctorShouldCopyAndDestroyedOnTargetThread)
{
    std::unique_ptr<TaskThread> background_thread(TaskThread::Create());
    background_thread->Start();

    Event event;
    bool was_invoked_on_background_thread = false;
    DestructionFunctor functor(background_thread.get(),
                               &was_invoked_on_background_thread, &event);
    DestructionFunctor &functor_ref = functor;
    background_thread->PostTask(functor_ref);
    event.Wait(Event::foreverDuration());

    EXPECT_TRUE(was_invoked_on_background_thread);
}

TEST(ThreadPostTaskTest, InvokesOnBackgroundThread)
{
    std::unique_ptr<TaskThread> background_thread(TaskThread::Create());
    background_thread->Start();

    Event event;
    bool was_invoked_on_background_thread = false;
    TaskThread *background_thread_ptr = background_thread.get();
    background_thread->PostTask(
        [background_thread_ptr, &was_invoked_on_background_thread, &event] {
            was_invoked_on_background_thread = background_thread_ptr->IsCurrent();
            event.Set();
        });
    event.Wait(Event::foreverDuration());

    EXPECT_TRUE(was_invoked_on_background_thread);
}

TEST(ThreadPostTaskTest, InvokesAsynchronously)
{
    std::unique_ptr<TaskThread> background_thread(TaskThread::Create());
    background_thread->Start();

// The first event ensures that SendSingleMessage() is not blocking this
// thread. The second event ensures that the message is processed.
    Event event_set_by_test_thread;
    Event event_set_by_background_thread;
    background_thread->PostTask([&event_set_by_test_thread,
                                    &event_set_by_background_thread] {
        WaitAndSetEvent(&event_set_by_test_thread, &event_set_by_background_thread);
    });
    event_set_by_test_thread.Set();
    event_set_by_background_thread.Wait(Event::foreverDuration());
}

TEST(ThreadPostTaskTest, InvokesInPostedOrder)
{
    std::unique_ptr<TaskThread> background_thread(TaskThread::Create());
    background_thread->Start();

    Event first;
    Event second;
    Event third;
    Event fourth;

    background_thread->PostTask(
        [&first, &second] { WaitAndSetEvent(&first, &second); });
    background_thread->PostTask(
        [&second, &third] { WaitAndSetEvent(&second, &third); });
    background_thread->PostTask(
        [&third, &fourth] { WaitAndSetEvent(&third, &fourth); });

// All tasks have been posted before the first one is unblocked.
    first.Set();
// Only if the chain is invoked in posted order will the last event be set.
    fourth.Wait(Event::foreverDuration());
}

TEST(ThreadPostDelayedTaskTest, DISABLED_InvokesAsynchronously)
{
    std::unique_ptr<TaskThread> background_thread(TaskThread::Create());
    background_thread->Start();

// The first event ensures that SendSingleMessage() is not blocking this
// thread. The second event ensures that the message is processed.
    Event event_set_by_test_thread;
    Event event_set_by_background_thread;
    background_thread->PostDelayedTask(
        [&event_set_by_test_thread, &event_set_by_background_thread] {
            WaitAndSetEvent(&event_set_by_test_thread,
                            &event_set_by_background_thread);
        },
        TimeDelta::Millis(10));
    event_set_by_test_thread.Set();
    event_set_by_background_thread.Wait(Event::foreverDuration());
}

TEST(ThreadPostDelayedTaskTest, DISABLED_InvokesInDelayOrder)
{
    ScopedFakeClock clock;
    std::unique_ptr<TaskThread> background_thread(TaskThread::Create());
    background_thread->Start();

    Event first;
    Event second;
    Event third;
    Event fourth;

    background_thread->PostDelayedTask(
        [&third, &fourth] { WaitAndSetEvent(&third, &fourth); },
        TimeDelta::Millis(11));
    background_thread->PostDelayedTask(
        [&first, &second] { WaitAndSetEvent(&first, &second); },
        TimeDelta::Millis(9));
    background_thread->PostDelayedTask(
        [&second, &third] { WaitAndSetEvent(&second, &third); },
        TimeDelta::Millis(10));

// All tasks have been posted before the first one is unblocked.
    first.Set();
// Only if the chain is invoked in delay order will the last event be set.
    clock.AdvanceTime(TimeDelta::Millis(11));
    EXPECT_TRUE(fourth.Wait(TimeDelta::Zero()));
}

TEST(ThreadPostDelayedTaskTest, IsCurrentTaskQueue)
{
    auto current_tq = TaskQueue::Current();
    {
        std::unique_ptr<TaskThread> thread(TaskThread::Create());
        thread->WrapCurrent();
        EXPECT_EQ(TaskQueue::Current(),
                  static_cast<TaskQueue *>(thread.get()));
        thread->UnwrapCurrent();
    }
    EXPECT_EQ(TaskQueue::Current(), current_tq);
}

class ThreadFactory : public TaskQueueFactory
{
public:
    std::unique_ptr<TaskQueue, TaskQueueDeleter>
    CreateTaskQueue(StringView /* name */,
                    Priority /*priority*/) const override
    {
        std::unique_ptr<TaskThread> thread = TaskThread::Create();
        thread->Start();
        return std::unique_ptr<TaskQueue, TaskQueueDeleter>(
            thread.release());
    }
};

// std::unique_ptr<TaskQueueFactory> CreateDefaultThreadFactory(
//     const FieldTrialsView *)
// {
//     return utils::make_unique<ThreadFactory>();
// }
// std::unique_ptr<TaskQueueFactory> CreateDefaultThreadFactory()
// {
//     return utils::make_unique<ThreadFactory>();
// }
//
// using ::TaskQueueTest;
//
// INSTANTIATE_TEST_SUITE_P(RtcThread,
//                          TaskQueueTest,
//                          ::testing::Values(CreateDefaultThreadFactory));
}  // namespace
