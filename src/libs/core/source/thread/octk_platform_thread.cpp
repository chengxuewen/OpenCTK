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

#include <octk_platform_thread.hpp>
#include <octk_checks.hpp>

#include <thread>

OCTK_BEGIN_NAMESPACE

namespace
{

#if defined(OCTK_OS_WIN)
int Win32PriorityFromThreadPriority(ThreadPriority priority) {
  switch (priority) {
    case ThreadPriority::kLow:
      return THREAD_PRIORITY_BELOW_NORMAL;
    case ThreadPriority::kNormal:
      return THREAD_PRIORITY_NORMAL;
    case ThreadPriority::kHigh:
      return THREAD_PRIORITY_ABOVE_NORMAL;
    case ThreadPriority::kRealtime:
      return THREAD_PRIORITY_TIME_CRITICAL;
  }
  return THREAD_PRIORITY_NORMAL;
}
#endif

bool SetPriority(ThreadPriority priority)
{
#if defined(OCTK_OS_WIN)
    return SetThreadPriority(GetCurrentThread(),
                           Win32PriorityFromThreadPriority(priority)) != FALSE;
#elif defined(__native_client__) || defined(OCTK_OS_FUCHSIA) || \
    (defined(__EMSCRIPTEN__) && !defined(__EMSCRIPTEN_PTHREADS__))
    // Setting thread priorities is not supported in NaCl, Fuchsia or Emscripten
  // without pthreads.
  return true;
#elif defined(OCTK_OS_LINUX)
    // TODO(tommi): Switch to the same mechanism as Chromium uses for changing
  // thread priorities.
  return true;
#else
    const int policy = SCHED_FIFO;
    const int min_prio = sched_get_priority_min(policy);
    const int max_prio = sched_get_priority_max(policy);
    if (min_prio == -1 || max_prio == -1)
    {
        return false;
    }

    if (max_prio - min_prio <= 2)
    {
        return false;
    }

    // Convert webrtc priority to system priorities:
    sched_param param;
    const int top_prio = max_prio - 1;
    const int low_prio = min_prio + 1;
    switch (priority)
    {
        case ThreadPriority::kLow:
            param.sched_priority = low_prio;
            break;
        case ThreadPriority::kNormal:
            // The -1 ensures that the kHighPriority is always greater or equal to
            // kNormalPriority.
            param.sched_priority = (low_prio + top_prio - 1) / 2;
            break;
        case ThreadPriority::kHigh:
            param.sched_priority = std::max(top_prio - 2, low_prio);
            break;
        case ThreadPriority::kRealtime:
            param.sched_priority = top_prio;
            break;
    }
    return pthread_setschedparam(pthread_self(), policy, &param) == 0;
#endif  // defined(OCTK_OS_WIN)
}

#if defined(OCTK_OS_WIN)
DWORD WINAPI RunPlatformThread(void* param) {
  // The GetLastError() function only returns valid results when it is called
  // after a Win32 API function that returns a "failed" result. A crash dump
  // contains the result from GetLastError() and to make sure it does not
  // falsely report a Windows error we call SetLastError here.
  ::SetLastError(ERROR_SUCCESS);
  auto function = static_cast<std::function<void()>*>(param);
  (*function)();
  delete function;
  return 0;
}
#else
void *RunPlatformThread(void *param)
{
    auto function = static_cast<std::function<void()> *>(param);
    (*function)();
    delete function;
    return 0;
}
#endif  // defined(OCTK_OS_WIN)

void SetCurrentThreadName(const char *name)
{
#if defined(WEBOCTK_WIN)
    // The SetThreadDescription API works even if no debugger is attached.
  // The names set with this API also show up in ETW traces. Very handy.
  static auto set_thread_description_func =
      reinterpret_cast<OCTK_SetThreadDescription>(::GetProcAddress(
          ::GetModuleHandleA("Kernel32.dll"), "SetThreadDescription"));
  if (set_thread_description_func) {
    // Convert from ASCII to UTF-16.
    wchar_t wide_thread_name[64];
    for (size_t i = 0; i < arraysize(wide_thread_name) - 1; ++i) {
      wide_thread_name[i] = name[i];
      if (wide_thread_name[i] == L'\0')
        break;
    }
    // Guarantee null-termination.
    wide_thread_name[arraysize(wide_thread_name) - 1] = L'\0';
    set_thread_description_func(::GetCurrentThread(), wide_thread_name);
  }

  // For details see:
  // https://docs.microsoft.com/en-us/visualstudio/debugger/how-to-set-a-thread-name-in-native-code
#pragma pack(push, 8)
  struct {
    DWORD dwType;
    LPCSTR szName;
    DWORD dwThreadID;
    DWORD dwFlags;
  } threadname_info = {0x1000, name, static_cast<DWORD>(-1), 0};
#pragma pack(pop)

#pragma warning(push)
#pragma warning(disable : 6320 6322)
  __try {
    ::RaiseException(0x406D1388, 0, sizeof(threadname_info) / sizeof(ULONG_PTR),
                     reinterpret_cast<ULONG_PTR*>(&threadname_info));
  } __except (EXCEPTION_EXECUTE_HANDLER) {  // NOLINT
  }
#pragma warning(pop)
#elif defined(OCTK_OS_LINUX) || defined(OCTK_OS_ANDROID)
    prctl(PR_SET_NAME, reinterpret_cast<unsigned long>(name));  // NOLINT
#elif defined(OCTK_OS_MAC) || defined(OCTK_OS_IOS)
    pthread_setname_np(name);
#elif defined(OCTK_OS_FUCHSIA)
    zx_status_t status = zx_object_set_property(zx_thread_self(), ZX_PROP_NAME,
                                              name, strlen(name));
  OCTK_DCHECK_EQ(status, ZX_OK);
#endif
}
}  // namespace

PlatformThread::PlatformThread(Handle handle, bool joinable)
    : handle_(handle), joinable_(joinable) {}

PlatformThread::PlatformThread(PlatformThread &&rhs)
    : handle_(rhs.handle_), joinable_(rhs.joinable_)
{
    rhs.handle_ = utils::nullopt;
}

PlatformThread &PlatformThread::operator=(PlatformThread &&rhs)
{
    Finalize();
    handle_ = rhs.handle_;
    joinable_ = rhs.joinable_;
    rhs.handle_ = utils::nullopt;
    return *this;
}

PlatformThread::~PlatformThread()
{
    Finalize();
}

PlatformThread PlatformThread::SpawnJoinable(std::function<void()> thread_function,
                                             StringView name,
                                             ThreadAttributes attributes)
{
    return SpawnThread(std::move(thread_function), name, attributes, /*joinable=*/true);
}

PlatformThread PlatformThread::SpawnDetached(std::function<void()> thread_function,
                                             StringView name,
                                             ThreadAttributes attributes)
{
    return SpawnThread(std::move(thread_function), name, attributes, /*joinable=*/false);
}

Optional<PlatformThread::Handle> PlatformThread::GetHandle() const
{
    return handle_;
}

#if defined(OCTK_OS_WIN)
bool PlatformThread::QueueAPC(PAPCFUNC function, ULONG_PTR data) {
  OCTK_DCHECK(handle_.has_value());
  return handle_.has_value() ? QueueUserAPC(function, *handle_, data) != FALSE
                             : false;
}
#endif

void PlatformThread::Finalize()
{
    if (!handle_.has_value())
    {
        return;
    }
#if defined(OCTK_OS_WIN)
    if (joinable_)
    WaitForSingleObject(*handle_, INFINITE);
  CloseHandle(*handle_);
#else
    if (joinable_)
    {
        OCTK_CHECK_EQ(0, pthread_join(*handle_, nullptr));
    }
#endif
    handle_ = utils::nullopt;
}

PlatformThread PlatformThread::SpawnThread(std::function<void()> thread_function,
                                           StringView name,
                                           ThreadAttributes attributes,
                                           bool joinable)
{
    OCTK_DCHECK(thread_function);
    OCTK_DCHECK(!name.empty());
    // TODO(tommi): Consider lowering the limit to 15 (limit on Linux).
    OCTK_DCHECK(name.length() < 64);
    auto threadName = std::string(name);
    auto start_thread_function_ptr = new std::function<void()>([thread_function, threadName, attributes] {
        SetCurrentThreadName(threadName.c_str());
        SetPriority(attributes.priority);
        thread_function();
    });
#if defined(OCTK_OS_WIN)
    // See bug 2902 for background on STACK_SIZE_PARAM_IS_A_RESERVATION.
  // Set the reserved stack stack size to 1M, which is the default on Windows
  // and Linux.
  DWORD thread_id = 0;
  PlatformThread::Handle handle = ::CreateThread(nullptr, 1024 * 1024, &RunPlatformThread, start_thread_function_ptr,
      STACK_SIZE_PARAM_IS_A_RESERVATION, &thread_id);
  OCTK_CHECK(handle) << "CreateThread failed";
#else
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    // Set the stack stack size to 1M.
    pthread_attr_setstacksize(&attr, 1024 * 1024);
    pthread_attr_setdetachstate(&attr, joinable ? PTHREAD_CREATE_JOINABLE : PTHREAD_CREATE_DETACHED);
    PlatformThread::Handle handle;
    OCTK_CHECK_EQ(0, pthread_create(&handle, &attr, &RunPlatformThread, start_thread_function_ptr));
    pthread_attr_destroy(&attr);
#endif  // defined(OCTK_OS_WIN)
    return PlatformThread(handle, joinable);
}

PlatformThread::Id PlatformThread::currentThreadId()
{
#if defined(OCTK_OS_WIN)
    return GetCurrentThreadId();
#else
#   if defined(OCTK_OS_MAC) || defined(OCTK_OS_IOS)
    return pthread_mach_thread_np(pthread_self());
#   elif defined(OCTK_OS_ANDROID)
    return gettid();
#   elif defined(OCTK_OS_LINUX)
    return syscall(__NR_gettid);
#   elif defined(__EMSCRIPTEN__)
    return static_cast<PlatformThread::Id>(pthread_self());
#   else
    // Default implementation for nacl and solaris.
    return reinterpret_cast<PlatformThread::Id>(pthread_self());
#   endif
#endif
}

PlatformThread::Ref PlatformThread::currentThreadRef()
{
#if defined(OCTK_OS_WIN)
    return GetCurrentThreadId();
#else
    return pthread_self();
#endif
}

std::string PlatformThread::currentThreadIdString()
{
    return std::to_string(PlatformThread::currentThreadId());
}

std::string PlatformThread::currentThreadIdHexString()
{
    std::stringstream ss;
    ss << std::this_thread::get_id();
    return ss.str();
}

bool PlatformThread::isThreadRefEqual(const PlatformThread::Ref &lhs, const PlatformThread::Ref &rhs)
{
#if defined(OCTK_OS_WIN)
    return lhs == rhs;
#else
    return pthread_equal(lhs, rhs);
#endif
}
OCTK_END_NAMESPACE
