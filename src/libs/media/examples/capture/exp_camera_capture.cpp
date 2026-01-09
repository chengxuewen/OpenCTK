/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2026~Present ChengXueWen.
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

#include <octk_camera_capture.hpp>
#include <octk_logging.hpp>
#include <octk_memory.hpp>

#include "video_renderer.hpp"

#include <iostream>
#include <thread>

using namespace octk;

int main()
{
    OCTK_LOGGER().switchLevel(octk::LogLevel::Trace);

    auto deviceInfo = CameraCapture::createDeviceInfo();
    if (deviceInfo->numberOfDevices() <= 0)
    {
        OCTK_FATAL("deviceInfo->numberOfDevices() <= 0");
    }
    char device_name[256];
    char unique_name[256];
    if (deviceInfo->getDeviceName(0, device_name, 256,
                                  unique_name, 256))
    {
        OCTK_FATAL("deviceInfo->numberOfDevices() <= 0");
    }
    CameraCapture::Capability capability;
    auto capture = CameraCapture::create(unique_name);
    deviceInfo->getCapability(capture->currentDeviceName(), 0, capability);
    capture->startCapture(capability);

    std::unique_ptr<VideoRenderer> renderer = octk::utils::make_unique<VideoRenderer>(VideoRenderer::VideoType::I420,
                                                                                      device_name,
                                                                                      640,
                                                                                      480);
    capture->registerCaptureDataCallback(renderer.get());

    if (renderer->init())
    {
        renderer->loop();
    }
    capture->deregisterCaptureDataCallback();

    OCTK_INFO() << "Demo exit";
    return 0;
}
