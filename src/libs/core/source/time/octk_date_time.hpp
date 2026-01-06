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

#ifndef _OCTK_DATE_TIME_HPP
#define _OCTK_DATE_TIME_HPP

#include <octk_global.hpp>
#include <octk_checks.hpp>
#include <octk_string_view.hpp>

OCTK_BEGIN_NAMESPACE

struct OCTK_CORE_API ClockInterface
{
    virtual ~ClockInterface() { }
    virtual int64_t TimeNanos() const = 0;

    static ClockInterface *SetClockForTesting(ClockInterface *clock);
    // Returns previously set clock, or nullptr if no custom clock is being used.
    static ClockInterface *GetClockForTesting();
};

class OCTK_CORE_API DateTime
{
public:
    struct LocalTime
    {
        int mil;  /* milliseconds after the minute [0-1000] */
        int sec;  /* seconds after the minute [0-60] */
        int min;  /* minutes after the hour [0-59] */
        int hour; /* hours since midnight [0-23] */
        int day;  /* day of the month [1-31] */
        int mon;  /* months since January [0-11] */
        int year; /* years since 1900 */

        int days_since_sunday;  /* days since Sunday [0-6] */
        int days_since_january; /* days since January 1 [0-365] */
        int isdst;              /* Daylight Savings Time flag */
    //TODO:del
//        long gmtoff;            /* offset from UTC in seconds */
//        char *zone;             /* timezone abbreviation */
    };

    OCTK_STATIC_CONSTANT_NUMBER(kNSecsPerUSec, int64_t(1000))

    OCTK_STATIC_CONSTANT_NUMBER(kUSecsPerMSec, int64_t(1000))
    OCTK_STATIC_CONSTANT_NUMBER(kNSecsPerMSec, int64_t(kNSecsPerUSec *kUSecsPerMSec))

    OCTK_STATIC_CONSTANT_NUMBER(kMSecsPerSec, int64_t(1000))
    OCTK_STATIC_CONSTANT_NUMBER(kUSecsPerSec, int64_t(kUSecsPerMSec *kMSecsPerSec))
    OCTK_STATIC_CONSTANT_NUMBER(kNSecsPerSec, int64_t(kNSecsPerMSec *kMSecsPerSec))

    OCTK_STATIC_CONSTANT_NUMBER(kSecsPerMin, int64_t(60))
    OCTK_STATIC_CONSTANT_NUMBER(kMSecsPerMin, int64_t(kMSecsPerSec *kSecsPerMin))
    OCTK_STATIC_CONSTANT_NUMBER(kUSecsPerMin, int64_t(kUSecsPerSec *kSecsPerMin))
    OCTK_STATIC_CONSTANT_NUMBER(kNSecsPerMin, int64_t(kNSecsPerSec *kSecsPerMin))

    OCTK_STATIC_CONSTANT_NUMBER(kMinsPerHour, int64_t(60))
    OCTK_STATIC_CONSTANT_NUMBER(kSecsPerHour, int64_t(kSecsPerMin *kMinsPerHour))
    OCTK_STATIC_CONSTANT_NUMBER(kMSecsPerHour, int64_t(kMSecsPerMin *kMinsPerHour))
    OCTK_STATIC_CONSTANT_NUMBER(kUSecsPerHour, int64_t(kUSecsPerMin *kMinsPerHour))
    OCTK_STATIC_CONSTANT_NUMBER(kNSecsPerHour, int64_t(kNSecsPerMin *kMinsPerHour))

    OCTK_STATIC_CONSTANT_NUMBER(kHoursPerDay, int64_t(24))
    OCTK_STATIC_CONSTANT_NUMBER(kMinsPerDay, int64_t(kMinsPerHour *kHoursPerDay))
    OCTK_STATIC_CONSTANT_NUMBER(kSecsPerDay, int64_t(kSecsPerHour *kHoursPerDay))
    OCTK_STATIC_CONSTANT_NUMBER(kMSecsPerDay, int64_t(kMSecsPerHour *kHoursPerDay))
    OCTK_STATIC_CONSTANT_NUMBER(kUSecsPerDay, int64_t(kUSecsPerHour *kHoursPerDay))
    OCTK_STATIC_CONSTANT_NUMBER(kNSecsPerDay, int64_t(kNSecsPerHour *kHoursPerDay))

    /* system_clock CLOCK_REALTIME for log/datetime */
    static int64_t systemTimeSecs();
    static int64_t systemTimeMSecs();
    static int64_t systemTimeUSecs();
    static int64_t systemTimeNSecs();
    static int64_t systemTimeFromSteadyNSecs(int64_t nsecs);

    /* steady_clock CLOCK_MONOTONIC for wait/hrtime */
    static int64_t steadyTimeSecs();
    static int64_t steadyTimeMSecs();
    static int64_t steadyTimeUSecs();
    static int64_t steadyTimeNSecs();
    static int64_t steadyTimeFromSystemNSecs(int64_t nsecs);

    static LocalTime localTimeFromSystemTimeSecs(int64_t secs = -1);
    static LocalTime localTimeFromSystemTimeMSecs(int64_t msecs = -1);
    static std::string localTimeStringFromSystemTimeSecs(int64_t secs = -1);
    static std::string localTimeStringFromSystemTimeMSecs(int64_t msecs = -1);

    static LocalTime localTimeFromSteadyTimeSecs(int64_t secs = -1)
    {
        secs = secs > 0 ? secs : steadyTimeSecs();
        return localTimeFromSystemTimeSecs(systemTimeFromSteadyNSecs(secs * kNSecsPerSec) / kNSecsPerSec);
    }
    static LocalTime localTimeFromSteadyTimeMSecs(int64_t msecs = -1)
    {
        msecs = msecs > 0 ? msecs : steadyTimeMSecs();
        return localTimeFromSystemTimeMSecs(systemTimeFromSteadyNSecs(msecs * kNSecsPerMSec) / kNSecsPerMSec);
    }
    static OCTK_FORCE_INLINE std::string localTimeStringFromSteadyTimeSecs(int64_t secs = -1)
    {
        secs = secs > 0 ? secs : steadyTimeSecs();
        return localTimeStringFromSystemTimeSecs(systemTimeFromSteadyNSecs(secs * kNSecsPerSec) / kNSecsPerSec);
    }
    static OCTK_FORCE_INLINE std::string localTimeStringFromSteadyTimeMSecs(int64_t msecs = -1)
    {
        msecs = msecs > 0 ? msecs : steadyTimeMSecs();
        return localTimeStringFromSystemTimeMSecs(systemTimeFromSteadyNSecs(msecs * kNSecsPerMSec) / kNSecsPerMSec);
    }
    static OCTK_FORCE_INLINE std::string localTimeString() { return localTimeStringFromSteadyTimeMSecs(); }

    static OCTK_FORCE_INLINE int64_t TimeUTCNanos()
    {
        auto clock = ClockInterface::GetClockForTesting();
        return clock ? clock->TimeNanos() : systemTimeNSecs();
    }
    static OCTK_FORCE_INLINE int64_t TimeUTCMicros() { return TimeUTCNanos() / kNSecsPerUSec; }
    static OCTK_FORCE_INLINE int64_t TimeUTCMillis() { return TimeUTCNanos() / kNSecsPerMSec; }

    static OCTK_FORCE_INLINE int64_t TimeNanos()
    {
        auto clock = ClockInterface::GetClockForTesting();
        return clock ? clock->TimeNanos() : steadyTimeNSecs();
    }
    static OCTK_FORCE_INLINE int64_t TimeMicros() { return TimeNanos() / kNSecsPerUSec; }
    static OCTK_FORCE_INLINE int64_t TimeMillis() { return TimeNanos() / kNSecsPerMSec; }
    static OCTK_FORCE_INLINE int64_t TimeAfter(int64_t elapsed)
    {
        OCTK_DCHECK_GE(elapsed, 0);
        return TimeMillis() + elapsed;
    }
    static OCTK_FORCE_INLINE int64_t TimeSince(int64_t earlier) { return DateTime::TimeMillis() - earlier; }
    static OCTK_FORCE_INLINE int32_t TimeDiff32(uint32_t later, uint32_t earlier) { return later - earlier; }
    static OCTK_FORCE_INLINE int64_t TimeDiff(int64_t later, int64_t earlier) { return later - earlier; }
    static OCTK_FORCE_INLINE int64_t TimeUntil(int64_t later) { return later - TimeMillis(); }
    static OCTK_FORCE_INLINE uint32_t Time32() { return static_cast<uint32_t>(TimeNanos() / kNSecsPerMSec); }

    static OCTK_FORCE_INLINE int64_t timeAfterMSecs(int64_t elapsed)
    {
        OCTK_DCHECK_GE(elapsed, 0);
        return DateTime::steadyTimeMSecs() + elapsed;
    }
    static OCTK_FORCE_INLINE int64_t timeSinceMSecs(int64_t earlier)
    {
        OCTK_DCHECK_GE(earlier, 0);
        return DateTime::steadyTimeMSecs() - earlier;
    }
    static OCTK_FORCE_INLINE int64_t timeUntilMSecs(int64_t later)
    {
        OCTK_DCHECK_GE(later, 0);
        return later - DateTime::steadyTimeMSecs();
    }

    static int64_t TmToSeconds(const tm &tm);
};
OCTK_END_NAMESPACE

#endif // _OCTK_DATE_TIME_HPP
