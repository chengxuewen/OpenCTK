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

#include <octk_desktop_capture_metrics_helper.hpp>
#include <octk_desktop_capture_types.hpp>
#include <octk_metrics.hpp>

OCTK_BEGIN_NAMESPACE

namespace
{
// This enum is logged via UMA so entries should not be reordered or have their
// values changed. This should also be kept in sync with the values in the
// DesktopCapturerId namespace.
enum class SequentialDesktopCapturerId
{
    kUnknown = 0,
    kWgcCapturerWin = 1,
    // kScreenCapturerWinMagnifier = 2,
    kWindowCapturerWinGdi = 3,
    kScreenCapturerWinGdi = 4,
    kScreenCapturerWinDirectx = 5,
    kMaxValue = kScreenCapturerWinDirectx
};
}  // namespace

void RecordCapturerImpl(uint32_t capturer_id)
{
    SequentialDesktopCapturerId sequential_id;
    switch (capturer_id)
    {
        case DesktopCapturerId::kWgcCapturerWin:
            sequential_id = SequentialDesktopCapturerId::kWgcCapturerWin;
            break;
        case DesktopCapturerId::kWindowCapturerWinGdi:
            sequential_id = SequentialDesktopCapturerId::kWindowCapturerWinGdi;
            break;
        case DesktopCapturerId::kScreenCapturerWinGdi:
            sequential_id = SequentialDesktopCapturerId::kScreenCapturerWinGdi;
            break;
        case DesktopCapturerId::kScreenCapturerWinDirectx:
            sequential_id = SequentialDesktopCapturerId::kScreenCapturerWinDirectx;
            break;
        case DesktopCapturerId::kUnknown:
        default:
            sequential_id = SequentialDesktopCapturerId::kUnknown;
    }
    OCTK_HISTOGRAM_ENUMERATION("WebRTC.DesktopCapture.Win.DesktopCapturerImpl",
                               static_cast<int>(sequential_id),
                               static_cast<int>(SequentialDesktopCapturerId::kMaxValue));
}
OCTK_END_NAMESPACE
