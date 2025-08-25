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

#include <octk_ssl_certificate.hpp>
#include <octk_ssl_fingerprint.hpp>
#include <octk_memory.hpp>
#include <octk_base64.hpp>

OCTK_BEGIN_NAMESPACE

//////////////////////////////////////////////////////////////////////
// SSLCertificateStats
//////////////////////////////////////////////////////////////////////

SSLCertificateStats::SSLCertificateStats(std::string &&fingerprint,
                                         std::string &&fingerprint_algorithm,
                                         std::string &&base64_certificate,
                                         std::unique_ptr<SSLCertificateStats> issuer)
    : fingerprint(std::move(fingerprint)), fingerprint_algorithm(std::move(fingerprint_algorithm)), base64_certificate(
    std::move(base64_certificate)), issuer(std::move(issuer)) {}

SSLCertificateStats::~SSLCertificateStats() {}

std::unique_ptr<SSLCertificateStats> SSLCertificateStats::Copy() const
{
    return utils::make_unique<SSLCertificateStats>(std::string(fingerprint), std::string(fingerprint_algorithm),
                                                   std::string(base64_certificate), issuer ? issuer->Copy() : nullptr);
}

//////////////////////////////////////////////////////////////////////
// SSLCertificate
//////////////////////////////////////////////////////////////////////

std::unique_ptr<SSLCertificateStats> SSLCertificate::GetStats() const
{
    // TODO(bemasc): Move this computation to a helper class that caches these
    // values to reduce CPU use in `StatsCollector::GetStats`. This will require
    // adding a fast `SSLCertificate::Equals` to detect certificate changes.
    std::string digest_algorithm;
    if (!GetSignatureDigestAlgorithm(&digest_algorithm))
    {
        return nullptr;
    }

    // `SSLFingerprint::Create` can fail if the algorithm returned by
    // `SSLCertificate::GetSignatureDigestAlgorithm` is not supported by the
    // implementation of `SSLCertificate::ComputeDigest`. This currently happens
    // with MD5- and SHA-224-signed certificates when linked to libNSS.
    std::unique_ptr<SSLFingerprint> ssl_fingerprint = SSLFingerprint::Create(digest_algorithm, *this);
    if (!ssl_fingerprint)
    {
        return nullptr;
    }
    std::string fingerprint = ssl_fingerprint->GetRfc4572Fingerprint();

    Buffer der_buffer;
    ToDER(&der_buffer);
    std::string der_base64;
    Base64::EncodeFromArray(der_buffer.data(), der_buffer.size(), &der_base64);

    return utils::make_unique<SSLCertificateStats>(std::move(fingerprint),
                                                   std::move(digest_algorithm),
                                                   std::move(der_base64), nullptr);
}

//////////////////////////////////////////////////////////////////////
// SSLCertChain
//////////////////////////////////////////////////////////////////////

SSLCertChain::SSLCertChain(std::unique_ptr<SSLCertificate> single_cert)
{
    certs_.push_back(std::move(single_cert));
}

SSLCertChain::SSLCertChain(std::vector<std::unique_ptr<SSLCertificate>> certs) : certs_(std::move(certs)) {}

SSLCertChain::SSLCertChain(SSLCertChain &&rhs) = default;

SSLCertChain &SSLCertChain::operator=(SSLCertChain &&) = default;

SSLCertChain::~SSLCertChain() = default;

std::unique_ptr<SSLCertChain> SSLCertChain::Clone() const
{
    std::vector<std::unique_ptr<SSLCertificate>> new_certs(certs_.size());
    std::transform(certs_.begin(), certs_.end(), new_certs.begin(),
                   [](const std::unique_ptr<SSLCertificate> &cert) -> std::unique_ptr<SSLCertificate> {
                       return cert->Clone();
                   });
    // absl::c_transform(
    //     certs_, new_certs.begin(),
    //     [](const std::unique_ptr<SSLCertificate> &cert)
    //         -> std::unique_ptr<SSLCertificate> { return cert->Clone(); });
    return utils::make_unique<SSLCertChain>(std::move(new_certs));
}

std::unique_ptr<SSLCertificateStats> SSLCertChain::GetStats() const
{
    // We have a linked list of certificates, starting with the first element of
    // `certs_` and ending with the last element of `certs_`. The "issuer" of a
    // certificate is the next certificate in the chain. Stats are produced for
    // each certificate in the list. Here, the "issuer" is the issuer's stats.
    std::unique_ptr<SSLCertificateStats> issuer;
    // The loop runs in reverse so that the `issuer` is known before the
    // certificate issued by `issuer`.
    for (ptrdiff_t i = certs_.size() - 1; i >= 0; --i)
    {
        std::unique_ptr<SSLCertificateStats> new_stats = certs_[i]->GetStats();
        if (new_stats)
        {
            new_stats->issuer = std::move(issuer);
        }
        issuer = std::move(new_stats);
    }
    return issuer;
}

// static
std::unique_ptr<SSLCertificate> SSLCertificate::FromPEMString(StringView pem_string)
{
// #ifdef OPENSSL_IS_BORINGSSL
//     return BoringSSLCertificate::FromPEMString(pem_string);
// #else
//     return OpenSSLCertificate::FromPEMString(pem_string);
// #endif
    return nullptr;
}
OCTK_END_NAMESPACE
