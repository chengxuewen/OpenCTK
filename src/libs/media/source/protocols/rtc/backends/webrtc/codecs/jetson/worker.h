#pragma once

#include <rtc_base/platform_thread.h>

#include <functional>
#include <atomic>
#include <string>

#include <octk_concurrent_queue.hpp>
#include <octk_reader_writer_queue.hpp>

template <typename T>
#if 0
using LockFreeQueue = octk::ConcurrentQueue<T>;
#else
using LockFreeQueue = octk::ReaderWriterQueue<T>;
#endif

class Worker
{
public:
    Worker(std::string name, std::function<void()> executing_function);
    ~Worker();
    void Run();

private:
    std::atomic<bool> abort_;
    std::string name_;
    std::function<void()> executing_function_;
    rtc::PlatformThread thread_;

    void Thread();
};