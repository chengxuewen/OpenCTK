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

#include <private/octk_task_thread_p.hpp>
#include <octk_sequence_checker.hpp>
#include <octk_move_wrapper.hpp>
#include <octk_scope_guard.hpp>
#include <octk_task_queue.hpp>
#include <octk_task_event.hpp>
#include <octk_date_time.hpp>
#include <octk_memory.hpp>

#if defined(OCTK_OS_WIN)
#   include <comdef.h>
#elif defined(OCTK_OS_UNIX)
#   include <time.h>
#else
#   error "Either OCTK_OS_WIN or OCTK_OS_UNIX needs to be defined."
#endif

#if defined(OCTK_OS_WIN)
// Disable warning that we don't care about:
// warning C4722: destructor never returns, potential memory leak
#pragma warning(disable : 4722)
#endif

#if defined(OCTK_OS_MAC)
/*
 * These are forward-declarations for methods that are part of the
 * ObjC runtime. They are declared in the private header objc-internal.h.
 * These calls are what clang inserts when using @autoreleasepool in ObjC,
 * but here they are used directly in order to keep this file C++.
 * https://clang.llvm.org/docs/AutomaticReferenceCounting.html#runtime-support
 */
extern "C" {
void *objc_autoreleasePoolPush(void);
void objc_autoreleasePoolPop(void *pool);
}

namespace
{
class ScopedAutoReleasePool
{
public:
    ScopedAutoReleasePool() : pool_(objc_autoreleasePoolPush()) {}
    ~ScopedAutoReleasePool() { objc_autoreleasePoolPop(pool_); }

private:
    void *const pool_;
};
}  // namespace
#endif

OCTK_BEGIN_NAMESPACE

/*using ::Mutex::Locker;
using ::TimeDelta;*/

std::unique_ptr<SocketServer> CreateDefaultSocketServer()
{
#if defined(__native_client__)
    return std::unique_ptr<SocketServer>(new NullSocketServer);
#else
    // return std::unique_ptr<SocketServer>(new PhysicalSocketServer);
    return std::unique_ptr<SocketServer>(new NullSocketServer);
#endif
}

TaskThreadManager *TaskThreadManager::Instance()
{
    static TaskThreadManager *const thread_manager = new TaskThreadManager();
    return thread_manager;
}

TaskThreadManager::~TaskThreadManager()
{
    // By above OCTK_DEFINE_STATIC_LOCAL.
    OCTK_DCHECK_NOTREACHED() << "TaskThreadManager should never be destructed.";
}

// static
void TaskThreadManager::Add(TaskThread *message_queue)
{
    return Instance()->AddInternal(message_queue);
}
void TaskThreadManager::AddInternal(TaskThread *message_queue)
{
    Mutex::Locker cs(&crit_);
    message_queues_.push_back(message_queue);
}

// static
void TaskThreadManager::Remove(TaskThread *message_queue)
{
    return Instance()->RemoveInternal(message_queue);
}
void TaskThreadManager::RemoveInternal(TaskThread *message_queue)
{
    {
        Mutex::Locker cs(&crit_);
        std::vector<TaskThread *>::iterator iter;
        // iter = absl::c_find(message_queues_, message_queue);
        iter = std::find(message_queues_.begin(), message_queues_.end(), message_queue);
        if (iter != message_queues_.end())
        {
            message_queues_.erase(iter);
        }
#if OCTK_DCHECK_IS_ON
        RemoveFromSendGraph(message_queue);
#endif
    }
}

#if OCTK_DCHECK_IS_ON
void TaskThreadManager::RemoveFromSendGraph(TaskThread *thread)
{
    for (auto it = send_graph_.begin(); it != send_graph_.end();)
    {
        if (it->first == thread)
        {
            it = send_graph_.erase(it);
        }
        else
        {
            it->second.erase(thread);
            ++it;
        }
    }
}

void TaskThreadManager::RegisterSendAndCheckForCycles(TaskThread *source,
                                                      TaskThread *target)
{
    OCTK_DCHECK(source);
    OCTK_DCHECK(target);

    Mutex::Locker cs(&crit_);
    std::deque<TaskThread *> all_targets({target});
    // We check the pre-existing who-sends-to-who graph for any path from target
    // to source. This loop is guaranteed to terminate because per the send graph
    // invariant, there are no cycles in the graph.
    for (size_t i = 0; i < all_targets.size(); i++)
    {
        const auto &targets = send_graph_[all_targets[i]];
        all_targets.insert(all_targets.end(), targets.begin(), targets.end());
    }
    OCTK_CHECK_EQ(std::count(all_targets.begin(), all_targets.end(), source), 0)
        << " send loop between " << source->name() << " and " << target->name();

    // We may now insert source -> target without creating a cycle, since there
    // was no path from target to source per the prior CHECK.
    send_graph_[source].insert(target);
}
#endif

// static
void TaskThreadManager::ProcessAllMessageQueuesForTesting()
{
    return Instance()->ProcessAllMessageQueuesInternal();
}

void TaskThreadManager::ProcessAllMessageQueuesInternal()
{
    // This works by posting a delayed message at the current time and waiting
    // for it to be dispatched on all queues, which will ensure that all messages
    // that came before it were also dispatched.
    std::atomic<int> queues_not_done(0);

    {
        Mutex::Locker cs(&crit_);
        for (TaskThread *queue: message_queues_)
        {
            if (!queue->IsProcessingMessagesForTesting())
            {
                // If the queue is not processing messages, it can be ignored.
                // If we tried to post a message to it, it would be dropped or ignored.
                continue;
            }
            queues_not_done.fetch_add(1);
            // Whether the task is processed, or the thread is simply cleared,
            // queues_not_done gets decremented.
            auto sub = utils::makeScopeGuard([&queues_not_done] {
                queues_not_done.fetch_sub(1);
            });
            auto subMove = utils::makeMoveWrapper(std::move(sub));
            // Post delayed task instead of regular task to wait for all delayed tasks
            // that are ready for processing.
            queue->PostDelayedTask([&queues_not_done, subMove]() mutable {
                subMove.move();
            }, TimeDelta::Zero());
        }
    }

    TaskThread *current = TaskThread::Current();
    // Note: One of the message queues may have been on this thread, which is
    // why we can't synchronously wait for queues_not_done to go to 0; we need
    // to process messages as well.
    while (queues_not_done.load() > 0)
    {
        if (current)
        {
            current->ProcessMessages(0);
        }
    }
}

// static
TaskThread *TaskThread::Current()
{
    TaskThreadManager *manager = TaskThreadManager::Instance();
    TaskThread *thread = manager->CurrentTaskThread();
    return thread;
}

#if defined(OCTK_OS_UNIX)
TaskThreadManager::TaskThreadManager()
{
#if defined(OCTK_OS_MAC)
    InitCocoaMultiThreading();
#endif
    pthread_key_create(&key_, nullptr);
}

TaskThread *TaskThreadManager::CurrentTaskThread()
{
    return static_cast<TaskThread *>(pthread_getspecific(key_));
}

void TaskThreadManager::SetCurrentTaskThreadInternal(TaskThread *thread)
{
    pthread_setspecific(key_, thread);
}
#endif

#if defined(OCTK_OS_WIN)
TaskThreadManager::TaskThreadManager() : key_(TlsAlloc()) {}

TaskThread* TaskThreadManager::CurrentTaskThread() {
  return static_cast<TaskThread*>(TlsGetValue(key_));
}

void TaskThreadManager::SetCurrentTaskThreadInternal(TaskThread* thread) {
  TlsSetValue(key_, thread);
}
#endif

void TaskThreadManager::SetCurrentTaskThread(TaskThread *thread)
{
#if OCTK_DLOG_IS_ON
    if (CurrentTaskThread() && thread)
    {
    OCTK_DLOG(LS_ERROR) << "SetCurrentTaskThread: Overwriting an existing value?";
  }
#endif  // OCTK_DLOG_IS_ON

    if (thread)
    {
        thread->EnsureIsCurrentTaskQueue();
    }
    else
    {
        TaskThread *current = CurrentTaskThread();
        if (current)
        {
            // The current thread is being cleared, e.g. as a result of
            // UnwrapCurrent() being called or when a thread is being stopped
            // (see PreRun()). This signals that the TaskThread instance is being detached
            // from the thread, which also means that TaskQueue::Current() must not
            // return a pointer to the TaskThread instance.
            current->ClearCurrentTaskQueue();
        }
    }

    SetCurrentTaskThreadInternal(thread);
}

void TaskThreadManager::ChangeCurrentTaskThreadForTest(TaskThread *thread)
{
    SetCurrentTaskThreadInternal(thread);
}

TaskThread *TaskThreadManager::WrapCurrentTaskThread()
{
    TaskThread *result = CurrentTaskThread();
    if (nullptr == result)
    {
        result = new TaskThread(CreateDefaultSocketServer());
        result->WrapCurrentWithTaskThreadManager(this, true);
    }
    return result;
}

void TaskThreadManager::UnwrapCurrentTaskThread()
{
    TaskThread *t = CurrentTaskThread();
    if (t && !(t->IsOwned()))
    {
        t->UnwrapCurrent();
        delete t;
    }
}

TaskThread::ScopedDisallowBlockingCalls::ScopedDisallowBlockingCalls()
    : thread_(TaskThread::Current()), previous_state_(thread_->SetAllowBlockingCalls(false)) {}

TaskThread::ScopedDisallowBlockingCalls::~ScopedDisallowBlockingCalls()
{
    OCTK_DCHECK(thread_->IsCurrent());
    thread_->SetAllowBlockingCalls(previous_state_);
}

#if OCTK_DCHECK_IS_ON
TaskThread::ScopedCountBlockingCalls::ScopedCountBlockingCalls(
    std::function<void(uint32_t, uint32_t)> callback)
    : thread_(TaskThread::Current()), base_blocking_call_count_(thread_->GetBlockingCallCount())
    , base_could_be_blocking_call_count_(
        thread_->GetCouldBeBlockingCallCount()), result_callback_(std::move(callback)) {}

TaskThread::ScopedCountBlockingCalls::~ScopedCountBlockingCalls()
{
    if (GetTotalBlockedCallCount() >= min_blocking_calls_for_callback_)
    {
        result_callback_(GetBlockingCallCount(), GetCouldBeBlockingCallCount());
    }
}

uint32_t TaskThread::ScopedCountBlockingCalls::GetBlockingCallCount() const
{
    return thread_->GetBlockingCallCount() - base_blocking_call_count_;
}

uint32_t TaskThread::ScopedCountBlockingCalls::GetCouldBeBlockingCallCount() const
{
    return thread_->GetCouldBeBlockingCallCount() -
           base_could_be_blocking_call_count_;
}

uint32_t TaskThread::ScopedCountBlockingCalls::GetTotalBlockedCallCount() const
{
    return GetBlockingCallCount() + GetCouldBeBlockingCallCount();
}
#endif

TaskThread::TaskThread(SocketServer *ss) : TaskThread(ss, /*do_init=*/true) {}

TaskThread::TaskThread(std::unique_ptr<SocketServer> ss) : TaskThread(std::move(ss), /*do_init=*/true) {}

TaskThread::TaskThread(SocketServer *ss, bool do_init)
    : delayed_next_num_(0), fInitialized_(false), fDestroyed_(false), stop_(0), ss_(ss)
{
    OCTK_DCHECK(ss);
    ss_->SetMessageQueue(this);
    SetName("TaskThread", this);  // default name
    if (do_init)
    {
        DoInit();
    }
}

TaskThread::TaskThread(std::unique_ptr<SocketServer> ss, bool do_init)
    : TaskThread(ss.get(), do_init)
{
    own_ss_ = std::move(ss);
}

TaskThread::~TaskThread()
{
    Stop();
    DoDestroy();
}

void TaskThread::DoInit()
{
    if (fInitialized_)
    {
        return;
    }

    fInitialized_ = true;
    TaskThreadManager::Add(this);
}

void TaskThread::DoDestroy()
{
    if (fDestroyed_)
    {
        return;
    }

    fDestroyed_ = true;
    // The signal is done from here to ensure
    // that it always gets called when the queue
    // is going away.
    if (ss_)
    {
        ss_->SetMessageQueue(nullptr);
    }
    TaskThreadManager::Remove(this);
    // Clear.
    CurrentTaskQueueSetter set_current(this);
    messages_ = { };
    delayed_messages_ = { };
}

SocketServer *TaskThread::socketserver()
{
    return ss_;
}

void TaskThread::WakeUpSocketServer()
{
    ss_->WakeUp();
}

void TaskThread::Quit()
{
    stop_.store(1, std::memory_order_release);
    WakeUpSocketServer();
}

bool TaskThread::IsQuitting()
{
    return stop_.load(std::memory_order_acquire) != 0;
}

void TaskThread::Restart()
{
    stop_.store(0, std::memory_order_release);
}

TaskThread::Task TaskThread::Get(int cmsWait)
{
    // Get w/wait + timer scan / dispatch + socket / event multiplexer dispatch

    int64_t cmsTotal = cmsWait;
    int64_t cmsElapsed = 0;
    int64_t msStart = DateTime::TimeMillis();
    int64_t msCurrent = msStart;
    while (true)
    {
        // Check for posted events
        int64_t cmsDelayNext = kForever;
        {
            // All queue operations need to be locked, but nothing else in this loop
            // can happen while holding the `mutex_`.
            Mutex::Locker lock(&mutex_);
            // Check for delayed messages that have been triggered and calculate the
            // next trigger time.
            while (!delayed_messages_.empty())
            {
                if (msCurrent < delayed_messages_.top().run_time_ms)
                {
                    cmsDelayNext = delayed_messages_.top().run_time_ms - msCurrent;
                    break;
                }
                messages_.push(std::move(delayed_messages_.top().functor));
                delayed_messages_.pop();
            }
            // Pull a message off the message queue, if available.
            if (!messages_.empty())
            {
                Task task = std::move(messages_.front());
                messages_.pop();
                return task;
            }
        }

        if (IsQuitting())
        {
            break;
        }

        // Which is shorter, the delay wait or the asked wait?

        int64_t cmsNext;
        if (cmsWait == kForever)
        {
            cmsNext = cmsDelayNext;
        }
        else
        {
            cmsNext = std::max<int64_t>(0, cmsTotal - cmsElapsed);
            if ((cmsDelayNext != kForever) && (cmsDelayNext < cmsNext))
            {
                cmsNext = cmsDelayNext;
            }
        }

        {
            // Wait and multiplex in the meantime
            if (!ss_->Wait(cmsNext == kForever ? SocketServer::foreverDuration()
                                               : TimeDelta::Millis(cmsNext),
                /*process_io=*/true))
            {
                return nullptr;
            }
        }

        // If the specified timeout expired, return

        msCurrent = DateTime::TimeMillis();
        cmsElapsed = msCurrent = msStart;
        if (cmsWait != kForever)
        {
            if (cmsElapsed >= cmsWait)
            {
                return nullptr;
            }
        }
    }
    return nullptr;
}

void TaskThread::PostTaskImpl(Task task,
                              const PostTaskTraits & /* traits */,
                              const SourceLocation & /* location */)
{
    if (IsQuitting())
    {
        return;
    }

    // Keep thread safe
    // Add the message to the end of the queue
    // Signal for the multiplexer to return

    {
        Mutex::Locker lock(&mutex_);
        messages_.push(std::move(task));
    }
    WakeUpSocketServer();
}

void TaskThread::PostDelayedTaskImpl(Task task,
                                     TimeDelta delay,
                                     const PostDelayedTaskTraits & /* traits */,
                                     const SourceLocation & /* location */)
{
    if (IsQuitting())
    {
        return;
    }

    // Keep thread safe
    // Add to the priority queue. Gets sorted soonest first.
    // Signal for the multiplexer to return.

    int64_t delay_ms = delay.RoundUpTo(TimeDelta::Millis(1)).ms<int>();
    int64_t run_time_ms = DateTime::timeAfterMSecs(delay_ms);
    {
        Mutex::Locker lock(&mutex_);
        delayed_messages_.push({.delay_ms = delay_ms,
                                   .run_time_ms = run_time_ms,
                                   .message_number = delayed_next_num_,
                                   .functor = std::move(task)});
        // If this message queue processes 1 message every millisecond for 50 days,
        // we will wrap this number.  Even then, only messages with identical times
        // will be misordered, and then only briefly.  This is probably ok.
        ++delayed_next_num_;
        OCTK_DCHECK_NE(0, delayed_next_num_);
    }
    WakeUpSocketServer();
}

int TaskThread::GetDelay()
{
    Mutex::Locker lock(&mutex_);

    if (!messages_.empty())
    {
        return 0;
    }

    if (!delayed_messages_.empty())
    {
        int delay = DateTime::timeUntilMSecs(delayed_messages_.top().run_time_ms);
        if (delay < 0)
        {
            delay = 0;
        }
        return delay;
    }

    return kForever;
}

void TaskThread::Dispatch(Task task)
{
    // TRACE_EVENT0("webrtc", "TaskThread::Dispatch");
    OCTK_DCHECK_RUN_ON(this);
    int64_t start_time = DateTime::TimeMillis();
    std::move(task)();
    int64_t end_time = DateTime::TimeMillis();
    int64_t diff = end_time - start_time;
    if (diff >= dispatch_warning_ms_)
    {
        OCTK_INFO() << "Message to " << name() << " took " << diff
                    << "ms to dispatch.";
        // To avoid log spew, move the warning limit to only give warning
        // for delays that are larger than the one observed.
        dispatch_warning_ms_ = diff + 1;
    }
}

bool TaskThread::IsCurrent() const
{
    return TaskThreadManager::Instance()->CurrentTaskThread() == this;
}

std::unique_ptr<TaskThread> TaskThread::CreateWithSocketServer()
{
    return std::unique_ptr<TaskThread>(new TaskThread(CreateDefaultSocketServer()));
}

std::unique_ptr<TaskThread> TaskThread::Create()
{
    return std::unique_ptr<TaskThread>(new TaskThread(std::unique_ptr<SocketServer>(new NullSocketServer())));
}

bool TaskThread::SleepMs(int milliseconds)
{
    AssertBlockingIsAllowedOnCurrentTaskThread();

#if defined(OCTK_OS_WIN)
    ::Sleep(milliseconds);
  return true;
#else
    // POSIX has both a usleep() and a nanosleep(), but the former is deprecated,
    // so we use nanosleep() even though it has greater precision than necessary.
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    int ret = nanosleep(&ts, nullptr);
    if (ret != 0)
    {
        OCTK_WARNING() << "nanosleep() returning early";
        return false;
    }
    return true;
#endif
}

bool TaskThread::SetName(StringView name, const void *obj)
{
    OCTK_DCHECK(!IsRunning());

    name_ = std::string(name);
    if (obj)
    {
        // The %p specifier typically produce at most 16 hex digits, possibly with a
        // 0x prefix. But format is implementation defined, so add some margin.
        char buf[30];
        snprintf(buf, sizeof(buf), " 0x%p", obj);
        name_ += buf;
    }
    return true;
}

void TaskThread::SetDispatchWarningMs(int deadline)
{
    if (!IsCurrent())
    {
        PostTask([this, deadline]() { SetDispatchWarningMs(deadline); });
        return;
    }
    OCTK_DCHECK_RUN_ON(this);
    dispatch_warning_ms_ = deadline;
}

bool TaskThread::Start()
{
    OCTK_DCHECK(!IsRunning());

    if (IsRunning())
    {
        return false;
    }

    Restart();  // reset IsQuitting() if the thread is being restarted

    // Make sure that TaskThreadManager is created on the main thread before we start a new thread.
    TaskThreadManager::Instance();

    owned_ = true;

    SpinLock::Locker locker(mStartSpinLock);
#if defined(OCTK_OS_WIN)
    thread_ = CreateTaskThread(nullptr, 0, PreRun, this, 0, &thread_id_);
    if (!thread_)
    {
        return false;
    }
#elif defined(OCTK_OS_UNIX)
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    int error_code = pthread_create(&thread_, &attr, PreRun, this);
    if (0 != error_code)
    {
        OCTK_ERROR() << "Unable to create pthread, error " << error_code;
        thread_ = 0;
        return false;
    }
    OCTK_DCHECK(thread_);
#endif
    locker.relock();
    return true;
}

bool TaskThread::WrapCurrent()
{
    return WrapCurrentWithTaskThreadManager(TaskThreadManager::Instance(), true);
}

void TaskThread::UnwrapCurrent()
{
    // Clears the platform-specific thread-specific storage.
    TaskThreadManager::Instance()->SetCurrentTaskThread(nullptr);
#if 1
#if defined(OCTK_OS_WIN)
    if (thread_ != nullptr) {
    if (!CloseHandle(thread_)) {
      OCTK_ERROR() << "When unwrapping thread, failed to close handle.";
    }
    thread_ = nullptr;
    thread_id_ = 0;
  }
#elif defined(OCTK_OS_UNIX)
    thread_ = 0;
#endif
#else
    mThread.detach();
    mThread = std::thread();
#endif
}

void TaskThread::SafeWrapCurrent()
{
    WrapCurrentWithTaskThreadManager(TaskThreadManager::Instance(), false);
}

void TaskThread::Join()
{
    if (!IsRunning())
    {
        return;
    }

    OCTK_DCHECK(!IsCurrent());
    if (Current() && !Current()->blocking_calls_allowed_)
    {
        OCTK_WARNING() << "Waiting for the thread to join, "
                          "but blocking calls have been disallowed";
    }

#if 1
#if defined(OCTK_OS_WIN)
    OCTK_DCHECK(thread_ != nullptr);
  WaitForSingleObject(thread_, INFINITE);
  CloseHandle(thread_);
  thread_ = nullptr;
  thread_id_ = 0;
#elif defined(OCTK_OS_UNIX)
    pthread_join(thread_, nullptr);
    thread_ = 0;
#endif
#else
    mThread.join();
#endif
}

bool TaskThread::SetAllowBlockingCalls(bool allow)
{
    OCTK_DCHECK(IsCurrent());
    bool previous = blocking_calls_allowed_;
    blocking_calls_allowed_ = allow;
    return previous;
}

// static
void TaskThread::AssertBlockingIsAllowedOnCurrentTaskThread()
{
#if !defined(NDEBUG)
    TaskThread *current = TaskThread::Current();
    OCTK_DCHECK(!current || current->blocking_calls_allowed_);
#endif
}

// static
#if defined(OCTK_OS_WIN)
DWORD WINAPI TaskThread::PreRun(LPVOID pv) {
#else
void *TaskThread::PreRun(void *pv)
{
    OCTK_TRACE("TaskThread::PreRun(%p)", pv);
#endif
    TaskThread *thread = static_cast<TaskThread *>(pv);
    thread->mIdString = PlatformThread::currentThreadIdString();
    auto hex = PlatformThread::currentThreadIdHexString();
    auto id = PlatformThread::currentThreadId();
    auto ref = PlatformThread::currentThreadRef();
    TaskThreadManager::Instance()->SetCurrentTaskThread(thread);
    // SetCurrentTaskThreadName(thread->name_.c_str()); //TODO
#if defined(OCTK_OS_MAC)
    ScopedAutoReleasePool pool;
#endif
    thread->mStartSpinLock.unlock();
    thread->Run();

    TaskThreadManager::Instance()->SetCurrentTaskThread(nullptr);
#ifdef OCTK_OS_WIN
    return 0;
#else
    return nullptr;
#endif
}  // namespace rtc

void TaskThread::Run()
{
    OCTK_TRACE("TaskThread::Run(%p)", this);
    ProcessMessages(kForever);
}

bool TaskThread::IsOwned()
{
    OCTK_DCHECK(IsRunning());
    return owned_;
}

void TaskThread::Stop()
{
    TaskThread::Quit();
    Join();
}

void TaskThread::BlockingCallImpl(FunctionView<void()> functor,
                                  const SourceLocation & /* location */)
{
    // TRACE_EVENT0("webrtc", "TaskThread::BlockingCall");

    OCTK_DCHECK(!IsQuitting());
    if (IsQuitting())
    {
        return;
    }

    if (IsCurrent())
    {
#if OCTK_DCHECK_IS_ON
        OCTK_DCHECK(this->IsInvokeToTaskThreadAllowed(this));
        OCTK_DCHECK_RUN_ON(this);
        could_be_blocking_call_count_++;
#endif
        functor();
        return;
    }

#if OCTK_DCHECK_IS_ON
    if (TaskThread *current_thread = TaskThread::Current())
    {
        OCTK_DCHECK_RUN_ON(current_thread);
        OCTK_DCHECK(current_thread->blocking_calls_allowed_);
        current_thread->blocking_call_count_++;
        OCTK_DCHECK(current_thread->IsInvokeToTaskThreadAllowed(this));
        TaskThreadManager::Instance()->RegisterSendAndCheckForCycles(current_thread, this);
    }
#endif

    Event done;
    PostTask([functor, &done] {
        auto guard = utils::makeScopeGuard([&done] { done.Set(); });
        functor();
    });
    done.Wait(Event::foreverDuration());
}

// Called by the TaskThreadManager when being set as the current thread.
void TaskThread::EnsureIsCurrentTaskQueue()
{
    task_queue_registration_ = utils::make_unique<TaskQueue::CurrentTaskQueueSetter>(this);
}

// Called by the TaskThreadManager when being set as the current thread.
void TaskThread::ClearCurrentTaskQueue()
{
    task_queue_registration_.reset();
}

void TaskThread::AllowInvokesToTaskThread(TaskThread *thread)
{
#if (!defined(NDEBUG) || OCTK_DCHECK_IS_ON)
    if (!IsCurrent())
    {
        PostTask([thread, this]() { AllowInvokesToTaskThread(thread); });
        return;
    }
    OCTK_DCHECK_RUN_ON(this);
    allowed_threads_.push_back(thread);
    invoke_policy_enabled_ = true;
#endif
}

void TaskThread::DisallowAllInvokes()
{
#if (!defined(NDEBUG) || OCTK_DCHECK_IS_ON)
    if (!IsCurrent())
    {
        PostTask([this]() { DisallowAllInvokes(); });
        return;
    }
    OCTK_DCHECK_RUN_ON(this);
    allowed_threads_.clear();
    invoke_policy_enabled_ = true;
#endif
}

#if OCTK_DCHECK_IS_ON
uint32_t TaskThread::GetBlockingCallCount() const
{
    OCTK_DCHECK_RUN_ON(this);
    return blocking_call_count_;
}
uint32_t TaskThread::GetCouldBeBlockingCallCount() const
{
    OCTK_DCHECK_RUN_ON(this);
    return could_be_blocking_call_count_;
}
#endif

// Returns true if no policies added or if there is at least one policy
// that permits invocation to `target` thread.
bool TaskThread::IsInvokeToTaskThreadAllowed(TaskThread *target)
{
#if (!defined(NDEBUG) || OCTK_DCHECK_IS_ON)
    OCTK_DCHECK_RUN_ON(this);
    if (!invoke_policy_enabled_)
    {
        return true;
    }
    for (const auto *thread: allowed_threads_)
    {
        if (thread == target)
        {
            return true;
        }
    }
    return false;
#else
    return true;
#endif
}

void TaskThread::Delete()
{
    Stop();
    delete this;
}

bool TaskThread::IsProcessingMessagesForTesting()
{
    return (owned_ || IsCurrent()) && !IsQuitting();
}

bool TaskThread::ProcessMessages(int cmsLoop)
{
    // Using ProcessMessages with a custom clock for testing and a time greater
    // than 0 doesn't work, since it's not guaranteed to advance the custom
    // clock's time, and may get stuck in an infinite loop.
    // OCTK_DCHECK(GetClockForTesting() == nullptr || cmsLoop == 0 || cmsLoop == kForever); //TODO
    int64_t msEnd = (kForever == cmsLoop) ? 0 : DateTime::timeAfterMSecs(cmsLoop);
    int cmsNext = cmsLoop;

    while (true)
    {
#if defined(OCTK_OS_MAC)
        ScopedAutoReleasePool pool;
#endif
        Task task = Get(cmsNext);
        if (!task)
        {
            return !IsQuitting();
        }
        Dispatch(std::move(task));

        if (cmsLoop != kForever)
        {
            cmsNext = static_cast<int>(DateTime::timeUntilMSecs(msEnd));
            if (cmsNext < 0)
            {
                return true;
            }
        }
    }
}

bool TaskThread::WrapCurrentWithTaskThreadManager(TaskThreadManager *thread_manager,
                                                  [[maybe_unused]] bool need_synchronize_access)
{
    OCTK_DCHECK(!IsRunning());

// #if 1
#if defined(OCTK_OS_WIN)
    if (need_synchronize_access) {
    // We explicitly ask for no rights other than synchronization.
    // This gives us the best chance of succeeding.
    thread_ = OpenTaskThread(SYNCHRONIZE, FALSE, GetCurrentTaskThreadId());
    if (!thread_) {
      OCTK_LOG_GLE(LS_ERROR) << "Unable to get handle to thread.";
      return false;
    }
    thread_id_ = GetCurrentTaskThreadId();
  }
#elif defined(OCTK_OS_UNIX)
    thread_ = pthread_self();
#endif
// #else
//     mThread = std::this_thread::
// #endif
    owned_ = false;
    thread_manager->SetCurrentTaskThread(this);
    return true;
}

bool TaskThread::IsRunning()
{
#if defined(OCTK_OS_WIN)
    return thread_ != nullptr;
#elif defined(OCTK_OS_UNIX)
    return thread_ != 0;
#endif
}

AutoTaskThread::AutoTaskThread() : TaskThread(CreateDefaultSocketServer(), /*do_init=*/false)
{
    if (!TaskThreadManager::Instance()->CurrentTaskThread())
    {
        // DoInit registers with TaskThreadManager. Do that only if we intend to
        // be TaskThread::Current(), otherwise ProcessAllMessageQueuesInternal will
        // post a message to a queue that no running thread is serving.
        DoInit();
        TaskThreadManager::Instance()->SetCurrentTaskThread(this);
    }
}

AutoTaskThread::~AutoTaskThread()
{
    Stop();
    DoDestroy();
    if (TaskThreadManager::Instance()->CurrentTaskThread() == this)
    {
        TaskThreadManager::Instance()->SetCurrentTaskThread(nullptr);
    }
}

AutoSocketServerTaskThread::AutoSocketServerTaskThread(SocketServer *ss)
    : TaskThread(ss, /*do_init=*/false)
{
    DoInit();
    old_thread_ = TaskThreadManager::Instance()->CurrentTaskThread();
    // Temporarily set the current thread to nullptr so that we can keep checks
    // around that catch unintentional pointer overwrites.
    TaskThreadManager::Instance()->SetCurrentTaskThread(nullptr);
    TaskThreadManager::Instance()->SetCurrentTaskThread(this);
    if (old_thread_)
    {
        TaskThreadManager::Remove(old_thread_);
    }
}

AutoSocketServerTaskThread::~AutoSocketServerTaskThread()
{
    OCTK_DCHECK(TaskThreadManager::Instance()->CurrentTaskThread() == this);
    // Stop and destroy the thread before clearing it as the current thread.
    // Sometimes there are messages left in the TaskThread that will be
    // destroyed by DoDestroy, and sometimes the destructors of the message and/or
    // its contents rely on this thread still being set as the current thread.
    Stop();
    DoDestroy();
    TaskThreadManager::Instance()->SetCurrentTaskThread(nullptr);
    TaskThreadManager::Instance()->SetCurrentTaskThread(old_thread_);
    if (old_thread_)
    {
        TaskThreadManager::Add(old_thread_);
    }
}
OCTK_END_NAMESPACE
