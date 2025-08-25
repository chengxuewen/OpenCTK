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

#ifndef _OCTK_CRYPTO_RANDOM_HPP
#define _OCTK_CRYPTO_RANDOM_HPP

#include <octk_string_view.hpp>

OCTK_BEGIN_NAMESPACE

// Interface for RNG implementations.
class RandomGenerator
{
public:
    virtual ~RandomGenerator() {}
    virtual bool Init(const void *seed, size_t len) = 0;
    virtual bool Generate(void *buf, size_t len) = 0;
};

namespace utils
{
// Sets the default random generator as the source of randomness. The default
// source uses the OpenSSL RNG and provides cryptographically secure randomness.
void SetDefaultRandomGenerator();

// Set a custom random generator. Results produced by CreateRandomXyz
// are cryptographically random iff the output of the supplied generator is
// cryptographically random.
void SetRandomGenerator(std::unique_ptr<RandomGenerator> generator);

// For testing, we can return predictable data.
void SetRandomTestMode(bool test);

// Initializes the RNG, and seeds it with the specified entropy.
bool InitRandom(int seed);
bool InitRandom(const char *seed, size_t len);

// Generates a (cryptographically) random string of the given length.
// We generate base64 values so that they will be printable.
OCTK_CORE_API std::string CreateRandomString(size_t length);

// Generates a (cryptographically) random string of the given length.
// We generate base64 values so that they will be printable.
// Return false if the random number generator failed.
OCTK_CORE_API bool CreateRandomString(size_t length, std::string *str);

// Generates a (cryptographically) random string of the given length,
// with characters from the given table. Return false if the random
// number generator failed.
// For ease of implementation, the function requires that the table
// size evenly divide 256; otherwise, it returns false.
OCTK_CORE_API bool CreateRandomString(size_t length,
                                      StringView table,
                                      std::string *str);

// Generates (cryptographically) random data of the given length.
// Return false if the random number generator failed.
bool CreateRandomData(size_t length, std::string *data);

// Generates a (cryptographically) random UUID version 4 string.
std::string CreateRandomUuid();

// Generates a random id.
uint32_t CreateRandomId();

// Generates a 64 bit random id.
OCTK_CORE_API uint64_t CreateRandomId64();

// Generates a random id > 0.
uint32_t CreateRandomNonZeroId();

// Generates a random double between 0.0 (inclusive) and 1.0 (exclusive).
double CreateRandomDouble();
} // namespace utils

OCTK_END_NAMESPACE

#endif // _OCTK_CRYPTO_RANDOM_HPP
