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

#ifndef _OCTK_TASK_THREAD_HPP
#define _OCTK_TASK_THREAD_HPP

#include <octk_function_view.hpp>
#include <octk_task_queue.hpp>
#include <octk_spinlock.hpp>
#include <octk_socket.hpp>
#include <octk_mutex.hpp>

#include <thread>
#include <queue>
#include <map>
#include <set>

OCTK_BEGIN_NAMESPACE

class TaskThread;

class OCTK_CORE_API TaskThreadManager
{
public:
    static const int kForever = -1;

    // Singleton, constructor and destructor are private.
    static TaskThreadManager *Instance();

    static void Add(TaskThread *message_queue);
    static void Remove(TaskThread *message_queue);

    // For testing purposes, for use with a simulated clock.
    // Ensures that all message queues have processed delayed messages
    // up until the current point in time.
    static void ProcessAllMessageQueuesForTesting();

    TaskThread *CurrentTaskThread();
    void SetCurrentTaskThread(TaskThread *thread);
    // Allows changing the current thread, this is intended for tests where we
    // want to simulate multiple threads running on a single physical thread.
    void ChangeCurrentTaskThreadForTest(TaskThread *thread);

    // Returns a thread object with its thread_ ivar set
    // to whatever the OS uses to represent the thread.
    // If there already *is* a TaskThread object corresponding to this thread,
    // this method will return that.  Otherwise it creates a new TaskThread
    // object whose wrapped() method will return true, and whose
    // handle will, on Win32, be opened with only synchronization privileges -
    // if you need more privilegs, rather than changing this method, please
    // write additional code to adjust the privileges, or call a different
    // factory method of your own devising, because this one gets used in
    // unexpected contexts (like inside browser plugins) and it would be a
    // shame to break it.  It is also conceivable on Win32 that we won't even
    // be able to get synchronization privileges, in which case the result
    // will have a null handle.
    TaskThread *WrapCurrentTaskThread();
    void UnwrapCurrentTaskThread();

#if OCTK_DCHECK_IS_ON
    // Registers that a Send operation is to be performed between `source` and
    // `target`, while checking that this does not cause a send cycle that could
    // potentially cause a deadlock.
    void RegisterSendAndCheckForCycles(TaskThread *source, TaskThread *target);
#endif

private:
    TaskThreadManager();
    ~TaskThreadManager();

    TaskThreadManager(const TaskThreadManager &) = delete;
    TaskThreadManager &operator=(const TaskThreadManager &) = delete;

    void SetCurrentTaskThreadInternal(TaskThread *thread);
    void AddInternal(TaskThread *message_queue);
    void RemoveInternal(TaskThread *message_queue);
    void ProcessAllMessageQueuesInternal();
#if OCTK_DCHECK_IS_ON
    void RemoveFromSendGraph(TaskThread *thread) OCTK_ATTRIBUTE_EXCLUSIVE_LOCKS_REQUIRED(crit_);
#endif

    // This list contains all live Threads.
    std::vector<TaskThread *> message_queues_ OCTK_ATTRIBUTE_GUARDED_BY(crit_);

    Mutex crit_;

#if OCTK_DCHECK_IS_ON
    // Represents all thread seand actions by storing all send targets per thread.
    // This is used by RegisterSendAndCheckForCycles. This graph has no cycles
    // since we will trigger a CHECK failure if a cycle is introduced.
    std::map<TaskThread *, std::set<TaskThread *>> send_graph_ OCTK_ATTRIBUTE_GUARDED_BY(crit_);
#endif

#if defined(OCTK_OS_UNIX)
    pthread_key_t key_;
#endif

#if defined(OCTK_OS_WIN)
    const DWORD key_;
#endif
};

// WARNING! SUBCLASSES MUST CALL Stop() IN THEIR DESTRUCTORS!  See ~TaskThread().

class OCTK_ATTRIBUTE_LOCKABLE OCTK_CORE_API TaskThread : public TaskQueue
{
public:
    static const int kForever = -1;

    // Create a new TaskThread and optionally assign it to the passed
    // SocketServer. Subclasses that override Clear should pass false for
    // init_queue and call DoInit() from their constructor to prevent races
    // with the TaskThreadManager using the object while the vtable is still
    // being created.
    explicit TaskThread(SocketServer *ss);
    explicit TaskThread(std::unique_ptr<SocketServer> ss);

    // Constructors meant for subclasses; they should call DoInit themselves and
    // pass false for `do_init`, so that DoInit is called only on the fully
    // instantiated class, which avoids a vptr data race.
    TaskThread(SocketServer *ss, bool do_init);
    TaskThread(std::unique_ptr<SocketServer> ss, bool do_init);

    // NOTE: ALL SUBCLASSES OF TaskThread MUST CALL Stop() IN THEIR DESTRUCTORS (or
    // guarantee Stop() is explicitly called before the subclass is destroyed).
    // This is required to avoid a data race between the destructor modifying the
    // vtable, and the TaskThread::PreRun calling the virtual method Run().

    // NOTE: SUBCLASSES OF TaskThread THAT OVERRIDE Clear MUST CALL
    // DoDestroy() IN THEIR DESTRUCTORS! This is required to avoid a data race
    // between the destructor modifying the vtable, and the TaskThreadManager
    // calling Clear on the object from a different thread.
    ~TaskThread() override;

    TaskThread(const TaskThread &) = delete;
    TaskThread &operator=(const TaskThread &) = delete;

    static std::unique_ptr<TaskThread> CreateWithSocketServer();
    static std::unique_ptr<TaskThread> Create();
    static TaskThread *Current();

    // Used to catch performance regressions. Use this to disallow BlockingCall
    // for a given scope.  If a synchronous call is made while this is in
    // effect, an assert will be triggered.
    // Note that this is a single threaded class.
    class ScopedDisallowBlockingCalls
    {
    public:
        ScopedDisallowBlockingCalls();
        ScopedDisallowBlockingCalls(const ScopedDisallowBlockingCalls &) = delete;
        ScopedDisallowBlockingCalls &operator=(const ScopedDisallowBlockingCalls &) = delete;
        ~ScopedDisallowBlockingCalls();

    private:
        TaskThread *const thread_;
        const bool previous_state_;
    };

#if OCTK_DCHECK_IS_ON

    class ScopedCountBlockingCalls
    {
    public:
        ScopedCountBlockingCalls(std::function<void(uint32_t, uint32_t)> callback);
        ScopedCountBlockingCalls(const ScopedDisallowBlockingCalls &) = delete;
        ScopedCountBlockingCalls &operator=(const ScopedDisallowBlockingCalls &) = delete;
        ~ScopedCountBlockingCalls();

        uint32_t GetBlockingCallCount() const;
        uint32_t GetCouldBeBlockingCallCount() const;
        uint32_t GetTotalBlockedCallCount() const;

        void set_minimum_call_count_for_callback(uint32_t minimum) { min_blocking_calls_for_callback_ = minimum; }

    private:
        TaskThread *const thread_;
        const uint32_t base_blocking_call_count_;
        const uint32_t base_could_be_blocking_call_count_;
        // The minimum number of blocking calls required in order to issue the
        // result_callback_. This is used by OCTK_DCHECK_BLOCK_COUNT_NO_MORE_THAN to
        // tame log spam.
        // By default we always issue the callback, regardless of callback count.
        uint32_t min_blocking_calls_for_callback_ = 0;
        std::function<void(uint32_t, uint32_t)> result_callback_;
    };

    uint32_t GetBlockingCallCount() const;
    uint32_t GetCouldBeBlockingCallCount() const;
#endif

    SocketServer *socketserver();

    // Note: The behavior of TaskThread has changed.  When a thread is stopped,
    // futher Posts and Sends will fail.  However, any pending Sends and *ready*
    // Posts (as opposed to unexpired delayed Posts) will be delivered before
    // Get (or Peek) returns false.  By guaranteeing delivery of those messages,
    // we eliminate the race condition when an MessageHandler and TaskThread
    // may be destroyed independently of each other.
    virtual void Quit();
    virtual bool IsQuitting();
    virtual void Restart();
    // Not all message queues actually process messages (such as SignalThread).
    // In those cases, it's important to know, before posting, that it won't be
    // Processed.  Normally, this would be true until IsQuitting() is true.
    virtual bool IsProcessingMessagesForTesting();

    // Amount of time until the next message can be retrieved
    virtual int GetDelay();

    bool empty() const { return size() == 0u; }
    size_t size() const
    {
        Mutex::Locker locker(&mutex_);
        return messages_.size() + delayed_messages_.size();
    }

    bool IsCurrent() const;

    // Sleeps the calling thread for the specified number of milliseconds, during
    // which time no processing is performed. Returns false if sleeping was
    // interrupted by a signal (POSIX only).
    static bool SleepMs(int millis);

    // Sets the thread's name, for debugging. Must be called before Start().
    // If `obj` is non-null, its value is appended to `name`.
    const std::string &name() const { return name_; }
    bool SetName(StringView name, const void *obj);

    // Sets the expected processing time in ms. The thread will write
    // log messages when Dispatch() takes more time than this.
    // Default is 50 ms.
    void SetDispatchWarningMs(int deadline);

    // Starts the execution of the thread.
    bool Start();

    // Tells the thread to stop and waits until it is joined.
    // Never call Stop on the current thread.  Instead use the inherited Quit
    // function which will exit the base TaskThread without terminating the
    // underlying OS thread.
    virtual void Stop();

    // By default, TaskThread::Run() calls ProcessMessages(kForever).  To do other
    // work, override Run().  To receive and dispatch messages, call
    // ProcessMessages occasionally.
    virtual void Run();

    // Convenience method to invoke a functor on another thread.
    // Blocks the current thread until execution is complete.
    // Ex: thread.BlockingCall([&] { result = MyFunctionReturningBool(); });
    // NOTE: This function can only be called when synchronous calls are allowed.
    // See ScopedDisallowBlockingCalls for details.
    // NOTE: Blocking calls are DISCOURAGED, consider if what you're doing can
    // be achieved with PostTask() and callbacks instead.
    void BlockingCall(FunctionView<void()> functor, const SourceLocation &location = SourceLocation::current())
    {
        BlockingCallImpl(std::move(functor), location);
    }

    template <typename Functor,
              typename ReturnT = type_traits::invoke_result_t<Functor>,
              typename = typename std::enable_if<!std::is_void<ReturnT>::value>::type>
    ReturnT BlockingCall(Functor &&functor, const SourceLocation &location = SourceLocation::current())
    {
        ReturnT result;
        BlockingCall([&] { result = std::forward<Functor>(functor)(); }, location);
        return result;
    }

    // Allows BlockingCall to specified `thread`. TaskThread never will be
    // dereferenced and will be used only for reference-based comparison, so
    // instance can be safely deleted. If NDEBUG is defined and OCTK_DCHECK_IS_ON
    // is undefined do nothing.
    void AllowInvokesToTaskThread(TaskThread *thread);

    // If NDEBUG is defined and OCTK_DCHECK_IS_ON is undefined do nothing.
    void DisallowAllInvokes();
    // Returns true if `target` was allowed by AllowInvokesToTaskThread() or if no
    // calls were made to AllowInvokesToTaskThread and DisallowAllInvokes. Otherwise
    // returns false.
    // If NDEBUG is defined and OCTK_DCHECK_IS_ON is undefined always returns
    // true.
    bool IsInvokeToTaskThreadAllowed(TaskThread *target);

    // From TaskQueue
    void Delete() override;

    // ProcessMessages will process I/O and dispatch messages until:
    //  1) cms milliseconds have elapsed (returns true)
    //  2) Stop() is called (returns false)
    bool ProcessMessages(int cms);

    // Returns true if this is a thread that we created using the standard
    // constructor, false if it was created by a call to
    // TaskThreadManager::WrapCurrentTaskThread().  The main thread of an application
    // is generally not owned, since the OS representation of the thread
    // obviously exists before we can get to it.
    // You cannot call Start on non-owned threads.
    bool IsOwned();

    // Expose private method IsRunning() for tests.
    //
    // DANGER: this is a terrible public API.  Most callers that might want to
    // call this likely do not have enough control/knowledge of the TaskThread in
    // question to guarantee that the returned value remains true for the duration
    // of whatever code is conditionally executing because of the return value!
    bool RunningForTest() { return IsRunning(); }

    // These functions are public to avoid injecting test hooks. Don't call them
    // outside of tests.
    // This method should be called when thread is created using non standard
    // method, like derived implementation of TaskThread and it can not be
    // started by calling Start(). This will set started flag to true and
    // owned to false. This must be called from the current thread.
    bool WrapCurrent();
    void UnwrapCurrent();

    // Sets the per-thread allow-blocking-calls flag to false; this is
    // irrevocable. Must be called on this thread.
    void DisallowBlockingCalls() { SetAllowBlockingCalls(false); }

protected:
    class CurrentTaskThreadSetter : CurrentTaskQueueSetter
    {
    public:
        explicit CurrentTaskThreadSetter(TaskThread *thread)
            : CurrentTaskQueueSetter(thread)
            , manager_(TaskThreadManager::Instance())
            , previous_(manager_->CurrentTaskThread())
        {
            manager_->ChangeCurrentTaskThreadForTest(thread);
        }
        ~CurrentTaskThreadSetter() { manager_->ChangeCurrentTaskThreadForTest(previous_); }

    private:
        TaskThreadManager *const manager_;
        TaskThread *const previous_;
    };

    // DelayedMessage goes into a priority queue, sorted by trigger time. Messages
    // with the same trigger time are processed in num_ (FIFO) order.
    struct DelayedMessage
    {
        bool operator<(const DelayedMessage &dmsg) const
        {
            return (dmsg.run_time_ms < run_time_ms) ||
                   ((dmsg.run_time_ms == run_time_ms) && (dmsg.message_number < message_number));
        }

        int64_t delay_ms; // for debugging
        int64_t run_time_ms;
        // Monotonicaly incrementing number used for ordering of messages
        // targeted to execute at the same time.
        uint32_t message_number;
        // std::priority_queue doesn't allow to extract elements, but functor
        // is move-only and thus need to be changed when pulled out of the
        // priority queue. That is ok because `functor` doesn't affect operator<
        mutable Task functor;
    };

    // TaskQueue implementation.
    void PostTaskImpl(Task task, const PostTaskTraits &traits, const SourceLocation &location) override;
    void PostDelayedTaskImpl(Task task,
                             TimeDelta delay,
                             const PostDelayedTaskTraits &traits,
                             const SourceLocation &location) override;

    virtual void BlockingCallImpl(FunctionView<void()> functor, const SourceLocation &location);

    // Perform initialization, subclasses must call this from their constructor
    // if false was passed as init_queue to the TaskThread constructor.
    void DoInit();

    // Perform cleanup; subclasses must call this from the destructor,
    // and are not expected to actually hold the lock.
    void DoDestroy() OCTK_ATTRIBUTE_EXCLUSIVE_LOCKS_REQUIRED(mutex_);

    void WakeUpSocketServer();

    // Same as WrapCurrent except that it never fails as it does not try to
    // acquire the synchronization access of the thread. The caller should never
    // call Stop() or Join() on this thread.
    void SafeWrapCurrent();

    // Blocks the calling thread until this thread has terminated.
    void Join();

    static void AssertBlockingIsAllowedOnCurrentTaskThread();

    friend class ScopedDisallowBlockingCalls;

private:
    static const int kSlowDispatchLoggingThreshold = 50; // 50 ms

    // Get() will process I/O until:
    //  1) A task is available (returns it)
    //  2) cmsWait seconds have elapsed (returns empty task)
    //  3) Stop() is called (returns empty task)
    Task Get(int cmsWait);
    void Dispatch(Task task);

    // Sets the per-thread allow-blocking-calls flag and returns the previous
    // value. Must be called on this thread.
    bool SetAllowBlockingCalls(bool allow);

#if defined(OCTK_OS_WIN)
    static DWORD WINAPI PreRun(LPVOID context);
#else
    static void *PreRun(void *pv);
#endif

    // TaskThreadManager calls this instead WrapCurrent() because
    // TaskThreadManager::Instance() cannot be used while TaskThreadManager is
    // being created.
    // The method tries to get synchronization rights of the thread on Windows if
    // `need_synchronize_access` is true.
    bool WrapCurrentWithTaskThreadManager(TaskThreadManager *thread_manager, bool need_synchronize_access);

    // Return true if the thread is currently running.
    bool IsRunning();

    // Called by the TaskThreadManager when being set as the current thread.
    void EnsureIsCurrentTaskQueue();

    // Called by the TaskThreadManager when being unset as the current thread.
    void ClearCurrentTaskQueue();

    std::queue<Task> messages_ OCTK_ATTRIBUTE_GUARDED_BY(mutex_);
    std::priority_queue<DelayedMessage> delayed_messages_ OCTK_ATTRIBUTE_GUARDED_BY(mutex_);
    uint32_t delayed_next_num_ OCTK_ATTRIBUTE_GUARDED_BY(mutex_);
#if OCTK_DCHECK_IS_ON
    uint32_t blocking_call_count_ OCTK_ATTRIBUTE_GUARDED_BY(this) = 0;
    uint32_t could_be_blocking_call_count_ OCTK_ATTRIBUTE_GUARDED_BY(this) = 0;
    std::vector<TaskThread *> allowed_threads_ OCTK_ATTRIBUTE_GUARDED_BY(this);
    bool invoke_policy_enabled_ OCTK_ATTRIBUTE_GUARDED_BY(this) = false;
#endif
    mutable Mutex mutex_;
    bool fInitialized_;
    bool fDestroyed_;

    std::atomic<int> stop_;

    // The SocketServer might not be owned by TaskThread.
    SocketServer *const ss_;
    // Used if SocketServer ownership lies with `this`.
    std::unique_ptr<SocketServer> own_ss_;

    std::string name_;
    std::string mIdString;

    // TODO(tommi): Add thread checks for proper use of control methods.
    // Ideally we should be able to just use PlatformThread.

#if defined(OCTK_OS_UNIX)
    pthread_t thread_ = 0;
#endif
    SpinLock mStartSpinLock;
    // std::thread mThread;

#if defined(OCTK_OS_WIN)
    HANDLE thread_ = nullptr;
    DWORD thread_id_ = 0;
#endif

    // Indicates whether or not ownership of the worker thread lies with
    // this instance or not. (i.e. owned_ == !wrapped).
    // Must only be modified when the worker thread is not running.
    bool owned_ = true;

    // Only touched from the worker thread itself.
    bool blocking_calls_allowed_ = true;

    std::unique_ptr<TaskQueue::CurrentTaskQueueSetter> task_queue_registration_;

    friend class TaskThreadManager;

    int dispatch_warning_ms_ OCTK_ATTRIBUTE_GUARDED_BY(this) = kSlowDispatchLoggingThreshold;
};

// AutoTaskThread automatically installs itself at construction
// uninstalls at destruction, if a TaskThread object is
// _not already_ associated with the current OS thread.
//
// NOTE: *** This class should only be used by tests ***
//
class AutoTaskThread : public TaskThread
{
public:
    AutoTaskThread();
    ~AutoTaskThread() override;

    AutoTaskThread(const AutoTaskThread &) = delete;
    AutoTaskThread &operator=(const AutoTaskThread &) = delete;
};

// AutoSocketServerTaskThread automatically installs itself at
// construction and uninstalls at destruction. If a TaskThread object is
// already associated with the current OS thread, it is temporarily
// disassociated and restored by the destructor.
//
class AutoSocketServerTaskThread : public TaskThread
{
public:
    explicit AutoSocketServerTaskThread(SocketServer *ss);
    ~AutoSocketServerTaskThread() override;

    AutoSocketServerTaskThread(const AutoSocketServerTaskThread &) = delete;
    AutoSocketServerTaskThread &operator=(const AutoSocketServerTaskThread &) = delete;

private:
    TaskThread *old_thread_;
};
OCTK_END_NAMESPACE

#endif // _OCTK_TASK_THREAD_HPP
