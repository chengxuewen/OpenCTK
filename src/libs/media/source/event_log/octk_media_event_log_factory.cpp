/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present ChengXueWen.
** Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
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

#include <octk_media_event_log_factory.hpp>
#include <octk_field_trials_view.hpp>
#include <octk_memory.hpp>

#include <memory>

// #ifdef WEBRTC_ENABLE_RTC_EVENT_LOG
// #    include "logging/rtc_event_log/rtc_event_log_impl.h"
// #endif

OCTK_BEGIN_NAMESPACE

Nonnull<std::unique_ptr<MediaEventLog>> MediaEventLogFactory::Create(const MediaContext &env) const
{
    // #ifndef WEBRTC_ENABLE_RTC_EVENT_LOG
    //     return std::make_unique<MediaEventLogNull>();
    // #else
    if (env.field_trials().IsEnabled("WebRTC-MediaEventLogKillSwitch"))
    {
        return utils::make_unique<MediaEventLogNull>();
    }
    // return std::make_unique<MediaEventLogImpl>(env);
    return utils::make_unique<MediaEventLogNull>();
    // #endif
}

OCTK_END_NAMESPACE