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

#ifndef _OCTK_DTLS_TRANSPORT_INTERFACE_HPP
#define _OCTK_DTLS_TRANSPORT_INTERFACE_HPP

#include <octk_ice_transport_interface.hpp>
#include <octk_ssl_certificate.hpp>
#include <octk_shared_ref_ptr.hpp>
#include <octk_media_global.hpp>
#include <octk_ref_count.hpp>
#include <octk_rtc_error.hpp>
#include <octk_optional.hpp>

OCTK_BEGIN_NAMESPACE

// States of a DTLS transport, corresponding to the JS API specification.
// http://w3c.github.io/webrtc-pc/#dom-rtcdtlstransportstate
enum class DtlsTransportState
{
    kNew,        // Has not started negotiating yet.
    kConnecting, // In the process of negotiating a secure connection.
    kConnected,  // Completed negotiation and verified fingerprints.
    kClosed,     // Intentionally closed.
    kFailed,     // Failure due to an error or failing to verify a remote
    // fingerprint.
    kNumValues
};

enum class DtlsTransportTlsRole
{
    kServer, // Other end sends CLIENT_HELLO
    kClient  // This end sends CLIENT_HELLO
};

// This object gives snapshot information about the changeable state of a DTLSTransport.
class OCTK_MEDIA_API DtlsTransportInformation
{
public:
    DtlsTransportInformation();
    explicit DtlsTransportInformation(DtlsTransportState state);
    DtlsTransportInformation(DtlsTransportState state,
                             Optional<DtlsTransportTlsRole> role,
                             Optional<int> tls_version,
                             Optional<int> ssl_cipher_suite,
                             Optional<int> srtp_cipher_suite,
                             std::unique_ptr<SSLCertChain> remote_ssl_certificates);
    // ABSL_DEPRECATED("Use version with role parameter")

    DtlsTransportInformation(DtlsTransportState state,
                             Optional<int> tls_version,
                             Optional<int> ssl_cipher_suite,
                             Optional<int> srtp_cipher_suite,
                             std::unique_ptr<SSLCertChain> remote_ssl_certificates);

    // Copy and assign
    DtlsTransportInformation(const DtlsTransportInformation &c);
    DtlsTransportInformation &operator=(const DtlsTransportInformation &c);
    // Move
    DtlsTransportInformation(DtlsTransportInformation &&other) = default;
    DtlsTransportInformation &operator=(DtlsTransportInformation &&other) = default;

    DtlsTransportState state() const { return state_; }
    Optional<DtlsTransportTlsRole> role() const { return role_; }
    Optional<int> tls_version() const { return tls_version_; }
    Optional<int> ssl_cipher_suite() const { return ssl_cipher_suite_; }
    Optional<int> srtp_cipher_suite() const { return srtp_cipher_suite_; }
    // The accessor returns a temporary pointer, it does not release ownership.
    const SSLCertChain *remote_ssl_certificates() const { return remote_ssl_certificates_.get(); }

private:
    DtlsTransportState state_;
    Optional<DtlsTransportTlsRole> role_;
    Optional<int> tls_version_;
    Optional<int> ssl_cipher_suite_;
    Optional<int> srtp_cipher_suite_;
    std::unique_ptr<SSLCertChain> remote_ssl_certificates_;
};

class DtlsTransportObserverInterface
{
public:
    // This callback carries information about the state of the transport.
    // The argument is a pass-by-value snapshot of the state.
    virtual void OnStateChange(DtlsTransportInformation info) = 0;
    // This callback is called when an error occurs, causing the transport
    // to go to the kFailed state.
    virtual void OnError(RTCError error) = 0;

protected:
    virtual ~DtlsTransportObserverInterface() = default;
};

// A DTLS transport, as represented to the outside world.
// This object is created on the network thread, and can only be
// accessed on that thread, except for functions explicitly marked otherwise.
// References can be held by other threads, and destruction can therefore
// be initiated by other threads.
class DtlsTransportInterface : public RefCountInterface
{
public:
    // Returns a pointer to the ICE transport that is owned by the DTLS transport.
    virtual SharedRefPtr<IceTransportInterface> ice_transport() = 0;
    // Returns information on the state of the DtlsTransport.
    // This function can be called from other threads.
    virtual DtlsTransportInformation Information() = 0;
    // Observer management.
    virtual void RegisterObserver(DtlsTransportObserverInterface *observer) = 0;
    virtual void UnregisterObserver() = 0;
};

OCTK_END_NAMESPACE

#endif // _OCTK_DTLS_TRANSPORT_INTERFACE_HPP
