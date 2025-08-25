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

#ifndef MSRTC_OCTK_YUV_HPP
#define MSRTC_OCTK_YUV_HPP

#include <octk_video_frame_buffer.hpp>
#include <octk_media_global.hpp>
#include <octk_video_frame.hpp>
#include <octk_video_type.hpp>

OCTK_BEGIN_NAMESPACE

namespace utils
{

// This is the max PSNR value our algorithms can return.
const double kPerfectPSNR = 48.0f;

// Calculate the required buffer size.
// Input:
//   - type         :The type of the designated video frame.
//   - width        :frame width in pixels.
//   - height       :frame height in pixels.
// Return value:    :The required size in bytes to accommodate the specified
//                   video frame.
size_t calcBufferSize(VideoType type, int width, int height);

// Extract buffer from VideoFrame or I420BufferInterface (consecutive
// planes, no stride)
// Input:
//   - frame       : Reference to video frame.
//   - size        : pointer to the size of the allocated buffer. If size is
//                   insufficient, an error will be returned.
//   - buffer      : Pointer to buffer
// Return value: length of buffer if OK, < 0 otherwise.
int ExtractBuffer(const std::shared_ptr<I420BufferInterface> &input_frame,
                  size_t size,
                  uint8_t *buffer);
int ExtractBuffer(const VideoFrame &input_frame, size_t size, uint8_t *buffer);
// Convert From I420
// Input:
//   - src_frame        : Reference to a source frame.
//   - dst_video_type   : Type of output video.
//   - dst_sample_size  : Required only for the parsing of MJPG.
//   - dst_frame        : Pointer to a destination frame.
// Return value: 0 if OK, < 0 otherwise.
// It is assumed that source and destination have equal height.
int ConvertFromI420(const VideoFrame &src_frame,
                    VideoType dst_video_type,
                    int dst_sample_size,
                    uint8_t *dst_frame);

std::shared_ptr<I420BufferInterface> ScaleVideoFrameBuffer(const I420BufferInterface &source,
                                                           int dst_width,
                                                           int dst_height);

double I420SSE(const I420BufferInterface &ref_buffer,
               const I420BufferInterface &test_buffer);

// Compute PSNR for an I420 frame (all planes).
// Returns the PSNR in decibel, to a maximum of kPerfectPSNR.
double I420PSNR(const VideoFrame *ref_frame, const VideoFrame *test_frame);
double I420PSNR(const I420BufferInterface &ref_buffer,
                const I420BufferInterface &test_buffer);

// Computes the weighted PSNR-YUV for an I420 buffer.
//
// For the definition and motivation, see
// J. Ohm, G. J. Sullivan, H. Schwarz, T. K. Tan and T. Wiegand,
// "Comparison of the Coding Efficiency of Video Coding Standards—Including
// High Efficiency Video Coding (HEVC)," in IEEE Transactions on Circuits and
// Systems for Video Technology, vol. 22, no. 12, pp. 1669-1684, Dec. 2012
// doi: 10.1109/TCSVT.2012.2221192.
//
// Returns the PSNR-YUV in decibel, to a maximum of kPerfectPSNR.
double I420WeightedPSNR(const I420BufferInterface &ref_buffer,
                        const I420BufferInterface &test_buffer);

// Compute SSIM for an I420 frame (all planes).
double I420SSIM(const VideoFrame *ref_frame, const VideoFrame *test_frame);
double I420SSIM(const I420BufferInterface &ref_buffer,
                const I420BufferInterface &test_buffer);

// Helper function for scaling NV12 to NV12.
// If the `src_width` and `src_height` matches the `dst_width` and `dst_height`,
// then `tmp_buffer` is not used. In other cases, the minimum size of
// `tmp_buffer` should be:
//   (src_width/2) * (src_height/2) * 2 + (dst_width/2) * (dst_height/2) * 2
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
               int dst_height);

// Helper class for directly converting and scaling NV12 to I420. The Y-plane
// will be scaled directly to the I420 destination, which makes this faster
// than separate NV12->I420 + I420->I420 scaling.
class OCTK_CORE_API NV12ToI420Scaler
{
public:
    NV12ToI420Scaler();
    ~NV12ToI420Scaler();
    void NV12ToI420Scale(const uint8_t *src_y,
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
                         int dst_height);

private:
    std::vector<uint8_t> tmp_uv_planes_;
};

// Convert VideoType to libyuv FourCC type
int ConvertVideoType(VideoType video_type);
} // namespace utils

OCTK_END_NAMESPACE

#endif //MSRTC_OCTK_YUV_HPP
