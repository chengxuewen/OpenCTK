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

#include <octk_ssl_fingerprint.hpp>
#include <octk_ssl_certificate.hpp>
#include <octk_ssl_identity.hpp>
#include <octk_string_encode.hpp>
#include <octk_memory.hpp>

OCTK_BEGIN_NAMESPACE

std::unique_ptr<SSLFingerprint> SSLFingerprint::Create(StringView algorithm, const SSLCertificate &cert)
{
    uint8_t digest_val[64];
    size_t digest_len;
    bool ret = cert.ComputeDigest(algorithm, digest_val, sizeof(digest_val), &digest_len);
    if (!ret)
    {
        return nullptr;
    }
    return utils::make_unique<SSLFingerprint>(algorithm, ArrayView<const uint8_t>(digest_val, digest_len));
}

SSLFingerprint *SSLFingerprint::Create(StringView algorithm, const SSLIdentity *identity)
{
    return CreateUnique(algorithm, *identity).release();
}

std::unique_ptr<SSLFingerprint> SSLFingerprint::CreateUnique(StringView algorithm, const SSLIdentity &identity)
{
    return Create(algorithm, identity.certificate());
}

#if 0

SSLFingerprint *SSLFingerprint::CreateFromRfc4572(StringView algorithm,
                                                  StringView fingerprint)
{
    return CreateUniqueFromRfc4572(algorithm, fingerprint).release();
}

std::unique_ptr<SSLFingerprint> SSLFingerprint::CreateUniqueFromRfc4572(StringView algorithm,
                                                                        StringView fingerprint)
{
    if (algorithm.empty() || !IsFips180DigestAlgorithm(algorithm))
    {
        return nullptr;
    }

    if (fingerprint.empty())
    {
        return nullptr;
    }

    char value[MessageDigest::kMaxSize];
    size_t value_len = utils::hex_decode_with_delimiter(ArrayView<char>(value), fingerprint, ':');
    if (!value_len)
    {
        return nullptr;
    }

    return std::make_unique<SSLFingerprint>(algorithm,
                                            ArrayView<const uint8_t>(reinterpret_cast<uint8_t *>(value), value_len));
}
std::unique_ptr<SSLFingerprint> SSLFingerprint::CreateFromCertificate(const RTCCertificate &cert)
{
    std::string digest_alg;
    if (!cert.GetSSLCertificate().GetSignatureDigestAlgorithm(&digest_alg))
    {
        OCTK_ERROR() << "Failed to retrieve the certificate's digest algorithm";
        return nullptr;
    }

    std::unique_ptr<SSLFingerprint> fingerprint =
        CreateUnique(digest_alg, *cert.identity());
    if (!fingerprint)
    {
        OCTK_ERROR() << "Failed to create identity fingerprint, alg="
                     << digest_alg;
    }
    return fingerprint;
}
#endif

SSLFingerprint::SSLFingerprint(StringView algorithm, ArrayView<const uint8_t> digest_view)
    : algorithm(algorithm)
    , digest(digest_view.data(), digest_view.size())
{
}

SSLFingerprint::SSLFingerprint(StringView algorithm, const uint8_t *digest_in, size_t digest_len)
    : SSLFingerprint(algorithm, utils::makeArrayView(digest_in, digest_len))
{
}

bool SSLFingerprint::operator==(const SSLFingerprint &other) const
{
    return algorithm == other.algorithm && digest == other.digest;
}

std::string SSLFingerprint::GetRfc4572Fingerprint() const
{
    std::string fingerprint = utils::hex_encode_with_delimiter(StringView(digest.data<char>(), digest.size()), ':');
    // absl::c_transform(fingerprint, fingerprint.begin(), ::toupper);
    std::transform(fingerprint.begin(), fingerprint.end(), fingerprint.begin(), ::toupper);
    return fingerprint;
}

std::string SSLFingerprint::toString() const
{
    std::string fp_str = algorithm;
    fp_str.append(" ");
    fp_str.append(GetRfc4572Fingerprint());
    return fp_str;
}

OCTK_END_NAMESPACE
