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

#include <private/octk_dependency_descriptor_p.hpp>
#include <octk_checks.hpp>

OCTK_BEGIN_NAMESPACE

constexpr int DependencyDescriptor::kMaxSpatialIds;
constexpr int DependencyDescriptor::kMaxTemporalIds;
constexpr int DependencyDescriptor::kMaxTemplates;
constexpr int DependencyDescriptor::kMaxDecodeTargets;

namespace detail
{
std::vector<DecodeTargetIndication> StringToDecodeTargetIndications(StringView symbols)
{
    std::vector<DecodeTargetIndication> dtis;
    dtis.reserve(symbols.size());
    for (char symbol : symbols)
    {
        DecodeTargetIndication indication;
        switch (symbol)
        {
            case '-': indication = DecodeTargetIndication::kNotPresent; break;
            case 'D': indication = DecodeTargetIndication::kDiscardable; break;
            case 'R': indication = DecodeTargetIndication::kRequired; break;
            case 'S': indication = DecodeTargetIndication::kSwitch; break;
            default: OCTK_DCHECK_NOTREACHED();
        }
        dtis.push_back(indication);
    }
    return dtis;
}
} // namespace detail

OCTK_END_NAMESPACE
