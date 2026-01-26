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

#include <octk_media_event.hpp>
#include <octk_media_event_log_output.hpp>

#include <functional>
#include <cstddef>
#include <cstdint>
#include <memory>

OCTK_BEGIN_NAMESPACE

class MediaEventLog
{
public:
    enum : size_t
    {
        kUnlimitedOutput = 0
    };
    enum : int64_t
    {
        kImmediateOutput = 0
    };

    // TODO(eladalon):  Get rid of the legacy encoding and this enum once all
    // clients have migrated to the new format.
    enum class EncodingType
    {
        Legacy,
        NewFormat,
        ProtoFree
    };

    virtual ~MediaEventLog() = default;

    // Starts logging to a given output. The output might be limited in size,
    // and may close itself once it has reached the maximum size.
    virtual bool StartLogging(std::unique_ptr<MediaEventLogOutput> output, int64_t output_period_ms) = 0;

    // Stops logging to file and waits until the file has been closed, after
    // which it would be permissible to read and/or modify it.
    virtual void StopLogging() = 0;

    // Stops logging to file and calls `callback` when the file has been closed.
    // Note that it is not safe to call any other members, including the
    // destructor, until the callback has been called.
    // TODO(srte): Remove default implementation when it's safe to do so.
    virtual void StopLogging(std::function<void()> callback)
    {
        StopLogging();
        callback();
    }

    // Log an RTC event (the type of event is determined by the subclass).
    virtual void Log(std::unique_ptr<MediaEvent> event) = 0;
};

// No-op implementation is used if flag is not set, or in tests.
class MediaEventLogNull final : public MediaEventLog
{
public:
    bool StartLogging(std::unique_ptr<MediaEventLogOutput> output, int64_t output_period_ms) override;
    void StopLogging() override { }
    void Log(std::unique_ptr<MediaEvent> /* event */) override { }
};

OCTK_END_NAMESPACE