//
// Created by cxw on 25-9-5.
//

#include <octk_rtp_receiver_interface.hpp>

OCTK_BEGIN_NAMESPACE

std::vector<std::string> RtpReceiverInterface::stream_ids() const { return {}; }

std::vector<SharedRefPtr<MediaStreamInterface>> RtpReceiverInterface::streams() const { return {}; }

std::vector<RtpSource> RtpReceiverInterface::GetSources() const { return {}; }

void RtpReceiverInterface::SetFrameDecryptor(SharedRefPtr<FrameDecryptorInterface> /* frame_decryptor */) { }

SharedRefPtr<FrameDecryptorInterface> RtpReceiverInterface::GetFrameDecryptor() const { return nullptr; }

// SharedRefPtr<DtlsTransportInterface> RtpReceiverInterface::dtls_transport() const { return nullptr; }

void RtpReceiverInterface::SetFrameTransformer(SharedRefPtr<FrameTransformerInterface> /* frame_transformer */) { }

OCTK_END_NAMESPACE
