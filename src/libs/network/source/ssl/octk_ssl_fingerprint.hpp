/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
** Copyright 2018 The WebRTC Project Authors. All rights reserved.
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

#ifndef _OCTK_SSL_FINGERPRINT_HPP
#define _OCTK_SSL_FINGERPRINT_HPP

#include <octk_network_global.hpp>
#include <octk_shared_buffer.hpp>
#include <octk_string_view.hpp>
#include <octk_array_view.hpp>

OCTK_BEGIN_NAMESPACE

class RTCCertificate;

class SSLCertificate;

class SSLIdentity;

struct OCTK_NETWORK_API SSLFingerprint
{

    static std::unique_ptr<SSLFingerprint> Create(StringView algorithm,
                                                  const SSLCertificate &cert);
    // TODO(steveanton): Remove once downstream projects have moved off of this.
    static SSLFingerprint *Create(StringView algorithm,
                                  const SSLIdentity *identity);
    // TODO(steveanton): Rename to Create once projects have migrated.
    static std::unique_ptr<SSLFingerprint> CreateUnique(StringView algorithm,
                                                        const SSLIdentity &identity);
#if 0
    // TODO(steveanton): Remove once downstream projects have moved off of this.
    static SSLFingerprint *CreateFromRfc4572(StringView algorithm,
                                             StringView fingerprint);
    // TODO(steveanton): Rename to CreateFromRfc4572 once projects have migrated.
    static std::unique_ptr<SSLFingerprint> CreateUniqueFromRfc4572(StringView algorithm,
                                                                   StringView fingerprint);

    // Creates a fingerprint from a certificate, using the same digest algorithm
    // as the certificate's signature.
    static std::unique_ptr<SSLFingerprint> CreateFromCertificate(const RTCCertificate &cert);
#endif

    SSLFingerprint(StringView algorithm,
                   ArrayView<const uint8_t> digest_view);
    // TODO(steveanton): Remove once downstream projects have moved off of this.
    SSLFingerprint(StringView algorithm,
                   const uint8_t *digest_in,
                   size_t digest_len);

    SSLFingerprint(const SSLFingerprint &from) = default;
    SSLFingerprint &operator=(const SSLFingerprint &from) = default;

    bool operator==(const SSLFingerprint &other) const;

    std::string GetRfc4572Fingerprint() const;

    std::string toString() const;

    std::string algorithm;
    SharedBuffer digest;
};
OCTK_END_NAMESPACE

#endif // _OCTK_SSL_FINGERPRINT_HPP
