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

#include <octk_differ_vector_sse2.hpp>

#if defined(_MSC_VER)
#include <intrin.h>
#else
#include <emmintrin.h>
#include <mmintrin.h>
#endif

OCTK_BEGIN_NAMESPACE

extern bool VectorDifference_SSE2_W16(const uint8_t *image1,
                                      const uint8_t *image2)
{
    __m128i acc = _mm_setzero_si128();
    __m128i v0;
    __m128i v1;
    __m128i sad;
    const __m128i *i1 = reinterpret_cast<const __m128i *>(image1);
    const __m128i *i2 = reinterpret_cast<const __m128i *>(image2);
    v0 = _mm_loadu_si128(i1);
    v1 = _mm_loadu_si128(i2);
    sad = _mm_sad_epu8(v0, v1);
    acc = _mm_adds_epu16(acc, sad);
    v0 = _mm_loadu_si128(i1 + 1);
    v1 = _mm_loadu_si128(i2 + 1);
    sad = _mm_sad_epu8(v0, v1);
    acc = _mm_adds_epu16(acc, sad);
    v0 = _mm_loadu_si128(i1 + 2);
    v1 = _mm_loadu_si128(i2 + 2);
    sad = _mm_sad_epu8(v0, v1);
    acc = _mm_adds_epu16(acc, sad);
    v0 = _mm_loadu_si128(i1 + 3);
    v1 = _mm_loadu_si128(i2 + 3);
    sad = _mm_sad_epu8(v0, v1);
    acc = _mm_adds_epu16(acc, sad);

    // This essential means sad = acc >> 64. We only care about the lower 16
    // bits.
    sad = _mm_shuffle_epi32(acc, 0xEE);
    sad = _mm_adds_epu16(sad, acc);
    return _mm_cvtsi128_si32(sad) != 0;
}

extern bool VectorDifference_SSE2_W32(const uint8_t *image1,
                                      const uint8_t *image2)
{
    __m128i acc = _mm_setzero_si128();
    __m128i v0;
    __m128i v1;
    __m128i sad;
    const __m128i *i1 = reinterpret_cast<const __m128i *>(image1);
    const __m128i *i2 = reinterpret_cast<const __m128i *>(image2);
    v0 = _mm_loadu_si128(i1);
    v1 = _mm_loadu_si128(i2);
    sad = _mm_sad_epu8(v0, v1);
    acc = _mm_adds_epu16(acc, sad);
    v0 = _mm_loadu_si128(i1 + 1);
    v1 = _mm_loadu_si128(i2 + 1);
    sad = _mm_sad_epu8(v0, v1);
    acc = _mm_adds_epu16(acc, sad);
    v0 = _mm_loadu_si128(i1 + 2);
    v1 = _mm_loadu_si128(i2 + 2);
    sad = _mm_sad_epu8(v0, v1);
    acc = _mm_adds_epu16(acc, sad);
    v0 = _mm_loadu_si128(i1 + 3);
    v1 = _mm_loadu_si128(i2 + 3);
    sad = _mm_sad_epu8(v0, v1);
    acc = _mm_adds_epu16(acc, sad);
    v0 = _mm_loadu_si128(i1 + 4);
    v1 = _mm_loadu_si128(i2 + 4);
    sad = _mm_sad_epu8(v0, v1);
    acc = _mm_adds_epu16(acc, sad);
    v0 = _mm_loadu_si128(i1 + 5);
    v1 = _mm_loadu_si128(i2 + 5);
    sad = _mm_sad_epu8(v0, v1);
    acc = _mm_adds_epu16(acc, sad);
    v0 = _mm_loadu_si128(i1 + 6);
    v1 = _mm_loadu_si128(i2 + 6);
    sad = _mm_sad_epu8(v0, v1);
    acc = _mm_adds_epu16(acc, sad);
    v0 = _mm_loadu_si128(i1 + 7);
    v1 = _mm_loadu_si128(i2 + 7);
    sad = _mm_sad_epu8(v0, v1);
    acc = _mm_adds_epu16(acc, sad);

    // This essential means sad = acc >> 64. We only care about the lower 16
    // bits.
    sad = _mm_shuffle_epi32(acc, 0xEE);
    sad = _mm_adds_epu16(sad, acc);
    return _mm_cvtsi128_si32(sad) != 0;
}
OCTK_END_NAMESPACE
