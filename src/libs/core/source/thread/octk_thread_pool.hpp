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

#ifndef _OCTK_THREAD_POOL_HPP
#define _OCTK_THREAD_POOL_HPP

#include <octk_global.hpp>

#include <string>
#include <functional>

OCTK_BEGIN_NAMESPACE

class ThreadPoolPrivate;
class OCTK_CORE_API ThreadPool
{
public:
    enum Priority : int8_t
    {
        kLowest = -128,
        kLow = -64,
        kNormal = 0,
        kHigh = +64,
        kHighest = +127
    };

    class Task
    {
        OCTK_DISABLE_COPY_MOVE(Task)
        struct Deleter final
        {
            const bool autoDelete;
            void operator()(Task *task) const
            {
                if (autoDelete)
                {
                    delete task;
                }
            }
        };

    public:
        using SharedPtr = std::shared_ptr<Task>;

        Task() = default;
        virtual ~Task() = default;

        virtual void run() = 0;

        static SharedPtr create(std::function<void()> function);
        static SharedPtr makeShared(Task *task, bool autoDelete = false);
    };

    using TaskFunc = std::function<void(void)>;

    ThreadPool();
    ThreadPool(ThreadPoolPrivate *d);
    virtual ~ThreadPool();

    static ThreadPool *defaultInstance();

    void start(std::function<void()> function, Priority priority = Priority::kNormal);
    bool tryStart(std::function<void()> function);

    void start(const Task::SharedPtr &task, Priority priority = Priority::kNormal);
    bool tryStart(const Task::SharedPtr &task);

    int maxThreadCount() const;
    void setMaxThreadCount(int count);

    int expiryTimeout() const;
    void setExpiryTimeout(int msecs);

    OCTK_STATIC_CONSTANT_NUMBER(kWaitForeverMSecs, std::numeric_limits<unsigned long>::max())
    bool waitForDone(unsigned long msecs = kWaitForeverMSecs);
    // bool contains(Task *task) const;
    // void cancel(Task *task);
    // bool take(Task *task);
    void clear();

    int maxQueueSize() const;

protected:
    OCTK_DEFINE_DPTR(ThreadPool)
    OCTK_DECLARE_PRIVATE(ThreadPool)
    OCTK_DISABLE_COPY_MOVE(ThreadPool)
};
OCTK_END_NAMESPACE

#endif // _OCTK_THREAD_POOL_HPP
