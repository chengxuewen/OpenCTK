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

#include <octk_yuv.hpp>
#include <octk_i420_buffer.hpp>
#include <octk_checks.hpp>

#include <libyuv.h>

#include <cstdint>
#include <string>

OCTK_BEGIN_NAMESPACE

namespace utils
{
    namespace detail
    {
    libyuv::FourCC videoTypeToFourCC(VideoType videoType)
    {
        switch (videoType)
        {
        case VideoType::kRGB24: return libyuv::FOURCC_24BG;
        case VideoType::kBGR24: return libyuv::FOURCC_RAW;
        case VideoType::kARGB: return libyuv::FOURCC_ARGB;
        case VideoType::kBGRA: return libyuv::FOURCC_BGRA;
        case VideoType::kABGR: return libyuv::FOURCC_ABGR;
        case VideoType::kRGBA: return libyuv::FOURCC_RGBA;
        case VideoType::kRAW: return libyuv::FOURCC_RAW;

        case VideoType::kI420: return libyuv::FOURCC_I420;
        case VideoType::kI422: return libyuv::FOURCC_I422;
        case VideoType::kI444: return libyuv::FOURCC_I444;
        case VideoType::kI400: return libyuv::FOURCC_I400;
        case VideoType::kNV21: return libyuv::FOURCC_NV21;
        case VideoType::kNV12: return libyuv::FOURCC_NV12;
        case VideoType::kYUY2: return libyuv::FOURCC_YUY2;
        case VideoType::kUYVY: return libyuv::FOURCC_UYVY;
        case VideoType::kI010: return libyuv::FOURCC_I010;
        case VideoType::kI210: return libyuv::FOURCC_I210;

        case VideoType::kMJPG: return libyuv::FOURCC_MJPG;
        default: break;
        }
        OCTK_CHECK_NOTREACHED();
        return libyuv::FOURCC_ANY;
    }
    } // namespace detail

    int ExtractBuffer(const std::shared_ptr<I420BufferInterface> &input_frame,
                      size_t size,
                      uint8_t *buffer)
    {
        OCTK_DCHECK(buffer);
        if (!input_frame)
        {
            return -1;
        }
        int width = input_frame->width();
        int height = input_frame->height();
        size_t length = videoTypeBufferSize(VideoType::kI420, width, height);
        if (size < length)
        {
            return -1;
        }

        int chroma_width = input_frame->chromaWidth();
        int chroma_height = input_frame->chromaHeight();

        libyuv::I420Copy(input_frame->dataY(),
                         input_frame->strideY(),
                         input_frame->dataU(),
                         input_frame->strideU(),
                         input_frame->dataV(),
                         input_frame->strideV(),
                         buffer,
                         width,
                         buffer + width * height,
                         chroma_width,
                         buffer + width * height + chroma_width * chroma_height,
                         chroma_width,
                         width,
                         height);

        return static_cast<int>(length);
    }

    int ExtractBuffer(const VideoFrame &input_frame, size_t size, uint8_t *buffer)
    {
        return ExtractBuffer(input_frame.videoFrameBuffer()->toI420(),
                             size,
                             buffer);
    }

    int ConvertFromI420(const VideoFrame &src_frame,
                        VideoType dst_video_type,
                        int dst_sample_size,
                        uint8_t *dst_frame)
    {
        std::shared_ptr<I420BufferInterface> i420_buffer =
                src_frame.videoFrameBuffer()->toI420();
        return libyuv::ConvertFromI420(
                    i420_buffer->dataY(),
                    i420_buffer->strideY(),
                    i420_buffer->dataU(),
                    i420_buffer->strideU(),
                    i420_buffer->dataV(),
                    i420_buffer->strideV(),
                    dst_frame,
                    dst_sample_size,
                    src_frame.width(),
                    src_frame.height(),
                    detail::videoTypeToFourCC(dst_video_type));
    }

    std::shared_ptr<I420ABufferInterface> ScaleI420ABuffer(
                const I420ABufferInterface &buffer,
                int target_width,
                int target_height)
    {
        std::shared_ptr<I420Buffer> yuv_buffer =
                I420Buffer::create(target_width, target_height);
        yuv_buffer->scaleFrom(buffer);
        std::shared_ptr<I420Buffer> axx_buffer =
                I420Buffer::create(target_width, target_height);
        libyuv::ScalePlane(buffer.dataA(),
                           buffer.strideA(),
                           buffer.width(),
                           buffer.height(),
                           axx_buffer->MutableDataY(),
                           axx_buffer->strideY(),
                           target_width,
                           target_height,
                           libyuv::kFilterBox);
        std::shared_ptr<I420ABufferInterface> merged_buffer = utils::wrapI420ABuffer(
                    yuv_buffer->width(),
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
                    [yuv_buffer, axx_buffer]
        {
        });
        return merged_buffer;
    }

    std::shared_ptr<I420BufferInterface> ScaleVideoFrameBuffer(
                const I420BufferInterface &source,
                int dst_width,
                int dst_height)
    {
        std::shared_ptr<I420Buffer> scaled_buffer =
                I420Buffer::create(dst_width, dst_height);
        scaled_buffer->scaleFrom(source);
        return scaled_buffer;
    }

    double I420SSE(const I420BufferInterface &ref_buffer,
                   const I420BufferInterface &test_buffer)
    {
        OCTK_DCHECK_EQ(ref_buffer.width(), test_buffer.width());
        OCTK_DCHECK_EQ(ref_buffer.height(), test_buffer.height());
        const uint64_t width = test_buffer.width();
        const uint64_t height = test_buffer.height();
        const uint64_t sse_y = libyuv::ComputeSumSquareErrorPlane(
                    ref_buffer.dataY(),
                    ref_buffer.strideY(),
                    test_buffer.dataY(),
                    test_buffer.strideY(),
                    width,
                    height);
        const int width_uv = (width + 1) >> 1;
        const int height_uv = (height + 1) >> 1;
        const uint64_t sse_u = libyuv::ComputeSumSquareErrorPlane(
                    ref_buffer.dataU(),
                    ref_buffer.strideU(),
                    test_buffer.dataU(),
                    test_buffer.strideU(),
                    width_uv,
                    height_uv);
        const uint64_t sse_v = libyuv::ComputeSumSquareErrorPlane(
                    ref_buffer.dataV(),
                    ref_buffer.strideV(),
                    test_buffer.dataV(),
                    test_buffer.strideV(),
                    width_uv,
                    height_uv);
        const double samples = width * height + 2 * (width_uv * height_uv);
        const double sse = sse_y + sse_u + sse_v;
        return sse / (samples * 255.0 * 255.0);
    }

    // Compute PSNR for an I420A frame (all planes). Can upscale test frame.
    double I420APSNR(const I420ABufferInterface &ref_buffer,
                     const I420ABufferInterface &test_buffer)
    {
        OCTK_DCHECK_GE(ref_buffer.width(), test_buffer.width());
        OCTK_DCHECK_GE(ref_buffer.height(), test_buffer.height());
        if ((ref_buffer.width() != test_buffer.width()) ||
                (ref_buffer.height() != test_buffer.height()))
        {
            std::shared_ptr<I420ABufferInterface> scaled_buffer =
                    ScaleI420ABuffer(test_buffer, ref_buffer.width(), ref_buffer.height());
            return I420APSNR(ref_buffer, *scaled_buffer);
        }
        const int width = test_buffer.width();
        const int height = test_buffer.height();
        const uint64_t sse_y = libyuv::ComputeSumSquareErrorPlane(
                    ref_buffer.dataY(),
                    ref_buffer.strideY(),
                    test_buffer.dataY(),
                    test_buffer.strideY(),
                    width,
                    height);
        const int width_uv = (width + 1) >> 1;
        const int height_uv = (height + 1) >> 1;
        const uint64_t sse_u = libyuv::ComputeSumSquareErrorPlane(
                    ref_buffer.dataU(),
                    ref_buffer.strideU(),
                    test_buffer.dataU(),
                    test_buffer.strideU(),
                    width_uv,
                    height_uv);
        const uint64_t sse_v = libyuv::ComputeSumSquareErrorPlane(
                    ref_buffer.dataV(),
                    ref_buffer.strideV(),
                    test_buffer.dataV(),
                    test_buffer.strideV(),
                    width_uv,
                    height_uv);
        const uint64_t sse_a = libyuv::ComputeSumSquareErrorPlane(
                    ref_buffer.dataA(),
                    ref_buffer.strideA(),
                    test_buffer.dataA(),
                    test_buffer.strideA(),
                    width,
                    height);
        const uint64_t samples = 2 * (uint64_t)width * (uint64_t)height +
                2 * ((uint64_t)width_uv * (uint64_t)height_uv);
        const uint64_t sse = sse_y + sse_u + sse_v + sse_a;
        const double psnr = libyuv::SumSquareErrorToPsnr(sse, samples);
        return (psnr > kPerfectPSNR) ? kPerfectPSNR : psnr;
    }

    // Compute PSNR for an I420A frame (all planes)
    double I420APSNR(const VideoFrame *ref_frame, const VideoFrame *test_frame)
    {
        if (!ref_frame || !test_frame)
        {
            return -1;
        }
        OCTK_DCHECK(ref_frame->videoFrameBuffer()->type() ==
                    VideoFrameBuffer::Type::kI420A);
        OCTK_DCHECK(test_frame->videoFrameBuffer()->type() ==
                    VideoFrameBuffer::Type::kI420A);
        return I420APSNR(*ref_frame->videoFrameBuffer()->getI420A(),
                         *test_frame->videoFrameBuffer()->getI420A());
    }

    // Compute PSNR for an I420 frame (all planes). Can upscale test frame.
    double I420PSNR(const I420BufferInterface &ref_buffer,
                    const I420BufferInterface &test_buffer)
    {
        OCTK_DCHECK_GE(ref_buffer.width(), test_buffer.width());
        OCTK_DCHECK_GE(ref_buffer.height(), test_buffer.height());
        if ((ref_buffer.width() != test_buffer.width()) ||
                (ref_buffer.height() != test_buffer.height()))
        {
            std::shared_ptr<I420Buffer> scaled_buffer =
                    I420Buffer::create(ref_buffer.width(), ref_buffer.height());
            scaled_buffer->scaleFrom(test_buffer);
            return I420PSNR(ref_buffer, *scaled_buffer);
        }
        double psnr = libyuv::I420Psnr(
                    ref_buffer.dataY(),
                    ref_buffer.strideY(),
                    ref_buffer.dataU(),
                    ref_buffer.strideU(),
                    ref_buffer.dataV(),
                    ref_buffer.strideV(),
                    test_buffer.dataY(),
                    test_buffer.strideY(),
                    test_buffer.dataU(),
                    test_buffer.strideU(),
                    test_buffer.dataV(),
                    test_buffer.strideV(),
                    test_buffer.width(),
                    test_buffer.height());
        // LibYuv sets the max psnr value to 128, we restrict it here.
        // In case of 0 mse in one frame, 128 can skew the results significantly.
        return (psnr > kPerfectPSNR) ? kPerfectPSNR : psnr;
    }

    // Compute PSNR for an I420 frame (all planes)
    double I420PSNR(const VideoFrame *ref_frame, const VideoFrame *test_frame)
    {
        if (!ref_frame || !test_frame)
        {
            return -1;
        }
        return I420PSNR(*ref_frame->videoFrameBuffer()->toI420(),
                        *test_frame->videoFrameBuffer()->toI420());
    }

    double I420WeightedPSNR(const I420BufferInterface &ref_buffer,
                            const I420BufferInterface &test_buffer)
    {
        OCTK_DCHECK_GE(ref_buffer.width(), test_buffer.width());
        OCTK_DCHECK_GE(ref_buffer.height(), test_buffer.height());
        if ((ref_buffer.width() != test_buffer.width()) ||
                (ref_buffer.height() != test_buffer.height()))
        {
            std::shared_ptr<I420Buffer> scaled_ref_buffer =
                    I420Buffer::create(test_buffer.width(), test_buffer.height());
            scaled_ref_buffer->scaleFrom(ref_buffer);
            return I420WeightedPSNR(*scaled_ref_buffer, test_buffer);
        }

        // Luma.
        int width_y = test_buffer.width();
        int height_y = test_buffer.height();
        uint64_t sse_y = libyuv::ComputeSumSquareErrorPlane(
                    ref_buffer.dataY(),
                    ref_buffer.strideY(),
                    test_buffer.dataY(),
                    test_buffer.strideY(),
                    width_y,
                    height_y);
        uint64_t num_samples_y = (uint64_t)width_y * (uint64_t)height_y;
        double psnr_y = libyuv::SumSquareErrorToPsnr(sse_y, num_samples_y);

        // Chroma.
        int width_uv = (width_y + 1) >> 1;
        int height_uv = (height_y + 1) >> 1;
        uint64_t sse_u = libyuv::ComputeSumSquareErrorPlane(
                    ref_buffer.dataU(),
                    ref_buffer.strideU(),
                    test_buffer.dataU(),
                    test_buffer.strideU(),
                    width_uv,
                    height_uv);
        uint64_t num_samples_uv = (uint64_t)width_uv * (uint64_t)height_uv;
        double psnr_u = libyuv::SumSquareErrorToPsnr(sse_u, num_samples_uv);
        uint64_t sse_v = libyuv::ComputeSumSquareErrorPlane(
                    ref_buffer.dataV(),
                    ref_buffer.strideV(),
                    test_buffer.dataV(),
                    test_buffer.strideV(),
                    width_uv,
                    height_uv);
        double psnr_v = libyuv::SumSquareErrorToPsnr(sse_v, num_samples_uv);

        // Weights from Ohm et. al 2012.
        double psnr_yuv = (6.0 * psnr_y + psnr_u + psnr_v) / 8.0;
        return (psnr_yuv > kPerfectPSNR) ? kPerfectPSNR : psnr_yuv;
    }

    // Compute SSIM for an I420A frame (all planes). Can upscale test frame.
    double I420ASSIM(const I420ABufferInterface &ref_buffer,
                     const I420ABufferInterface &test_buffer)
    {
        OCTK_DCHECK_GE(ref_buffer.width(), test_buffer.width());
        OCTK_DCHECK_GE(ref_buffer.height(), test_buffer.height());
        if ((ref_buffer.width() != test_buffer.width()) ||
                (ref_buffer.height() != test_buffer.height()))
        {
            std::shared_ptr<I420ABufferInterface> scaled_buffer =
                    ScaleI420ABuffer(test_buffer, ref_buffer.width(), ref_buffer.height());
            return I420ASSIM(ref_buffer, *scaled_buffer);
        }
        const double yuv_ssim = libyuv::I420Ssim(
                    ref_buffer.dataY(),
                    ref_buffer.strideY(),
                    ref_buffer.dataU(),
                    ref_buffer.strideU(),
                    ref_buffer.dataV(),
                    ref_buffer.strideV(),
                    test_buffer.dataY(),
                    test_buffer.strideY(),
                    test_buffer.dataU(),
                    test_buffer.strideU(),
                    test_buffer.dataV(),
                    test_buffer.strideV(),
                    test_buffer.width(),
                    test_buffer.height());
        const double a_ssim = libyuv::CalcFrameSsim(
                    ref_buffer.dataA(),
                    ref_buffer.strideA(),
                    test_buffer.dataA(),
                    test_buffer.strideA(),
                    test_buffer.width(),
                    test_buffer.height());
        return (yuv_ssim + (a_ssim * 0.8)) / 1.8;
    }

    // Compute SSIM for an I420A frame (all planes)
    double I420ASSIM(const VideoFrame *ref_frame, const VideoFrame *test_frame)
    {
        if (!ref_frame || !test_frame)
        {
            return -1;
        }
        OCTK_DCHECK(ref_frame->videoFrameBuffer()->type() ==
                    VideoFrameBuffer::Type::kI420A);
        OCTK_DCHECK(test_frame->videoFrameBuffer()->type() ==
                    VideoFrameBuffer::Type::kI420A);
        return I420ASSIM(*ref_frame->videoFrameBuffer()->getI420A(),
                         *test_frame->videoFrameBuffer()->getI420A());
    }

    // Compute SSIM for an I420 frame (all planes). Can upscale test_buffer.
    double I420SSIM(const I420BufferInterface &ref_buffer,
                    const I420BufferInterface &test_buffer)
    {
        OCTK_DCHECK_GE(ref_buffer.width(), test_buffer.width());
        OCTK_DCHECK_GE(ref_buffer.height(), test_buffer.height());
        if ((ref_buffer.width() != test_buffer.width()) ||
                (ref_buffer.height() != test_buffer.height()))
        {
            std::shared_ptr<I420Buffer> scaled_buffer =
                    I420Buffer::create(ref_buffer.width(), ref_buffer.height());
            scaled_buffer->scaleFrom(test_buffer);
            return I420SSIM(ref_buffer, *scaled_buffer);
        }
        return libyuv::I420Ssim(ref_buffer.dataY(),
                                ref_buffer.strideY(),
                                ref_buffer.dataU(),
                                ref_buffer.strideU(),
                                ref_buffer.dataV(),
                                ref_buffer.strideV(),
                                test_buffer.dataY(),
                                test_buffer.strideY(),
                                test_buffer.dataU(),
                                test_buffer.strideU(),
                                test_buffer.dataV(),
                                test_buffer.strideV(),
                                test_buffer.width(),
                                test_buffer.height());
    }

    double I420SSIM(const VideoFrame *ref_frame, const VideoFrame *test_frame)
    {
        if (!ref_frame || !test_frame)
        {
            return -1;
        }
        return I420SSIM(*ref_frame->videoFrameBuffer()->toI420(),
                        *test_frame->videoFrameBuffer()->toI420());
    }

    void NV12Scale(uint8_t *tmp_buffer,
                   const uint8_t *src_y,
                   int src_stride_y,
                   const uint8_t *src_uv,
                   int src_stride_uv,
                   int src_width,
                   int src_height,
                   uint8_t *dst_y,
                   int dst_stride_y,
                   uint8_t *dst_uv,
                   int dst_stride_uv,
                   int dst_width,
                   int dst_height)
    {
        const int src_chroma_width = (src_width + 1) / 2;
        const int src_chroma_height = (src_height + 1) / 2;

        if (src_width == dst_width && src_height == dst_height)
        {
            // No scaling.
            libyuv::CopyPlane(src_y,
                              src_stride_y,
                              dst_y,
                              dst_stride_y,
                              src_width,
                              src_height);
            libyuv::CopyPlane(src_uv,
                              src_stride_uv,
                              dst_uv,
                              dst_stride_uv,
                              src_chroma_width * 2,
                              src_chroma_height);
            return;
        }

        // Scaling.
        // Allocate temporary memory for spitting UV planes and scaling them.
        const int dst_chroma_width = (dst_width + 1) / 2;
        const int dst_chroma_height = (dst_height + 1) / 2;

        uint8_t *const src_u = tmp_buffer;
        uint8_t *const src_v = src_u + src_chroma_width * src_chroma_height;
        uint8_t *const dst_u = src_v + src_chroma_width * src_chroma_height;
        uint8_t *const dst_v = dst_u + dst_chroma_width * dst_chroma_height;

        // Split source UV plane into separate U and V plane using the temporary data.
        libyuv::SplitUVPlane(src_uv,
                             src_stride_uv,
                             src_u,
                             src_chroma_width,
                             src_v,
                             src_chroma_width,
                             src_chroma_width,
                             src_chroma_height);

        // Scale the planes.
        libyuv::I420Scale(
                    src_y,
                    src_stride_y,
                    src_u,
                    src_chroma_width,
                    src_v,
                    src_chroma_width,
                    src_width,
                    src_height,
                    dst_y,
                    dst_stride_y,
                    dst_u,
                    dst_chroma_width,
                    dst_v,
                    dst_chroma_width,
                    dst_width,
                    dst_height,
                    libyuv::kFilterBox);

        // Merge the UV planes into the destination.
        libyuv::MergeUVPlane(dst_u,
                             dst_chroma_width,
                             dst_v,
                             dst_chroma_width,
                             dst_uv,
                             dst_stride_uv,
                             dst_chroma_width,
                             dst_chroma_height);
    }


    NV12ToI420Scaler::NV12ToI420Scaler() = default;
    NV12ToI420Scaler::~NV12ToI420Scaler() = default;

    void NV12ToI420Scaler::NV12ToI420Scale(const uint8_t *src_y,
                                           int src_stride_y,
                                           const uint8_t *src_uv,
                                           int src_stride_uv,
                                           int src_width,
                                           int src_height,
                                           uint8_t *dst_y,
                                           int dst_stride_y,
                                           uint8_t *dst_u,
                                           int dst_stride_u,
                                           uint8_t *dst_v,
                                           int dst_stride_v,
                                           int dst_width,
                                           int dst_height)
    {
        if (src_width == dst_width && src_height == dst_height)
        {
            // No scaling.
            tmp_uv_planes_.clear();
            tmp_uv_planes_.shrink_to_fit();
            libyuv::NV12ToI420(src_y,
                               src_stride_y,
                               src_uv,
                               src_stride_uv,
                               dst_y,
                               dst_stride_y,
                               dst_u,
                               dst_stride_u,
                               dst_v,
                               dst_stride_v,
                               src_width,
                               src_height);
            return;
        }

        // Scaling.
        // Allocate temporary memory for spitting UV planes.
        const int src_uv_width = (src_width + 1) / 2;
        const int src_uv_height = (src_height + 1) / 2;
        tmp_uv_planes_.resize(src_uv_width * src_uv_height * 2);
        tmp_uv_planes_.shrink_to_fit();

        // Split source UV plane into separate U and V plane using the temporary data.
        uint8_t *const src_u = tmp_uv_planes_.data();
        uint8_t *const src_v = tmp_uv_planes_.data() + src_uv_width * src_uv_height;
        libyuv::SplitUVPlane(src_uv,
                             src_stride_uv,
                             src_u,
                             src_uv_width,
                             src_v,
                             src_uv_width,
                             src_uv_width,
                             src_uv_height);

        // Scale the planes into the destination.
        libyuv::I420Scale(src_y,
                          src_stride_y,
                          src_u,
                          src_uv_width,
                          src_v,
                          src_uv_width,
                          src_width,
                          src_height,
                          dst_y,
                          dst_stride_y,
                          dst_u,
                          dst_stride_u,
                          dst_v,
                          dst_stride_v,
                          dst_width,
                          dst_height,
                          libyuv::kFilterBox);
    }

#define OCTK_I420_Y_PTR(buffer, width, height)        (buffer)
#define OCTK_I420_U_PTR(buffer, width, height)        (buffer + width * height)
#define OCTK_I420_V_PTR(buffer, width, height)        (buffer + width * height + (width >> 1) * (height >> 1))
#define OCTK_I420_Y_STRIDE(width)                     (width)
#define OCTK_I420_U_STRIDE(width)                     (width >> 1)
#define OCTK_I420_V_STRIDE(width)                     (width >> 1)
#define OCTK_I420_Y_OFFSET_PTR(buffer, width, height, xOffset, yOffset) \
    (OCTK_I420_Y_PTR(buffer, width, height) + OCTK_I420_Y_STRIDE(width) * yOffset + xOffset)
#define OCTK_I420_U_OFFSET_PTR(buffer, width, height, xOffset, yOffset) \
    (OCTK_I420_U_PTR(buffer, width, height) + OCTK_I420_U_STRIDE(width) * (yOffset / 2) + (xOffset / 2))
#define OCTK_I420_V_OFFSET_PTR(buffer, width, height, xOffset, yOffset) \
    (OCTK_I420_V_PTR(buffer, width, height) + OCTK_I420_V_STRIDE(width) * (yOffset / 2) + (xOffset / 2))

#define OCTK_NV12_Y_PTR(buffer, width, height)          (buffer)
#define OCTK_NV12_UV_PTR(buffer, width, height)         (buffer + width * height)
#define OCTK_NV12_Y_STRIDE(width)                       (width)
#define OCTK_NV12_UV_STRIDE(width)                      (width)
#define OCTK_NV12_Y_OFFSET_PTR(buffer, width, height, xOffset, yOffset) \
    (OCTK_NV12_Y_PTR(buffer, width, height) + width * yOffset + xOffset)
#define OCTK_NV12_UV_OFFSET_PTR(buffer, width, height, xOffset, yOffset) \
    (OCTK_NV12_UV_PTR(buffer, width, height) + width * (yOffset / 2) + xOffset)

#define OCTK_NV21_Y_PTR(buffer, width, height)         (buffer)
#define OCTK_NV21_VU_PTR(buffer, width, height)        (buffer + width * height)
#define OCTK_NV21_Y_STRIDE(width)                      (width)
#define OCTK_NV21_VU_STRIDE(width)                     (width)

#define OCTK_ARGB_STRIDE(width)                         (width * 4)
#define OCTK_ARGB_OFFSET_PTR(buffer, width, xOffset, yOffset) \
    (buffer + OCTK_ARGB_STRIDE(width) * yOffset + OCTK_ARGB_STRIDE(xOffset))

    namespace yuv
    {
    void scaleI420(const uint8_t *srcBuffer,
                   int srcWidth,
                   int srcHeight,
                   uint8_t *dstBuffer,
                   int dstWidth,
                   int dstHeight,
                   bool highestQuality)
    {
        libyuv::I420Scale(OCTK_I420_Y_PTR(srcBuffer, srcWidth, srcHeight),
                          OCTK_I420_Y_STRIDE(srcWidth),
                          OCTK_I420_U_PTR(srcBuffer, srcWidth, srcHeight),
                          OCTK_I420_U_STRIDE(srcWidth),
                          OCTK_I420_V_PTR(srcBuffer, srcWidth, srcHeight),
                          OCTK_I420_V_STRIDE(srcWidth),
                          srcWidth,
                          srcHeight,
                          OCTK_I420_Y_PTR(dstBuffer, dstWidth, dstHeight),
                          OCTK_I420_Y_STRIDE(dstWidth),
                          OCTK_I420_U_PTR(dstBuffer, dstWidth, dstHeight),
                          OCTK_I420_U_STRIDE(dstWidth),
                          OCTK_I420_V_PTR(dstBuffer, dstWidth, dstHeight),
                          OCTK_I420_V_STRIDE(dstWidth),
                          dstWidth,
                          dstHeight,
                          highestQuality ? libyuv::kFilterBox : libyuv::kFilterBilinear);
    }

    void scaleARGB(const uint8_t *srcBuffer,
                   int srcWidth,
                   int srcHeight,
                   uint8_t *dstBuffer,
                   int dstWidth,
                   int dstHeight,
                   bool highestQuality)
    {
        libyuv::ARGBScale(srcBuffer,
                          OCTK_ARGB_STRIDE(srcWidth),
                          srcWidth,
                          srcHeight,
                          dstBuffer,
                          OCTK_ARGB_STRIDE(dstWidth),
                          dstWidth,
                          dstHeight,
                          highestQuality ? libyuv::kFilterBox : libyuv::kFilterBilinear);
    }

    void scaleNV12(const uint8_t *srcBuffer,
                   int srcWidth,
                   int srcHeight,
                   uint8_t *dstBuffer,
                   int dstWidth,
                   int dstHeight,
                   bool highestQuality)
    {
        libyuv::NV12Scale(OCTK_NV12_Y_PTR(srcBuffer, srcWidth, srcHeight),
                          OCTK_NV12_Y_STRIDE(srcWidth),
                          OCTK_NV12_UV_PTR(srcBuffer, srcWidth, srcHeight),
                          OCTK_NV12_UV_STRIDE(srcWidth),
                          srcWidth,
                          srcHeight,
                          OCTK_NV12_Y_PTR(dstBuffer, dstWidth, dstHeight),
                          OCTK_NV12_Y_STRIDE(dstWidth),
                          OCTK_NV12_UV_PTR(dstBuffer, dstWidth, dstHeight),
                          OCTK_NV12_UV_STRIDE(dstWidth),
                          dstWidth,
                          dstHeight,
                          highestQuality ? libyuv::kFilterBox : libyuv::kFilterBilinear);
    }

    void copyI420(const uint8_t *srcDataY,
                  int srcStrideY,
                  const uint8_t *srcDataU,
                  int srcStrideU,
                  const uint8_t *srcDataV,
                  int srcStrideV,
                  uint8_t *dstDataY,
                  int dstStrideY,
                  uint8_t *dstDataU,
                  int dstStrideU,
                  uint8_t *dstDataV,
                  int dstStrideV,
                  int width,
                  int height)
    {
        libyuv::I420Copy(srcDataY,
                         srcStrideY,
                         srcDataU,
                         srcStrideU,
                         srcDataV,
                         srcStrideV,
                         dstDataY,
                         dstStrideY,
                         dstDataU,
                         dstStrideU,
                         dstDataV,
                         dstStrideV,
                         width,
                         height);
    }

    void copyCenterInI420(const uint8_t *srcBuffer,
                          int srcWidth,
                          int srcHeight,
                          uint8_t *dstBuffer,
                          int dstWidth,
                          int dstHeight)
    {
        libyuv::I420Rect(OCTK_I420_Y_PTR(dstBuffer, dstWidth, dstHeight),
                         OCTK_I420_Y_STRIDE(dstWidth),
                         OCTK_I420_U_PTR(dstBuffer, dstWidth, dstHeight),
                         OCTK_I420_U_STRIDE(dstWidth),
                         OCTK_I420_V_PTR(dstBuffer, dstWidth, dstHeight),
                         OCTK_I420_V_STRIDE(dstWidth),
                         0,
                         0,
                         dstWidth,
                         dstHeight,
                         0,
                         128,
                         128);

        const int fixWidth = std::min(srcWidth, dstWidth);
        const int fixHeight = std::min(srcHeight, dstHeight);
        const int xOffset = srcWidth > dstWidth ? (srcWidth - dstWidth) / 2 : 0;
        const int yOffset = srcHeight > dstHeight ? (srcHeight - dstHeight) / 2 : 0;
        const int dstXOffset = srcWidth < dstWidth ? (dstWidth - srcWidth) / 2 : 0;
        const int dstYOffset = srcHeight < dstHeight ? (dstHeight - srcHeight) / 2 : 0;
        libyuv::I420Copy(OCTK_I420_Y_OFFSET_PTR(srcBuffer, srcWidth, srcHeight, xOffset, yOffset),
                         OCTK_I420_Y_STRIDE(srcWidth),
                         OCTK_I420_U_OFFSET_PTR(srcBuffer, srcWidth, srcHeight, xOffset, yOffset),
                         OCTK_I420_U_STRIDE(srcWidth),
                         OCTK_I420_V_OFFSET_PTR(srcBuffer, srcWidth, srcHeight, xOffset, yOffset),
                         OCTK_I420_V_STRIDE(srcWidth),
                         OCTK_I420_Y_OFFSET_PTR(dstBuffer, dstWidth, dstHeight, dstXOffset, dstYOffset),
                         OCTK_I420_Y_STRIDE(dstWidth),
                         OCTK_I420_U_OFFSET_PTR(dstBuffer, dstWidth, dstHeight, dstXOffset, dstYOffset),
                         OCTK_I420_U_STRIDE(dstWidth),
                         OCTK_I420_V_OFFSET_PTR(dstBuffer, dstWidth, dstHeight, dstXOffset, dstYOffset),
                         OCTK_I420_V_STRIDE(dstWidth),
                         fixWidth,
                         fixHeight);
    }

    void copyCenterInNV12(const uint8_t *srcBuffer,
                          int srcWidth,
                          int srcHeight,
                          uint8_t *dstBuffer,
                          int dstWidth,
                          int dstHeight)
    {
        libyuv::SetPlane(OCTK_NV12_Y_PTR(dstBuffer, dstWidth, dstHeight),
                         OCTK_NV12_Y_STRIDE(dstWidth),
                         dstWidth,
                         dstHeight,
                         16);
        libyuv::SetPlane(OCTK_NV12_UV_PTR(dstBuffer, dstWidth, dstHeight),
                         OCTK_NV12_UV_STRIDE(dstWidth),
                         dstWidth,
                         dstHeight / 2,
                         128);

        const int fixWidth = std::min(srcWidth, dstWidth);
        const int fixHeight = std::min(srcHeight, dstHeight);
        const int xOffset = srcWidth > dstWidth ? (srcWidth - dstWidth) / 2 : 0;
        const int yOffset = srcHeight > dstHeight ? (srcHeight - dstHeight) / 2 : 0;
        const int dstXOffset = srcWidth < dstWidth ? (dstWidth - srcWidth) / 2 : 0;
        const int dstYOffset = srcHeight < dstHeight ? (dstHeight - srcHeight) / 2 : 0;
        libyuv::NV12Copy(OCTK_NV12_Y_OFFSET_PTR(srcBuffer, srcWidth, srcHeight, xOffset, yOffset),
                         OCTK_NV12_Y_STRIDE(srcWidth),
                         OCTK_NV12_UV_OFFSET_PTR(srcBuffer, srcWidth, srcHeight, xOffset, yOffset),
                         OCTK_NV12_UV_STRIDE(srcWidth),
                         OCTK_NV12_Y_OFFSET_PTR(dstBuffer, dstWidth, dstHeight, dstXOffset, dstYOffset),
                         OCTK_NV12_Y_STRIDE(dstWidth),
                         OCTK_NV12_UV_OFFSET_PTR(dstBuffer, dstWidth, dstHeight, dstXOffset, dstYOffset),
                         OCTK_NV12_UV_STRIDE(dstWidth),
                         fixWidth,
                         fixHeight);
    }

    void copyCenterInARGB(const uint8_t *srcBuffer,
                          int srcWidth,
                          int srcHeight,
                          uint8_t *dstBuffer,
                          int dstWidth,
                          int dstHeight)
    {
        libyuv::ARGBRect(dstBuffer, OCTK_ARGB_STRIDE(dstWidth), 0, 0, dstWidth, dstHeight, 0);

        const int fixWidth = std::min(srcWidth, dstWidth);
        const int fixHeight = std::min(srcHeight, dstHeight);
        const int xOffset = srcWidth > dstWidth ? (srcWidth - dstWidth) / 2 : 0;
        const int yOffset = srcHeight > dstHeight ? (srcHeight - dstHeight) / 2 : 0;
        const int dstXOffset = srcWidth < dstWidth ? (dstWidth - srcWidth) / 2 : 0;
        const int dstYOffset = srcHeight < dstHeight ? (dstHeight - srcHeight) / 2 : 0;
        libyuv::ARGBCopy(OCTK_ARGB_OFFSET_PTR(srcBuffer, srcWidth, xOffset, yOffset),
                         OCTK_ARGB_STRIDE(srcWidth),
                         OCTK_ARGB_OFFSET_PTR(dstBuffer, dstWidth, dstXOffset, dstYOffset),
                         OCTK_ARGB_STRIDE(dstWidth),
                         fixWidth,
                         fixHeight);
    }

    bool convertToI420(const uint8_t *sample,
                       size_t sampleSize,
                       uint8_t *dstY,
                       int dstStrideY,
                       uint8_t *dstU,
                       int dstStrideU,
                       uint8_t *dstV,
                       int dstStrideV,
                       int cropX,
                       int cropY,
                       int srcWidth,
                       int srcHeight,
                       int cropWidth,
                       int cropHeight,
                       VideoRotation rotation,
                       VideoType videoType)
    {
        return 0 == libyuv::ConvertToI420(sample,
                                          sampleSize,
                                          dstY,
                                          dstStrideY,
                                          dstU,
                                          dstStrideU,
                                          dstV,
                                          dstStrideV,
                                          cropX,
                                          cropY,
                                          srcWidth,
                                          srcHeight,
                                          cropWidth,
                                          cropHeight,
                                          static_cast<libyuv::RotationMode>(rotation),
                                          detail::videoTypeToFourCC(videoType));
    }

    void convertI420ToARGB(const uint8_t *srcBuffer,
                           uint8_t *dstBuffer,
                           int width,
                           int height)
    {
        libyuv::I420ToARGB(OCTK_I420_Y_PTR(srcBuffer, width, height),
                           OCTK_I420_Y_STRIDE(width),
                           OCTK_I420_U_PTR(srcBuffer, width, height),
                           OCTK_I420_U_STRIDE(width),
                           OCTK_I420_V_PTR(srcBuffer, width, height),
                           OCTK_I420_V_STRIDE(width),
                           dstBuffer,
                           OCTK_ARGB_STRIDE(width),
                           width,
                           height);
    }

    void convertI420ToABGR(const uint8_t *srcBuffer,
                           uint8_t *dstBuffer,
                           int width,
                           int height)
    {
        libyuv::I420ToABGR(OCTK_I420_Y_PTR(srcBuffer, width, height),
                           OCTK_I420_Y_STRIDE(width),
                           OCTK_I420_U_PTR(srcBuffer, width, height),
                           OCTK_I420_U_STRIDE(width),
                           OCTK_I420_V_PTR(srcBuffer, width, height),
                           OCTK_I420_V_STRIDE(width),
                           dstBuffer,
                           OCTK_ARGB_STRIDE(width),
                           width,
                           height);
    }

    void convertI420ToBGRA(const uint8_t *srcBuffer,
                           uint8_t *dstBuffer,
                           int width,
                           int height)
    {
        libyuv::I420ToBGRA(OCTK_I420_Y_PTR(srcBuffer, width, height),
                           OCTK_I420_Y_STRIDE(width),
                           OCTK_I420_U_PTR(srcBuffer, width, height),
                           OCTK_I420_U_STRIDE(width),
                           OCTK_I420_V_PTR(srcBuffer, width, height),
                           OCTK_I420_V_STRIDE(width),
                           dstBuffer,
                           OCTK_ARGB_STRIDE(width),
                           width,
                           height);
    }

    void convertI420ToRGBA(const uint8_t *srcBuffer,
                           uint8_t *dstBuffer,
                           int width,
                           int height)
    {
        libyuv::I420ToRGBA(OCTK_I420_Y_PTR(srcBuffer, width, height),
                           OCTK_I420_Y_STRIDE(width),
                           OCTK_I420_U_PTR(srcBuffer, width, height),
                           OCTK_I420_U_STRIDE(width),
                           OCTK_I420_V_PTR(srcBuffer, width, height),
                           OCTK_I420_V_STRIDE(width),
                           dstBuffer,
                           OCTK_ARGB_STRIDE(width),
                           width,
                           height);
    }

    void convertI420ToRGB24(const uint8_t *srcBuffer,
                            uint8_t *dstBuffer,
                            int width,
                            int height)
    {
        libyuv::I420ToRGB24(OCTK_I420_Y_PTR(srcBuffer, width, height),
                            OCTK_I420_Y_STRIDE(width),
                            OCTK_I420_U_PTR(srcBuffer, width, height),
                            OCTK_I420_U_STRIDE(width),
                            OCTK_I420_V_PTR(srcBuffer, width, height),
                            OCTK_I420_V_STRIDE(width),
                            dstBuffer,
                            width * 3,
                            width,
                            height);
    }

    void convertI420ToNV12(const uint8_t *srcBuffer,
                           uint8_t *dstBuffer,
                           int width,
                           int height)
    {
        libyuv::I420ToNV12(OCTK_I420_Y_PTR(srcBuffer, width, height),
                           OCTK_I420_Y_STRIDE(width),
                           OCTK_I420_U_PTR(srcBuffer, width, height),
                           OCTK_I420_U_STRIDE(width),
                           OCTK_I420_V_PTR(srcBuffer, width, height),
                           OCTK_I420_V_STRIDE(width),
                           OCTK_NV12_Y_PTR(dstBuffer, width, height),
                           OCTK_NV12_Y_STRIDE(width),
                           OCTK_NV12_UV_PTR(dstBuffer, width, height),
                           OCTK_NV12_UV_STRIDE(width),
                           width,
                           height);
    }

    void convertBGRAToARGB(const uint8_t *srcBuffer,
                           uint8_t *dstBuffer,
                           int width,
                           int height)
    {
        libyuv::BGRAToARGB(srcBuffer,
                           width * 4,
                           dstBuffer,
                           width * 4,
                           width,
                           height);
    }

    void convertABGRToARGB(const uint8_t *srcBuffer,
                           uint8_t *dstBuffer,
                           int width,
                           int height)
    {
        libyuv::ABGRToARGB(srcBuffer,
                           width * 4,
                           dstBuffer,
                           width * 4,
                           width,
                           height);
    }

    void convertRGBAToARGB(const uint8_t *srcBuffer,
                           uint8_t *dstBuffer,
                           int width,
                           int height)
    {
        libyuv::RGBAToARGB(srcBuffer,
                           width * 4,
                           dstBuffer,
                           width * 4,
                           width,
                           height);
    }

    void convertI420ToNV21(const uint8_t *srcBuffer,
                           uint8_t *dstBuffer,
                           int width,
                           int height)
    {
        libyuv::I420ToNV21(OCTK_I420_Y_PTR(srcBuffer, width, height),
                           OCTK_I420_Y_STRIDE(width),
                           OCTK_I420_U_PTR(srcBuffer, width, height),
                           OCTK_I420_U_STRIDE(width),
                           OCTK_I420_V_PTR(srcBuffer, width, height),
                           OCTK_I420_V_STRIDE(width),
                           OCTK_NV21_Y_PTR(dstBuffer, width, height),
                           OCTK_NV21_Y_STRIDE(width),
                           OCTK_NV21_VU_PTR(dstBuffer, width, height),
                           OCTK_NV21_VU_STRIDE(width),
                           width,
                           height);
    }

    void convertARGBToI420(const uint8_t *srcBuffer,
                           uint8_t *dstBuffer,
                           int width,
                           int height)
    {
        libyuv::ARGBToI420(srcBuffer,
                           width * 4,
                           OCTK_I420_Y_PTR(dstBuffer, width, height),
                           OCTK_I420_Y_STRIDE(width),
                           OCTK_I420_U_PTR(dstBuffer, width, height),
                           OCTK_I420_U_STRIDE(width),
                           OCTK_I420_V_PTR(dstBuffer, width, height),
                           OCTK_I420_V_STRIDE(width),
                           width,
                           height);
    }

    void convertABGRToI420(const uint8_t *srcBuffer,
                           uint8_t *dstBuffer,
                           int width,
                           int height)
    {
        libyuv::ABGRToI420(srcBuffer,
                           width * 4,
                           OCTK_I420_Y_PTR(dstBuffer, width, height),
                           OCTK_I420_Y_STRIDE(width),
                           OCTK_I420_U_PTR(dstBuffer, width, height),
                           OCTK_I420_U_STRIDE(width),
                           OCTK_I420_V_PTR(dstBuffer, width, height),
                           OCTK_I420_V_STRIDE(width),
                           width,
                           height);
    }

    void convertBGRAToI420(const uint8_t *srcBuffer,
                           uint8_t *dstBuffer,
                           int width,
                           int height)
    {
        libyuv::BGRAToI420(srcBuffer,
                           width * 4,
                           OCTK_I420_Y_PTR(dstBuffer, width, height),
                           OCTK_I420_Y_STRIDE(width),
                           OCTK_I420_U_PTR(dstBuffer, width, height),
                           OCTK_I420_U_STRIDE(width),
                           OCTK_I420_V_PTR(dstBuffer, width, height),
                           OCTK_I420_V_STRIDE(width),
                           width,
                           height);
    }

    void convertRGBAToI420(const uint8_t *srcBuffer,
                           uint8_t *dstBuffer,
                           int width,
                           int height)
    {
        libyuv::RGBAToI420(srcBuffer,
                           width * 4,
                           OCTK_I420_Y_PTR(dstBuffer, width, height),
                           OCTK_I420_Y_STRIDE(width),
                           OCTK_I420_U_PTR(dstBuffer, width, height),
                           OCTK_I420_U_STRIDE(width),
                           OCTK_I420_V_PTR(dstBuffer, width, height),
                           OCTK_I420_V_STRIDE(width),
                           width,
                           height);
    }

    void convertNV21ToI420(const uint8_t *srcBuffer,
                           uint8_t *dstBuffer,
                           int width,
                           int height)
    {
        libyuv::RGB24ToI420(srcBuffer,
                            width * 3,
                            OCTK_I420_Y_PTR(dstBuffer, width, height),
                            OCTK_I420_Y_STRIDE(width),
                            OCTK_I420_U_PTR(dstBuffer, width, height),
                            OCTK_I420_U_STRIDE(width),
                            OCTK_I420_V_PTR(dstBuffer, width, height),
                            OCTK_I420_V_STRIDE(width),
                            width,
                            height);
    }

    void convertNV12ToI420(const uint8_t *srcBuffer,
                           uint8_t *dstBuffer,
                           int width,
                           int height)
    {
        libyuv::NV12ToI420(OCTK_NV12_Y_PTR(srcBuffer, width, height),
                           OCTK_NV12_Y_STRIDE(width),
                           OCTK_NV12_UV_PTR(srcBuffer, width, height),
                           OCTK_NV12_UV_STRIDE(width),
                           OCTK_I420_Y_PTR(dstBuffer, width, height),
                           OCTK_I420_Y_STRIDE(width),
                           OCTK_I420_U_PTR(dstBuffer, width, height),
                           OCTK_I420_U_STRIDE(width),
                           OCTK_I420_V_PTR(dstBuffer, width, height),
                           OCTK_I420_V_STRIDE(width),
                           width,
                           height);
    }

    void convertNV12ToARGB(const uint8_t *srcBuffer,
                           uint8_t *dstBuffer,
                           int width,
                           int height)
    {
        libyuv::NV12ToARGB(OCTK_NV12_Y_PTR(srcBuffer, width, height),
                           OCTK_NV12_Y_STRIDE(width),
                           OCTK_NV12_UV_PTR(srcBuffer, width, height),
                           OCTK_NV12_UV_STRIDE(width),
                           dstBuffer,
                           OCTK_ARGB_STRIDE(width),
                           width,
                           height);
    }

    void convertCenterInARGBToI420(const uint8_t *srcBuffer,
                                   int srcWidth,
                                   int srcHeight,
                                   uint8_t *dstBuffer,
                                   int dstWidth,
                                   int dstHeight)
    {
        libyuv::I420Rect(OCTK_I420_Y_PTR(dstBuffer, dstWidth, dstHeight),
                         OCTK_I420_Y_STRIDE(dstWidth),
                         OCTK_I420_U_PTR(dstBuffer, dstWidth, dstHeight),
                         OCTK_I420_U_STRIDE(dstWidth),
                         OCTK_I420_V_PTR(dstBuffer, dstWidth, dstHeight),
                         OCTK_I420_V_STRIDE(dstWidth),
                         0,
                         0,
                         dstWidth,
                         dstHeight,
                         0,
                         128,
                         128);

        const int fixWidth = std::min(srcWidth, dstWidth);
        const int fixHeight = std::min(srcHeight, dstHeight);
        const int xOffset = srcWidth > dstWidth ? (srcWidth - dstWidth) / 2 : 0;
        const int yOffset = srcHeight > dstHeight ? (srcHeight - dstHeight) / 2 : 0;
        const int dstXOffset = srcWidth < dstWidth ? (dstWidth - srcWidth) / 2 : 0;
        const int dstYOffset = srcHeight < dstHeight ? (dstHeight - srcHeight) / 2 : 0;
        libyuv::ARGBToI420(OCTK_ARGB_OFFSET_PTR(srcBuffer, srcWidth, xOffset, yOffset),
                           OCTK_ARGB_STRIDE(srcWidth),
                           OCTK_I420_Y_OFFSET_PTR(dstBuffer, dstWidth, dstHeight, dstXOffset, dstYOffset),
                           OCTK_I420_Y_STRIDE(dstWidth),
                           OCTK_I420_U_OFFSET_PTR(dstBuffer, dstWidth, dstHeight, dstXOffset, dstYOffset),
                           OCTK_I420_U_STRIDE(dstWidth),
                           OCTK_I420_V_OFFSET_PTR(dstBuffer, dstWidth, dstHeight, dstXOffset, dstYOffset),
                           OCTK_I420_V_STRIDE(dstWidth),
                           fixWidth,
                           fixHeight);
    }

    void convertCenterInRGBAToI420(const uint8_t *srcBuffer,
                                   int srcWidth,
                                   int srcHeight,
                                   uint8_t *dstBuffer,
                                   int dstWidth,
                                   int dstHeight)
    {
        libyuv::I420Rect(OCTK_I420_Y_PTR(dstBuffer, dstWidth, dstHeight),
                         OCTK_I420_Y_STRIDE(dstWidth),
                         OCTK_I420_U_PTR(dstBuffer, dstWidth, dstHeight),
                         OCTK_I420_U_STRIDE(dstWidth),
                         OCTK_I420_V_PTR(dstBuffer, dstWidth, dstHeight),
                         OCTK_I420_V_STRIDE(dstWidth),
                         0,
                         0,
                         dstWidth,
                         dstHeight,
                         0,
                         128,
                         128);

        const int fixWidth = std::min(srcWidth, dstWidth);
        const int fixHeight = std::min(srcHeight, dstHeight);
        const int xOffset = srcWidth > dstWidth ? (srcWidth - dstWidth) / 2 : 0;
        const int yOffset = srcHeight > dstHeight ? (srcHeight - dstHeight) / 2 : 0;
        const int dstXOffset = srcWidth < dstWidth ? (dstWidth - srcWidth) / 2 : 0;
        const int dstYOffset = srcHeight < dstHeight ? (dstHeight - srcHeight) / 2 : 0;
        libyuv::RGBAToI420(OCTK_ARGB_OFFSET_PTR(srcBuffer, srcWidth, xOffset, yOffset),
                           OCTK_ARGB_STRIDE(srcWidth),
                           OCTK_I420_Y_OFFSET_PTR(dstBuffer, dstWidth, dstHeight, dstXOffset, dstYOffset),
                           OCTK_I420_Y_STRIDE(dstWidth),
                           OCTK_I420_U_OFFSET_PTR(dstBuffer, dstWidth, dstHeight, dstXOffset, dstYOffset),
                           OCTK_I420_U_STRIDE(dstWidth),
                           OCTK_I420_V_OFFSET_PTR(dstBuffer, dstWidth, dstHeight, dstXOffset, dstYOffset),
                           OCTK_I420_V_STRIDE(dstWidth),
                           fixWidth,
                           fixHeight);
    }

    void convertCenterInNV12ToI420(const uint8_t *srcBuffer,
                                   int srcWidth,
                                   int srcHeight,
                                   uint8_t *dstBuffer,
                                   int dstWidth,
                                   int dstHeight)
    {
        libyuv::I420Rect(OCTK_I420_Y_PTR(dstBuffer, dstWidth, dstHeight),
                         OCTK_I420_Y_STRIDE(dstWidth),
                         OCTK_I420_U_PTR(dstBuffer, dstWidth, dstHeight),
                         OCTK_I420_U_STRIDE(dstWidth),
                         OCTK_I420_V_PTR(dstBuffer, dstWidth, dstHeight),
                         OCTK_I420_V_STRIDE(dstWidth),
                         0,
                         0,
                         dstWidth,
                         dstHeight,
                         0,
                         128,
                         128);

        const int fixWidth = std::min(srcWidth, dstWidth);
        const int fixHeight = std::min(srcHeight, dstHeight);
        const int xOffset = srcWidth > dstWidth ? (srcWidth - dstWidth) / 2 : 0;
        const int yOffset = srcHeight > dstHeight ? (srcHeight - dstHeight) / 2 : 0;
        const int dstXOffset = srcWidth < dstWidth ? (dstWidth - srcWidth) / 2 : 0;
        const int dstYOffset = srcHeight < dstHeight ? (dstHeight - srcHeight) / 2 : 0;
        libyuv::NV12ToI420(OCTK_NV12_Y_OFFSET_PTR(srcBuffer, srcWidth, srcHeight, xOffset, yOffset),
                           OCTK_NV12_Y_STRIDE(srcWidth),
                           OCTK_NV12_UV_OFFSET_PTR(srcBuffer, srcWidth, srcHeight, xOffset, yOffset),
                           OCTK_NV12_UV_STRIDE(srcWidth),
                           OCTK_I420_Y_OFFSET_PTR(dstBuffer, dstWidth, dstHeight, dstXOffset, dstYOffset),
                           OCTK_I420_Y_STRIDE(dstWidth),
                           OCTK_I420_U_OFFSET_PTR(dstBuffer, dstWidth, dstHeight, dstXOffset, dstYOffset),
                           OCTK_I420_U_STRIDE(dstWidth),
                           OCTK_I420_V_OFFSET_PTR(dstBuffer, dstWidth, dstHeight, dstXOffset, dstYOffset),
                           OCTK_I420_V_STRIDE(dstWidth),
                           fixWidth,
                           fixHeight);
    }

    } // namespace yuv
} // namespace utils

OCTK_END_NAMESPACE
