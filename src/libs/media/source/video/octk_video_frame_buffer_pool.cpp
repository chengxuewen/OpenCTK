/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
** Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
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

#include <octk_video_frame_buffer_pool.hpp>
#include <octk_checks.hpp>

#include <limits>

OCTK_BEGIN_NAMESPACE

namespace
{
bool HasOneRef(const std::shared_ptr<VideoFrameBuffer> &buffer)
{
    // Cast to RefCountedObject is safe because this function is only called
    // on locally created VideoFrameBuffers, which are either
    // `RefCountedObject<I420Buffer>`, `RefCountedObject<I444Buffer>` or
    // `RefCountedObject<NV12Buffer>`.
    switch (buffer->type())
    {
        case VideoFrameBuffer::Type::kI420:
        {
            return std::dynamic_pointer_cast<I420Buffer>(buffer).use_count() == 2;
        }
        case VideoFrameBuffer::Type::kI444:
        {
            return std::dynamic_pointer_cast<I444Buffer>(buffer).use_count() == 2;
        }
        case VideoFrameBuffer::Type::kI422:
        {
            return std::dynamic_pointer_cast<I422Buffer>(buffer).use_count() == 2;
        }
        case VideoFrameBuffer::Type::kI010:
        {
            return std::dynamic_pointer_cast<I010Buffer>(buffer).use_count() == 2;
        }
        case VideoFrameBuffer::Type::kI210:
        {
            return std::dynamic_pointer_cast<I210Buffer>(buffer).use_count() == 2;
        }
        case VideoFrameBuffer::Type::kI410:
        {
            return std::dynamic_pointer_cast<I410Buffer>(buffer).use_count() == 2;
        }
        case VideoFrameBuffer::Type::kNV12:
        {
            return std::dynamic_pointer_cast<NV12Buffer>(buffer).use_count() == 2;
        }
        default: OCTK_DCHECK_NOTREACHED();
    }
    return false;
}

} // namespace

VideoFrameBufferPool::VideoFrameBufferPool()
    : VideoFrameBufferPool(false)
{
}

VideoFrameBufferPool::VideoFrameBufferPool(bool zero_initialize)
    : VideoFrameBufferPool(zero_initialize, std::numeric_limits<size_t>::max())
{
}

VideoFrameBufferPool::VideoFrameBufferPool(bool zero_initialize, size_t max_number_of_buffers)
    : zero_initialize_(zero_initialize)
    , max_number_of_buffers_(max_number_of_buffers)
{
}

VideoFrameBufferPool::~VideoFrameBufferPool() = default;

void VideoFrameBufferPool::Release()
{
    buffers_.clear();
}

bool VideoFrameBufferPool::Resize(size_t max_number_of_buffers)
{
    OCTK_DCHECK_RUNS_SERIALIZED(&race_checker_);
    size_t used_buffers_count = 0;
    for (const std::shared_ptr<VideoFrameBuffer> &buffer : buffers_)
    {
        // If the buffer is in use, the ref count will be >= 2, one from the list we
        // are looping over and one from the application. If the ref count is 1,
        // then the list we are looping over holds the only reference and it's safe
        // to reuse.
        if (!HasOneRef(buffer))
        {
            used_buffers_count++;
        }
    }
    if (used_buffers_count > max_number_of_buffers)
    {
        return false;
    }
    max_number_of_buffers_ = max_number_of_buffers;

    size_t buffers_to_purge = buffers_.size() - max_number_of_buffers_;
    auto iter = buffers_.begin();
    while (iter != buffers_.end() && buffers_to_purge > 0)
    {
        if (HasOneRef(*iter))
        {
            iter = buffers_.erase(iter);
            buffers_to_purge--;
        }
        else
        {
            ++iter;
        }
    }
    return true;
}

std::shared_ptr<I420Buffer> VideoFrameBufferPool::CreateI420Buffer(int width, int height)
{
    OCTK_DCHECK_RUNS_SERIALIZED(&race_checker_);

    std::shared_ptr<VideoFrameBuffer> existing_buffer = GetExistingBuffer(width, height, VideoFrameBuffer::Type::kI420);
    if (existing_buffer)
    {
        // Cast is safe because the only way kI420 buffer is created is
        // in the same function below, where `RefCountedObject<I420Buffer>` is
        // created.
        auto raw_buffer = std::dynamic_pointer_cast<I420Buffer>(existing_buffer);
        // Creates a new scoped_refptr, which is also pointing to the same
        // RefCountedObject as buffer, increasing ref count.
        return raw_buffer;
    }

    if (buffers_.size() >= max_number_of_buffers_)
        return nullptr;
    // Allocate new buffer.
    std::shared_ptr<I420Buffer> buffer = std::make_shared<I420Buffer>(width, height);

    if (zero_initialize_)
        buffer->InitializeData();

    buffers_.push_back(buffer);
    return buffer;
}

std::shared_ptr<I444Buffer> VideoFrameBufferPool::CreateI444Buffer(int width, int height)
{
    OCTK_DCHECK_RUNS_SERIALIZED(&race_checker_);

    std::shared_ptr<VideoFrameBuffer> existing_buffer = GetExistingBuffer(width, height, VideoFrameBuffer::Type::kI444);
    if (existing_buffer)
    {
        // Cast is safe because the only way kI444 buffer is created is
        // in the same function below, where |RefCountedObject<I444Buffer>|
        // is created.
        auto raw_buffer = std::dynamic_pointer_cast<I444Buffer>(existing_buffer);
        // Creates a new scoped_refptr, which is also pointing to the same
        // RefCountedObject as buffer, increasing ref count.
        return raw_buffer;
    }

    if (buffers_.size() >= max_number_of_buffers_)
        return nullptr;
    // Allocate new buffer.
    std::shared_ptr<I444Buffer> buffer = std::make_shared<I444Buffer>(width, height);

    if (zero_initialize_)
        buffer->InitializeData();

    buffers_.push_back(buffer);
    return buffer;
}

std::shared_ptr<I422Buffer> VideoFrameBufferPool::CreateI422Buffer(int width, int height)
{
    OCTK_DCHECK_RUNS_SERIALIZED(&race_checker_);

    std::shared_ptr<VideoFrameBuffer> existing_buffer = GetExistingBuffer(width, height, VideoFrameBuffer::Type::kI422);
    if (existing_buffer)
    {
        // Cast is safe because the only way kI422 buffer is created is
        // in the same function below, where |RefCountedObject<I422Buffer>|
        // is created.
        auto raw_buffer = std::dynamic_pointer_cast<I422Buffer>(existing_buffer);
        // Creates a new scoped_refptr, which is also pointing to the same
        // RefCountedObject as buffer, increasing ref count.
        return raw_buffer;
    }

    if (buffers_.size() >= max_number_of_buffers_)
        return nullptr;
    // Allocate new buffer.
    std::shared_ptr<I422Buffer> buffer = std::make_shared<I422Buffer>(width, height);

    if (zero_initialize_)
        buffer->InitializeData();

    buffers_.push_back(buffer);
    return buffer;
}

std::shared_ptr<NV12Buffer> VideoFrameBufferPool::CreateNV12Buffer(int width, int height)
{
    OCTK_DCHECK_RUNS_SERIALIZED(&race_checker_);

    std::shared_ptr<VideoFrameBuffer> existing_buffer = GetExistingBuffer(width, height, VideoFrameBuffer::Type::kNV12);
    if (existing_buffer)
    {
        // Cast is safe because the only way kI420 buffer is created is
        // in the same function below, where `RefCountedObject<I420Buffer>` is
        // created.
        auto raw_buffer = std::dynamic_pointer_cast<NV12Buffer>(existing_buffer);
        // Creates a new scoped_refptr, which is also pointing to the same
        // RefCountedObject as buffer, increasing ref count.
        return raw_buffer;
    }

    if (buffers_.size() >= max_number_of_buffers_)
        return nullptr;
    // Allocate new buffer.
    std::shared_ptr<NV12Buffer> buffer = std::make_shared<NV12Buffer>(width, height);

    if (zero_initialize_)
        buffer->InitializeData();

    buffers_.push_back(buffer);
    return buffer;
}

std::shared_ptr<I010Buffer> VideoFrameBufferPool::CreateI010Buffer(int width, int height)
{
    OCTK_DCHECK_RUNS_SERIALIZED(&race_checker_);

    std::shared_ptr<VideoFrameBuffer> existing_buffer = GetExistingBuffer(width, height, VideoFrameBuffer::Type::kI010);
    if (existing_buffer)
    {
        // Cast is safe because the only way kI010 buffer is created is
        // in the same function below, where |RefCountedObject<I010Buffer>|
        // is created.
        auto raw_buffer = std::dynamic_pointer_cast<I010Buffer>(existing_buffer);
        // Creates a new scoped_refptr, which is also pointing to the same
        // RefCountedObject as buffer, increasing ref count.
        return raw_buffer;
    }

    if (buffers_.size() >= max_number_of_buffers_)
        return nullptr;
    // Allocate new buffer.
    std::shared_ptr<I010Buffer> buffer = I010Buffer::Create(width, height);

    buffers_.push_back(buffer);
    return buffer;
}

std::shared_ptr<I210Buffer> VideoFrameBufferPool::CreateI210Buffer(int width, int height)
{
    OCTK_DCHECK_RUNS_SERIALIZED(&race_checker_);

    std::shared_ptr<VideoFrameBuffer> existing_buffer = GetExistingBuffer(width, height, VideoFrameBuffer::Type::kI210);
    if (existing_buffer)
    {
        // Cast is safe because the only way kI210 buffer is created is
        // in the same function below, where |RefCountedObject<I210Buffer>|
        // is created.
        auto raw_buffer = std::dynamic_pointer_cast<I210Buffer>(existing_buffer);
        // Creates a new scoped_refptr, which is also pointing to the same
        // RefCountedObject as buffer, increasing ref count.
        return raw_buffer;
    }

    if (buffers_.size() >= max_number_of_buffers_)
        return nullptr;
    // Allocate new buffer.
    std::shared_ptr<I210Buffer> buffer = I210Buffer::Create(width, height);

    buffers_.push_back(buffer);
    return buffer;
}

std::shared_ptr<I410Buffer> VideoFrameBufferPool::CreateI410Buffer(int width, int height)
{
    OCTK_DCHECK_RUNS_SERIALIZED(&race_checker_);

    std::shared_ptr<VideoFrameBuffer> existing_buffer = GetExistingBuffer(width, height, VideoFrameBuffer::Type::kI410);
    if (existing_buffer)
    {
        // Cast is safe because the only way kI410 buffer is created is
        // in the same function below, where |RefCountedObject<I410Buffer>|
        // is created.
        auto raw_buffer = std::dynamic_pointer_cast<I410Buffer>(existing_buffer);
        // Creates a new scoped_refptr, which is also pointing to the same
        // RefCountedObject as buffer, increasing ref count.
        return raw_buffer;
    }

    if (buffers_.size() >= max_number_of_buffers_)
        return nullptr;
    // Allocate new buffer.
    std::shared_ptr<I410Buffer> buffer = I410Buffer::Create(width, height);

    buffers_.push_back(buffer);
    return buffer;
}

std::shared_ptr<VideoFrameBuffer> VideoFrameBufferPool::GetExistingBuffer(int width,
                                                                          int height,
                                                                          VideoFrameBuffer::Type type)
{
    // Release buffers with wrong resolution or different type.
    for (auto it = buffers_.begin(); it != buffers_.end();)
    {
        const auto &buffer = *it;
        if (buffer->width() != width || buffer->height() != height || buffer->type() != type)
        {
            it = buffers_.erase(it);
        }
        else
        {
            ++it;
        }
    }
    // Look for a free buffer.
    for (const std::shared_ptr<VideoFrameBuffer> &buffer : buffers_)
    {
        // If the buffer is in use, the ref count will be >= 2, one from the list we
        // are looping over and one from the application. If the ref count is 1,
        // then the list we are looping over holds the only reference and it's safe
        // to reuse.
        if (HasOneRef(buffer))
        {
            OCTK_CHECK(buffer->type() == type);
            return buffer;
        }
    }
    return nullptr;
}

OCTK_END_NAMESPACE