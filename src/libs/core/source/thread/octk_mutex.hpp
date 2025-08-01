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

#include <octk_global.hpp>
#include <octk_assert.hpp>

#include <mutex>

OCTK_BEGIN_NAMESPACE

class Mutex : public std::mutex
{
public:
    using Base = std::mutex;

    class Locker
    {
    public:
        Locker(Base &mutex)
            : mMutex(mutex)
        {
            OCTK_ASSERT(mMutex.try_lock());
            mMutexLocked.store(true);
        }
        virtual ~Locker()
        {
            if (mMutexLocked.load())
            {
                mMutex.unlock();
            }
        }
        virtual void relock()
        {
            mMutex.lock();
            mMutexLocked.store(true);
        }
        virtual void unlock()
        {
            mMutex.unlock();
            mMutexLocked.store(false);
        }

    private:
        Base &mMutex;
        std::atomic_bool mMutexLocked{false};
        OCTK_DISABLE_COPY_MOVE(Locker)
    };

    using Base::Base;
    Mutex() = default;
};

OCTK_END_NAMESPACE

#endif // _OCTK_MUTEX_HPP
