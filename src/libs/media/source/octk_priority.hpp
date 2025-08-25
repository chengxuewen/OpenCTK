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

#ifndef _OCTK_PRIORITY_HPP
#define _OCTK_PRIORITY_HPP

#include <octk_strong_alias.hpp>
#include <octk_checks.hpp>

OCTK_BEGIN_NAMESPACE

// GENERATED_JAVA_ENUM_PACKAGE: org.webrtc
enum class Priority
{
    kVeryLow,
    kLow,
    kMedium,
    kHigh,
};

class PriorityValue : public StrongAlias<class PriorityValueTag, uint16_t>
{
public:
    explicit PriorityValue(Priority priority)
    {
        switch (priority)
        {
            case Priority::kVeryLow: value_ = 128; break;
            case Priority::kLow: value_ = 256; break;
            case Priority::kMedium: value_ = 512; break;
            case Priority::kHigh: value_ = 1024; break;
            default: OCTK_CHECK_NOTREACHED();
        }
    }

    explicit PriorityValue(uint16_t priority)
        : StrongAlias(priority)
    {
    }
};

OCTK_END_NAMESPACE

#endif // _OCTK_PRIORITY_HPP
