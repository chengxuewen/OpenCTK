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

#include <octk_date_time.hpp>

#include <ctime>
#include <chrono>
#include <sstream>
#include <iomanip>

OCTK_BEGIN_NAMESPACE

ClockInterface *g_clock = nullptr;
ClockInterface *ClockInterface::SetClockForTesting(ClockInterface *clock)
{
    ClockInterface *prev = g_clock;
    g_clock = clock;
    return prev;
}

ClockInterface *ClockInterface::GetClockForTesting()
{
    return g_clock;
}

int64_t DateTime::systemTimeSecs()
{
    const auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
}

int64_t DateTime::systemTimeMSecs()
{
    const auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
}

int64_t DateTime::systemTimeUSecs()
{
    const auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
}

int64_t DateTime::systemTimeNSecs()
{
    const auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
}

int64_t DateTime::systemTimeFromSteadyNSecs(int64_t nsecs)
{
    std::chrono::nanoseconds nanoseconds(nsecs);
    std::chrono::steady_clock::time_point timePoint(nanoseconds);

    auto steadyNow = std::chrono::steady_clock::now();
    auto offset = std::chrono::duration_cast<std::chrono::system_clock::duration>(timePoint - steadyNow);
    auto systemTimePoint = std::chrono::system_clock::now() + offset;
    return std::chrono::duration_cast<std::chrono::nanoseconds>(systemTimePoint.time_since_epoch()).count();
}

int64_t DateTime::steadyTimeSecs()
{
    const auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();;
}

int64_t DateTime::steadyTimeMSecs()
{
    const auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
}

int64_t DateTime::steadyTimeUSecs()
{
    const auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
}

int64_t DateTime::steadyTimeNSecs()
{
    const auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
}

int64_t DateTime::steadyTimeFromSystemNSecs(int64_t nsecs)
{
    auto systemNow = std::chrono::system_clock::now();
    auto steadyNow = std::chrono::steady_clock::now();
    auto steadyTimePoint = std::chrono::steady_clock::time_point(std::chrono::nanoseconds(nsecs));

    auto offset = steadyTimePoint - steadyNow;
    auto systemTimePoint = systemNow + std::chrono::duration_cast<std::chrono::system_clock::duration>(offset);
    return systemTimePoint.time_since_epoch().count();
}

DateTime::LocalTime DateTime::localTimeFromSystemTimeSecs(int64_t secs)
{
    secs = secs > 0 ? secs : systemTimeSecs();
    std::chrono::milliseconds milliseconds(secs);
    std::chrono::system_clock::time_point timePoint(milliseconds);
    std::time_t time = std::chrono::system_clock::to_time_t(timePoint);
    std::tm *localTime = std::localtime(&time);
    const int mil = int(milliseconds.count() % 1000);
    return {mil, localTime->tm_sec, localTime->tm_min, localTime->tm_hour, localTime->tm_mday, localTime->tm_mon,
            localTime->tm_year + 1900,
            localTime->tm_wday, localTime->tm_yday, localTime->tm_isdst, localTime->tm_gmtoff, localTime->tm_zone};
}

DateTime::LocalTime DateTime::localTimeFromSystemTimeMSecs(int64_t msecs)
{
    msecs = msecs > 0 ? msecs : systemTimeMSecs();
    std::chrono::milliseconds milliseconds(msecs);
    std::chrono::system_clock::time_point timePoint(milliseconds);
    std::time_t time = std::chrono::system_clock::to_time_t(timePoint);
    std::tm *localTime = std::localtime(&time);
    const int mil = int(milliseconds.count() % 1000);
    return {mil, localTime->tm_sec, localTime->tm_min, localTime->tm_hour, localTime->tm_mday, localTime->tm_mon,
            localTime->tm_year + 1900,
            localTime->tm_wday, localTime->tm_yday, localTime->tm_isdst, localTime->tm_gmtoff, localTime->tm_zone};
}

std::string DateTime::localTimeStringFromSystemTimeSecs(int64_t secs)
{
    secs = secs > 0 ? secs : DateTime::systemTimeSecs();
    std::chrono::seconds seconds(secs);
    std::chrono::system_clock::time_point timePoint(seconds);
    std::time_t time = std::chrono::system_clock::to_time_t(timePoint);
    std::tm *localTime = std::localtime(&time);
    std::stringstream ss;
    ss << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string DateTime::localTimeStringFromSystemTimeMSecs(int64_t msecs)
{
    msecs = msecs > 0 ? msecs : systemTimeMSecs();
    std::chrono::milliseconds milliseconds(msecs);
    std::chrono::system_clock::time_point timePoint(milliseconds);
    std::time_t time = std::chrono::system_clock::to_time_t(timePoint);
    std::tm *localTime = std::localtime(&time);
    std::stringstream ss;
    ss << std::put_time(localTime, "%Y-%m-%d %H:%M:%S") << '.' << std::setfill('0') << std::setw(3)
       << (milliseconds.count() % 1000);
    return ss.str();
}

int64_t DateTime::TmToSeconds(const tm &tm)
{
    static short int mdays[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    static short int cumul_mdays[12] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
    int year = tm.tm_year + 1900;
    int month = tm.tm_mon;
    int day = tm.tm_mday - 1;  // Make 0-based like the rest.
    int hour = tm.tm_hour;
    int min = tm.tm_min;
    int sec = tm.tm_sec;

    bool expiry_in_leap_year = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));

    if (year < 1970)
    {
        return -1;
    }
    if (month < 0 || month > 11)
    {
        return -1;
    }
    if (day < 0 || day >= mdays[month] + (expiry_in_leap_year && month == 2 - 1))
    {
        return -1;
    }
    if (hour < 0 || hour > 23)
    {
        return -1;
    }
    if (min < 0 || min > 59)
    {
        return -1;
    }
    if (sec < 0 || sec > 59)
    {
        return -1;
    }

    day += cumul_mdays[month];

    // Add number of leap days between 1970 and the expiration year, inclusive.
    day += ((year / 4 - 1970 / 4) - (year / 100 - 1970 / 100) + (year / 400 - 1970 / 400));

    // We will have added one day too much above if expiration is during a leap
    // year, and expiration is in January or February.
    if (expiry_in_leap_year && month <= 2 - 1)
    {  // `month` is zero based.
        day -= 1;
    }

    // Combine all variables into seconds from 1970-01-01 00:00 (except `month`
    // which was accumulated into `day` above).
    return (((static_cast<int64_t>(year - 1970) * 365 + day) * 24 + hour) * 60 + min) * 60 + sec;
}
OCTK_END_NAMESPACE