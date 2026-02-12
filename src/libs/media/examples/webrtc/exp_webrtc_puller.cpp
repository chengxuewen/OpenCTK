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
#include <octk_logging.hpp>
#include <octk_json.hpp>

#include <cpr/cpr.h>

#include "../capture/video_renderer.hpp"

#include <iostream>

OCTK_DEFINE_LOGGER("exp", EXP_LOGGER)

int main(int argc, char **argv)
{
    OCTK_LOGGER().switchLevel(octk::LogLevel::Trace);
    octk::RtcEngine::switchLogLevel(octk::LogLevel::Warning);
    OCTK_LOGGING_INFO(EXP_LOGGER(), "octk_media_exp_webrtc_pusher");

    octk::Status status;
    const auto width = 1280;
    const auto height = 720;
    auto renderer = octk::utils::make_shared<VideoRenderer>(VideoRenderer::VideoType::I420,
                                                            "SDLRendererVideoSink",
                                                            width,
                                                            height);
    if (!renderer->init())
    {
        OCTK_LOGGING_FATAL(EXP_LOGGER(), "renderer->init failed");
    }
    auto sinkAdapter = octk::utils::make_shared<octk::RtcVideoSinkAdapter>(renderer);

    auto peerConnectionFactory = octk::RtcEngine::create();
    if (!peerConnectionFactory)
    {
        OCTK_LOGGING_FATAL(EXP_LOGGER(), "createPeerConnectionFactory create failed");
    }
    octk::RtcPeerConnectionFactory::Settings settings;
    status = peerConnectionFactory->initialize(settings);
    if (!status)
    {
        OCTK_LOGGING_FATAL(EXP_LOGGER(), "peerConnectionFactory.init failed: {}", status.errorString().c_str());
    }

    octk::RtcConfiguration pcConfiguration;
    auto peerConnection = peerConnectionFactory->create(pcConfiguration, nullptr);
    status = peerConnection->initialize();
    if (!status)
    {
        OCTK_LOGGING_FATAL(EXP_LOGGER(), "peerConnection.init failed: {}", status.errorString().c_str());
    }
    auto transceiverResult = peerConnection->addTransceiver(octk::RtcMediaType::kVideo);
    if (!transceiverResult.ok())
    {
        OCTK_LOGGING_FATAL(EXP_LOGGER(), "addTransceiver failed: {}", transceiverResult.errorString().c_str());
    }
    auto videoTransceiver = transceiverResult.value();
    auto videoReceiver = videoTransceiver->receiver();
    if (!videoReceiver)
    {
        OCTK_LOGGING_FATAL(EXP_LOGGER(), "addTransceiver failed");
    }
    auto videoTrack = std::static_pointer_cast<octk::RtcVideoTrack>(videoReceiver->track());
    if (!videoTrack)
    {
        OCTK_LOGGING_FATAL(EXP_LOGGER(), "track failed");
    }
    videoTrack->addSink(sinkAdapter);
    // videoTrack->frameReady.connect([renderer](const octk::VideoFrame &frame) { renderer->onFrame(frame); });
    status = videoTransceiver->setDirection(octk::RtcRtpTransceiverDirection::kRecvOnly);
    if (!status)
    {
        OCTK_LOGGING_FATAL(EXP_LOGGER(), "setDirectionWithError failed: {}", status.errorString().c_str());
    }
    auto offerResult = peerConnection->createOffer();
    if (!offerResult.ok())
    {
        OCTK_LOGGING_FATAL(EXP_LOGGER(), "createOffer failed: {}", offerResult.errorString().c_str());
    }
    const auto &offer = offerResult.value();
    status = peerConnection->setLocalDescription(offer.sdp, offer.type);
    if (!status)
    {
        OCTK_LOGGING_FATAL(EXP_LOGGER(), "peerHandler.setLocalDescription failed: {}", status.errorString().c_str());
    }

    octk::Json offerJson{{"offer", offer.sdp}};
    std::cout << offerJson.dump() << std::endl;
    OCTK_LOGGING_INFO(EXP_LOGGER(), "offer:{}", offer.sdp.c_str());
#if 1
    const std::string ipaddr("http://192.168.100.47");
#else
    const std::string ipaddr("http://127.0.0.1");
#endif
    cpr::Response r = cpr::Post(cpr::Url{ipaddr + "/index/api/webrtc?app=live&stream=test&type=play"},
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
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    renderer->loop();
    return 0;
}