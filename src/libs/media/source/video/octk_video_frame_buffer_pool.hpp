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

#pragma once

#include <octk_race_checker.hpp>
#include <octk_i010_buffer.hpp>
#include <octk_i210_buffer.hpp>
#include <octk_i410_buffer.hpp>
#include <octk_i420_buffer.hpp>
#include <octk_i422_buffer.hpp>
#include <octk_i444_buffer.hpp>
#include <octk_nv12_buffer.hpp>

#include <stddef.h>

#include <list>

OCTK_BEGIN_NAMESPACE

// Simple buffer pool to avoid unnecessary allocations of video frame buffers.
// The pool manages the memory of the I420Buffer/NV12Buffer returned from
// Create(I420|NV12)Buffer. When the buffer is destructed, the memory is
// returned to the pool for use by subsequent calls to Create(I420|NV12)Buffer.
// If the resolution passed to Create(I420|NV12)Buffer changes or requested
// pixel format changes, old buffers will be purged from the pool.
// Note that Create(I420|NV12)Buffer will crash if more than
// kMaxNumberOfFramesBeforeCrash are created. This is to prevent memory leaks
// where frames are not returned.
class VideoFrameBufferPool
{
public:
    VideoFrameBufferPool();
    explicit VideoFrameBufferPool(bool zero_initialize);
    VideoFrameBufferPool(bool zero_initialize, size_t max_number_of_buffers);
    ~VideoFrameBufferPool();

    // Returns a buffer from the pool. If no suitable buffer exist in the pool
    // and there are less than `max_number_of_buffers` pending, a buffer is
    // created. Returns null otherwise.
    std::shared_ptr<I420Buffer> CreateI420Buffer(int width, int height);
    std::shared_ptr<I422Buffer> CreateI422Buffer(int width, int height);
    std::shared_ptr<I444Buffer> CreateI444Buffer(int width, int height);
    std::shared_ptr<I010Buffer> CreateI010Buffer(int width, int height);
    std::shared_ptr<I210Buffer> CreateI210Buffer(int width, int height);
    std::shared_ptr<I410Buffer> CreateI410Buffer(int width, int height);
    std::shared_ptr<NV12Buffer> CreateNV12Buffer(int width, int height);

    // Changes the max amount of buffers in the pool to the new value.
    // Returns true if change was successful and false if the amount of already
    // allocated buffers is bigger than new value.
    bool Resize(size_t max_number_of_buffers);

    // Clears buffers_ and detaches the thread checker so that it can be reused
    // later from another thread.
    void Release();

private:
    std::shared_ptr<VideoFrameBuffer> GetExistingBuffer(int width, int height, VideoFrameBuffer::Type type);

    RaceChecker race_checker_;
    std::list<std::shared_ptr<VideoFrameBuffer>> buffers_;
    // If true, newly allocated buffers are zero-initialized. Note that recycled
    // buffers are not zero'd before reuse. This is required of buffers used by
    // FFmpeg according to http://crbug.com/390941, which only requires it for the
    // initial allocation (as shown by FFmpeg's own buffer allocation code). It
    // has to do with "Use-of-uninitialized-value" on "Linux_msan_chrome".
    const bool zero_initialize_;
    // Max number of buffers this pool can have pending.
    size_t max_number_of_buffers_;
};

OCTK_END_NAMESPACE