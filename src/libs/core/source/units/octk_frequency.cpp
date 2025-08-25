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

#include <octk_frequency.hpp>

#include <cstdint>
#include <string>

OCTK_BEGIN_NAMESPACE
std::string toString(Frequency value)
{
    std::stringstream ss;
    if (value.IsPlusInfinity())
    {
        ss << "+inf Hz";
    }
    else if (value.IsMinusInfinity())
    {
        ss << "-inf Hz";
    }
    else if (value.millihertz<int64_t>() % 1000 != 0)
    {
        char buf[64] = {0};
        std::snprintf(buf, sizeof(buf), "%.3f Hz", value.hertz<double>());
        ss << buf;
    }
    else
    {
        ss << value.hertz<int64_t>() << " Hz";
    }
    return ss.str();
}
OCTK_END_NAMESPACE
