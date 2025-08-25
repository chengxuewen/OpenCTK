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

#ifndef _OCTK_PLATFORM_THREAD_HPP
#define _OCTK_PLATFORM_THREAD_HPP

#include <octk_optional.hpp>
#include <octk_string_view.hpp>

#include <functional>

#if defined(OCTK_OS_UNIX)
#   include <pthread.h>
#endif  // defined(OCTK_OS_UNIX)

OCTK_BEGIN_NAMESPACE

enum class ThreadPriority
{
    kLow = 1,
    kNormal,
    kHigh,
    kRealtime,
};

struct ThreadAttributes
{
    ThreadPriority priority = ThreadPriority::kNormal;
    ThreadAttributes &SetPriority(ThreadPriority priority_param)
    {
        priority = priority_param;
        return *this;
    }
};

// Represents a simple worker thread.
class OCTK_CORE_API PlatformThread final
{
public:
    // Handle is the base platform thread handle.
#if defined(OCTK_OS_WIN)
    using Handle = void *;
    using Ref = unsigned long;
    using Id = unsigned long;
#else
    using Handle = pthread_t;
    using Ref = pthread_t;
    using Id = pid_t;
#endif  // defined(OCTK_OS_WIN)

    // This ctor creates the PlatformThread with an unset handle (returning true
    // in empty()) and is provided for convenience.
    // TODO(bugs.webrtc.org/12727) Look into if default and move support can be
    // removed.
    PlatformThread() = default;

    // Moves `rhs` into this, storing an empty state in `rhs`.
    // TODO(bugs.webrtc.org/12727) Look into if default and move support can be
    // removed.
    PlatformThread(PlatformThread &&rhs);

    // Copies won't work since we'd have problems with joinable threads.
    PlatformThread(const PlatformThread &) = delete;
    PlatformThread &operator=(const PlatformThread &) = delete;

    // Moves `rhs` into this, storing an empty state in `rhs`.
    // TODO(bugs.webrtc.org/12727) Look into if default and move support can be
    // removed.
    PlatformThread &operator=(PlatformThread &&rhs);

    // For a PlatformThread that's been spawned joinable, the destructor suspends
    // the calling thread until the created thread exits unless the thread has
    // already exited.
    virtual ~PlatformThread();

    // Finalizes any allocated resources.
    // For a PlatformThread that's been spawned joinable, Finalize() suspends
    // the calling thread until the created thread exits unless the thread has
    // already exited.
    // empty() returns true after completion.
    void Finalize();

    // Returns true if default constructed, moved from, or Finalize()ed.
    bool empty() const { return !handle_.has_value(); }

    // Creates a started joinable thread which will be joined when the returned
    // PlatformThread destructs or Finalize() is called.
    static PlatformThread SpawnJoinable(std::function<void()> thread_function,
                                        StringView name,
                                        ThreadAttributes attributes = ThreadAttributes());

    // Creates a started detached thread. The caller has to use external
    // synchronization as nothing is provided by the PlatformThread construct.
    static PlatformThread SpawnDetached(std::function<void()> thread_function,
                                        StringView name,
                                        ThreadAttributes attributes = ThreadAttributes());

    // Returns the base platform thread handle of this thread.
    Optional<Handle> GetHandle() const;

#if defined(OCTK_OS_WIN)
    // Queue a Windows APC function that runs when the thread is alertable.
  bool QueueAPC(PAPCFUNC apc_function, ULONG_PTR data);
#endif

    static Id currentThreadId();
    static Ref currentThreadRef();
    static std::string currentThreadIdString();
    static std::string currentThreadIdHexString();
    static bool isThreadRefEqual(const Ref &lhs, const Ref &rhs);

private:
    PlatformThread(Handle handle, bool joinable);
    static PlatformThread SpawnThread(std::function<void()> thread_function,
                                      StringView name,
                                      ThreadAttributes attributes,
                                      bool joinable);

    Optional<Handle> handle_;
    bool joinable_ = false;
};
OCTK_END_NAMESPACE

#endif // _OCTK_PLATFORM_THREAD_HPP
