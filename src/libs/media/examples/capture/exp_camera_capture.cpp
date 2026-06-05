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

#include <openctk/core/camera_capture.hpp>
#include <openctk/core/logging.hpp>
#include <openctk/core/memory.hpp>

#include "video_renderer.hpp"

#include <iostream>
#include <thread>

using namespace octk;

int main()
{
    OCTK_LOGGER().switchLevel(LogLevel::Trace);

    CameraCapture::Options options;
    // options.allowPipeWire = true;
    auto deviceInfo = CameraCapture::createDeviceInfo(&options);
    if (!deviceInfo)
    {
        OCTK_FATAL("deviceInfo null");
    }
    for (int i = 0; i < deviceInfo->numberOfDevices(); ++i)
    {
        char device_name[256];
        char unique_name[256];
        auto status = deviceInfo->getDeviceName(i, device_name, 256, unique_name, 256);
        if (!status.isOk())
        {
            OCTK_FATAL("deviceInfo->getDeviceName({}) failed:{}", i, status.errorMessage());
        }
        OCTK_INFO("device{}: device_name={} unique_name={}", i, device_name, unique_name);
    }
    if (deviceInfo->numberOfDevices() <= 0)
    {
        OCTK_FATAL("deviceInfo->numberOfDevices() <= 0");
    }
    OCTK_INFO("numberOfDevices={}", deviceInfo->numberOfDevices());

    char device_name[256];
    char unique_name[256];
    const auto status = deviceInfo->getDeviceName(1, device_name, 256, unique_name, 256);
    if (!status.isOk())
    {
        OCTK_FATAL("deviceInfo->getDeviceName() failed:{}", status.errorMessage());
    }
    OCTK_INFO("unique_name={}", unique_name);
    CameraCapture::Capability capability;
    auto capture = CameraCapture::create(unique_name);
    deviceInfo->getCapability(capture->currentDeviceName(), 0, capability);
    capability.width = 1920;
    capability.height = 1080;
    capture->startCapture(capability);

    std::unique_ptr<VideoRenderer> renderer = octk::utils::make_unique<VideoRenderer>(VideoRenderer::VideoType::I420,
                                                                                      device_name,
                                                                                      capability.width,
                                                                                      capability.height);
    capture->registerCaptureDataCallback(renderer.get());

    if (renderer->init())
    {
        renderer->loop();
    }
    capture->deregisterCaptureDataCallback();

    OCTK_INFO() << "Demo exit";
    return 0;
}
