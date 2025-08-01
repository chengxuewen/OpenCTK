/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present chengxuewen.
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

#ifndef _OCTK_THREAD_POOL_P_HPP
#define _OCTK_THREAD_POOL_P_HPP

#include <octk_thread_pool.hpp>

#include <mutex>
#include <queue>
#include <thread>
#include <condition_variable>

OCTK_BEGIN_NAMESPACE

class OCTK_CORE_API ThreadPoolPrivate
{
public:
    explicit ThreadPoolPrivate(ThreadPool *p);
    virtual ~ThreadPoolPrivate();

    void dispatch(void);

    bool mQuit;
    std::string mName;
    std::mutex mMutex;
    std::vector<std::thread> mThreads;
    std::condition_variable mCondition;
    std::queue<ThreadPool::TaskFunc> mQueue;

private:
    OCTK_DEFINE_PPTR(ThreadPool)
    OCTK_DECLARE_PUBLIC(ThreadPool)
    OCTK_DISABLE_COPY_MOVE(ThreadPoolPrivate)
};

OCTK_END_NAMESPACE

#endif // _OCTK_THREAD_POOL_P_HPP
