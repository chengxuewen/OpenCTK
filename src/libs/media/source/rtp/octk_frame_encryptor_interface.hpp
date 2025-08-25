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

#ifndef _OCTK_FRAME_ENCRYPTOR_INTERFACE_HPP
#define _OCTK_FRAME_ENCRYPTOR_INTERFACE_HPP

#include <octk_media_types.hpp>
#include <octk_array_view.hpp>
#include <octk_ref_count.hpp>

OCTK_BEGIN_NAMESPACE

// FrameEncryptorInterface allows users to provide a custom encryption
// implementation to encrypt all outgoing audio and video frames. The user must
// also provide a FrameDecryptorInterface to be able to decrypt the frames on
// the receiving device. Note this is an additional layer of encryption in
// addition to the standard SRTP mechanism and is not intended to be used
// without it. Implementations of this interface will have the same lifetime as
// the RTPSenders it is attached to. Additional data may be null.
class FrameEncryptorInterface : public RefCountInterface
{
public:
    ~FrameEncryptorInterface() override { }

    // Attempts to encrypt the provided frame. You may assume the encrypted_frame
    // will match the size returned by GetMaxCiphertextByteSize for a give frame.
    // You may assume that the frames will arrive in order if SRTP is enabled.
    // The ssrc will simply identify which stream the frame is travelling on. You
    // must set bytes_written to the number of bytes you wrote in the
    // encrypted_frame. 0 must be returned if successful all other numbers can be
    // selected by the implementer to represent error codes.
    virtual int Encrypt(MediaType media_type,
                        uint32_t ssrc,
                        ArrayView<const uint8_t> additional_data,
                        ArrayView<const uint8_t> frame,
                        ArrayView<uint8_t> encrypted_frame,
                        size_t *bytes_written) = 0;

    // Returns the total required length in bytes for the output of the
    // encryption. This can be larger than the actual number of bytes you need but
    // must never be smaller as it informs the size of the encrypted_frame buffer.
    virtual size_t GetMaxCiphertextByteSize(MediaType media_type, size_t frame_size) = 0;
};

OCTK_END_NAMESPACE

#endif // _OCTK_FRAME_ENCRYPTOR_INTERFACE_HPP
