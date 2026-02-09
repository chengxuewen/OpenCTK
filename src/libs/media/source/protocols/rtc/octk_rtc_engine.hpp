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

#pragma once

#include <octk_rtc_peerconnection_factory.hpp>
#include <octk_rtc_peerconnection.hpp>
#include <octk_media_global.hpp>

OCTK_BEGIN_NAMESPACE

class OCTK_MEDIA_API RtcEngine
{
public:
    OCTK_STATIC_CONSTANT_STRING(kBackendNameWebRTC, "WebRTC");
    OCTK_STATIC_CONSTANT_STRING(kBackendNameGStreamer, "GStreamer");

    static SharedPointer<RtcPeerConnectionFactory> create(StringView backendName = "");

    struct Creater final
    {
        using CreateFunction = std::function<RtcPeerConnectionFactory::SharedPtr()>;
        using SwitchLogLevelFunc = std::function<void(LogLevel)>;
        using InitializeFunc = std::function<void()>;

        template <typename T>
        static CreateFunction makeCreateFunction()
        {
            return []() { return utils::make_shared<T>(); };
        }

        SwitchLogLevelFunc switchLogLevelFunc;
        InitializeFunc initializeFunc;
        CreateFunction createFunc;
    };

    static void registerFactory(StringView backendName, Creater creater);
    template <typename T>
    static void registerFactory(StringView backendName, Creater creater)
    {
        enum
        {
            Valid = std::is_base_of<RtcPeerConnectionFactory, T>::value
        };
        OCTK_STATIC_ASSERT_X(Valid, "type must base on RtcPeerConnectionFactory.");
        registerFactory(backendName, std::move(creater));
    }
    static std::vector<std::string> registeredTypes();

    template <typename T>
    struct Registrar final
    {
        explicit Registrar(StringView backendName, Creater creater)
        {
            registerFactory<T>(backendName, std::move(creater));
        }
        explicit Registrar(StringView backendName)
        {
            registerFactory<T>(backendName, {nullptr, nullptr, Creater::makeCreateFunction<T>()});
        }
        explicit Registrar(StringView backendName,
                           Creater::InitializeFunc initializeFunc,
                           Creater::SwitchLogLevelFunc switchLogLevelFunc)
        {
            registerFactory<T>(backendName, {switchLogLevelFunc, initializeFunc, Creater::makeCreateFunction<T>()});
        }
    };

    static void switchLogLevel(LogLevel level, StringView backendName = "");
    // static void installLogHandler(Logger::MessageHandler handler, bool uniqueOwnership, StringView backendName = "");
};

OCTK_END_NAMESPACE

#define OCTK_RTC_ENGINE_REGISTER_FACTORY(Type, ...)                                                                    \
    namespace detail                                                                                                   \
    {                                                                                                                  \
    static octk::RtcEngine::Registrar<Type> rtcFactoryRegistrar##Type(__VA_ARGS__);                                    \
    }
