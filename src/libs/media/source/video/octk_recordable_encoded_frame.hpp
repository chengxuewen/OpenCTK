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

#ifndef _OCTK_VIDEO_RECORDABLE_ENCODED_FRAME_HPP
#define _OCTK_VIDEO_RECORDABLE_ENCODED_FRAME_HPP

#include <octk_video_codec_types.hpp>
#include <octk_encoded_image.hpp>
#include <octk_color_space.hpp>
#include <octk_timestamp.hpp>
#include <octk_optional.hpp>

OCTK_BEGIN_NAMESPACE

// Interface for accessing recordable elements of an encoded frame.
class RecordableEncodedFrame
{
public:
    // Encoded resolution in pixels
    // TODO(bugs.webrtc.org/12114) : remove in favor of Resolution.
    struct EncodedResolution
    {
        bool empty() const { return width == 0 && height == 0; }

        unsigned width = 0;
        unsigned height = 0;
    };

    virtual ~RecordableEncodedFrame() = default;

    // Provides access to encoded data
    virtual std::shared_ptr<const EncodedImageBufferInterface> encoded_buffer() const = 0;

    // Optionally returns the colorspace of the encoded frame. This can differ
    // from the eventually decoded frame's colorspace.
    virtual Optional<ColorSpace> colorSpace() const = 0;

    // Returns the codec of the encoded frame
    virtual VideoCodecType codec() const = 0;

    // Returns whether the encoded frame is a key frame
    virtual bool is_key_frame() const = 0;

    // Returns the frame's encoded resolution. May be 0x0 if the frame
    // doesn't contain resolution information
    virtual EncodedResolution resolution() const = 0;

    // Returns the computed render time
    virtual Timestamp render_time() const = 0;
};

OCTK_END_NAMESPACE

#endif  // _OCTK_VIDEO_RECORDABLE_ENCODED_FRAME_HPP
