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

#include <octk_differ_block.hpp>

#include <string.h>

// #include "rtc_base/system/arch.h"
// #include "system_wrappers/include/cpu_features_wrapper.h"

// This needs to be after rtc_base/system/arch.h which defines
// architecture macros.
#if defined(WEBRTC_ARCH_X86_FAMILY)
// #   include "octk_differ_vector_sse2.h"
#endif

OCTK_BEGIN_NAMESPACE

namespace
{

bool VectorDifference_C(const uint8_t *image1, const uint8_t *image2)
{
    return memcmp(image1, image2, kBlockSize * kBytesPerPixel) != 0;
}
}  // namespace

bool VectorDifference(const uint8_t *image1, const uint8_t *image2)
{
    static bool (*diff_proc)(const uint8_t *, const uint8_t *) = nullptr;

    if (!diff_proc)
    {
#if defined(WEBRTC_ARCH_X86_FAMILY)
        bool have_sse2 = GetCPUInfo(kSSE2) != 0;
        // For x86 processors, check if SSE2 is supported.
        if (have_sse2 && kBlockSize == 32) {
          diff_proc = &VectorDifference_SSE2_W32;
        } else if (have_sse2 && kBlockSize == 16) {
          diff_proc = &VectorDifference_SSE2_W16;
        } else {
          diff_proc = &VectorDifference_C;
        }
#else
        // For other processors, always use C version.
        // TODO(hclam): Implement a NEON version.
        diff_proc = &VectorDifference_C;
#endif
    }

    return diff_proc(image1, image2);
}

bool BlockDifference(const uint8_t *image1,
                     const uint8_t *image2,
                     int height,
                     int stride)
{
    for (int i = 0; i < height; i++)
    {
        if (VectorDifference(image1, image2))
        {
            return true;
        }
        image1 += stride;
        image2 += stride;
    }
    return false;
}

bool BlockDifference(const uint8_t *image1, const uint8_t *image2, int stride)
{
    return BlockDifference(image1, image2, kBlockSize, stride);
}
OCTK_END_NAMESPACE
