//
// Created by cxw on 25-9-5.
//

#include <octk_rtp_receiver_interface.hpp>

OCTK_BEGIN_NAMESPACE

std::vector<std::string> RtpReceiverInterface::stream_ids() const { return {}; }

std::vector<ScopedRefPtr<MediaStreamInterface>> RtpReceiverInterface::streams() const { return {}; }

std::vector<RtpSource> RtpReceiverInterface::GetSources() const { return {}; }

void RtpReceiverInterface::SetFrameDecryptor(ScopedRefPtr<FrameDecryptorInterface> /* frame_decryptor */) { }

ScopedRefPtr<FrameDecryptorInterface> RtpReceiverInterface::GetFrameDecryptor() const { return nullptr; }

// ScopedRefPtr<DtlsTransportInterface> RtpReceiverInterface::dtls_transport() const { return nullptr; }

void RtpReceiverInterface::SetFrameTransformer(ScopedRefPtr<FrameTransformerInterface> /* frame_transformer */) { }

OCTK_END_NAMESPACE
