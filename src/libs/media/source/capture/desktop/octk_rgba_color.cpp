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

#include <octk_rgba_color.hpp>

OCTK_BEGIN_NAMESPACE

namespace
{

bool AlphaEquals(uint8_t i, uint8_t j)
{
    // On Linux and Windows 8 or early version, '0' was returned for alpha channel
    // from capturer APIs, on Windows 10, '255' was returned. So a workaround is
    // to treat 0 as 255.
    return i == j || ((i == 0 || i == 255) && (j == 0 || j == 255));
}
}  // namespace

RgbaColor::RgbaColor(uint8_t blue, uint8_t green, uint8_t red, uint8_t alpha)
{
    this->blue = blue;
    this->green = green;
    this->red = red;
    this->alpha = alpha;
}

RgbaColor::RgbaColor(uint8_t blue, uint8_t green, uint8_t red) : RgbaColor(blue, green, red, 0xff) {}

RgbaColor::RgbaColor(const uint8_t *bgra) : RgbaColor(bgra[0], bgra[1], bgra[2], bgra[3]) {}

RgbaColor::RgbaColor(uint32_t bgra) : RgbaColor(reinterpret_cast<uint8_t *>(&bgra)) {}

bool RgbaColor::operator==(const RgbaColor &right) const
{
    return blue == right.blue && green == right.green && red == right.red && AlphaEquals(alpha, right.alpha);
}

bool RgbaColor::operator!=(const RgbaColor &right) const
{
    return !(*this == right);
}

uint32_t RgbaColor::ToUInt32() const
{
#if defined(OCTK_LITTLE_ENDIAN)
    return blue | (green << 8) | (red << 16) | (alpha << 24);
#else
    return (blue << 24) | (green << 16) | (red << 8) | alpha;
#endif
}
OCTK_END_NAMESPACE
