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

#include "rtc_session_description.hpp"
#include <openctk/media/rtc_constants.hpp>
#include <openctk/core/checks.hpp>

OCTK_BEGIN_NAMESPACE

StringView RtcSessionDescription::sdpTypeToString(SdpType type)
{
    switch (type)
    {
        case SdpType::kOffer: return constants::rtc::sdp::kOffer;
        case SdpType::kPrAnswer: return constants::rtc::sdp::kPrAnswer;
        case SdpType::kAnswer: return constants::rtc::sdp::kAnswer;
        case SdpType::kRollback: return constants::rtc::sdp::kRollback;
        default: break;
    }
    OCTK_CHECK_NOTREACHED();
    return "";
}

Optional<RtcSessionDescription::SdpType> RtcSessionDescription::sdpTypeFromString(StringView string)
{
    if (string == constants::rtc::sdp::kOffer)
    {
        return SdpType::kOffer;
    }
    else if (string == constants::rtc::sdp::kPrAnswer)
    {
        return SdpType::kPrAnswer;
    }
    else if (string == constants::rtc::sdp::kAnswer)
    {
        return SdpType::kAnswer;
    }
    else if (string == constants::rtc::sdp::kRollback)
    {
        return SdpType::kRollback;
    }
    else
    {
        return utils::nullopt;
    }
}
std::string RtcSessionDescription::sdpTypeVariantToString(const SdpTypeVariant &variant)
{
    if (auto *value = utils::get_if<std::string>(&variant))
    {
        return *value;
    }
    else if (auto *value = utils::get_if<SdpType>(&variant))
    {
        return RtcSessionDescription::sdpTypeToString(*value).data();
    }
    OCTK_CHECK_NOTREACHED();
    return "";
}

Optional<RtcSessionDescription::SdpType> RtcSessionDescription::sdpTypeFromVariant(const SdpTypeVariant &variant)
{
    if (auto *value = utils::get_if<std::string>(&variant))
    {
        return RtcSessionDescription::sdpTypeFromString(*value);
    }
    else if (auto *value = utils::get_if<SdpType>(&variant))
    {
        return *value;
    }
    OCTK_CHECK_NOTREACHED();
    return utils::nullopt;
}

OCTK_END_NAMESPACE
