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

#include <octk_rtc_error.hpp>
#include <octk_iterator.hpp>

OCTK_BEGIN_NAMESPACE

namespace
{
StringView kRTCErrorTypeNames[] = {
    "NONE",
    "UNSUPPORTED_OPERATION",
    "UNSUPPORTED_PARAMETER",
    "INVALID_PARAMETER",
    "INVALID_RANGE",
    "SYNTAX_ERROR",
    "INVALID_STATE",
    "INVALID_MODIFICATION",
    "NETWORK_ERROR",
    "RESOURCE_EXHAUSTED",
    "INTERNAL_ERROR",
    "OPERATION_ERROR_WITH_DATA",
};
static_assert(static_cast<int>(RTCErrorType::OPERATION_ERROR_WITH_DATA) ==
              (utils::size(kRTCErrorTypeNames) - 1),
              "kRTCErrorTypeNames must have as many strings as RTCErrorType "
              "has values.");

StringView kRTCErrorDetailTypeNames[] = {
    "NONE",
    "DATA_CHANNEL_FAILURE",
    "DTLS_FAILURE",
    "FINGERPRINT_FAILURE",
    "SCTP_FAILURE",
    "SDP_SYNTAX_ERROR",
    "HARDWARE_ENCODER_NOT_AVAILABLE",
    "HARDWARE_ENCODER_ERROR",
};
static_assert(static_cast<int>(RTCErrorDetailType::HARDWARE_ENCODER_ERROR) ==
              (utils::size(kRTCErrorDetailTypeNames) - 1),
              "kRTCErrorDetailTypeNames must have as many strings as "
              "RTCErrorDetailType has values.");
}  // namespace


// static
RTCError RTCError::OK()
{
    return RTCError();
}

const char *RTCError::message() const
{
    return message_.c_str();
}

void RTCError::set_message(StringView message)
{
    message_ = std::string(message);
}
namespace utils
{
StringView toString(RTCErrorType error)
{
    const int index = static_cast<int>(error);
    return kRTCErrorTypeNames[index];
}

StringView toString(RTCErrorDetailType error)
{
    const int index = static_cast<int>(error);
    return kRTCErrorDetailTypeNames[index];
}
} // namespace utils

OCTK_END_NAMESPACE
