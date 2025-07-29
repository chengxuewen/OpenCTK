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

#ifndef _OCTK_MUTEX_HPP
#define _OCTK_MUTEX_HPP

#include <octk_engine_global.hpp>

#include <mutex>
#include <atomic>

OCTK_BEGIN_NAMESPACE

template <typename T>
class MutexLocker
{
public:
    using MutexType = T;

    MutexLocker(MutexType& mutex, bool lock = true)
        : mMutex(mutex)
    {
        if (lock) { this->lock(); }
    }

    ~MutexLocker() noexcept { this->unlock(); }

    void unlock()
    {
        if (mLocked.exchange(false))
        {
            mMutex.unlock();
        }
    }

    void lock()
    {
        mMutex.lock();
        mLocked.store(true);
    }

private:
    MutexType& mMutex;
    std::atomic_bool mLocked = ATOMIC_VAR_INIT(false);
    OCTK_DISABLE_COPY_MOVE(MutexLocker)
};

OCTK_END_NAMESPACE

#endif // _OCTK_MUTEX_HPP
