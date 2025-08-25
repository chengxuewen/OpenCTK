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

#ifndef _OCTK_FRAMERATE_CONTROLLER_HPP
#define _OCTK_FRAMERATE_CONTROLLER_HPP

#include <octk_media_global.hpp>
#include <octk_optional.hpp>

#include <optional>

OCTK_BEGIN_NAMESPACE

// Determines which frames that should be dropped based on input framerate and
// requested framerate.
class OCTK_MEDIA_API FramerateController
{
public:
    FramerateController();
    explicit FramerateController(double max_framerate);
    ~FramerateController();

    // Sets max framerate (default is maxdouble).
    void SetMaxFramerate(double max_framerate);
    double GetMaxFramerate() const;

    // Returns true if the frame should be dropped, false otherwise.
    bool ShouldDropFrame(int64_t inTimestampNSecs);

    void Reset();

    void KeepFrame(int64_t inTimestampNSecs);

private:
    double max_framerate_;
    Optional<int64_t> next_frame_timestamp_ns_;
};
OCTK_END_NAMESPACE

#endif  // _OCTK_FRAMERATE_CONTROLLER_HPP
