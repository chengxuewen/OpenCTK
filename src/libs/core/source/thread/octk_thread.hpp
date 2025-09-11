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

#ifndef _OCTK_THREAD_HPP
#define _OCTK_THREAD_HPP

#include <octk_global.hpp>

#include <memory>

OCTK_BEGIN_NAMESPACE

class ThreadPrivate;

class OCTK_CORE_API Thread
{
public:
#if defined(OCTK_OS_WIN)
    using Handle = void *;
#else
    using Handle = pthread_t;
#endif

    enum class Priority
    {
        kLow = 1,
        kNormal,
        kHigh,
        kRealtime,
    };

    struct attributes
    {
        Priority priority = Priority::kNormal;
        attributes &SetPriority(Priority p)
        {
            priority = p;
            return *this;
        }
    };

    Thread();
    explicit Thread(ThreadPrivate *d);
    virtual ~Thread();

//    static bool SleepMSecs(int millis);
//    bool IsCurrent() const;
//
//    template <typename Function, typename... Args>
//    void Start(Function &&f, Args &&... args);
//
//    static Thread *Current();

    /**
     * @brief Request rescheduling of threads.
     */
    static void yield();

protected:
    OCTK_DEFINE_DPTR(Thread);
    OCTK_DECLARE_PRIVATE(Thread);
    OCTK_DISABLE_COPY_MOVE(Thread)
};
OCTK_END_NAMESPACE

#endif  // _OCTK_THREAD_HPP
