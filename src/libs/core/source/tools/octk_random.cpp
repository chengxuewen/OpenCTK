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

#include <octk_random.hpp>
#include <octk_safe_conversions.hpp>

OCTK_BEGIN_NAMESPACE

Random::Random(uint64_t seed)
{
    OCTK_DCHECK(seed != 0x0ull);
    state_ = seed;
}

uint32_t Random::Rand(uint32_t t)
{
    // Casting the output to 32 bits will give an almost uniform number.
    // Pr[x=0] = (2^32-1) / (2^64-1)
    // Pr[x=k] = 2^32 / (2^64-1) for k!=0
    // Uniform would be Pr[x=k] = 2^32 / 2^64 for all 32-bit integers k.
    uint32_t x = NextOutput();
    // If x / 2^32 is uniform on [0,1), then x / 2^32 * (t+1) is uniform on
    // the interval [0,t+1), so the integer part is uniform on [0,t].
    uint64_t result = x * (static_cast<uint64_t>(t) + 1);
    result >>= 32;
    return result;
}

uint32_t Random::Rand(uint32_t low, uint32_t high)
{
    OCTK_DCHECK(low <= high);
    return Rand(high - low) + low;
}

int32_t Random::Rand(int32_t low, int32_t high)
{
    OCTK_DCHECK(low <= high);
    const int64_t low_i64{low};
    return utils::dchecked_cast<int32_t>(Rand(utils::dchecked_cast<uint32_t>(high - low_i64)) + low_i64);
}

template <>
float Random::Rand<float>()
{
    double result = NextOutput() - 1;
    result = result / static_cast<double>(0xFFFFFFFFFFFFFFFFull);
    return static_cast<float>(result);
}

template <>
double Random::Rand<double>()
{
    double result = NextOutput() - 1;
    result = result / static_cast<double>(0xFFFFFFFFFFFFFFFFull);
    return result;
}

template <>
bool Random::Rand<bool>()
{
    return Rand(0, 1) == 1;
}

double Random::Gaussian(double mean, double standard_deviation)
{
    // Creating a Normal distribution variable from two independent uniform
    // variables based on the Box-Muller transform, which is defined on the
    // interval (0, 1]. Note that we rely on NextOutput to generate integers
    // in the range [1, 2^64-1]. Normally this behavior is a bit frustrating,
    // but here it is exactly what we need.
    const double kPi = 3.14159265358979323846;
    double u1 = static_cast<double>(NextOutput()) /
                static_cast<double>(0xFFFFFFFFFFFFFFFFull);
    double u2 = static_cast<double>(NextOutput()) /
                static_cast<double>(0xFFFFFFFFFFFFFFFFull);
    return mean + standard_deviation * sqrt(-2 * log(u1)) * cos(2 * kPi * u2);
}

double Random::Exponential(double lambda)
{
    double uniform = Rand<double>();
    return -log(uniform) / lambda;
}
OCTK_END_NAMESPACE
