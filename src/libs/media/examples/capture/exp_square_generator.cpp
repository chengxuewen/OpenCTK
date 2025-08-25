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

#include <octk_frame_generator_capturer_video_track_source.hpp>
#include <octk_frame_generator_capturer.hpp>
#include <octk_frame_generator.hpp>
#include <octk_memory.hpp>
#include <octk_clock.hpp>

#include "video_renderer.hpp"

#include <iostream>
#include <thread>

using namespace octk;

int main()
{
    OCTK_LOGGER().switchLevel(octk::LogLevel::Trace);
    auto msecs = DateTime::steadyTimeMSecs();
    OCTK_WARNING("ts:%s", DateTime::localTimeStringFromSteadyTimeMSecs(msecs).c_str());
    octk::FrameGeneratorCapturerVideoTrackSource::Config config;
    config.width = 1920;
    config.height = 1080;
    config.frames_per_second = 25;
    auto capturer = utils::make_unique<octk::SquareGenerator>(config.width, config.height,
                                                              SquareGenerator::OutputType::kI420,
                                                              config.num_squares_generated);
    auto trackSource = utils::make_unique<FrameGeneratorCapturerVideoTrackSource>(std::move(capturer),
                                                                                  config.frames_per_second,
                                                                                  Clock::GetRealTimeClock(),
                                                                                  false);
    // trackSource->Start();

    std::unique_ptr<VideoRenderer> renderer = octk::utils::make_unique<VideoRenderer>("SquareGenerator",
                                                                                      config.width,
                                                                                      config.height);
    trackSource->addOrUpdateSink(renderer.get(), octk::VideoSinkWants());

    if (renderer->init())
    {
        renderer->loop();
    }
    trackSource->removeSink(renderer.get());

    OCTK_INFO() << "Demo exit";
    return 0;
}
