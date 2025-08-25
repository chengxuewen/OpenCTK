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

#ifndef _OCTK_SSL_CERTIFICATE_HPP
#define _OCTK_SSL_CERTIFICATE_HPP

#include <octk_network_global.hpp>
#include <octk_string_view.hpp>
#include <octk_buffer.hpp>

OCTK_BEGIN_NAMESPACE

struct OCTK_NETWORK_API SSLCertificateStats
{
    SSLCertificateStats(std::string &&fingerprint,
                        std::string &&fingerprint_algorithm,
                        std::string &&base64_certificate,
                        std::unique_ptr<SSLCertificateStats> issuer);
    ~SSLCertificateStats();
    std::string fingerprint;
    std::string fingerprint_algorithm;
    std::string base64_certificate;
    std::unique_ptr<SSLCertificateStats> issuer;

    std::unique_ptr<SSLCertificateStats> Copy() const;
};

// Abstract interface overridden by SSL library specific
// implementations.

// A somewhat opaque type used to encapsulate a certificate.
// Wraps the SSL library's notion of a certificate, with reference counting.
// The SSLCertificate object is pretty much immutable once created.
// (The OpenSSL implementation only does reference counting and
// possibly caching of intermediate results.)
class OCTK_NETWORK_API SSLCertificate
{
public:
    // Parses and builds a certificate from a PEM encoded string.
    // Returns null on failure.
    // The length of the string representation of the certificate is
    // stored in *pem_length if it is non-null, and only if
    // parsing was successful.
    static std::unique_ptr<SSLCertificate> FromPEMString(StringView pem_string);
    virtual ~SSLCertificate() = default;

    // Returns a new SSLCertificate object instance wrapping the same
    // underlying certificate, including its chain if present.
    virtual std::unique_ptr<SSLCertificate> Clone() const = 0;

    // Returns a PEM encoded string representation of the certificate.
    virtual std::string ToPEMString() const = 0;

    // Provides a DER encoded binary representation of the certificate.
    virtual void ToDER(Buffer *der_buffer) const = 0;

    // Gets the name of the digest algorithm that was used to compute this
    // certificate's signature.
    virtual bool GetSignatureDigestAlgorithm(std::string *algorithm) const = 0;

    // Compute the digest of the certificate given algorithm
    virtual bool ComputeDigest(StringView algorithm,
                               unsigned char *digest,
                               size_t size,
                               size_t *length) const = 0;

    // Returns the time in seconds relative to epoch, 1970-01-01T00:00:00Z (UTC),
    // or -1 if an expiration time could not be retrieved.
    virtual int64_t CertificateExpirationTime() const = 0;

    // Gets information (fingerprint, etc.) about this certificate. This is used
    // for certificate stats, see
    // https://w3c.github.io/webrtc-stats/#certificatestats-dict*.
    std::unique_ptr<SSLCertificateStats> GetStats() const;
};

// SSLCertChain is a simple wrapper for a vector of SSLCertificates. It serves
// primarily to ensure proper memory management (especially deletion) of the
// SSLCertificate pointers.
class OCTK_NETWORK_API SSLCertChain final
{
public:
    explicit SSLCertChain(std::unique_ptr<SSLCertificate> single_cert);
    explicit SSLCertChain(std::vector<std::unique_ptr<SSLCertificate>> certs);
// Allow move semantics for the object.
    SSLCertChain(SSLCertChain &&);
    SSLCertChain &operator=(SSLCertChain &&);
    ~SSLCertChain();

    SSLCertChain(const SSLCertChain &) = delete;
    SSLCertChain &operator=(const SSLCertChain &) = delete;

// Vector access methods.
    size_t GetSize() const { return certs_.size(); }

// Returns a temporary reference, only valid until the chain is destroyed.
    const SSLCertificate &Get(size_t pos) const { return *(certs_[pos]); }

// Returns a new SSLCertChain object instance wrapping the same underlying
// certificate chain.
    std::unique_ptr<SSLCertChain> Clone() const;

// Gets information (fingerprint, etc.) about this certificate chain. This is
// used for certificate stats, see
// https://w3c.github.io/webrtc-stats/#certificatestats-dict*.
    std::unique_ptr<SSLCertificateStats> GetStats() const;

private:
    std::vector<std::unique_ptr<SSLCertificate>> certs_;
};

// SSLCertificateVerifier provides a simple interface to allow third parties to
// define their own certificate verification code. It is completely independent
// from the underlying SSL implementation.
class SSLCertificateVerifier
{
public:
    virtual ~SSLCertificateVerifier() = default;
    // Returns true if the certificate is valid, else false. It is up to the
    // implementer to define what a valid certificate looks like.
    virtual bool Verify(const SSLCertificate &certificate) = 0;
};
OCTK_END_NAMESPACE

#endif // _OCTK_SSL_CERTIFICATE_HPP
