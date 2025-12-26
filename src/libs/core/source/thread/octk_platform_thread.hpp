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

#pragma once

#include <octk_status.hpp>

#include <thread>

#if defined(OCTK_OS_WIN)
#    define OCTK_PLATFORM_THREAD_HANDLE_T void *;
#else
#    define OCTK_PLATFORM_THREAD_HANDLE_T pthread_t;
#endif // defined(OCTK_OS_WIN)

OCTK_BEGIN_NAMESPACE

class PlatformThreadPrivate;
class OCTK_CORE_API PlatformThread
{
public:
    using Handle = void *;
    class Id
    {
    public:
        using Handle = Handle;

        Id() = default;
        Id(Handle handle) noexcept { mHandle = handle; }
        Id(const Id &other) noexcept { mHandle = other.mHandle; }
        Id(Id &&other) noexcept { std::swap(mHandle, other.mHandle); }
        ~Id() = default;

        Handle handle() const { return mHandle; }
        Id &operator=(const Id &other) noexcept
        {
            mHandle = other.mHandle;
            return *this;
        }
        Id &operator=(Id &&other) noexcept
        {
            mHandle = std::move(other.mHandle);
            return *this;
        }

        std::string toString(bool hex = false) const
        {
            if (hex)
            {
                char buf[30];
                std::snprintf(buf, sizeof(buf), " 0x%p", mHandle);
                return buf;
            }
            return std::to_string(reinterpret_cast<uint64_t>(mHandle));
        }
        std::string toHexString(bool hex = false) const { return this->toString(true); }

        bool isEqual(const Id &other) const noexcept;
        bool operator==(const Id &other) const noexcept { return this->isEqual(other); }
        bool operator!=(const Id &other) const noexcept { return !this->isEqual(other); }

    private:
        Handle mHandle{nullptr};
    };

    enum class Priority : int
    {
        kIdle,
        kLowest,
        kLow,
        kNormal,
        kHigh,
        kHighest,
        kTimeCritical,
        kInherit,
    };

    PlatformThread();
    PlatformThread(PlatformThreadPrivate *d);
    virtual ~PlatformThread();

    bool isInterruptionRequested() const;
    Status requestInterruption();

    const std::string &name() const;
    Status setName(const StringView name, const void *obj = nullptr);

    /**
     * If the thread is not running, this function returns \c InheritPriority.
     * \sa Priority, setPriority(), start()
     * @return Returns the priority for a running thread.
     */
    Priority priority() const;
    /**
     * This function sets the \a priority for a running thread.
     * If the thread is not running, this function does nothing and returns immediately.
     * Use start() to start a thread with a specific priority.
     *
     * The \a priority argument can be any value in the \c Priority enum except for \c InheritPriority.
     * The effect of the \a priority parameter is dependent on the operating system's scheduling policy.
     * In particular, the \a priority will be ignored on systems that do not support thread priorities
     * (such as on Linux, see http://linux.die.net/man/2/sched_setscheduler for more details).
     *
     * \sa Priority, priority(), start()
     * @param priority
     * @return
     */
    Status setPriority(Priority priority);

    /**
     * @return Returns the maximum stack size for the thread (if set with setStackSize()); otherwise returns zero.
     */
    uint stackSize() const;
    /**
     * Sets the maximum stack size for the thread to \a stackSize.
     * If \a stackSize is greater than zero, the maximum stack size is set to \a stackSize bytes,
     * otherwise the maximum stack size is automatically determined by the operating system.
     *
     * \warning Most operating systems place minimum and maximum limits on thread stack sizes.
     * The thread will fail to start if the stack size is outside these limits.
     *
     * \sa stackSize()
     * @param stackSize
     * @return
     */
    Status setStackSize(uint stackSize);

    bool isFinished() const;
    bool isRunning() const;
    Id id() const;

    void start(Priority = Priority::kInherit);
    void terminate();

    bool wait(unsigned long msecs = std::numeric_limits<unsigned long>::max());

    static PlatformThread *currentThread() noexcept;
    static Id currentThreadId() noexcept;

    // static int idealConcurrencyCount() noexcept;

    static void yieldCurrentThread();

    static void usleep(unsigned long usecs);
    static void msleep(unsigned long msecs);
    static void sleep(unsigned long secs);
    static void exit(int code = 0);

protected:
    static void setTerminationEnabled(bool enabled = true);

    // virtual void run();

protected:
    OCTK_DEFINE_DPTR(PlatformThread)
    OCTK_DECLARE_PRIVATE(PlatformThread)
    OCTK_DISABLE_COPY_MOVE(PlatformThread)
};

OCTK_END_NAMESPACE