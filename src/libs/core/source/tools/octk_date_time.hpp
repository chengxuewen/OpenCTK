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

#ifndef _OCTK_DATE_TIME_HPP
#define _OCTK_DATE_TIME_HPP

#include <octk_global.hpp>
#include <octk_string_view.hpp>

OCTK_BEGIN_NAMESPACE

class OCTK_CORE_API DateTime
{
public:
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

    static int64_t secsTimeSinceEpoch();
    static int64_t msecsTimeSinceEpoch();
    static int64_t usecsTimeSinceEpoch();
    static int64_t nsecsTimeSinceEpoch();
    static std::string localTimeStringFromSecsSinceEpoch(int64_t secs = -1);
    static std::string localTimeStringFromMSecsSinceEpoch(int64_t msecs = -1);
};

OCTK_END_NAMESPACE

#endif // _OCTK_DATE_TIME_HPP
