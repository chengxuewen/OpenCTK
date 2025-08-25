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

#include <octk_encoded_image.hpp>
#include <octk_ref_counted_object.hpp>

#include <algorithm>
#include <cstdint>
#include <memory>

OCTK_BEGIN_NAMESPACE

EncodedImageBuffer::EncodedImageBuffer(size_t size)
    : buffer_(size)
{
}

EncodedImageBuffer::EncodedImageBuffer(const uint8_t *data, size_t size)
    : buffer_(data, size)
{
}

EncodedImageBuffer::EncodedImageBuffer(Buffer buffer)
    : buffer_(std::move(buffer))
{
}

// static
std::shared_ptr<EncodedImageBuffer> EncodedImageBuffer::Create(size_t size)
{
    return std::make_shared<EncodedImageBuffer>(size);
}
// static
std::shared_ptr<EncodedImageBuffer> EncodedImageBuffer::Create(const uint8_t *data, size_t size)
{
    return std::make_shared<EncodedImageBuffer>(data, size);
}
// static
std::shared_ptr<EncodedImageBuffer> EncodedImageBuffer::Create(Buffer buffer)
{
    return std::make_shared<EncodedImageBuffer>(std::move(buffer));
}

const uint8_t *EncodedImageBuffer::data() const { return buffer_.data(); }
uint8_t *EncodedImageBuffer::data() { return buffer_.data(); }
size_t EncodedImageBuffer::size() const { return buffer_.size(); }

void EncodedImageBuffer::Realloc(size_t size) { buffer_.SetSize(size); }

EncodedImage::EncodedImage() = default;

EncodedImage::EncodedImage(EncodedImage &&) = default;
EncodedImage::EncodedImage(const EncodedImage &) = default;

EncodedImage::~EncodedImage() = default;

EncodedImage &EncodedImage::operator=(EncodedImage &&) = default;
EncodedImage &EncodedImage::operator=(const EncodedImage &) = default;

void EncodedImage::setEncodeTime(int64_t encode_start_ms, int64_t encode_finish_ms)
{
    timing_.encode_start_ms = encode_start_ms;
    timing_.encode_finish_ms = encode_finish_ms;
}

Timestamp EncodedImage::captureTime() const
{
    return capture_time_ms_ > 0 ? Timestamp::Millis(capture_time_ms_) : Timestamp::MinusInfinity();
}

Optional<size_t> EncodedImage::spatialLayerFrameSize(int spatial_index) const
{
    OCTK_DCHECK_GE(spatial_index, 0);
    OCTK_DCHECK_LE(spatial_index, spatial_index_.value_or(0));

    auto it = spatial_layer_frame_size_bytes_.find(spatial_index);
    if (it == spatial_layer_frame_size_bytes_.end())
    {
        return utils::nullopt;
    }

    return it->second;
}

void EncodedImage::setSpatialLayerFrameSize(int spatial_index, size_t size_bytes)
{
    OCTK_DCHECK_GE(spatial_index, 0);
    OCTK_DCHECK_LE(spatial_index, spatial_index_.value_or(0));
    OCTK_DCHECK_GE(size_bytes, 0);
    spatial_layer_frame_size_bytes_[spatial_index] = size_bytes;
}

OCTK_END_NAMESPACE
