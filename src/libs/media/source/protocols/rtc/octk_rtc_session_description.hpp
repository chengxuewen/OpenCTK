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

#include <octk_shared_pointer.hpp>
#include <octk_media_global.hpp>
#include <octk_optional.hpp>
#include <octk_variant.hpp>
#include <octk_string.hpp>

OCTK_BEGIN_NAMESPACE

class OCTK_MEDIA_API RtcSessionDescription
{
public:
    OCTK_STATIC_CONSTANT_STRING(kOffer, "offer")
    OCTK_STATIC_CONSTANT_STRING(kAnswer, "answer")
    OCTK_STATIC_CONSTANT_STRING(kPrAnswer, "pranswer")
    OCTK_STATIC_CONSTANT_STRING(kRollback, "rollback")

    enum class SdpType
    {
        kOffer = 0, // Description must be treated as an SDP offer.
        kPrAnswer,  // Description must be treated as an SDP answer, but not a final answer.
        kAnswer,    // Description must be treated as an SDP final answer, and the offer-answer exchange must be
                    // considered complete after receiving this.
        kRollback   // Resets any pending offers and sets signaling state back to stable.
    };
    static StringView sdpTypeToString(SdpType type);
    static Optional<SdpType> sdpTypeFromString(StringView string);

    using SdpTypeVariant = Variant<std::string, SdpType>;
    static std::string sdpTypeVariantToString(const SdpTypeVariant &variant);
    static Optional<SdpType> sdpTypeFromVariant(const SdpTypeVariant &variant);

    struct Data
    {
        std::string sdp;
        std::string type;
    };

    virtual bool toString(String &out) = 0;
    virtual String type() const = 0;
    virtual String sdp() const = 0;
    virtual SdpType sdpType() = 0;

protected:
    virtual ~RtcSessionDescription() = default;
};

OCTK_END_NAMESPACE
