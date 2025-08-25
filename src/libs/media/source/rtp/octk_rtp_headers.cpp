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

#include <octk_rtp_headers.hpp>

OCTK_BEGIN_NAMESPACE

AudioLevel::AudioLevel() : voice_activity_(false), audio_level_(0) {}

AudioLevel::AudioLevel(bool voice_activity, int audio_level)
    : voice_activity_(voice_activity), audio_level_(audio_level)
{
    OCTK_CHECK_GE(audio_level, 0);
    OCTK_CHECK_LE(audio_level, 127);
}

RTPHeaderExtension::RTPHeaderExtension()
    : hasTransmissionTimeOffset(false)
    , transmissionTimeOffset(0)
    , hasAbsoluteSendTime(false)
    , absoluteSendTime(0)
    , hasTransportSequenceNumber(false)
    , transportSequenceNumber(0)
    , hasVideoRotation(false)
    , videoRotation(VideoRotation::Angle0)
    , hasVideoContentType(false)
    , videoContentType(VideoContentType::Unspecified)
    , has_video_timing(false) {}

RTPHeaderExtension::RTPHeaderExtension(const RTPHeaderExtension &other) = default;

RTPHeaderExtension &RTPHeaderExtension::operator=(const RTPHeaderExtension &other) = default;

RTPHeader::RTPHeader()
    : markerBit(false), payloadType(0), sequenceNumber(0), timestamp(0), ssrc(0), numCSRCs(0), arrOfCSRCs()
    , paddingLength(0), headerLength(0), extension() {}

RTPHeader::RTPHeader(const RTPHeader &other) = default;

RTPHeader &RTPHeader::operator=(const RTPHeader &other) = default;
OCTK_END_NAMESPACE
