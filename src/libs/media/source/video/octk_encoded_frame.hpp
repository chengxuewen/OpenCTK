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

#ifndef _OCTK_VIDEO_ENCODED_FRAME_HPP
#define _OCTK_VIDEO_ENCODED_FRAME_HPP

#include <octk_video_codec_interface.hpp>
#include <octk_video_codec_types.hpp>
#include <octk_rtp_video_header.hpp>
#include <octk_encoded_image.hpp>
#include <octk_timestamp.hpp>
#include <octk_optional.hpp>

#include <cstddef>
#include <cstdint>

OCTK_BEGIN_NAMESPACE

// TODO(philipel): Move transport specific info out of EncodedFrame.
// NOTE: This class is still under development and may change without notice.
class EncodedFrame : public EncodedImage
{
public:
    static const uint8_t kMaxFrameReferences = 5;

    EncodedFrame() = default;
    EncodedFrame(const EncodedFrame &) = default;
    virtual ~EncodedFrame() { }

    // When this frame was received.
    // TODO(bugs.webrtc.org/13756): Use Timestamp instead of int.
    virtual int64_t ReceivedTime() const { return -1; }
    // Returns a Timestamp from `ReceivedTime`, or utils::nullopt if there is no receive
    // time.
    Optional<Timestamp> ReceivedTimestamp() const;

    // When this frame should be rendered.
    // TODO(bugs.webrtc.org/13756): Use Timestamp instead of int.
    virtual int64_t RenderTime() const { return _renderTimeMs; }
    // TODO(bugs.webrtc.org/13756): Migrate to ReceivedTimestamp.
    int64_t RenderTimeMs() const { return _renderTimeMs; }
    // Returns a Timestamp from `RenderTime`, or utils::nullopt if there is no
    // render time.
    Optional<Timestamp> RenderTimestamp() const;

    // This information is currently needed by the timing calculation class.
    // TODO(philipel): Remove this function when a new timing class has
    //                 been implemented.
    virtual bool delayed_by_retransmission() const;

    bool is_keyframe() const { return num_references == 0; }

    void SetId(int64_t id) { mId = id; }
    int64_t Id() const { return mId; }

    uint8_t PayloadType() const { return _payloadType; }

    void SetRenderTime(const int64_t renderTimeMs) { _renderTimeMs = renderTimeMs; }

    const EncodedImage &EncodedImage() const { return static_cast<const octk::EncodedImage &>(*this); }

    const CodecSpecificInfo *CodecSpecific() const { return &_codecSpecificInfo; }
    void SetCodecSpecific(const CodecSpecificInfo *codec_specific) { _codecSpecificInfo = *codec_specific; }
    void SetFrameInstrumentationData(
        const Optional<Variant<FrameInstrumentationSyncData, FrameInstrumentationData>> frame_instrumentation)
    {
        _codecSpecificInfo.frame_instrumentation_data = frame_instrumentation;
    }

    // TODO(philipel): Add simple modify/access functions to prevent adding too
    // many `references`.
    size_t num_references = 0;
    int64_t references[kMaxFrameReferences];
    // Is this subframe the last one in the superframe (In RTP stream that would
    // mean that the last packet has a marker bit set).
    bool is_last_spatial_layer = true;

protected:
    // TODO(https://bugs.webrtc.org/9378): Move RTP specifics down into a
    // transport-aware subclass, eg RtpFrameObject.
    void CopyCodecSpecific(const RTPVideoHeader *header);

    // TODO(https://bugs.webrtc.org/9378): Make fields private with
    // getters/setters as needed.
    int64_t _renderTimeMs = -1;
    uint8_t _payloadType = 0;
    CodecSpecificInfo _codecSpecificInfo;
    VideoCodecType _codec = kVideoCodecGeneric;

private:
    // The ID of the frame is determined from RTP level information. The IDs are
    // used to describe order and dependencies between frames.
    int64_t mId = -1;
};

OCTK_END_NAMESPACE

#endif // _OCTK_VIDEO_ENCODED_FRAME_HPP
