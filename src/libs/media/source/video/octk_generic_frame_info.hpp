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

#pragma once

#include <octk_inlined_vector.hpp>
#include <octk_video_codec_types.hpp>
#include <private/octk_dependency_descriptor_p.hpp>
#include <octk_video_codec_constants.hpp>

#include <bitset>
#include <vector>
#include <initializer_list>

//#include "absl/container/inlined_vector.h"
//#include "absl/strings/string_view.h"
//#include "api/transport/rtp/dependency_descriptor.h"
//#include "api/video/video_codec_constants.h"
//#include "rtc_base/system/rtc_export.h"

OCTK_BEGIN_NAMESPACE

// Describes how a certain encoder buffer was used when encoding a frame.
struct CodecBufferUsage
{
    constexpr CodecBufferUsage(int id, bool referenced, bool updated)
        : id(id)
        , referenced(referenced)
        , updated(updated)
    {
    }

    int id = 0;
    bool referenced = false;
    bool updated = false;
};

struct OCTK_MEDIA_API GenericFrameInfo : public FrameDependencyTemplate
{
    class Builder;

    GenericFrameInfo();
    GenericFrameInfo(const GenericFrameInfo &);
    ~GenericFrameInfo();

    //    absl::InlinedVector<CodecBufferUsage, kMaxEncoderBuffers> encoder_buffers;
    InlinedVector<CodecBufferUsage, kMaxEncoderBuffers> encoder_buffers;
    std::vector<bool> part_of_chain;
    std::bitset<32> active_decode_targets = ~uint32_t{0};
};

class GenericFrameInfo::Builder
{
public:
    Builder();
    ~Builder();

    GenericFrameInfo Build() const;
    Builder &T(int temporal_id);
    Builder &S(int spatial_id);
    Builder &Dtis(StringView indication_symbols);

private:
    GenericFrameInfo info_;
};

OCTK_END_NAMESPACE