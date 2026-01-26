/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
** Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
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

#include <octk_string_view.hpp>

OCTK_BEGIN_NAMESPACE

// NOTE: This class is still under development and may change without notice.
class MediaEventLogOutput
{
public:
    virtual ~MediaEventLogOutput() = default;

    // An output normally starts out active, though that might not always be
    // the case (e.g. failed to open a file for writing).
    // Once an output has become inactive (e.g. maximum file size reached), it can
    // never become active again.
    virtual bool IsActive() const = 0;

    // Write encoded events to an output. Returns true if the output was
    // successfully written in its entirety. Otherwise, no guarantee is given
    // about how much data was written, if any. The output sink becomes inactive
    // after the first time `false` is returned. Write() may not be called on
    // an inactive output sink.
    virtual bool Write(StringView output) = 0;

    // Indicates that buffers should be written to disk if applicable.
    virtual void Flush() { }
};

OCTK_END_NAMESPACE