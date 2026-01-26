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

#include <octk_scalability_mode.hpp>
#include <octk_inlined_vector.hpp>
#include <octk_rtp_parameters.hpp>
#include <octk_array_view.hpp>
#include <octk_optional.hpp>

#include <map>

OCTK_BEGIN_NAMESPACE

// SDP specification for a single video codec.
// NOTE: This class is still under development and may change without notice.
struct OCTK_MEDIA_API SdpVideoFormat
{
    using Parameters [[deprecated("Use webCodecParameterMap")]] = std::map<std::string, std::string>;

    explicit SdpVideoFormat(const std::string &name);
    SdpVideoFormat(const std::string &name, const CodecParameterMap &parameters);
    SdpVideoFormat(const std::string &name,
                   const CodecParameterMap &parameters,
                   const InlinedVector<ScalabilityMode, kScalabilityModeCount> &scalability_modes);
    // Creates a new SdpVideoFormat object identical to the supplied
    // SdpVideoFormat except the scalability_modes that are set to be the same as
    // the supplied scalability modes.
    SdpVideoFormat(const SdpVideoFormat &format,
                   const InlinedVector<ScalabilityMode, kScalabilityModeCount> &scalability_modes);

    SdpVideoFormat(const SdpVideoFormat &);
    SdpVideoFormat(SdpVideoFormat &&);
    SdpVideoFormat &operator=(const SdpVideoFormat &);
    SdpVideoFormat &operator=(SdpVideoFormat &&);

    ~SdpVideoFormat();

    // Returns true if the SdpVideoFormats have the same names as well as codec
    // specific parameters. Please note that two SdpVideoFormats can represent the
    // same codec even though not all parameters are the same.
    bool IsSameCodec(const SdpVideoFormat &other) const;
    bool IsCodecInList(ArrayView<const SdpVideoFormat> formats) const;

    std::string toString() const;

    friend OCTK_MEDIA_API bool operator==(const SdpVideoFormat &a, const SdpVideoFormat &b);
    friend OCTK_MEDIA_API bool operator!=(const SdpVideoFormat &a, const SdpVideoFormat &b) { return !(a == b); }

    std::string name;
    CodecParameterMap parameters;
    InlinedVector<ScalabilityMode, kScalabilityModeCount> scalability_modes;

    // Well-known video codecs and their format parameters.
    static const SdpVideoFormat VP8();
    static const SdpVideoFormat H264();
    static const SdpVideoFormat VP9Profile0();
    static const SdpVideoFormat VP9Profile1();
    static const SdpVideoFormat VP9Profile2();
    static const SdpVideoFormat VP9Profile3();
    static const SdpVideoFormat AV1Profile0();
    static const SdpVideoFormat AV1Profile1();
};

// For not so good reasons sometimes additional parameters are added to an
// SdpVideoFormat, which makes instances that should compare equal to not match
// anymore. Until we stop misusing SdpVideoFormats provide this convenience
// function to perform fuzzy matching.
Optional<SdpVideoFormat> FuzzyMatchSdpVideoFormat(ArrayView<const SdpVideoFormat> supported_formats,
                                                  const SdpVideoFormat &format);

OCTK_END_NAMESPACE