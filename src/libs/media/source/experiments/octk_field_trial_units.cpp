/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
** Copyright 2018 The WebRTC project authors. All Rights Reserved.
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

#include <private/octk_field_trial_units_p.hpp>

#include <stdio.h>
#include <limits>
#include <string>

// Large enough to fit "seconds", the longest supported unit name.
#define RTC_TRIAL_UNIT_LENGTH_STR "7"
#define RTC_TRIAL_UNIT_SIZE       8

OCTK_BEGIN_NAMESPACE

namespace
{

struct ValueWithUnit
{
    double value;
    std::string unit;
};

Optional<ValueWithUnit> ParseValueWithUnit(StringView str)
{
    if (str == "inf")
    {
        return ValueWithUnit{std::numeric_limits<double>::infinity(), ""};
    }
    else if (str == "-inf")
    {
        return ValueWithUnit{-std::numeric_limits<double>::infinity(), ""};
    }
    else
    {
        double double_val;
        char unit_char[RTC_TRIAL_UNIT_SIZE];
        unit_char[0] = 0;
        if (sscanf(std::string(str).c_str(), "%lf%" RTC_TRIAL_UNIT_LENGTH_STR "s", &double_val, unit_char) >= 1)
        {
            return ValueWithUnit{double_val, unit_char};
        }
    }
    return utils::nullopt;
}
} // namespace

template <>
Optional<DataRate> ParseTypedParameter<DataRate>(StringView str)
{
    Optional<ValueWithUnit> result = ParseValueWithUnit(str);
    if (result)
    {
        if (result->unit.empty() || result->unit == "kbps")
        {
            return DataRate::KilobitsPerSec(result->value);
        }
        else if (result->unit == "bps")
        {
            return DataRate::BitsPerSec(result->value);
        }
    }
    return utils::nullopt;
}

template <>
Optional<DataSize> ParseTypedParameter<DataSize>(StringView str)
{
    Optional<ValueWithUnit> result = ParseValueWithUnit(str);
    if (result)
    {
        if (result->unit.empty() || result->unit == "bytes")
            return DataSize::Bytes(result->value);
    }
    return utils::nullopt;
}

template <>
Optional<TimeDelta> ParseTypedParameter<TimeDelta>(StringView str)
{
    Optional<ValueWithUnit> result = ParseValueWithUnit(str);
    if (result)
    {
        if (result->unit == "s" || result->unit == "seconds")
        {
            return TimeDelta::Seconds(result->value);
        }
        else if (result->unit == "us")
        {
            return TimeDelta::Micros(result->value);
        }
        else if (result->unit.empty() || result->unit == "ms")
        {
            return TimeDelta::Millis(result->value);
        }
    }
    return utils::nullopt;
}

template <>
Optional<Optional<DataRate>> ParseTypedParameter<Optional<DataRate>>(StringView str)
{
    return ParseOptionalParameter<DataRate>(str);
}
template <>
Optional<Optional<DataSize>> ParseTypedParameter<Optional<DataSize>>(StringView str)
{
    return ParseOptionalParameter<DataSize>(str);
}
template <>
Optional<Optional<TimeDelta>> ParseTypedParameter<Optional<TimeDelta>>(StringView str)
{
    return ParseOptionalParameter<TimeDelta>(str);
}

template class FieldTrialParameter<DataRate>;
template class FieldTrialParameter<DataSize>;
template class FieldTrialParameter<TimeDelta>;

template class FieldTrialConstrained<DataRate>;
template class FieldTrialConstrained<DataSize>;
template class FieldTrialConstrained<TimeDelta>;

template class FieldTrialOptional<DataRate>;
template class FieldTrialOptional<DataSize>;
template class FieldTrialOptional<TimeDelta>;

OCTK_END_NAMESPACE