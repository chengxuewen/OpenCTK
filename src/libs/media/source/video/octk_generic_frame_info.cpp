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

#include <octk_generic_frame_info.hpp>
#include <octk_checks.hpp>

#include <utility>

OCTK_BEGIN_NAMESPACE

GenericFrameInfo::GenericFrameInfo() = default;
GenericFrameInfo::GenericFrameInfo(const GenericFrameInfo &) = default;
GenericFrameInfo::~GenericFrameInfo() = default;

GenericFrameInfo::Builder::Builder() = default;
GenericFrameInfo::Builder::~Builder() = default;

GenericFrameInfo GenericFrameInfo::Builder::Build() const
{
    return info_;
}

GenericFrameInfo::Builder &GenericFrameInfo::Builder::T(int temporal_id)
{
    info_.temporal_id = temporal_id;
    return *this;
}

GenericFrameInfo::Builder &GenericFrameInfo::Builder::S(int spatial_id)
{
    info_.spatial_id = spatial_id;
    return *this;
}

GenericFrameInfo::Builder &GenericFrameInfo::Builder::Dtis(StringView indication_symbols)
{
    info_.decode_target_indications = detail::StringToDecodeTargetIndications(indication_symbols);
    return *this;
}

OCTK_END_NAMESPACE
