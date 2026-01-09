/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2026~Present ChengXueWen.
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

#include <octk_platform_thread.hpp>
#include <octk_checks.hpp>

#include <mutex>
#include <shared_mutex>
#include <condition_variable>

OCTK_BEGIN_NAMESPACE

class OCTK_CORE_API OCTK_ATTRIBUTE_LOCKABLE RaceChecker final
{
public:
    class OCTK_ATTRIBUTE_SCOPED_LOCKABLE Scope
    {
    public:
        explicit Scope(const RaceChecker* raceChecker) OCTK_ATTRIBUTE_EXCLUSIVE_LOCK_FUNCTION(raceChecker);
        ~Scope() OCTK_ATTRIBUTE_UNLOCK_FUNCTION();

        bool isDetected() const;

#if OCTK_DCHECK_IS_ON
    private:
        const RaceChecker* const mRaceChecker;
        const bool mRacecheckOk;
#endif
    };

    //    friend class internal::RaceCheckerScope;
    RaceChecker() = default;
    ~RaceChecker() = default;

private:
    bool acquire() const OCTK_ATTRIBUTE_EXCLUSIVE_LOCK_FUNCTION();
    void release() const OCTK_ATTRIBUTE_UNLOCK_FUNCTION();

    // Volatile to prevent code being optimized away in acquire()/release().
    mutable volatile int mAccessCount{0};
    mutable volatile PlatformThread::Id mAccessingThreadId{0};
};

OCTK_END_NAMESPACE

#define OCTK_CHECK_RUNS_SERIALIZED_IMPL(x, suffix) \
    octk::RaceChecker::Scope raceChecker##suffix(x); \
    OCTK_CHECK(!raceChecker##suffix.isDetected())

#define OCTK_CHECK_RUNS_SERIALIZED_NEXT(x, suffix) \
    OCTK_CHECK_RUNS_SERIALIZED_IMPL(x, suffix)

#define OCTK_DCHECK_RUNS_SERIALIZED(x) octk::RaceChecker::Scope raceChecker(x)

#define OCTK_CHECK_RUNS_SERIALIZED(x) OCTK_CHECK_RUNS_SERIALIZED_NEXT(x, __LINE__)
