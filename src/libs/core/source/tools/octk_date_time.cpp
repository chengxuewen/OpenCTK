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

#include <octk_date_time.hpp>

#include <ctime>
#include <chrono>
#include <sstream>
#include <iomanip>

OCTK_BEGIN_NAMESPACE

int64_t DateTime::secsTimeSinceEpoch()
{
    const auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
}

int64_t DateTime::msecsTimeSinceEpoch()
{
    const auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
}

int64_t DateTime::usecsTimeSinceEpoch()
{
    const auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
}

int64_t DateTime::nsecsTimeSinceEpoch()
{
    const auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
}

std::string DateTime::localTimeStringFromSecsSinceEpoch(int64_t secs)
{
    secs = secs > 0 ? secs : DateTime::secsTimeSinceEpoch();
    std::chrono::seconds seconds(secs);
    std::chrono::system_clock::time_point timePoint(seconds);
    std::time_t time = std::chrono::system_clock::to_time_t(timePoint);
    std::tm *localTime = std::localtime(&time);
    std::stringstream ss;
    ss << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string DateTime::localTimeStringFromMSecsSinceEpoch(int64_t msecs)
{
    msecs = msecs > 0 ? msecs : msecsTimeSinceEpoch();
    std::chrono::milliseconds milliseconds(msecs);
    std::chrono::system_clock::time_point timePoint(milliseconds);
    std::time_t time = std::chrono::system_clock::to_time_t(timePoint);
    std::tm *localTime = std::localtime(&time);
    std::stringstream ss;
    ss << std::put_time(localTime, "%Y-%m-%d %H:%M:%S") << '.' << std::setfill('0') << std::setw(3)
       << (milliseconds.count() % 1000);
    return ss.str();
}

OCTK_END_NAMESPACE