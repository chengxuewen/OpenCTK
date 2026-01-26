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

#ifndef _OCTK_FRAME_TRANSFORMER_INTERFACE_HPP
#define _OCTK_FRAME_TRANSFORMER_INTERFACE_HPP

#include <octk_video_frame_metadata.hpp>
#include <octk_shared_ref_ptr.hpp>
#include <octk_array_view.hpp>
#include <octk_timestamp.hpp>
#include <octk_ref_count.hpp>
#include <octk_optional.hpp>

OCTK_BEGIN_NAMESPACE

// Owns the frame payload data.
class OCTK_MEDIA_API TransformableFrameInterface
{
public:
    // Only a known list of internal implementations of transformable frames are
    // permitted to allow internal downcasting. This is enforced via the
    // internally-constructable Passkey.
    // TODO: bugs.webrtc.org/339815768 - Remove this passkey once the downcasts are removed.
    class Passkey;

    explicit TransformableFrameInterface(Passkey *) {}

    TransformableFrameInterface(TransformableFrameInterface &&) = default;
    TransformableFrameInterface &operator=(TransformableFrameInterface &&) = default;

    virtual ~TransformableFrameInterface() = default;

    // Returns the frame payload data. The data is valid until the next non-const
    // method call.
    virtual ArrayView<const uint8_t> GetData() const = 0;

    // Copies `data` into the owned frame payload data.
    virtual void SetData(ArrayView<const uint8_t> data) = 0;

    virtual uint8_t GetPayloadType() const = 0;
    virtual uint32_t GetSsrc() const = 0;
    virtual uint32_t GetTimestamp() const = 0;
    virtual void SetRTPTimestamp(uint32_t timestamp) = 0;

    // TODO(https://bugs.webrtc.org/373365537): Remove this once its usage is
    // removed from blink.
    virtual Optional<Timestamp> GetCaptureTimeIdentifier() const
    {
        return utils::nullopt;
    }

    // TODO(https://bugs.webrtc.org/14878): Change this to pure virtual after it
    // is implemented everywhere.
    virtual Optional<Timestamp> GetPresentationTimestamp() const
    {
        return utils::nullopt;
    }

    enum class Direction
    {
        kUnknown,
        kReceiver,
        kSender,
    };
    // TODO(crbug.com/1250638): Remove this distinction between receiver and
    // sender frames to allow received frames to be directly re-transmitted on
    // other PeerConnectionss.
    virtual Direction GetDirection() const { return Direction::kUnknown; }
    virtual std::string GetMimeType() const = 0;
};

class OCTK_MEDIA_API TransformableVideoFrameInterface : public TransformableFrameInterface
{
public:
    explicit TransformableVideoFrameInterface(Passkey *passkey) : TransformableFrameInterface(passkey) {}
    virtual ~TransformableVideoFrameInterface() = default;
    virtual bool IsKeyFrame() const = 0;

    virtual VideoFrameMetadata Metadata() const = 0;

    virtual void SetMetadata(const VideoFrameMetadata &) = 0;
};

// Extends the TransformableFrameInterface to expose audio-specific information.
class OCTK_MEDIA_API TransformableAudioFrameInterface : public TransformableFrameInterface
{
public:
    explicit TransformableAudioFrameInterface(Passkey *passkey) : TransformableFrameInterface(passkey) {}
    virtual ~TransformableAudioFrameInterface() = default;

    virtual ArrayView<const uint32_t> GetContributingSources() const = 0;

    virtual const Optional<uint16_t> SequenceNumber() const = 0;

    virtual Optional<uint64_t> AbsoluteCaptureTimestamp() const = 0;

    enum class FrameType { kEmptyFrame, kAudioFrameSpeech, kAudioFrameCN };

    // TODO(crbug.com/1456628): Change this to pure virtual after it
    // is implemented everywhere.
    virtual FrameType Type() const { return FrameType::kEmptyFrame; }

    // Audio level in -dBov. Values range from 0 to 127, representing 0 to -127
    // dBov. 127 represents digital silence. Only present on remote frames if
    // the audio level header extension was included.
    virtual Optional<uint8_t> AudioLevel() const = 0;

    // Timestamp at which the packet has been first seen on the network interface.
    // Only defined for received audio packet.
    virtual Optional<Timestamp> ReceiveTime() const = 0;
};

// Objects implement this interface to be notified with the transformed frame.
class TransformedFrameCallback : public RefCountInterface
{
public:
    virtual void OnTransformedFrame(std::unique_ptr<TransformableFrameInterface> frame) = 0;

    // Request to no longer be called on each frame, instead having frames be
    // sent directly to OnTransformedFrame without additional work.
    // TODO(crbug.com/1502781): Make pure virtual once all mocks have
    // implementations.
    virtual void StartShortCircuiting() {}

protected:
    ~TransformedFrameCallback() override = default;
};

// Transforms encoded frames. The transformed frame is sent in a callback using
// the TransformedFrameCallback interface (see above).
class FrameTransformerInterface : public RefCountInterface
{
public:
    // Transforms `frame` using the implementing class' processing logic.
    virtual void Transform(std::unique_ptr<TransformableFrameInterface> transformable_frame) = 0;

    virtual void RegisterTransformedFrameCallback(SharedRefPtr<TransformedFrameCallback>) {}
    virtual void RegisterTransformedFrameSinkCallback(SharedRefPtr<TransformedFrameCallback>,
                                                      uint32_t /* ssrc */) {}
    virtual void UnregisterTransformedFrameCallback() {}
    virtual void UnregisterTransformedFrameSinkCallback(uint32_t /* ssrc */) {}

protected:
    ~FrameTransformerInterface() override = default;
};

// An interface implemented by classes that can host a transform.
// Currently this is implemented by the RTCRtpSender and RTCRtpReceiver.
class FrameTransformerHost
{
public:
    virtual ~FrameTransformerHost() {}
    virtual void SetFrameTransformer(SharedRefPtr<FrameTransformerInterface> frame_transformer) = 0;
    // TODO: bugs.webrtc.org/15929 - To be added:
    // virtual AddIncomingMediaType(RtpCodec codec) = 0;
    // virtual AddOutgoingMediaType(RtpCodec codec) = 0;
};

//------------------------------------------------------------------------------
// Implementation details follow
//------------------------------------------------------------------------------
class TransformableFrameInterface::Passkey
{
public:
    ~Passkey() = default;

private:
    // Explicit list of allowed internal implmentations of
    // TransformableFrameInterface.
    friend class TransformableOutgoingAudioFrame;

    friend class TransformableIncomingAudioFrame;

    friend class TransformableVideoSenderFrame;

    friend class TransformableVideoReceiverFrame;

    friend class MockTransformableFrame;

    friend class MockTransformableAudioFrame;

    friend class MockTransformableVideoFrame;

    Passkey() = default;
};

OCTK_END_NAMESPACE

#endif // _OCTK_FRAME_TRANSFORMER_INTERFACE_HPP
