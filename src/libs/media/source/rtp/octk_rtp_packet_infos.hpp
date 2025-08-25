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

#ifndef _OCTK_RTP_PACKET_INFOS_HPP
#define _OCTK_RTP_PACKET_INFOS_HPP

#include <octk_rtp_packet_info.hpp>

#include <utility>
#include <vector>

OCTK_BEGIN_NAMESPACE

// Semi-immutable structure to hold information about packets used to assemble
// an audio or video frame. Uses internal reference counting to make it very
// cheap to copy.
//
// We should ideally just use `std::vector<RtpPacketInfo>` and have it
// `std::move()`-ed as the per-packet information is transferred from one object
// to another. But moving the info, instead of copying it, is not easily done
// for the current video code.
class OCTK_MEDIA_API RtpPacketInfos
{
public:
    using vector_type = std::vector<RtpPacketInfo>;

    using value_type = vector_type::value_type;
    using size_type = vector_type::size_type;
    using difference_type = vector_type::difference_type;
    using const_reference = vector_type::const_reference;
    using const_pointer = vector_type::const_pointer;
    using const_iterator = vector_type::const_iterator;
    using const_reverse_iterator = vector_type::const_reverse_iterator;

    using reference = const_reference;
    using pointer = const_pointer;
    using iterator = const_iterator;
    using reverse_iterator = const_reverse_iterator;

    RtpPacketInfos() {}
    explicit RtpPacketInfos(const vector_type &entries)
        : data_(Data::Create(entries)) {}

    explicit RtpPacketInfos(vector_type &&entries)
        : data_(Data::Create(std::move(entries))) {}

    RtpPacketInfos(const RtpPacketInfos &other) = default;
    RtpPacketInfos(RtpPacketInfos &&other) = default;
    RtpPacketInfos &operator=(const RtpPacketInfos &other) = default;
    RtpPacketInfos &operator=(RtpPacketInfos &&other) = default;

    const_reference operator[](size_type pos) const { return entries()[pos]; }

    const_reference at(size_type pos) const { return entries().at(pos); }
    const_reference front() const { return entries().front(); }
    const_reference back() const { return entries().back(); }

    const_iterator begin() const { return entries().begin(); }
    const_iterator end() const { return entries().end(); }
    const_reverse_iterator rbegin() const { return entries().rbegin(); }
    const_reverse_iterator rend() const { return entries().rend(); }

    const_iterator cbegin() const { return entries().cbegin(); }
    const_iterator cend() const { return entries().cend(); }
    const_reverse_iterator crbegin() const { return entries().crbegin(); }
    const_reverse_iterator crend() const { return entries().crend(); }

    bool empty() const { return entries().empty(); }
    size_type size() const { return entries().size(); }

private:
    class Data
    {
    public:
        static std::shared_ptr<Data> Create(const vector_type &entries)
        {
            // Performance optimization for the empty case.
            if (entries.empty())
            {
                return nullptr;
            }

            return std::make_shared<Data>(entries);
        }

        static std::shared_ptr<Data> Create(vector_type &&entries)
        {
            // Performance optimization for the empty case.
            if (entries.empty())
            {
                return nullptr;
            }

            return std::make_shared<Data>(std::move(entries));
        }

        const vector_type &entries() const { return entries_; }

        explicit Data(const vector_type &entries) : entries_(entries) {}
        explicit Data(vector_type &&entries) : entries_(std::move(entries)) {}
        ~Data() = default;

    private:
        const vector_type entries_;
    };

    static const vector_type &empty_entries()
    {
        static const vector_type &value = *new vector_type();
        return value;
    }

    const vector_type &entries() const
    {
        if (data_ != nullptr)
        {
            return data_->entries();
        }
        else
        {
            return empty_entries();
        }
    }

    std::shared_ptr<Data> data_;
};
OCTK_END_NAMESPACE

#endif // _OCTK_RTP_PACKET_INFOS_HPP
