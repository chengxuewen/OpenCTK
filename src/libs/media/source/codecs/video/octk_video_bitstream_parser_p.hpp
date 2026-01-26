/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2019~Present ChengXueWen.
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

#include <octk_optional.hpp>
#include <octk_array_view.hpp>

#include <stddef.h>
#include <stdint.h>

OCTK_BEGIN_NAMESPACE

// This class is an interface for bitstream parsers.
class BitStreamParser
{
public:
    virtual ~BitStreamParser() = default;

    // Parse an additional chunk of the bitstream.
    virtual void ParseBitstream(ArrayView<const uint8_t> bitstream) = 0;

    // Get the last extracted QP value from the parsed bitstream. If no QP
    // value could be parsed, returns utils::nullopt.
    virtual Optional<int> GetLastSliceQp() const = 0;
};

OCTK_END_NAMESPACE