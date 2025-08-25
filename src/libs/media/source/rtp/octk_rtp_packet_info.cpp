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

#include <octk_rtp_packet_info.hpp>

#include <algorithm>
#include <cstdint>
#include <utility>
#include <vector>

OCTK_BEGIN_NAMESPACE

RtpPacketInfo::RtpPacketInfo()
    : mSsrc(0), rtp_timestamp_(0), receive_time_(Timestamp::MinusInfinity()) {}

RtpPacketInfo::RtpPacketInfo(uint32_t ssrc,
                             std::vector<uint32_t> csrcs,
                             uint32_t rtp_timestamp,
                             Timestamp receive_time)
    : mSsrc(ssrc), csrcs_(std::move(csrcs)), rtp_timestamp_(rtp_timestamp), receive_time_(receive_time) {}

RtpPacketInfo::RtpPacketInfo(const RTPHeader &rtp_header,
                             Timestamp receive_time)
    : mSsrc(rtp_header.ssrc), rtp_timestamp_(rtp_header.timestamp), receive_time_(receive_time)
{
    const auto &extension = rtp_header.extension;
    const auto csrcs_count = std::min<size_t>(rtp_header.numCSRCs, kRtpCsrcSize);

    csrcs_.assign(&rtp_header.arrOfCSRCs[0], &rtp_header.arrOfCSRCs[csrcs_count]);

    if (extension.audioLevel)
    {
        audio_level_ = extension.audioLevel->level();
    }

    absolute_capture_time_ = extension.absolute_capture_time;
}

bool operator==(const RtpPacketInfo &lhs, const RtpPacketInfo &rhs)
{
    return (lhs.ssrc() == rhs.ssrc()) && (lhs.csrcs() == rhs.csrcs()) &&
           (lhs.rtp_timestamp() == rhs.rtp_timestamp()) &&
           (lhs.receive_time() == rhs.receive_time()) &&
           (lhs.audio_level() == rhs.audio_level()) &&
           (lhs.absolute_capture_time() == rhs.absolute_capture_time()) &&
           (lhs.local_capture_clock_offset() == rhs.local_capture_clock_offset());
}

OCTK_END_NAMESPACE

