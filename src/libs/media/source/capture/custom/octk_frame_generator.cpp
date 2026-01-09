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

#include <octk_frame_generator.hpp>
#include <octk_platform_thread.hpp>
#include <octk_i010_buffer.hpp>
#include <octk_frame_utils.hpp>
#include <octk_video_type.hpp>
#include <octk_date_time.hpp>
#include <octk_optional.hpp>
#include <octk_logging.hpp>
#include <octk_yuv.hpp>

#include <libyuv.h>

OCTK_BEGIN_NAMESPACE

const char *FrameGeneratorInterface::outputTypeToString(FrameGeneratorInterface::OutputType type)
{
    switch (type)
    {
        case OutputType::kI420: return "I420";
        case OutputType::kI420A: return "I420A";
        case OutputType::kI010: return "I010";
        case OutputType::kNV12: return "NV12";
        default: OCTK_DCHECK_NOTREACHED();
    }
    return "";
}

SquareGenerator::SquareGenerator(int width, int height, OutputType type, int num_squares)
    : mType(type)
{
    changeResolution(width, height);
    for (int i = 0; i < num_squares; ++i)
    {
        mSquares.emplace_back(new Square(width, height, i + 1));
    }
}

void SquareGenerator::changeResolution(size_t width, size_t height)
{
    Mutex::Lock locker(mMutex);
    mWidth = static_cast<int>(width);
    mHeight = static_cast<int>(height);
    OCTK_CHECK(mWidth > 0);
    OCTK_CHECK(mHeight > 0);
}

FrameGeneratorInterface::Resolution SquareGenerator::getResolution() const
{
    Mutex::Lock locker(mMutex);
    return {static_cast<size_t>(mWidth), static_cast<size_t>(mHeight)};
}

std::shared_ptr<I420Buffer> SquareGenerator::createI420Buffer(int width, int height)
{
    // OCTK_TRACE("SquareGenerator::createI420Buffer(%d, %d):tis:%s",
    // width, height, PlatformThread::currentThreadIdHexString().c_str());
    std::shared_ptr<I420Buffer> buffer(I420Buffer::create(width, height));
    memset(buffer->MutableDataY(), 127, height * buffer->strideY());
    memset(buffer->MutableDataU(), 127, buffer->chromaHeight() * buffer->strideU());
    memset(buffer->MutableDataV(), 127, buffer->chromaHeight() * buffer->strideV());
    return buffer;
}

FrameGeneratorInterface::VideoFrameData SquareGenerator::nextFrame()
{
    Mutex::Lock locker(mMutex);

    std::shared_ptr<VideoFrameBuffer> buffer = nullptr;
    switch (mType)
    {
        case OutputType::kI420:
        case OutputType::kI010:
        case OutputType::kNV12:
        {
            buffer = createI420Buffer(mWidth, mHeight);
            break;
        }
        case OutputType::kI420A:
        {
            std::shared_ptr<I420Buffer> yuv_buffer = createI420Buffer(mWidth, mHeight);
            std::shared_ptr<I420Buffer> axx_buffer = createI420Buffer(mWidth, mHeight);
            buffer = utils::wrapI420ABuffer(yuv_buffer->width(),
                                            yuv_buffer->height(),
                                            yuv_buffer->dataY(),
                                            yuv_buffer->strideY(),
                                            yuv_buffer->dataU(),
                                            yuv_buffer->strideU(),
                                            yuv_buffer->dataV(),
                                            yuv_buffer->strideV(),
                                            axx_buffer->dataY(),
                                            axx_buffer->strideY(),
                                            // To keep references alive.
                                            [yuv_buffer, axx_buffer] {});
            break;
        }
        default: OCTK_DCHECK_NOTREACHED() << "The given output format is not supported.";
    }

    for (const auto &square : mSquares)
    {
        square->draw(buffer);
    }

    if (mType == OutputType::kI010)
    {
        buffer = I010Buffer::Copy(*buffer->toI420());
    }
    else if (mType == OutputType::kNV12)
    {
        buffer = NV12Buffer::Copy(*buffer->toI420());
    }

    return VideoFrameData(buffer, utils::nullopt);
}

namespace detail
{
static constexpr int kBitmapWidth = 6;
static constexpr int kBitmapHeight = 10;
const uint8_t digitBitmap0[] = {
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y1
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y2
    0x00,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0x00, // Y3
    0x00,
    0xFF,
    0x00,
    0x00,
    0xFF,
    0x00, // Y4
    0x00,
    0xFF,
    0x00,
    0x00,
    0xFF,
    0x00, // Y5
    0x00,
    0xFF,
    0x00,
    0x00,
    0xFF,
    0x00, // Y6
    0x00,
    0xFF,
    0x00,
    0x00,
    0xFF,
    0x00, // Y7
    0x00,
    0xFF,
    0x00,
    0x00,
    0xFF,
    0x00, // Y8
    0x00,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0x00, // Y9
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y10
    /*-------------------------------------*/
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U1
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U2
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U3
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U4
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U5
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V1
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V2
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V3
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V4
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V5
};
const uint8_t digitBitmap1[] = {
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y1
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y2
    0x00,
    0xFF,
    0xFF,
    0x00,
    0x00,
    0x00, // Y3
    0x00,
    0x00,
    0xFF,
    0x00,
    0x00,
    0x00, // Y4
    0x00,
    0x00,
    0xFF,
    0x00,
    0x00,
    0x00, // Y5
    0x00,
    0x00,
    0xFF,
    0x00,
    0x00,
    0x00, // Y6
    0x00,
    0x00,
    0xFF,
    0x00,
    0x00,
    0x00, // Y7
    0x00,
    0x00,
    0xFF,
    0x00,
    0x00,
    0x00, // Y8
    0x00,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0x00, // Y9
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y10
    /*-------------------------------------*/
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U1
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U2
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U3
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U4
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U5
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V1
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V2
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V3
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V4
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V5
};
const uint8_t digitBitmap2[] = {
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y1
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y2
    0x00,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0x00, // Y3
    0x00,
    0x00,
    0x00,
    0x00,
    0xFF,
    0x00, // Y4
    0x00,
    0x00,
    0x00,
    0x00,
    0xFF,
    0x00, // Y5
    0x00,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0x00, // Y6
    0x00,
    0xFF,
    0x00,
    0x00,
    0x00,
    0x00, // Y7
    0x00,
    0xFF,
    0x00,
    0x00,
    0x00,
    0x00, // Y8
    0x00,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0x00, // Y9
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y10
    /*-------------------------------------*/
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U1
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U2
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U3
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U4
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U5
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V1
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V2
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V3
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V4
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V5
};
const uint8_t digitBitmap3[] = {
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y1
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y2
    0x00,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0x00, // Y3
    0x00,
    0x00,
    0x00,
    0x00,
    0xFF,
    0x00, // Y4
    0x00,
    0x00,
    0x00,
    0x00,
    0xFF,
    0x00, // Y5
    0x00,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0x00, // Y6
    0x00,
    0x00,
    0x00,
    0x00,
    0xFF,
    0x00, // Y7
    0x00,
    0x00,
    0x00,
    0x00,
    0xFF,
    0x00, // Y8
    0x00,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0x00, // Y9
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y10
    /*-------------------------------------*/
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U1
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U2
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U3
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U4
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U5
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V1
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V2
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V3
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V4
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V5
};
const uint8_t digitBitmap4[] = {
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y1
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y2
    0x00,
    0xFF,
    0x00,
    0x00,
    0xFF,
    0x00, // Y3
    0x00,
    0xFF,
    0x00,
    0x00,
    0xFF,
    0x00, // Y4
    0x00,
    0xFF,
    0x00,
    0x00,
    0xFF,
    0x00, // Y5
    0x00,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0x00, // Y6
    0x00,
    0x00,
    0x00,
    0x00,
    0xFF,
    0x00, // Y7
    0x00,
    0x00,
    0x00,
    0x00,
    0xFF,
    0x00, // Y8
    0x00,
    0x00,
    0x00,
    0x00,
    0xFF,
    0x00, // Y9
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y10
    /*-------------------------------------*/
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U1
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U2
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U3
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U4
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U5
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V1
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V2
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V3
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V4
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V5
};
const uint8_t digitBitmap5[] = {
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y1
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y2
    0x00,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0x00, // Y3
    0x00,
    0xFF,
    0x00,
    0x00,
    0x00,
    0x00, // Y4
    0x00,
    0xFF,
    0x00,
    0x00,
    0x00,
    0x00, // Y5
    0x00,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0x00, // Y6
    0x00,
    0x00,
    0x00,
    0x00,
    0xFF,
    0x00, // Y7
    0x00,
    0x00,
    0x00,
    0x00,
    0xFF,
    0x00, // Y8
    0x00,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0x00, // Y9
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y10
    /*-------------------------------------*/
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U1
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U2
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U3
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U4
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U5
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V1
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V2
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V3
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V4
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V5
};
const uint8_t digitBitmap6[] = {
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y1
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y2
    0x00,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0x00, // Y3
    0x00,
    0xFF,
    0x00,
    0x00,
    0x00,
    0x00, // Y4
    0x00,
    0xFF,
    0x00,
    0x00,
    0x00,
    0x00, // Y5
    0x00,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0x00, // Y6
    0x00,
    0xFF,
    0x00,
    0x00,
    0xFF,
    0x00, // Y7
    0x00,
    0xFF,
    0x00,
    0x00,
    0xFF,
    0x00, // Y8
    0x00,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0x00, // Y9
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y10
    /*-------------------------------------*/
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U1
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U2
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U3
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U4
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U5
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V1
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V2
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V3
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V4
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V5
};
const uint8_t digitBitmap7[] = {
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y1
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y2
    0x00,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0x00, // Y3
    0x00,
    0x00,
    0x00,
    0x00,
    0xFF,
    0x00, // Y4
    0x00,
    0x00,
    0x00,
    0x00,
    0xFF,
    0x00, // Y5
    0x00,
    0x00,
    0x00,
    0x00,
    0xFF,
    0x00, // Y6
    0x00,
    0x00,
    0x00,
    0x00,
    0xFF,
    0x00, // Y7
    0x00,
    0x00,
    0x00,
    0x00,
    0xFF,
    0x00, // Y8
    0x00,
    0x00,
    0x00,
    0x00,
    0xFF,
    0x00, // Y9
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y10
    /*-------------------------------------*/
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U1
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U2
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U3
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U4
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U5
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V1
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V2
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V3
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V4
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V5
};
const uint8_t digitBitmap8[] = {
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y1
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y2
    0x00,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0x00, // Y3
    0x00,
    0xFF,
    0x00,
    0x00,
    0xFF,
    0x00, // Y4
    0x00,
    0xFF,
    0x00,
    0x00,
    0xFF,
    0x00, // Y5
    0x00,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0x00, // Y6
    0x00,
    0xFF,
    0x00,
    0x00,
    0xFF,
    0x00, // Y7
    0x00,
    0xFF,
    0x00,
    0x00,
    0xFF,
    0x00, // Y8
    0x00,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0x00, // Y9
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y10
    /*-------------------------------------*/
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U1
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U2
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U3
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U4
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U5
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V1
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V2
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V3
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V4
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V5
};
const uint8_t digitBitmap9[] = {
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y1
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y2
    0x00,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0x00, // Y3
    0x00,
    0xFF,
    0x00,
    0x00,
    0xFF,
    0x00, // Y4
    0x00,
    0xFF,
    0x00,
    0x00,
    0xFF,
    0x00, // Y5
    0x00,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0x00, // Y6
    0x00,
    0x00,
    0x00,
    0x00,
    0xFF,
    0x00, // Y7
    0x00,
    0x00,
    0x00,
    0x00,
    0xFF,
    0x00, // Y8
    0x00,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0x00, // Y9
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y10
    /*-------------------------------------*/
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U1
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U2
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U3
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U4
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U5
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V1
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V2
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V3
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V4
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V5
};
const uint8_t digitBitmapNul[] = {
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y1
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y2
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y3
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y4
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y5
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y6
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y7
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y8
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y9
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y10
    /*-------------------------------------*/
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U1
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U2
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U3
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U4
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U5
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V1
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V2
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V3
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V4
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V5
};
const uint8_t digitBitmapDot[] = {
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y1
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y2
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y3
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y4
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y5
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y6
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y7
    0x00,
    0x00,
    0xFF,
    0x00,
    0x00,
    0x00, // Y8
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y9
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y10
    /*-------------------------------------*/
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U1
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U2
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U3
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U4
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U5
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V1
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V2
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V3
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V4
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V5
};
const uint8_t digitBitmapDDot[] = {
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y1
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y2
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y3
    0x00,
    0x00,
    0xFF,
    0x00,
    0x00,
    0x00, // Y4
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y5
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y6
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y7
    0x00,
    0x00,
    0xFF,
    0x00,
    0x00,
    0x00, // Y8
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y9
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y10
    /*-------------------------------------*/
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U1
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U2
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U3
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U4
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U5
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V1
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V2
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V3
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V4
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V5
};
const uint8_t digitBitmapLine[] = {
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y1
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y2
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y3
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y4
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y5
    0x00,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0x00, // Y6
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y7
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y8
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y9
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00, // Y10
    /*-------------------------------------*/
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U1
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U2
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U3
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U4
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // U5
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V1
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V2
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V3
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V4
    0x80,
    0x80,
    0x80,
    0x80,
    0x80,
    0x80, // V5
};

static constexpr const uint8_t *digitBitmaps[] = {digitBitmap0,
                                                  digitBitmap1,
                                                  digitBitmap2,
                                                  digitBitmap3,
                                                  digitBitmap4,
                                                  digitBitmap5,
                                                  digitBitmap6,
                                                  digitBitmap7,
                                                  digitBitmap8,
                                                  digitBitmap9,
                                                  digitBitmapNul};

#define I420_U_PTR(buffer, width, height)                          (buffer + width * height)
#define I420_V_PTR(buffer, width, height)                          (buffer + width * height + (width >> 1) * (height >> 1))
#define I420_UV_STRIDE(width)                                      (width >> 1)
#define I420_Y_OFFSET_PTR(buffer, width, height, xOffset, yOffset) (buffer + width * yOffset + xOffset)
#define I420_U_OFFSET_PTR(buffer, width, height, xOffset, yOffset)                                                     \
    (I420_U_PTR(buffer, width, height) + I420_UV_STRIDE(width) * (yOffset / 2) + (xOffset / 2))
#define I420_V_OFFSET_PTR(buffer, width, height, xOffset, yOffset)                                                     \
    (I420_V_PTR(buffer, width, height) + I420_UV_STRIDE(width) * (yOffset / 2) + (xOffset / 2))

#define NV12_UV_PTR(buffer, width, height)                         (buffer + width * height)
#define NV12_STRIDE(width)                                         (width)
#define NV12_Y_OFFSET_PTR(buffer, width, height, xOffset, yOffset) (buffer + width * yOffset + xOffset)
#define NV12_UV_OFFSET_PTR(buffer, width, height, xOffset, yOffset)                                                    \
    (NV12_UV_PTR(buffer, width, height) + width * (yOffset / 2) + xOffset)

#define NV21_VU_PTR(buffer, width, height) (buffer + width * height)
#define NV21_STRIDE(width)                 (width)

#define ARGB_OFFSET_PTR(buffer, width, xOffset, yOffset) (buffer + width * 4 * yOffset + xOffset * 4)
#define RGB_OFFSET_PTR(buffer, width, xOffset, yOffset)  (buffer + width * 3 * yOffset + xOffset * 3)

void drawI420DigitNumber(const uint8_t *src_data,
                         int src_width,
                         int src_height,
                         float scale,
                         uint8_t *dst_data,
                         int dst_width,
                         int dst_height,
                         int dst_x,
                         int dst_y)
{
    std::shared_ptr<I420Buffer> i420Buffer(I420Buffer::create(src_width, src_height));
    memcpy(i420Buffer->MutableDataY(), src_data, src_width * src_height * 3 / 2);
    std::shared_ptr<I420Buffer> scaledI420Buffer(I420Buffer::create(src_width * scale, src_height * scale));
    libyuv::I420Scale(
        i420Buffer->dataY(),
        i420Buffer->width(),
        I420_U_PTR(i420Buffer->dataY(), i420Buffer->width(), i420Buffer->height()),
        I420_UV_STRIDE(i420Buffer->width()),
        I420_V_PTR(i420Buffer->dataY(), i420Buffer->width(), i420Buffer->height()),
        I420_UV_STRIDE(i420Buffer->width()),
        i420Buffer->width(),
        i420Buffer->height(),
        scaledI420Buffer->MutableDataY(),
        scaledI420Buffer->width(),
        I420_U_PTR(scaledI420Buffer->MutableDataY(), scaledI420Buffer->width(), scaledI420Buffer->height()),
        I420_UV_STRIDE(scaledI420Buffer->width()),
        I420_V_PTR(scaledI420Buffer->MutableDataY(), scaledI420Buffer->width(), scaledI420Buffer->height()),
        I420_UV_STRIDE(scaledI420Buffer->width()),
        scaledI420Buffer->width(),
        scaledI420Buffer->height(),
        libyuv::kFilterNone);

    auto bufferIn = const_cast<uint8_t *>(scaledI420Buffer->dataY());
    const auto width = scaledI420Buffer->width();
    const auto height = scaledI420Buffer->height();
    const int fixWidth = std::min(width, dst_width);
    const int fixHeight = std::min(height, dst_height);
    libyuv::I420Copy(I420_Y_OFFSET_PTR(bufferIn, width, height, 0, 0),
                     width,
                     I420_U_OFFSET_PTR(bufferIn, width, height, 0, 0),
                     I420_UV_STRIDE(width),
                     I420_V_OFFSET_PTR(bufferIn, width, height, 0, 0),
                     I420_UV_STRIDE(width),
                     I420_Y_OFFSET_PTR(dst_data, dst_width, dst_height, dst_x, dst_y),
                     dst_width,
                     I420_U_OFFSET_PTR(dst_data, dst_width, dst_height, dst_x, dst_y),
                     I420_UV_STRIDE(dst_width),
                     I420_V_OFFSET_PTR(dst_data, dst_width, dst_height, dst_x, dst_y),
                     I420_UV_STRIDE(dst_width),
                     fixWidth,
                     fixHeight);
}
void drawI420LocalTime(uint8_t *dst_data,
                       int dst_width,
                       int dst_height,
                       int dst_x,
                       int dst_y,
                       float scale,
                       const DateTime::LocalTime &localTime = DateTime::localTimeFromSystemTimeMSecs())
{
    int index = 0;
    const auto offsetWidth = detail::kBitmapWidth * scale;
    detail::drawI420DigitNumber(detail::digitBitmaps[std::min(localTime.year / 1000, 10)],
                                  detail::kBitmapWidth,
                                  detail::kBitmapHeight,
                                  scale,
                                  dst_data,
                                  dst_width,
                                  dst_height,
                                  dst_x + offsetWidth * index++,
                                  dst_y);
    detail::drawI420DigitNumber(detail::digitBitmaps[std::min(localTime.year / 100 % 10, 10)],
                                  detail::kBitmapWidth,
                                  detail::kBitmapHeight,
                                  scale,
                                  dst_data,
                                  dst_width,
                                  dst_height,
                                  dst_x + offsetWidth * index++,
                                  dst_y);
    detail::drawI420DigitNumber(detail::digitBitmaps[std::min(localTime.year / 10 % 10, 10)],
                                  detail::kBitmapWidth,
                                  detail::kBitmapHeight,
                                  scale,
                                  dst_data,
                                  dst_width,
                                  dst_height,
                                  dst_x + offsetWidth * index++,
                                  dst_y);
    detail::drawI420DigitNumber(detail::digitBitmaps[std::min(localTime.year % 10, 10)],
                                  detail::kBitmapWidth,
                                  detail::kBitmapHeight,
                                  scale,
                                  dst_data,
                                  dst_width,
                                  dst_height,
                                  dst_x + offsetWidth * index++,
                                  dst_y);
    detail::drawI420DigitNumber(detail::digitBitmapLine,
                                  detail::kBitmapWidth,
                                  detail::kBitmapHeight,
                                  scale,
                                  dst_data,
                                  dst_width,
                                  dst_height,
                                  dst_x + offsetWidth * index++,
                                  dst_y);
    detail::drawI420DigitNumber(detail::digitBitmaps[std::min(localTime.mon / 10, 10)],
                                  detail::kBitmapWidth,
                                  detail::kBitmapHeight,
                                  scale,
                                  dst_data,
                                  dst_width,
                                  dst_height,
                                  dst_x + offsetWidth * index++,
                                  dst_y);
    detail::drawI420DigitNumber(detail::digitBitmaps[std::min(localTime.mon % 10, 10)],
                                  detail::kBitmapWidth,
                                  detail::kBitmapHeight,
                                  scale,
                                  dst_data,
                                  dst_width,
                                  dst_height,
                                  dst_x + offsetWidth * index++,
                                  dst_y);
    detail::drawI420DigitNumber(detail::digitBitmapLine,
                                  detail::kBitmapWidth,
                                  detail::kBitmapHeight,
                                  scale,
                                  dst_data,
                                  dst_width,
                                  dst_height,
                                  dst_x + offsetWidth * index++,
                                  dst_y);
    detail::drawI420DigitNumber(detail::digitBitmaps[std::min(localTime.day / 10, 10)],
                                  detail::kBitmapWidth,
                                  detail::kBitmapHeight,
                                  scale,
                                  dst_data,
                                  dst_width,
                                  dst_height,
                                  dst_x + offsetWidth * index++,
                                  dst_y);
    detail::drawI420DigitNumber(detail::digitBitmaps[std::min(localTime.day % 10, 10)],
                                  detail::kBitmapWidth,
                                  detail::kBitmapHeight,
                                  scale,
                                  dst_data,
                                  dst_width,
                                  dst_height,
                                  dst_x + offsetWidth * index++,
                                  dst_y);
    detail::drawI420DigitNumber(detail::digitBitmapNul,
                                  detail::kBitmapWidth,
                                  detail::kBitmapHeight,
                                  scale,
                                  dst_data,
                                  dst_width,
                                  dst_height,
                                  dst_x + offsetWidth * index++,
                                  dst_y);
    detail::drawI420DigitNumber(detail::digitBitmaps[std::min(localTime.hour / 10, 10)],
                                  detail::kBitmapWidth,
                                  detail::kBitmapHeight,
                                  scale,
                                  dst_data,
                                  dst_width,
                                  dst_height,
                                  dst_x + offsetWidth * index++,
                                  dst_y);
    detail::drawI420DigitNumber(detail::digitBitmaps[std::min(localTime.hour % 10, 10)],
                                  detail::kBitmapWidth,
                                  detail::kBitmapHeight,
                                  scale,
                                  dst_data,
                                  dst_width,
                                  dst_height,
                                  dst_x + offsetWidth * index++,
                                  dst_y);
    detail::drawI420DigitNumber(detail::digitBitmapDDot,
                                  detail::kBitmapWidth,
                                  detail::kBitmapHeight,
                                  scale,
                                  dst_data,
                                  dst_width,
                                  dst_height,
                                  dst_x + offsetWidth * index++,
                                  dst_y);
    detail::drawI420DigitNumber(detail::digitBitmaps[std::min(localTime.min / 10, 10)],
                                  detail::kBitmapWidth,
                                  detail::kBitmapHeight,
                                  scale,
                                  dst_data,
                                  dst_width,
                                  dst_height,
                                  dst_x + offsetWidth * index++,
                                  dst_y);
    detail::drawI420DigitNumber(detail::digitBitmaps[std::min(localTime.min % 10, 10)],
                                  detail::kBitmapWidth,
                                  detail::kBitmapHeight,
                                  scale,
                                  dst_data,
                                  dst_width,
                                  dst_height,
                                  dst_x + offsetWidth * index++,
                                  dst_y);
    detail::drawI420DigitNumber(detail::digitBitmapDDot,
                                  detail::kBitmapWidth,
                                  detail::kBitmapHeight,
                                  scale,
                                  dst_data,
                                  dst_width,
                                  dst_height,
                                  dst_x + offsetWidth * index++,
                                  dst_y);
    detail::drawI420DigitNumber(detail::digitBitmaps[std::min(localTime.sec / 10, 10)],
                                  detail::kBitmapWidth,
                                  detail::kBitmapHeight,
                                  scale,
                                  dst_data,
                                  dst_width,
                                  dst_height,
                                  dst_x + offsetWidth * index++,
                                  dst_y);
    detail::drawI420DigitNumber(detail::digitBitmaps[std::min(localTime.sec % 10, 10)],
                                  detail::kBitmapWidth,
                                  detail::kBitmapHeight,
                                  scale,
                                  dst_data,
                                  dst_width,
                                  dst_height,
                                  dst_x + offsetWidth * index++,
                                  dst_y);
    detail::drawI420DigitNumber(detail::digitBitmapDot,
                                  detail::kBitmapWidth,
                                  detail::kBitmapHeight,
                                  scale,
                                  dst_data,
                                  dst_width,
                                  dst_height,
                                  dst_x + offsetWidth * index++,
                                  dst_y);
    detail::drawI420DigitNumber(detail::digitBitmaps[std::min(localTime.mil / 100 % 10, 10)],
                                  detail::kBitmapWidth,
                                  detail::kBitmapHeight,
                                  scale,
                                  dst_data,
                                  dst_width,
                                  dst_height,
                                  dst_x + offsetWidth * index++,
                                  dst_y);
    detail::drawI420DigitNumber(detail::digitBitmaps[std::min(localTime.mil / 10 % 10, 10)],
                                  detail::kBitmapWidth,
                                  detail::kBitmapHeight,
                                  scale,
                                  dst_data,
                                  dst_width,
                                  dst_height,
                                  dst_x + offsetWidth * index++,
                                  dst_y);
    detail::drawI420DigitNumber(detail::digitBitmaps[std::min(localTime.mil % 10, 10)],
                                  detail::kBitmapWidth,
                                  detail::kBitmapHeight,
                                  scale,
                                  dst_data,
                                  dst_width,
                                  dst_height,
                                  dst_x + offsetWidth * index++,
                                  dst_y);
}
} // namespace detail

SquareGenerator::Square::Square(int width, int height, int seed)
    : random_generator_(seed)
    , x_(random_generator_.Rand(0, width))
    , y_(random_generator_.Rand(0, height))
    , mLength(random_generator_.Rand(1, width > 4 ? width / 4 : 1))
    , yuv_y_(random_generator_.Rand(0, 255))
    , yuv_u_(random_generator_.Rand(0, 255))
    , yuv_v_(random_generator_.Rand(0, 255))
    , yuv_a_(random_generator_.Rand(0, 255))
{
}

void SquareGenerator::Square::draw(std::shared_ptr<VideoFrameBuffer> &frame_buffer)
{
    OCTK_DCHECK(frame_buffer->type() == VideoFrameBuffer::Type::kI420 ||
                frame_buffer->type() == VideoFrameBuffer::Type::kI420A);
    auto buffer = frame_buffer->getI420();
    const int length_cap = std::min(frame_buffer->height(), frame_buffer->width()) / 4;
    const int length = std::min(mLength, length_cap);
    x_ = (x_ + random_generator_.Rand(0, 4)) % (buffer->width() - length);
    y_ = (y_ + random_generator_.Rand(0, 4)) % (buffer->height() - length);
    for (int y = y_; y < y_ + length; ++y)
    {
        uint8_t *pos_y = (const_cast<uint8_t *>(buffer->dataY()) + x_ + y * buffer->strideY());
        memset(pos_y, yuv_y_, length);
    }

    for (int y = y_; y < y_ + length; y = y + 2)
    {
        uint8_t *pos_u = (const_cast<uint8_t *>(buffer->dataU()) + x_ / 2 + y / 2 * buffer->strideU());
        memset(pos_u, yuv_u_, length / 2);
        uint8_t *pos_v = (const_cast<uint8_t *>(buffer->dataV()) + x_ / 2 + y / 2 * buffer->strideV());
        memset(pos_v, yuv_v_, length / 2);
    }

    if (frame_buffer->type() == VideoFrameBuffer::Type::kI420)
    {
        detail::drawI420LocalTime(const_cast<uint8_t *>(buffer->dataY()),
                                    buffer->width(),
                                    buffer->height(),
                                    10,
                                    100,
                                    4);
        return;
    }

    // Optionally draw on alpha plane if given.
    const I420ABufferInterface *yuva_buffer = frame_buffer->getI420A();
    for (int y = y_; y < y_ + length; ++y)
    {
        uint8_t *pos_y = (const_cast<uint8_t *>(yuva_buffer->dataA()) + x_ + y * yuva_buffer->strideA());
        memset(pos_y, yuv_a_, length);
    }
}

YuvFileGenerator::YuvFileGenerator(std::vector<FILE *> files, size_t width, size_t height, int frame_repeat_count)
    : file_index_(0)
    , frame_index_(std::numeric_limits<size_t>::max())
    , files_(files)
    , width_(width)
    , height_(height)
    , frame_size_(utils::videoTypeBufferSize(VideoType::kI420, static_cast<int>(width_), static_cast<int>(height_)))
    , frame_buffer_(new uint8_t[frame_size_])
    , frame_display_count_(frame_repeat_count)
    , current_display_count_(0)
{
    OCTK_DCHECK_GT(width, 0);
    OCTK_DCHECK_GT(height, 0);
    OCTK_DCHECK_GT(frame_repeat_count, 0);
}

YuvFileGenerator::~YuvFileGenerator()
{
    for (FILE *file : files_)
    {
        fclose(file);
    }
}

FrameGeneratorInterface::VideoFrameData YuvFileGenerator::nextFrame()
{
    // Empty update by default.
    VideoFrame::UpdateRect update_rect{0, 0, 0, 0};
    if (current_display_count_ == 0)
    {
        const bool got_new_frame = readnextFrame();
        // Full update on a new frame from file.
        if (got_new_frame)
        {
            update_rect = VideoFrame::UpdateRect{0, 0, static_cast<int>(width_), static_cast<int>(height_)};
        }
    }
    if (++current_display_count_ >= frame_display_count_)
    {
        current_display_count_ = 0;
    }

    return VideoFrameData(last_read_buffer_, update_rect);
}

bool YuvFileGenerator::readnextFrame()
{
    size_t prev_frame_index = frame_index_;
    size_t prev_file_index = file_index_;
    last_read_buffer_ = utils::ReadI420Buffer(static_cast<int>(width_), static_cast<int>(height_), files_[file_index_]);
    ++frame_index_;
    if (!last_read_buffer_)
    {
        // No more frames to read in this file, rewind and move to next file.
        rewind(files_[file_index_]);

        frame_index_ = 0;
        file_index_ = (file_index_ + 1) % files_.size();
        last_read_buffer_ = utils::ReadI420Buffer(static_cast<int>(width_),
                                                  static_cast<int>(height_),
                                                  files_[file_index_]);
        OCTK_CHECK(last_read_buffer_);
    }
    return frame_index_ != prev_frame_index || file_index_ != prev_file_index;
}

FrameGeneratorInterface::Resolution YuvFileGenerator::getResolution() const
{
    return {width_, height_};
}

NV12FileGenerator::NV12FileGenerator(std::vector<FILE *> files, size_t width, size_t height, int frame_repeat_count)
    : file_index_(0)
    , frame_index_(std::numeric_limits<size_t>::max())
    , files_(files)
    , width_(width)
    , height_(height)
    , frame_size_(utils::videoTypeBufferSize(VideoType::kNV12, static_cast<int>(width_), static_cast<int>(height_)))
    , frame_buffer_(new uint8_t[frame_size_])
    , frame_display_count_(frame_repeat_count)
    , current_display_count_(0)
{
    OCTK_DCHECK_GT(width, 0);
    OCTK_DCHECK_GT(height, 0);
    OCTK_DCHECK_GT(frame_repeat_count, 0);
}

NV12FileGenerator::~NV12FileGenerator()
{
    for (FILE *file : files_)
    {
        fclose(file);
    }
}

FrameGeneratorInterface::VideoFrameData NV12FileGenerator::nextFrame()
{
    // Empty update by default.
    VideoFrame::UpdateRect update_rect{0, 0, 0, 0};
    if (current_display_count_ == 0)
    {
        const bool got_new_frame = readnextFrame();
        // Full update on a new frame from file.
        if (got_new_frame)
        {
            update_rect = VideoFrame::UpdateRect{0, 0, static_cast<int>(width_), static_cast<int>(height_)};
        }
    }
    if (++current_display_count_ >= frame_display_count_)
    {
        current_display_count_ = 0;
    }

    return VideoFrameData(last_read_buffer_, update_rect);
}

FrameGeneratorInterface::Resolution NV12FileGenerator::getResolution() const
{
    return {width_, height_};
}

bool NV12FileGenerator::readnextFrame()
{
    size_t prev_frame_index = frame_index_;
    size_t prev_file_index = file_index_;
    last_read_buffer_ = utils::ReadNV12Buffer(static_cast<int>(width_), static_cast<int>(height_), files_[file_index_]);
    ++frame_index_;
    if (!last_read_buffer_)
    {
        // No more frames to read in this file, rewind and move to next file.
        rewind(files_[file_index_]);

        frame_index_ = 0;
        file_index_ = (file_index_ + 1) % files_.size();
        last_read_buffer_ = utils::ReadNV12Buffer(static_cast<int>(width_),
                                                  static_cast<int>(height_),
                                                  files_[file_index_]);
        OCTK_CHECK(last_read_buffer_);
    }
    return frame_index_ != prev_frame_index || file_index_ != prev_file_index;
}

SlideGenerator::SlideGenerator(int width, int height, int frame_repeat_count)
    : width_(width)
    , height_(height)
    , frame_display_count_(frame_repeat_count)
    , current_display_count_(0)
    , random_generator_(1234)
{
    OCTK_DCHECK_GT(width, 0);
    OCTK_DCHECK_GT(height, 0);
    OCTK_DCHECK_GT(frame_repeat_count, 0);
}

FrameGeneratorInterface::VideoFrameData SlideGenerator::nextFrame()
{
    if (current_display_count_ == 0)
    {
        generateNewFrame();
    }
    if (++current_display_count_ >= frame_display_count_)
    {
        current_display_count_ = 0;
    }

    return VideoFrameData(buffer_, utils::nullopt);
}

FrameGeneratorInterface::Resolution SlideGenerator::getResolution() const
{
    return {static_cast<size_t>(width_), static_cast<size_t>(height_)};
}

void SlideGenerator::generateNewFrame()
{
    // The squares should have a varying order of magnitude in order
    // to simulate variation in the slides' complexity.
    const int kSquareNum = 1 << (4 + (random_generator_.Rand(0, 3) * 2));

    buffer_ = I420Buffer::create(width_, height_);
    memset(buffer_->MutableDataY(), 127, height_ * buffer_->strideY());
    memset(buffer_->MutableDataU(), 127, buffer_->chromaHeight() * buffer_->strideU());
    memset(buffer_->MutableDataV(), 127, buffer_->chromaHeight() * buffer_->strideV());

    for (int i = 0; i < kSquareNum; ++i)
    {
        int length = random_generator_.Rand(1, width_ > 4 ? width_ / 4 : 1);
        // Limit the length of later squares so that they don't overwrite the
        // previous ones too much.
        length = (length * (kSquareNum - i)) / kSquareNum;

        int x = random_generator_.Rand(0, width_ - length);
        int y = random_generator_.Rand(0, height_ - length);
        uint8_t yuv_y = random_generator_.Rand(0, 255);
        uint8_t yuv_u = random_generator_.Rand(0, 255);
        uint8_t yuv_v = random_generator_.Rand(0, 255);

        for (int yy = y; yy < y + length; ++yy)
        {
            uint8_t *pos_y = (buffer_->MutableDataY() + x + yy * buffer_->strideY());
            memset(pos_y, yuv_y, length);
        }
        for (int yy = y; yy < y + length; yy += 2)
        {
            uint8_t *pos_u = (buffer_->MutableDataU() + x / 2 + yy / 2 * buffer_->strideU());
            memset(pos_u, yuv_u, length / 2);
            uint8_t *pos_v = (buffer_->MutableDataV() + x / 2 + yy / 2 * buffer_->strideV());
            memset(pos_v, yuv_v, length / 2);
        }
    }
}

ScrollingImageFrameGenerator::ScrollingImageFrameGenerator(Clock *clock,
                                                           const std::vector<FILE *> &files,
                                                           size_t source_width,
                                                           size_t source_height,
                                                           size_t target_width,
                                                           size_t target_height,
                                                           int64_t scroll_time_ms,
                                                           int64_t pause_time_ms)
    : clock_(clock)
    , start_time_(clock->TimeInMilliseconds())
    , scroll_time_(scroll_time_ms)
    , pause_time_(pause_time_ms)
    , num_frames_(files.size())
    , target_width_(static_cast<int>(target_width))
    , target_height_(static_cast<int>(target_height))
    , current_frame_num_(num_frames_ - 1)
    , prev_frame_not_scrolled_(false)
    , current_source_frame_(nullptr, utils::nullopt)
    , current_frame_(nullptr, utils::nullopt)
    , file_generator_(files, source_width, source_height, 1)
{
    OCTK_DCHECK(clock_ != nullptr);
    OCTK_DCHECK_GT(num_frames_, 0);
    OCTK_DCHECK_GE(source_height, target_height);
    OCTK_DCHECK_GE(source_width, target_width);
    OCTK_DCHECK_GE(scroll_time_ms, 0);
    OCTK_DCHECK_GE(pause_time_ms, 0);
    OCTK_DCHECK_GT(scroll_time_ms + pause_time_ms, 0);
}

FrameGeneratorInterface::VideoFrameData ScrollingImageFrameGenerator::nextFrame()
{
    const int64_t kFrameDisplayTime = scroll_time_ + pause_time_;
    const int64_t now = clock_->TimeInMilliseconds();
    int64_t ms_since_start = now - start_time_;

    size_t frame_num = (ms_since_start / kFrameDisplayTime) % num_frames_;
    updateSourceFrame(frame_num);

    bool cur_frame_not_scrolled;

    double scroll_factor;
    int64_t time_into_frame = ms_since_start % kFrameDisplayTime;
    if (time_into_frame < scroll_time_)
    {
        scroll_factor = static_cast<double>(time_into_frame) / scroll_time_;
        cur_frame_not_scrolled = false;
    }
    else
    {
        scroll_factor = 1.0;
        cur_frame_not_scrolled = true;
    }
    cropSourceToScrolledImage(scroll_factor);

    bool same_scroll_position = prev_frame_not_scrolled_ && cur_frame_not_scrolled;
    if (!same_scroll_position)
    {
        // If scrolling is not finished yet, force full frame update.
        current_frame_.updateRect = VideoFrame::UpdateRect{0, 0, target_width_, target_height_};
    }
    prev_frame_not_scrolled_ = cur_frame_not_scrolled;

    return current_frame_;
}

FrameGeneratorInterface::Resolution ScrollingImageFrameGenerator::getResolution() const
{
    return {static_cast<size_t>(target_width_), static_cast<size_t>(target_height_)};
}

void ScrollingImageFrameGenerator::updateSourceFrame(size_t frame_num)
{
    VideoFrame::UpdateRect acc_update{0, 0, 0, 0};
    while (current_frame_num_ != frame_num)
    {
        current_source_frame_ = file_generator_.nextFrame();
        if (current_source_frame_.updateRect)
        {
            acc_update.unionRect(*current_source_frame_.updateRect);
        }
        current_frame_num_ = (current_frame_num_ + 1) % num_frames_;
    }
    current_source_frame_.updateRect = acc_update;
}

void ScrollingImageFrameGenerator::cropSourceToScrolledImage(double scroll_factor)
{
    int scroll_margin_x = current_source_frame_.buffer->width() - target_width_;
    int pixels_scrolled_x = static_cast<int>(scroll_margin_x * scroll_factor + 0.5);
    int scroll_margin_y = current_source_frame_.buffer->height() - target_height_;
    int pixels_scrolled_y = static_cast<int>(scroll_margin_y * scroll_factor + 0.5);

    std::shared_ptr<I420BufferInterface> i420_buffer = current_source_frame_.buffer->toI420();
    int offset_y = (i420_buffer->strideY() * pixels_scrolled_y) + pixels_scrolled_x;
    int offset_u = (i420_buffer->strideU() * (pixels_scrolled_y / 2)) + (pixels_scrolled_x / 2);
    int offset_v = (i420_buffer->strideV() * (pixels_scrolled_y / 2)) + (pixels_scrolled_x / 2);

    VideoFrame::UpdateRect updateRect = current_source_frame_.updateRect->isEmpty()
                                            ? VideoFrame::UpdateRect{0, 0, 0, 0}
                                            : VideoFrame::UpdateRect{0, 0, target_width_, target_height_};
    current_frame_ = VideoFrameData(utils::wrapI420Buffer(target_width_,
                                                          target_height_,
                                                          &i420_buffer->dataY()[offset_y],
                                                          i420_buffer->strideY(),
                                                          &i420_buffer->dataU()[offset_u],
                                                          i420_buffer->strideU(),
                                                          &i420_buffer->dataV()[offset_v],
                                                          i420_buffer->strideV(), // To keep reference alive.
                                                          [i420_buffer] {}),
                                    updateRect);
}
OCTK_END_NAMESPACE
