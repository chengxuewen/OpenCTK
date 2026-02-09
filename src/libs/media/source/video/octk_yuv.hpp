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

#include <octk_video_frame_buffer.hpp>
#include <octk_media_global.hpp>
#include <octk_video_frame.hpp>
#include <octk_video_type.hpp>

#define OCTK_I420_Y_PTR(buffer, width, height) (buffer)
#define OCTK_I420_U_PTR(buffer, width, height) (buffer + width * height)
#define OCTK_I420_V_PTR(buffer, width, height) (buffer + width * height + (width >> 1) * (height >> 1))
#define OCTK_I420_Y_STRIDE(width)              (width)
#define OCTK_I420_U_STRIDE(width)              (width >> 1)
#define OCTK_I420_V_STRIDE(width)              (width >> 1)
#define OCTK_I420_Y_OFFSET_PTR(buffer, width, height, xOffset, yOffset)                                                \
    (OCTK_I420_Y_PTR(buffer, width, height) + OCTK_I420_Y_STRIDE(width) * yOffset + xOffset)
#define OCTK_I420_U_OFFSET_PTR(buffer, width, height, xOffset, yOffset)                                                \
    (OCTK_I420_U_PTR(buffer, width, height) + OCTK_I420_U_STRIDE(width) * (yOffset / 2) + (xOffset / 2))
#define OCTK_I420_V_OFFSET_PTR(buffer, width, height, xOffset, yOffset)                                                \
    (OCTK_I420_V_PTR(buffer, width, height) + OCTK_I420_V_STRIDE(width) * (yOffset / 2) + (xOffset / 2))

#define OCTK_NV12_Y_PTR(buffer, width, height)  (buffer)
#define OCTK_NV12_UV_PTR(buffer, width, height) (buffer + width * height)
#define OCTK_NV12_Y_STRIDE(width)               (width)
#define OCTK_NV12_UV_STRIDE(width)              (width)
#define OCTK_NV12_Y_OFFSET_PTR(buffer, width, height, xOffset, yOffset)                                                \
    (OCTK_NV12_Y_PTR(buffer, width, height) + width * yOffset + xOffset)
#define OCTK_NV12_UV_OFFSET_PTR(buffer, width, height, xOffset, yOffset)                                               \
    (OCTK_NV12_UV_PTR(buffer, width, height) + width * (yOffset / 2) + xOffset)

#define OCTK_NV21_Y_PTR(buffer, width, height)  (buffer)
#define OCTK_NV21_VU_PTR(buffer, width, height) (buffer + width * height)
#define OCTK_NV21_Y_STRIDE(width)               (width)
#define OCTK_NV21_VU_STRIDE(width)              (width)

#define OCTK_ARGB_STRIDE(width) (width * 4)
#define OCTK_ARGB_OFFSET_PTR(buffer, width, xOffset, yOffset)                                                          \
    (buffer + OCTK_ARGB_STRIDE(width) * yOffset + OCTK_ARGB_STRIDE(xOffset))


OCTK_BEGIN_NAMESPACE

namespace utils
{
// This is the max PSNR value our algorithms can return.
const double kPerfectPSNR = 48.0f;

// Extract buffer from VideoFrame or I420BufferInterface (consecutive
// planes, no stride)
// Input:
//   - frame       : Reference to video frame.
//   - size        : pointer to the size of the allocated buffer. If size is
//                   insufficient, an error will be returned.
//   - buffer      : Pointer to buffer
// Return value: length of buffer if OK, < 0 otherwise.
int ExtractBuffer(const std::shared_ptr<I420BufferInterface> &input_frame, size_t size, uint8_t *buffer);
int ExtractBuffer(const VideoFrame &input_frame, size_t size, uint8_t *buffer);
// Convert From I420
// Input:
//   - src_frame        : Reference to a source frame.
//   - dst_video_type   : Type of output video.
//   - dst_sample_size  : Required only for the parsing of MJPG.
//   - dst_frame        : Pointer to a destination frame.
// Return value: 0 if OK, < 0 otherwise.
// It is assumed that source and destination have equal height.
int ConvertFromI420(const VideoFrame &src_frame, VideoType dst_video_type, int dst_sample_size, uint8_t *dst_frame);

std::shared_ptr<I420BufferInterface> ScaleVideoFrameBuffer(const I420BufferInterface &source,
                                                           int dst_width,
                                                           int dst_height);

double I420SSE(const I420BufferInterface &ref_buffer, const I420BufferInterface &test_buffer);

// Compute PSNR for an I420 frame (all planes).
// Returns the PSNR in decibel, to a maximum of kPerfectPSNR.
double I420PSNR(const VideoFrame *ref_frame, const VideoFrame *test_frame);
double I420PSNR(const I420BufferInterface &ref_buffer, const I420BufferInterface &test_buffer);

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
double I420WeightedPSNR(const I420BufferInterface &ref_buffer, const I420BufferInterface &test_buffer);

// Compute SSIM for an I420 frame (all planes).
double I420SSIM(const VideoFrame *ref_frame, const VideoFrame *test_frame);
double I420SSIM(const I420BufferInterface &ref_buffer, const I420BufferInterface &test_buffer);

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
class OCTK_MEDIA_API NV12ToI420Scaler
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

namespace yuv
{
enum class FilterMode
{
    kFilterNone = 0,     // Point sample; Fastest.
    kFilterLinear = 1,   // Filter horizontally only.
    kFilterBilinear = 2, // Faster than box, but lower quality scaling down.
    kFilterBox = 3       // Highest quality.
};

OCTK_MEDIA_API void scaleI420(const uint8_t *srcY,
                              int srcStrideY,
                              const uint8_t *srcU,
                              int srcStrideU,
                              const uint8_t *srcV,
                              int srcStrideV,
                              int srcWidth,
                              int srcHeight,
                              uint8_t *dstY,
                              int dstStrideY,
                              uint8_t *dstU,
                              int dstStrideU,
                              uint8_t *dstV,
                              int dstStrideV,
                              int dstWidth,
                              int dstHeight,
                              FilterMode filtering);

OCTK_MEDIA_API void scaleI420(const uint8_t *srcBuffer,
                              int srcWidth,
                              int srcHeight,
                              uint8_t *dstBuffer,
                              int dstWidth,
                              int dstHeight,
                              bool highestQuality = true);

OCTK_MEDIA_API void scaleARGB(const uint8_t *srcBuffer,
                              int srcWidth,
                              int srcHeight,
                              uint8_t *dstBuffer,
                              int dstWidth,
                              int dstHeight,
                              bool highestQuality = true);

OCTK_MEDIA_API void scaleNV12(const uint8_t *bufferIn,
                              int width,
                              int height,
                              uint8_t *bufferOut,
                              int dstWidth,
                              int dstHeight,
                              bool highestQuality = true);

OCTK_MEDIA_API void copyI420(const uint8_t *srcDataY,
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
                             int height);

OCTK_MEDIA_API void copyCenterInI420(const uint8_t *srcBuffer,
                                     int srcWidth,
                                     int srcHeight,
                                     uint8_t *dstBuffer,
                                     int dstWidth,
                                     int dstHeight);

OCTK_MEDIA_API void copyCenterInNV12(const uint8_t *srcBuffer,
                                     int srcWidth,
                                     int srcHeight,
                                     uint8_t *dstBuffer,
                                     int dstWidth,
                                     int dstHeight);

OCTK_MEDIA_API void copyCenterInARGB(const uint8_t *srcBuffer,
                                     int srcWidth,
                                     int srcHeight,
                                     uint8_t *dstBuffer,
                                     int dstWidth,
                                     int dstHeight);

OCTK_MEDIA_API bool convertToI420(const uint8_t *sample,
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
                                  VideoType videoType);

OCTK_MEDIA_API void convertI420ToARGB(const uint8_t *srcBuffer, uint8_t *dstBuffer, int width, int height);

OCTK_MEDIA_API void convertI420ToABGR(const uint8_t *srcBuffer, uint8_t *dstBuffer, int width, int height);

OCTK_MEDIA_API void convertI420ToBGRA(const uint8_t *srcBuffer, uint8_t *dstBuffer, int width, int height);

OCTK_MEDIA_API void convertI420ToRGBA(const uint8_t *srcBuffer, uint8_t *dstBuffer, int width, int height);

OCTK_MEDIA_API void convertI420ToRGB24(const uint8_t *srcBuffer, uint8_t *dstBuffer, int width, int height);

OCTK_MEDIA_API void convertI420ToNV12(const uint8_t *srcBuffer, uint8_t *dstBuffer, int width, int height);

// BGRA little endian (argb in memory) to ARGB.
OCTK_MEDIA_API void convertBGRAToARGB(const uint8_t *srcBuffer, uint8_t *dstBuffer, int width, int height);

// ABGR little endian (rgba in memory) to ARGB.
OCTK_MEDIA_API void convertABGRToARGB(const uint8_t *srcBuffer, uint8_t *dstBuffer, int width, int height);

// RGBA little endian (abgr in memory) to ARGB.
OCTK_MEDIA_API void convertRGBAToARGB(const uint8_t *srcBuffer, uint8_t *dstBuffer, int width, int height);

OCTK_MEDIA_API void convertI420ToNV21(const uint8_t *srcBuffer, uint8_t *dstBuffer, int width, int height);

OCTK_MEDIA_API void convertARGBToI420(const uint8_t *srcBuffer, uint8_t *dstBuffer, int width, int height);

OCTK_MEDIA_API void convertABGRToI420(const uint8_t *srcBuffer, uint8_t *dstBuffer, int width, int height);

OCTK_MEDIA_API void convertBGRAToI420(const uint8_t *srcBuffer, uint8_t *dstBuffer, int width, int height);

OCTK_MEDIA_API void convertRGBAToI420(const uint8_t *srcBuffer, uint8_t *dstBuffer, int width, int height);

OCTK_MEDIA_API void convertNV21ToI420(const uint8_t *srcBuffer, uint8_t *dstBuffer, int width, int height);

OCTK_MEDIA_API void convertNV12ToI420(const uint8_t *srcBuffer, uint8_t *dstBuffer, int width, int height);

OCTK_MEDIA_API void convertNV12ToARGB(const uint8_t *srcBuffer, uint8_t *dstBuffer, int width, int height);

OCTK_MEDIA_API void convertCenterInARGBToI420(const uint8_t *srcBuffer,
                                              int srcWidth,
                                              int srcHeight,
                                              uint8_t *dstBuffer,
                                              int dstWidth,
                                              int dstHeight);

OCTK_MEDIA_API void convertCenterInRGBAToI420(const uint8_t *srcBuffer,
                                              int srcWidth,
                                              int srcHeight,
                                              uint8_t *dstBuffer,
                                              int dstWidth,
                                              int dstHeight);

OCTK_MEDIA_API void convertCenterInNV12ToI420(const uint8_t *srcBuffer,
                                              int srcWidth,
                                              int srcHeight,
                                              uint8_t *dstBuffer,
                                              int dstWidth,
                                              int dstHeight);
} // namespace yuv
} // namespace utils

OCTK_END_NAMESPACE