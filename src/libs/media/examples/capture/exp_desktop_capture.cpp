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

#include <octk_desktop_capture_source.hpp>
#include <octk_memory.hpp>

#include "video_renderer.hpp"

#include <iostream>
#include <thread>

int main()
{
    OCTK_LOGGER().switchLevel(octk::LogLevel::Trace);
    auto capturer = octk::utils::make_unique<octk::DesktopCapturer>(15, 2);
    capturer->startCapture();

    std::unique_ptr<VideoRenderer> renderer = octk::utils::make_unique<VideoRenderer>(capturer->windowTitle(),
                                                                                      640,
                                                                                      480);
    capturer->addOrUpdateSink(renderer.get(), octk::VideoSinkWants());

    if (renderer->init())
    {
        renderer->loop();
    }
    capturer->removeSink(renderer.get());

    OCTK_INFO() << "Demo exit";
    return 0;
}
