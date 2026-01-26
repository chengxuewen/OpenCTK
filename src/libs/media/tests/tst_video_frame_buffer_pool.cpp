/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2026~Present ChengXueWen.
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
#include <octk_video_frame_buffer.hpp>
#include <octk_i420_buffer.hpp>

#include <stdint.h>
#include <string.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

OCTK_BEGIN_NAMESPACE

TEST(VideoFrameBufferPoolTests, SimpleFrameReuse)
{
    VideoFrameBufferPool pool;
    auto buffer = pool.CreateI420Buffer(16, 16);
    EXPECT_EQ(16, buffer->width());
    EXPECT_EQ(16, buffer->height());
    // Extract non-refcounted pointers for testing.
    const uint8_t *y_ptr = buffer->dataY();
    const uint8_t *u_ptr = buffer->dataU();
    const uint8_t *v_ptr = buffer->dataV();
    // Release buffer so that it is returned to the pool.
    buffer = nullptr;
    // Check that the memory is resued.
    buffer = pool.CreateI420Buffer(16, 16);
    EXPECT_EQ(y_ptr, buffer->dataY());
    EXPECT_EQ(u_ptr, buffer->dataU());
    EXPECT_EQ(v_ptr, buffer->dataV());
}

TEST(VideoFrameBufferPoolTests, FailToReuseWrongSize)
{
    // Set max frames to 1, just to make sure the first buffer is being released.
    VideoFrameBufferPool pool(/*zero_initialize=*/false, 1);
    auto buffer = pool.CreateI420Buffer(16, 16);
    EXPECT_EQ(16, buffer->width());
    EXPECT_EQ(16, buffer->height());
    // Release buffer so that it is returned to the pool.
    buffer = nullptr;
    // Check that the pool doesn't try to reuse buffers of incorrect size.
    buffer = pool.CreateI420Buffer(32, 16);
    ASSERT_TRUE(buffer);
    EXPECT_EQ(32, buffer->width());
    EXPECT_EQ(16, buffer->height());
}

TEST(VideoFrameBufferPoolTests, FrameValidAfterPoolDestruction)
{
    std::shared_ptr<I420Buffer> buffer;
    {
        VideoFrameBufferPool pool;
        buffer = pool.CreateI420Buffer(16, 16);
    }
    EXPECT_EQ(16, buffer->width());
    EXPECT_EQ(16, buffer->height());
    // Access buffer, so that ASAN could find any issues if buffer
    // doesn't outlive the buffer pool.
    memset(buffer->MutableDataY(), 0xA5, 16 * buffer->strideY());
}

TEST(VideoFrameBufferPoolTests, MaxNumberOfBuffers)
{
    VideoFrameBufferPool pool(false, 1);
    auto buffer = pool.CreateI420Buffer(16, 16);
    EXPECT_NE(nullptr, buffer.get());
    EXPECT_EQ(nullptr, pool.CreateI420Buffer(16, 16).get());
}

TEST(VideoFrameBufferPoolTests, ProducesNv12)
{
    VideoFrameBufferPool pool(false, 1);
    auto buffer = pool.CreateNV12Buffer(16, 16);
    EXPECT_NE(nullptr, buffer.get());
}

TEST(VideoFrameBufferPoolTests, ProducesI422)
{
    VideoFrameBufferPool pool(false, 1);
    auto buffer = pool.CreateI422Buffer(16, 16);
    EXPECT_NE(nullptr, buffer.get());
}

TEST(VideoFrameBufferPoolTests, ProducesI444)
{
    VideoFrameBufferPool pool(false, 1);
    auto buffer = pool.CreateI444Buffer(16, 16);
    EXPECT_NE(nullptr, buffer.get());
}

TEST(VideoFrameBufferPoolTests, ProducesI010)
{
    VideoFrameBufferPool pool(false, 1);
    auto buffer = pool.CreateI010Buffer(16, 16);
    EXPECT_NE(nullptr, buffer.get());
}

TEST(VideoFrameBufferPoolTests, ProducesI210)
{
    VideoFrameBufferPool pool(false, 1);
    auto buffer = pool.CreateI210Buffer(16, 16);
    EXPECT_NE(nullptr, buffer.get());
}

TEST(VideoFrameBufferPoolTests, SwitchingPixelFormat)
{
    VideoFrameBufferPool pool(false, 1);
    auto buffeNV12 = pool.CreateNV12Buffer(16, 16);
    EXPECT_EQ(nullptr, pool.CreateNV12Buffer(16, 16).get());

    auto bufferI420 = pool.CreateI420Buffer(16, 16);
    EXPECT_NE(nullptr, bufferI420.get());
    EXPECT_EQ(nullptr, pool.CreateI420Buffer(16, 16).get());

    auto bufferI444 = pool.CreateI444Buffer(16, 16);
    EXPECT_NE(nullptr, bufferI444.get());
    EXPECT_EQ(nullptr, pool.CreateI444Buffer(16, 16).get());

    auto bufferI422 = pool.CreateI422Buffer(16, 16);
    EXPECT_NE(nullptr, bufferI422.get());
    EXPECT_EQ(nullptr, pool.CreateI422Buffer(16, 16).get());

    auto bufferI010 = pool.CreateI010Buffer(16, 16);
    EXPECT_NE(nullptr, bufferI010.get());
    EXPECT_EQ(nullptr, pool.CreateI010Buffer(16, 16).get());

    auto bufferI210 = pool.CreateI210Buffer(16, 16);
    EXPECT_NE(nullptr, bufferI210.get());
    EXPECT_EQ(nullptr, pool.CreateI210Buffer(16, 16).get());
}

OCTK_END_NAMESPACE
