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

#include <octk_rtc_engine.hpp>
#include <octk_json.hpp>
#include <octk_yuv.hpp>

#include <cpr/cpr.h>

#include "../capture/video_renderer.hpp"

#include <iostream>

OCTK_DEFINE_LOGGER("exp", EXP_LOGGER)
#define USE_SDL_RENDERER 0

int main(int argc, char **argv)
{
    OCTK_LOGGER().switchLevel(octk::LogLevel::Trace);
    octk::RtcEngine::switchLogLevel(octk::LogLevel::Warning);
    OCTK_LOGGING_INFO(EXP_LOGGER(), "octk_media_exp_webrtc_puller");

    octk::Status status;
    const auto width = 1280;
    const auto height = 720;
#if USE_SDL_RENDERER
    OCTK_LOGGING_INFO(EXP_LOGGER(), "create VideoRenderer");
    auto renderer = std::make_shared<VideoRenderer>(VideoRenderer::VideoType::I420,
                                                    "SDLRendererVideoSink",
                                                    width,
                                                    height);
    if (!renderer->init())
    {
        OCTK_LOGGING_FATAL(EXP_LOGGER(), "renderer->init failed");
    }
#endif

#if 1
    OCTK_LOGGING_INFO(EXP_LOGGER(), "create SquareGenerator");
    auto videoSource = octk::RtcVideoGenerator::createSquareGenerator(width, height, 50, 25, "VideoGenerator");
#    if USE_SDL_RENDERER
    videoSource->source()->addOrUpdateSink(renderer.get(), octk::VideoSinkWants());
#    endif
#else
    OCTK_LOGGING_INFO(EXP_LOGGER(), "create CameraCapture");
    auto deviceInfo = octk::CameraCapture::createDeviceInfo();
    if (deviceInfo->numberOfDevices() <= 0)
    {
        OCTK_FATAL("deviceInfo->numberOfDevices() <= 0");
    }
    char device_name[256];
    char unique_name[256];
    if (deviceInfo->getDeviceName(0, device_name, 256, unique_name, 256))
    {
        OCTK_FATAL("deviceInfo->numberOfDevices() <= 0");
    }
    octk::CameraCapture::Capability capability;
    auto capture = octk::CameraCapture::create(unique_name);
    deviceInfo->getCapability(capture->currentDeviceName(), 0, capability);
    capability.width = 1920;
    capability.height = 1080;
    capture->startCapture(capability);
    auto videoSource = octk::RtcVideoCapture::create(capture, "VideoGenerator");
#    if USE_SDL_RENDERER
//    videoSource->source()->addOrUpdateSink(renderer.get(), octk::VideoSinkWants());
#    endif
#endif
    if (!videoSource)
    {
        OCTK_LOGGING_FATAL(EXP_LOGGER(), "createSquareGenerator failed");
    }

    OCTK_LOGGING_INFO(EXP_LOGGER(), "peerConnectionFactory create");
    auto peerConnectionFactory = octk::RtcEngine::create();
    if (!peerConnectionFactory)
    {
        OCTK_LOGGING_FATAL(EXP_LOGGER(), "createPeerConnectionFactory create failed");
    }
    OCTK_LOGGING_INFO(EXP_LOGGER(), "peerConnectionFactory initialize");
    octk::RtcPeerConnectionFactory::Settings settings;
    settings.useHardwareCodec = true;
    status = peerConnectionFactory->initialize(settings);
    if (!status)
    {
        OCTK_LOGGING_FATAL(EXP_LOGGER(), "peerConnectionFactory.init failed: {}", status.errorString().c_str());
    }
    OCTK_LOGGING_INFO(EXP_LOGGER(), "peerConnectionFactory createVideoTrack");
    auto videoTrackResult = peerConnectionFactory->createVideoTrack(videoSource, "videoGenerator");
    if (!videoTrackResult)
    {
        OCTK_LOGGING_FATAL(EXP_LOGGER(),
                           "peerConnectionFactory.createVideoTrack failed, {}",
                           videoTrackResult.errorString().c_str());
    }
    auto videoTrack = videoTrackResult.value();

    octk::RtcConfiguration pcConfiguration;
    static constexpr int peerConnectionNum = 1;
    octk::SharedPointer<octk::RtcPeerConnection> peerConnections[peerConnectionNum];
    for (int i = 0; i < peerConnectionNum; ++i)
    {
        OCTK_LOGGING_INFO(EXP_LOGGER(), "peerConnection{} create", i);
        auto peerConnection = peerConnectionFactory->create(pcConfiguration, nullptr);
        peerConnections[i] = peerConnection;
        status = peerConnection->initialize();
        if (!status)
        {
            OCTK_LOGGING_FATAL(EXP_LOGGER(), "peerConnection.init failed: {}", status.errorString().c_str());
        }
        auto trackResult = peerConnection->addTrack(videoTrack, {"videoGenerator"});
        if (!trackResult)
        {
            OCTK_LOGGING_FATAL(EXP_LOGGER(), "addTrack failed: {}", trackResult.errorString().c_str());
        }
        auto offerResult = peerConnection->createOffer();
        if (!offerResult)
        {
            OCTK_LOGGING_FATAL(EXP_LOGGER(), "createOffer failed: {}", offerResult.errorString().c_str());
        }
        const auto offer = offerResult.value();
        status = peerConnection->setLocalDescription(offer.sdp, offer.type);
        if (!status)
        {
            OCTK_LOGGING_FATAL(EXP_LOGGER(),
                               "peerHandler.setLocalDescription failed: {}",
                               status.errorString().c_str());
        }

        octk::Json offerJson{{"offer", offer.sdp}};
        // std::cout << offerJson.dump() << std::endl;
        OCTK_LOGGING_INFO(EXP_LOGGER(), "offerJson:{}", offerJson.dump().c_str());
#if 1
        const std::string ipaddr("http://192.168.110.64");
#else
        const std::string ipaddr("http://127.0.0.1");
#endif
        const std::string streamName = "pusher-" + std::to_string(i);
        cpr::Response r = cpr::Post(cpr::Url{ipaddr + "/index/api/webrtc?app=live&stream=" + streamName + "&type=push"},
                                    cpr::Header{{"Content-Type", "text/plain;charset=UTF-8"}},
                                    cpr::Body{offer.sdp});
        OCTK_LOGGING_INFO(EXP_LOGGER(), "status_code:{}", r.status_code);
        OCTK_LOGGING_INFO(EXP_LOGGER(), "header:{}", r.header["content-type"].c_str());
        OCTK_LOGGING_INFO(EXP_LOGGER(), "text:{}", r.text.c_str());

        auto jsonExpected = octk::utils::parseJson(r.text);
        if (!jsonExpected.has_value())
        {
            OCTK_LOGGING_FATAL(EXP_LOGGER(), "parseJson failed: {}", jsonExpected.error().c_str());
        }
        auto json = jsonExpected.value();
        if (!json["sdp"].is_string())
        {
            OCTK_LOGGING_FATAL(EXP_LOGGER(), "sdp invalid!");
        }
        auto answer = json["sdp"].get<std::string>();
        status = peerConnection->setRemoteDescription(answer, octk::RtcSessionDescription::kAnswer);
        if (!status)
        {
            OCTK_LOGGING_FATAL(EXP_LOGGER(), "setRemoteDescription failed: {}", status.errorString().c_str());
        }
    }

    std::atomic_bool running{true};
    auto thread = std::thread(
        [=, &running]()
        {
            while (running.load())
            {
                std::this_thread::sleep_for(std::chrono::seconds(2));
            //OCTK_LOGGING_DEBUG(EXP_LOGGER(), "getStats start------");
#if 0
                peerConnection->getStats(
                    [](const std::vector<msrtc::MediaStats::SharedPtr> &reports)
                    {
                        //OCTK_LOGGING_DEBUG(EXP_LOGGER(), "getStats finish------");
                        for (auto &stats : reports)
                        {
                            auto json = stats->toJson();
                            auto expected = msrtc::utils::parseJson(json);
                            if (expected.has_value())
                            {
                                // OCTK_LOGGING_DEBUG(EXP_LOGGER(), "{}", expected->dump(4).c_str());
                            }
                        }
                    });
#endif
            }
        });
#if USE_SDL_RENDERER
    renderer->loop();
    videoSource->source()->removeSink(renderer.get());
    running.store(false);
#endif
    thread.join();
    return 0;
}
