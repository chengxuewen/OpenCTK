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

#include <octk_full_screen_window_detector.hpp>
#include <octk_date_time.hpp>

OCTK_BEGIN_NAMESPACE

FullScreenWindowDetector::FullScreenWindowDetector(ApplicationHandlerFactory application_handler_factory)
    : application_handler_factory_(application_handler_factory), last_update_time_ms_(0), previous_source_id_(0)
    , no_handler_source_id_(0) {}

DesktopCapturer::SourceId FullScreenWindowDetector::FindFullScreenWindow(DesktopCapturer::SourceId original_source_id)
{
    if (app_handler_ == nullptr || app_handler_->GetSourceId() != original_source_id)
    {
        return 0;
    }
    return app_handler_->FindFullScreenWindow(window_list_, last_update_time_ms_);
}

void FullScreenWindowDetector::UpdateWindowListIfNeeded(DesktopCapturer::SourceId original_source_id,
                                                        FunctionView<bool(DesktopCapturer::SourceList *)> get_sources)
{
    const bool skip_update = previous_source_id_ != original_source_id;
    previous_source_id_ = original_source_id;

    // Here is an attempt to avoid redundant creating application handler in case
    // when an instance of WindowCapturer is used to generate a thumbnail to show
    // in picker by calling SelectSource and CaptureFrame for every available
    // source.
    if (skip_update)
    {
        return;
    }

    CreateApplicationHandlerIfNeeded(original_source_id);
    if (app_handler_ == nullptr)
    {
        // There is no FullScreenApplicationHandler specific for
        // current application
        return;
    }

    constexpr int64_t kUpdateIntervalMs = 500;

    if ((DateTime::TimeMillis() - last_update_time_ms_) <= kUpdateIntervalMs)
    {
        return;
    }

    DesktopCapturer::SourceList window_list;
    if (get_sources(&window_list))
    {
        last_update_time_ms_ = DateTime::TimeMillis();
        window_list_.swap(window_list);
    }
}

void FullScreenWindowDetector::CreateApplicationHandlerIfNeeded(DesktopCapturer::SourceId source_id)
{
    if (no_handler_source_id_ == source_id)
    {
        return;
    }

    if (app_handler_ == nullptr || app_handler_->GetSourceId() != source_id)
    {
        app_handler_ = application_handler_factory_
                       ? application_handler_factory_(source_id)
                       : nullptr;
    }

    if (app_handler_ == nullptr)
    {
        no_handler_source_id_ = source_id;
    }
}
OCTK_END_NAMESPACE
