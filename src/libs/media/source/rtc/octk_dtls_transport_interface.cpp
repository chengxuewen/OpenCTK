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

#include <octk_dtls_transport_interface.hpp>
#include <octk_optional.hpp>

OCTK_BEGIN_NAMESPACE

DtlsTransportInformation::DtlsTransportInformation() : state_(DtlsTransportState::kNew) {}

DtlsTransportInformation::DtlsTransportInformation(DtlsTransportState state) : state_(state) {}

DtlsTransportInformation::DtlsTransportInformation(DtlsTransportState state,
                                                   Optional<DtlsTransportTlsRole> role,
                                                   Optional<int> tls_version,
                                                   Optional<int> ssl_cipher_suite,
                                                   Optional<int> srtp_cipher_suite,
                                                   std::unique_ptr<SSLCertChain> remote_ssl_certificates)
    : state_(state), role_(role), tls_version_(tls_version), ssl_cipher_suite_(ssl_cipher_suite)
    , srtp_cipher_suite_(srtp_cipher_suite)
    , remote_ssl_certificates_(std::move(remote_ssl_certificates)) {}

// Deprecated version
DtlsTransportInformation::DtlsTransportInformation(DtlsTransportState state,
                                                   Optional<int> tls_version,
                                                   Optional<int> ssl_cipher_suite,
                                                   Optional<int> srtp_cipher_suite,
                                                   std::unique_ptr<SSLCertChain> remote_ssl_certificates)
    : state_(state), role_(utils::nullopt), tls_version_(tls_version), ssl_cipher_suite_(ssl_cipher_suite)
    , srtp_cipher_suite_(srtp_cipher_suite), remote_ssl_certificates_(std::move(remote_ssl_certificates)) {}

DtlsTransportInformation::DtlsTransportInformation(const DtlsTransportInformation &c)
    : state_(c.state()), role_(c.role_)
    , tls_version_(c.tls_version_)
    , ssl_cipher_suite_(c.ssl_cipher_suite_)
    , srtp_cipher_suite_(c.srtp_cipher_suite_)
    , remote_ssl_certificates_(c.remote_ssl_certificates() ? c.remote_ssl_certificates()->Clone()
                                                           : nullptr) {}

DtlsTransportInformation &DtlsTransportInformation::operator=(const DtlsTransportInformation &c)
{
    state_ = c.state();
    role_ = c.role_;
    tls_version_ = c.tls_version_;
    ssl_cipher_suite_ = c.ssl_cipher_suite_;
    srtp_cipher_suite_ = c.srtp_cipher_suite_;
    remote_ssl_certificates_ = c.remote_ssl_certificates() ? c.remote_ssl_certificates()->Clone()
                                                           : nullptr;
    return *this;
}
OCTK_END_NAMESPACE
